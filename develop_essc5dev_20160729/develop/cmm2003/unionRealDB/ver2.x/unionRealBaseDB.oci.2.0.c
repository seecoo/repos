#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#include "oci.h"    

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"
#include "unionTableDef.h"
#include "unionXMLPackage.h"

#define MAX_ITEMS	 64

OCIEnv		*g_oci_env = NULL;	//OCI environment handle
OCIError	*g_oci_err = NULL;	//OCI error handle
OCISvcCtx	*g_oci_svc = NULL;	//OCI service context handel
//OCIServer	*g_oci_ser = NULL;	//OCI server handle
OCISession	*g_oci_usr = NULL;	//OCI user session handle
OCIStmt		*g_oci_stmt = NULL;	//OCI statement handle
//OCIDefine	*g_oci_dfn = NULL;	//OCI define handle
//OCIBind		*g_oci_bnd = NULL;	//OCI bind handle

typedef struct {
	OCIDefine *def;
	int	isNull;
	int	col_name_len;
	//char	name[32];
	char	*name;
	int	length;
	char	buffer[4096];
}TUnionDBField;
typedef TUnionDBField	*PUnionDBField;

/*
// 记录定义
typedef struct
{
	int		colNum;
	PUnionDBField	pfield;
}TUnionDBRecord;
typedef TUnionDBRecord   *PUnionDBRecord;
*/

static int		g_sqlret = 0;
//static TUnionDBRecord	gunionDBRecord;

int get_one_integer_value(void *arg ,int nCount,char** pValue,char** pName)
{
	int	*iCnt = (int *)arg;
	if (nCount > 1)
	{
		UnionUserErrLog("in get_one_integer_value:: nCount[%d] > 1\n",nCount);
		return(errCodeDatabaseMDL_MoreRecordFound);
	}
	*iCnt = atoi(pValue[0]);
	return(*iCnt);
}

static int getDiagRec()
{
	sb4	sqlcode = -1; 
	char	buffer[256 + 1]; 

	switch (g_sqlret)
	{
		case OCI_SUCCESS:
			return 0;
		case OCI_SUCCESS_WITH_INFO:
			strcpy(buffer, "Error - OCI_SUCCESS_WITH_INFO");
			break;
		case OCI_NEED_DATA:
			strcpy(buffer, "Error - OCI_NEED_DATA");
			break;
		case OCI_NO_DATA:
			strcpy(buffer, "Error - OCI_NODATA");
			break;
		case OCI_ERROR:
			(void) OCIErrorGet(g_oci_err, 1, NULL, &sqlcode, (OraText *)buffer, (ub4)sizeof(buffer), OCI_HTYPE_ERROR);
			break;
		case OCI_INVALID_HANDLE:
			strcpy(buffer, "Error - OCI_INVALID_HANDLE");
			break;
		case OCI_STILL_EXECUTING:
			strcpy(buffer, "Error - OCI_STILL_EXECUTE");
			break;
		case OCI_CONTINUE:
			strcpy(buffer, "Error - OCI_CONTINUE");
			break;
		default:
			return 0;
	}
	
	UnionUserErrLog("in getDiagRec:: SQLCODE[%d], MESSAGE[%s]\n", (int)sqlcode, buffer); 
	UnionSetResponseRemark(buffer);
	return(0-abs(sqlcode));
}

static int getDBErrCode()
{
	int	ret;

	if ((ret = getDiagRec()) < 0)
	{
		// 设计数据库错误码偏移	
		ret = ret + errCodeOffsetOfOracle;	
		return(UnionSetUserDefinedErrorCode(ret));
	}
	return(0);
}

// 初始化句柄
static int initAllocHandle()
{
	int	ret;
	
	/*
	if ((ret = OCIInitialize((ub4) OCI_DEFAULT, (dvoid *)0,
			(dvoid * (*)(dvoid *, size_t)) 0,
			(dvoid * (*)(dvoid *, dvoid *, size_t))0,
			(void (*)(dvoid *, dvoid *)) 0 )) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: OCIInitialize ret = [%d]\n",ret);
		return(0- abs(ret));
	}
	*/
	
	//if ((ret = OCIEnvInit(&g_oci_env, OCI_DEFAULT, (size_t) 0, (dvoid **) 0 )) != OCI_SUCCESS)
	if ((ret = OCIEnvCreate(&g_oci_env, OCI_DEFAULT, 0, 0, 0, 0, 0, 0)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Create a OCI environment failed\n");
		return(0- abs(ret));
	}

	/*
	if ((ret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_ser,OCI_HTYPE_SERVER,0,NULL)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Allocating server context failed\n");
		return(0- abs(ret));
	}
	*/

	if ((ret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_err,OCI_HTYPE_ERROR,0,NULL)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Allocating error handler failed\n");
		return(0- abs(ret));
	}

	if ((ret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_usr,OCI_HTYPE_SESSION,0,NULL)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Allocating session handler failed\n");
		return(0- abs(ret));
	}

	if ((ret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_svc,OCI_HTYPE_SVCCTX,0,NULL)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Allocating svcctx handler failed\n");
		return(0- abs(ret));
	}
	if ((ret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_stmt,OCI_HTYPE_STMT,0,NULL)) != OCI_SUCCESS)
	{
		UnionUserErrLog("in initAllocHandle:: Allocating stmt handler failed\n");
		return(0- abs(ret));
	}
	return(0);
}

// 释放句柄
static int freeAllocHandle()
{
	/*
	if (g_oci_ser)
	{
		OCIHandleFree(g_oci_ser,OCI_HTYPE_SERVER);	// 释放服务器句柄
		g_oci_ser = NULL;
	}
	*/
	if (g_oci_err)
	{
		OCIHandleFree(g_oci_err,OCI_HTYPE_ERROR);	// 释放错误句柄
		g_oci_err = NULL;
	}
	if (g_oci_usr)
	{
		OCIHandleFree(g_oci_usr,OCI_HTYPE_SESSION);	// 释放事务句柄
		g_oci_usr = NULL;
	}
	if (g_oci_svc)
	{
		OCIHandleFree(g_oci_svc,OCI_HTYPE_SVCCTX);	// 释放上下文句柄
		g_oci_svc = NULL;
	}
	if (g_oci_stmt)
	{
		OCIHandleFree(g_oci_stmt,OCI_HTYPE_STMT);	// 释放SQL语句句柄
		g_oci_stmt = NULL;
	}
	if (g_oci_env)
	{
		OCIHandleFree(g_oci_env,OCI_HTYPE_ENV);		// 释放环境句柄
		g_oci_env = NULL;
	}

	return(0);
}

int UnionIsFatalDBErrCode(int dbErrCode)
{       
	int	len;
	char    varName[100];

	len = sprintf(varName,"fatalDBErr%d",abs(dbErrCode));
	varName[len] = 0;

	if (UnionReadIntTypeRECVar(varName) > 0)
		return(1); // 兼容旧版本

	return 0;
}

//为结果集绑定缓冲区
/*
static int allocSpaceAndSpecToField(int index, ub2 type, int size)
{
	int	ret;
	int	bufSize;
	PUnionDBField	pdbField = NULL;
	
	bufSize = size + 1;
	pdbField = (gunionDBRecord.pfield + index);
	
	pdbField->length = bufSize;
	pdbField->isNull = 0;
	
	pdbField->name = NULL;
	pdbField->buffer = NULL;
	pdbField->date = NULL;
	pdbField->lob_loc = NULL;
	
	switch(type)
	{
		case SQLT_BLOB:
			OCIDescriptorAlloc((dvoid *)g_oci_env, (dvoid **) &(pdbField->lob_loc),
				(ub4) OCI_DTYPE_LOB,
				(size_t) 0, (dvoid **) 0);
			g_sqlret = OCIDefineByPos(g_oci_stmt, &pdbField->def, g_oci_err, index + 1,
				&(pdbField->lob_loc), bufSize, SQLT_BLOB, &(pdbField->isNull), 0, 0, OCI_DEFAULT);
			break;
		case SQLT_CLOB:
			OCIDescriptorAlloc((dvoid *)g_oci_env, (dvoid **) &(pdbField->lob_loc),
				(ub4) OCI_DTYPE_LOB,
				(size_t) 0, (dvoid **) 0);
			g_sqlret = OCIDefineByPos(g_oci_stmt, &pdbField->def, g_oci_err, index + 1,
				&(pdbField->lob_loc), bufSize, SQLT_CLOB, &(pdbField->isNull), 0, 0, OCI_DEFAULT);
			break;
		case SQLT_DAT:
		case SQLT_DATE:
		case SQLT_TIMESTAMP:
		case SQLT_TIMESTAMP_TZ:
		case SQLT_TIMESTAMP_LTZ:
			OCIDescriptorAlloc((dvoid *)g_oci_env, (dvoid **) &(pdbField->date),
				(ub4) OCI_DTYPE_TIMESTAMP,
				(size_t) 0, (dvoid **) 0);
			g_sqlret = OCIDefineByPos(g_oci_stmt, &pdbField->def, g_oci_err, index + 1,
				&(pdbField->date), sizeof(pdbField->date), SQLT_TIMESTAMP, &(pdbField->isNull), 0, 0, OCI_DEFAULT);
			break;
		case SQLT_NUM:
		case SQLT_INT:
			pdbField->buffer = malloc(bufSize+1);
			g_sqlret = OCIDefineByPos(g_oci_stmt, &pdbField->def, g_oci_err, index + 1,
				pdbField->buffer, bufSize, SQLT_INT, &(pdbField->isNull), 0, 0, OCI_DEFAULT);
			break;
		default:
			pdbField->buffer = malloc(bufSize+1);
			g_sqlret = OCIDefineByPos(g_oci_stmt, &pdbField->def, g_oci_err, index + 1,
				pdbField->buffer, bufSize, SQLT_STR, &(pdbField->isNull), 0, 0, OCI_DEFAULT);
			break;
	}

	if (g_sqlret != OCI_SUCCESS)	
	{
		ret = getDBErrCode();
		UnionUserErrLog("in allocSpaceAndSpecToField:: OCIDefineByPos");
		return(ret);
	}

	return(bufSize);
}

static int freeDBRecordSpace()
{
	int	i;

	for (i = 0; i < gunionDBRecord.colNum; i++) {
		if (gunionDBRecord.pfield[i].lob_loc)
			OCIDescriptorFree(gunionDBRecord.pfield[i].lob_loc, OCI_DTYPE_LOB);
		if (gunionDBRecord.pfield[i].date)
			OCIDescriptorFree((dvoid*)gunionDBRecord.pfield[i].date, OCI_DTYPE_TIMESTAMP);
		if (gunionDBRecord.pfield[i].buffer)
			free(gunionDBRecord.pfield[i].buffer);
		if (gunionDBRecord.pfield[i].name)
			free(gunionDBRecord.pfield[i].name);
		if (gunionDBRecord.pfield[i].def)
			OCIHandleFree(gunionDBRecord.pfield[i].def, OCI_HTYPE_DEFINE);
	}
	free(gunionDBRecord.pfield);
	gunionDBRecord.colNum = 0;
       	return(0);
}
*/
static int  freeStmtHandle()
{
	/*
	if (g_oci_stmt)
	{
		OCIHandleFree(g_oci_stmt, OCI_HTYPE_STMT);
		g_oci_stmt = NULL;
	}
	*/
	return 0;
}

static int createStmtHandle()
{
	int	ret = 0;
	/*
	freeStmtHandle();
	
	g_sqlret = OCIHandleAlloc(g_oci_env,(dvoid *)&g_oci_stmt,OCI_HTYPE_STMT,0,NULL);
	if (g_sqlret != OCI_SUCCESS)	
	{
		ret = getDBErrCode();
		UnionUserErrLog("in createStmtHandle:: Allocating stmt handler failed");
		return(ret);
	}
	*/
	return(ret);
}

int UnionConnectDB()
{
	int	ret;

	char	DBUserName[128];
	char	DBPasswd[128];
	char	DBname[128];
    
	memset(DBUserName, 0, sizeof(DBUserName));
	memset(DBPasswd, 0, sizeof(DBPasswd));
	memset(DBname, 0, sizeof(DBname));
	UnionGetDBUSER(DBUserName);
	UnionGetDBPASSWD(DBPasswd);
	UnionGetDBNAME(DBname);

	// 初始化句柄
	if ((ret = initAllocHandle()) < 0)
	{
		UnionUserErrLog("in UnionConnectDB:: initAllocHandle!\n");
		freeAllocHandle();
		return(ret);
	}
	
	// 连接数据库
	g_sqlret = OCILogon(g_oci_env,g_oci_err,&g_oci_svc,(const OraText*)DBUserName,strlen(DBUserName),(const OraText*)DBPasswd,strlen(DBPasswd),(const OraText*)DBname,strlen(DBname));
	if (g_sqlret != OCI_SUCCESS)
	{
		ret = getDBErrCode();
		UnionUserErrLog("in UnionConnectDB:: DBname = [%s] DBUserName = [%s]\n",DBname,DBUserName);
		UnionUserErrLog("in UnionConnectDB:: database g_sqlret = [%d]\n",g_sqlret);
		freeAllocHandle();
		return(ret);
	}

	return(0);
}

int UnionCloseDB()
{
	OCILogoff(g_oci_svc,g_oci_err);
	freeAllocHandle();

	return(0);
}

static int execSQLStatement(const char* sql,select_callback callback,void *pArg)
{
	int	i;
	int	ret = 0;
	int	colNum = 0;
	int	realRowNum;
	int	len;
	int	callbackIsInit = 0;
	char 	tmpBuf[32];
	ub2	dtype = 0;
	ub2	deptlen;
	//char	*col_name = NULL;
	//ub4	col_name_len;
	ub4	char_semantics = 0;
	char	**azCols = 0;	  /* Names of result columns */
	char	**azVals = 0;
	OCIParam* pard = NULL;
	//const char fmt[] = "IYYY-MM-DD HH24.MI.SS";
	TUnionDBField	dbField[MAX_ITEMS];
	
	if((g_sqlret = OCIStmtPrepare(g_oci_stmt, g_oci_err, (const OraText *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT)) != OCI_SUCCESS)
	{
		ret = getDBErrCode();
		UnionUserErrLog("in execSQLStatement:: OCIStmtPrepare sqlStr=[%s] Error !\n", sql);
		return(ret);
	}

	g_sqlret = OCIStmtExecute(g_oci_svc, g_oci_stmt, g_oci_err, 0, 0, NULL, NULL, OCI_DEFAULT);
	if(g_sqlret != OCI_SUCCESS)
	{
		ret = getDBErrCode();
		UnionUserErrLog("in execSQLStatement:: OCIStmtExecute g_sqlret = %d\n", g_sqlret);
		return(ret);
	}

	//获取结果集列数
	g_sqlret = OCIAttrGet(g_oci_stmt, OCI_HTYPE_STMT, &colNum, NULL, OCI_ATTR_PARAM_COUNT, g_oci_err);
	if (g_sqlret != OCI_SUCCESS)
	{
		if (g_sqlret != OCI_NO_DATA)
		{
			ret = getDBErrCode();
			UnionUserErrLog("in execSQLStatement:: OCIAttrGet g_sqlret = %d, ret = %d\n", g_sqlret, ret);
			return(ret);
		}
		else
			return(0);
	}

	realRowNum = 0;
	
	//获取字段属性
	for (i = 0; i < colNum; i++)
	{
		deptlen = 0;
		dbField[i].name = NULL;		
		g_sqlret = OCIParamGet(g_oci_stmt, OCI_HTYPE_STMT, g_oci_err, (void **)&pard, i+1);
		if(g_sqlret != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			UnionUserErrLog("in execSQLStatement:: OCIAttrGet OCI_HTYPE_STMT g_sqlret = %d ret = %d\n", g_sqlret, ret);
			//freeDBRecordSpace();
			goto exec_out;
		}
		
		g_sqlret = OCIAttrGet(pard, OCI_DTYPE_PARAM, &char_semantics, NULL, OCI_ATTR_CHAR_USED, g_oci_err);
		if(g_sqlret != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			OCIDescriptorFree(pard, OCI_DTYPE_PARAM);
			UnionUserErrLog("in execSQLStatement:: OCIAttrGet OCI_ATTR_CHAR_USED g_sqlret = %d ret = %d\n", g_sqlret, ret);
			//freeDBRecordSpace();
			goto exec_out;
		}
		
		if (char_semantics)
		{
			g_sqlret = OCIAttrGet(pard, OCI_DTYPE_PARAM, &deptlen, NULL, OCI_ATTR_CHAR_SIZE, g_oci_err);
			if(g_sqlret != OCI_SUCCESS)
			{
				ret = getDBErrCode();
				OCIDescriptorFree(pard, OCI_DTYPE_PARAM);
				UnionUserErrLog("in execSQLStatement:: OCIAttrGet OCI_ATTR_CHAR_SIZE g_sqlret = %d ret = %d\n", g_sqlret, ret);
			//	freeDBRecordSpace();
				goto exec_out;
			}
		}
		else
		{
			g_sqlret = OCIAttrGet(pard, OCI_DTYPE_PARAM, &deptlen, NULL, OCI_ATTR_DATA_SIZE, g_oci_err);
			if(g_sqlret != OCI_SUCCESS)
			{
				ret = getDBErrCode();
				OCIDescriptorFree(pard, OCI_DTYPE_PARAM);
				UnionUserErrLog("in execSQLStatement:: OCIAttrGet OCI_ATTR_DATA_SIZE g_sqlret = %d ret = %d\n", g_sqlret, ret);
				//freeDBRecordSpace();
				goto exec_out;
			}
		}
		OCIAttrGet(pard, OCI_DTYPE_PARAM, &dtype, NULL, OCI_ATTR_DATA_TYPE, g_oci_err);

		// 申请记录空间
		//allocSpaceAndSpecToField(i, dtype, deptlen);
		dbField[i].isNull = 0;
		dbField[i].length = deptlen;
		
		switch(dtype)
		{
			case SQLT_BLOB:
			case SQLT_CLOB:
			case SQLT_DAT:
			case SQLT_DATE:
			case SQLT_TIMESTAMP:
			case SQLT_TIMESTAMP_TZ:
			case SQLT_TIMESTAMP_LTZ:
				UnionUserErrLog("in execSQLStatement:: dtype[%d] error!\n",dtype);
				return(errCodeParameter);
			case SQLT_NUM:
			case SQLT_INT:
				g_sqlret = OCIDefineByPos(g_oci_stmt, &dbField[i].def, g_oci_err, (ub4)(i + 1),
					dbField[i].buffer, sizeof(dbField[i].buffer), SQLT_INT, &(dbField[i].isNull), 0, 0, OCI_DEFAULT);
				break;
			default:
				g_sqlret = OCIDefineByPos(g_oci_stmt, &dbField[i].def, g_oci_err, (ub4)(i + 1),
					dbField[i].buffer, sizeof(dbField[i].buffer), SQLT_STR, &(dbField[i].isNull), 0, 0, OCI_DEFAULT);
				break;
		}
	
		if (g_sqlret != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			UnionUserErrLog("in execSQLStatement:: OCIDefineByPos\n");
			goto exec_out;
		}
		
		g_sqlret = OCIAttrGet(pard, OCI_DTYPE_PARAM, &dbField[i].name, (ub4 *)&dbField[i].col_name_len, OCI_ATTR_NAME, g_oci_err);
		if(g_sqlret != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			OCIDescriptorFree(pard, OCI_DTYPE_PARAM);
			UnionUserErrLog("in execSQLStatement:: OCIAttrGet OCI_ATTR_NAME g_sqlret = %d ret = %d\n", g_sqlret, ret);
			//freeDBRecordSpace();
			goto exec_out;
		}
		dbField[i].name[dbField[i].col_name_len] = 0;
		// 改为小写
		UnionToLowerCase(dbField[i].name);

		OCIDescriptorFree(pard, OCI_DTYPE_PARAM);
	}

	if (!callback)
	{
		if ((ret = UnionInitXMLPackage(NULL,NULL,0)) < 0)
		{
			UnionUserErrLog("in execSQLStatement:: UnionInitXMLPackage\n");
			goto exec_out;
		}
	
		if ((ret = UnionSetXMLPackageValue("totalNum","0")) < 0)
		{
			UnionUserErrLog("in execSQLStatement:: UnionSetXMLPackageValue[%s][%s]\n","totalNum","0");
			goto exec_out;
		}
	}
	else
	{
		callbackIsInit = 0;
		azCols = (char **)calloc(1,(2 * colNum * sizeof(const char*) + 1));
	}

	while ((g_sqlret = OCIStmtFetch(g_oci_stmt, g_oci_err, 1, OCI_FETCH_NEXT, OCI_DEFAULT)) == OCI_SUCCESS || g_sqlret == OCI_SUCCESS_WITH_INFO)
	{
		if (!callback)
			UnionLocateNewXMLPackage("detail", ++realRowNum);
		else
			++realRowNum;

		for (i = 0; i < colNum; i++)
		{
			if (dbField[i].isNull)
				dbField[i].buffer[0] = 0;
			/*
			if (gunionDBRecord.pfield[i].date)
			{
				gunionDBRecord.pfield[i].length = 255;
				if (gunionDBRecord.pfield[i].buffer)
					free(gunionDBRecord.pfield[i].buffer);
				
				gunionDBRecord.pfield[i].buffer = malloc(gunionDBRecord.pfield[i].length + 1);
				g_sqlret = OCIDateTimeToText(g_oci_usr,g_oci_err,
					gunionDBRecord.pfield[i].date,
					(const OraText *)fmt, strlen(fmt),
					0,NULL, 0,
					(ub4*)&(gunionDBRecord.pfield[i].length), (OraText *)gunionDBRecord.pfield[i].buffer);
				if(g_sqlret != OCI_SUCCESS)
				{
					ret = getDBErrCode();
					UnionUserErrLog("in execSQLStatement:: SQLExecute g_sqlret = %d\n", g_sqlret);
					freeDBRecordSpace();
					return(ret);
				}
				UnionSetXMLPackageValue(dbField[i].name, dbField[i].buffer);
				continue;
			}
			*/
			if (callback)
			{
				if (!callbackIsInit)
				{
					azCols[i] = (char *)malloc(dbField[i].col_name_len + 1);
					azCols[colNum + i] = (char *)malloc(dbField[i].length + 1);
					strcpy(azCols[i],dbField[i].name);
				}
				azVals = &azCols[colNum];
				strcpy(azVals[i],dbField[i].buffer);
			}
			else
				UnionSetXMLPackageValue(dbField[i].name, dbField[i].buffer);
			continue;
		}
		if (callback)
		{
			callbackIsInit = 1;
			if ((ret = callback(pArg, colNum, azVals, azCols)) < 0)
			{
				UnionUserErrLog("in execSQLStatement:: callback ret = [%d]!\n",ret);
				goto exec_out;
			}
		}
	}

	ret = realRowNum;

	if (!callback)
	{
		len = sprintf(tmpBuf,"%d", (int)realRowNum);
		tmpBuf[len] = 0;
		UnionLocateXMLPackage("",0);
		UnionSetXMLPackageValue("totalNum",tmpBuf);
	}

exec_out:	
	for (i = 0; i < colNum; i++)
		OCIHandleFree(dbField[i].def,OCI_HTYPE_DEFINE);
		
	/*
		if (dbField[i].name)
			free(dbField[i].name);
	*/
	if (callback)
	{
		for (i = 0; i < 2*colNum; i++)
		{
			if (azCols && azCols[i])
				free(azCols[i]);
		}
		if (azCols)
			free(azCols);
	}
	return(ret);
}
int UnionSelectRealDBRecord2(char *sql,int currentPage,int numOfPerPage,select_callback callback,void *pArg)
{
	int     beginNum = 0;
	int 	endNum = 0;
	int	ret = -1;
	int	reconnectTimes = 0;

	char	dbSql[8192*2];
	char	*sqlPtr = NULL;

onceAgain:
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: UnionConnectDatabase ret = [%d]\n",ret);
		return(ret);
	}

	if ((currentPage <= 0) || (numOfPerPage <= 0))
		sqlPtr = sql;
	else
	{
		beginNum = (currentPage - 1) * numOfPerPage + 1;
		endNum = beginNum + numOfPerPage - 1;

		snprintf(dbSql,sizeof(dbSql),"select * from (select a.*,rownum r from (%s) a where rownum <= %d) where r >= %d",sql,endNum, beginNum);
		sqlPtr = dbSql;
	}

	UnionProgramerLog("in UnionSelectRealDBRecord2:: sql=[%s]!\n",sqlPtr);

	if ((ret = createStmtHandle()) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: createStmtHandle!\n");
		UnionCloseDatabase();
		return(ret);
	}

	//执行sql语句
	if ((ret = execSQLStatement(sqlPtr,callback,pArg)) < 0)
	{
		freeStmtHandle();
		UnionCloseDatabase();
		if (!reconnectTimes)
		{
			reconnectTimes ++;
			goto onceAgain;
		}
		else
		{
			UnionUserErrLog("in UnionSelectRealDBRecord2:: execSQLStatement!\n");
			return(ret);
		}
	}

	freeStmtHandle();
	return(ret);
}

int UnionSelectRealDBRecord(char *sql,int currentPage,int numOfPerPage)
{
	return(UnionSelectRealDBRecord2(sql,currentPage,numOfPerPage,NULL,NULL));
}

int UnionSelectRealDBRecordCounts(char *tableName,char *joinCondition,char *condition)
{
	int     	ret = 0;
	char    	sql[8192];
	int 		len = 0;
	int		counts;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: tableName is NULL!\n");
		return(errCodeParameter);
	}

	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: UnionConnectDatabase!\n");
		return(ret);
	}

	len = snprintf(sql,sizeof(sql),"select count(1) as counts from %s",tableName);

	if ((joinCondition != NULL) && (strlen(joinCondition) > 0))
		len += snprintf(sql + len,sizeof(sql) - len," %s",joinCondition);

	if ((condition != NULL) && (strlen(condition) > 0))
		len += snprintf(sql + len,sizeof(sql) - len," where %s",condition);

	if ((ret = UnionSelectRealDBRecord2(sql, 0, 0, get_one_integer_value,&counts)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: UnionSelectRealDBRecord2  ret = %d\n", ret);
		return(ret);
	}
	return(counts);
}

int UnionExecRealDBSql2(int syncFlag,char* sql)
{
	int	ret = 0;     
	int	len = 0;    
	int	realRowNum = 0;     
	char	dbSql[8192*2];
	char	*sqlPtr = NULL;
	int	reconnectTimes = 0;

	if (sql == NULL)
	{
		UnionUserErrLog("in UnionExecRealDBSql2:: sql is null\n");
		return(errCodeParameter);
	}

onceAgain:
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionExecRealDBSql2:: UnionConnectDatabase!\n");
		return(ret);
	}

	len = strlen(sql);
	if (sql[len-1] == ';')	// 匿名块，多条sql语句
	{
		len = snprintf(dbSql,sizeof(dbSql),"begin %s end;",sql);
		dbSql[len] = 0;
		sqlPtr = dbSql;
	}
	else
		sqlPtr = sql;

	UnionProgramerLog("in UnionExecRealDBSql2:: sql = [%s]\n", sqlPtr);

	do
	{
		if ((ret = createStmtHandle()) < 0)
		{
			UnionUserErrLog("in UnionExecRealDBSql2:: createStmtHandle()=[%d] Error \n", ret);
			break;
		}

		if((g_sqlret = OCIStmtPrepare(g_oci_stmt, g_oci_err, (const OraText *)sql, (ub4)strlen(sqlPtr), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT)) != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			UnionUserErrLog("in UnionExecRealDBSql2:: OCIStmtPrepare sqlStr=[%s] Error !\n", sqlPtr);
			break;
		}

		g_sqlret = OCIStmtExecute(g_oci_svc,g_oci_stmt,g_oci_err,1,0,NULL,NULL,OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS);
		if (g_sqlret != OCI_SUCCESS)
		{
			if (g_sqlret != OCI_NO_DATA)
			{
				ret = getDBErrCode();
				UnionUserErrLog("in  UnionExecRealDBSql2:: OCIStmtExecute g_sqlret = %d\n", g_sqlret);
			}
			else
				ret = 0;
			break;
		}

		g_sqlret = OCIAttrGet(g_oci_stmt, OCI_HTYPE_STMT, &realRowNum, 0, OCI_ATTR_ROW_COUNT, g_oci_err);
		if (g_sqlret != OCI_SUCCESS)
		{
			ret = getDBErrCode();
			UnionUserErrLog("in UnionExecRealDBSql2:: SQLRowCount g_sqlret = %d, ret = %d\n", g_sqlret, ret);
			break;
		}

		ret = realRowNum;
	}
	while(0);

	//OCITransCommit(g_oci_svc, g_oci_err, OCI_DEFAULT);
	
	freeStmtHandle();

	if (ret < 0)
	{
		if (!reconnectTimes)
		{
			reconnectTimes ++;
			goto onceAgain;
		}
		else
		{
			UnionCloseDatabase();
		}
	}

	return(ret);
}

int UnionExecRealDBSql(char *sql)
{
	return(UnionExecRealDBSql2(1,sql));
}

int UnionGetAddSelfNumBySequence(char *sequenceName)
{
	int     ret = 0;
	int	sequence = 0;
	char	sql[1024];

	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionGetAddSelfNumBySequence:: UnionConnectDatabase!\n");
		return(ret);
	}

	snprintf(sql,sizeof(sql),"select %s.nextval as nextval from sys.dual",sequenceName);
	
	//UnionProgramerLog("in UnionGetAddSelfNumBySequence:: sql=[%s]\n", sql);

	if ((ret = UnionSelectRealDBRecord2(sql, 0, 0, get_one_integer_value,&sequence)) < 0)
	{
		UnionUserErrLog("in UnionGetAddSelfNumBySequence:: UnionSelectRealDBRecord2  ret = %d\n", ret);
		return(ret);
	}
	if (sequence == 999999)
	{
		snprintf(sql,sizeof(sql),"alter sequence %s restart with 1 increment by 1 maxvalue 999999",sequenceName);	
		if ((ret =  UnionExecRealDBSql2(0,sql)) < 0)
		{
			UnionUserErrLog("in UnionGetAddSelfNumBySequence:: UnionExecRealDBSql2 ret = %d !\n", ret);
			return(ret);
		}
	}
	return(sequence);
}

int UnionSelectRealDBTree(int returnSql,char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue,char *buf,int sizeOfBuf)
{
	int	i;
	int	ret;
	int	num;
	int	len;
	char	sql[1024];
	char	tmpBuf[128];
	
	if (fieldType == 1)	// 字符串
		snprintf(tmpBuf,sizeof(tmpBuf),"'%s'",rootValue);
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"%s",rootValue);
	
	snprintf(sql,sizeof(sql),"select %s from %s start with %s = %s connect by prior %s = %s",fieldName,tableName,fieldName,tmpBuf,fieldName,parentFieldName);

	if (returnSql)
	{
		ret = snprintf(buf,sizeOfBuf,"%s",sql);
		return(ret);
	}

	if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBTree:: UnionSelectRealDBRecord,sql = [%s] ret = [%d] !\n",sql,num);
		return(num);
	}
	else if (num == 0)
	{
		buf[0] = 0;
		return(0);
	}
	
	len = 0;
	for (i = 1; i <= num; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionSelectRealDBTree:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
			return(ret);
		}
		if ((ret = UnionReadXMLPackageValue(fieldName,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionSelectRealDBTree:: UnionReadXMLPackageValue[%s][%d]!\n",fieldName,i);
			return(ret);
		}
		if (fieldType == 1)	// 字符串
		{
			if (i < num)
				len += snprintf(buf + len,sizeOfBuf - len,"'%s',",tmpBuf);
			else
				len += snprintf(buf + len,sizeOfBuf - len,"'%s'",tmpBuf);
		}
		else
		{
			if (i < num)
				len += snprintf(buf + len,sizeOfBuf - len,"%s,",tmpBuf);
			else
				len += snprintf(buf + len,sizeOfBuf - len,"%s",tmpBuf);
		}
	}
	return(len);
}

int UnionDeleteRealDBTree(char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue)
{
	int	ret;
	char	sql[10240];
	char	condition[8192];
	
	if ((ret = UnionSelectRealDBTree(1,tableName,fieldName,parentFieldName,fieldType,rootValue,condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionDeleteRealDBTree:: UnionSelectRealDBTree!\n");
		return(ret);
	}

	snprintf(sql,sizeof(sql),"delete from %s where %s in (%s)",
		tableName,fieldName,condition);
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRealDBTree:: UnionExecRealDBSql!\n");
		return(ret);
	}
	return(ret);
}

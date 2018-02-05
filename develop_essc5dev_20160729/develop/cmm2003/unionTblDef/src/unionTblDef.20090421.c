//	Author:		Union tech.
//	Date:		2012-05-07
//	Version:	1.0

#define _UnionLogMDL_3_x_	
//#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionModule.h"
//#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionStr.h"
//#include "unionWorkingDir.h"
//#include "unionRecFile.h"

#include "unionREC.h"

//#include "unionDBConf.h"
#include "unionTblDef.h"
#include "unionComplexDBObjectDef.h"

PUnionSharedMemoryModule	pgunionTblDefMDL = NULL;
PUnionTblDef			pgunionTblDef = NULL;
PUnionTblDefRec			pgunionTblDefRec = NULL;

int UnionGetCurrentTblDefNum()
{
	int	ret;
	
	if ((ret = UnionConnectTblDef()) < 0)
		return(ret);
	else
		return(pgunionTblDef->realNum);
}

PUnionTblDef UnionGetCurrentTblDefGrp()
{
	if (UnionConnectTblDef() < 0)
		return(NULL);
	else
		return(pgunionTblDef);
}

int UnionIsTblDefConnected()
{
	if ((pgunionTblDefRec == NULL) || (pgunionTblDef == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfTblDef)))
		return(0);
	else
		return(1);
}

int UnionConnectTblDef()
{
	//PUnionSharedMemoryModule	pmdl;
	int				num;
	
	if (UnionIsTblDefConnected())
		return(0);
		
	num = 128;
	
	if ((pgunionTblDefMDL = UnionConnectSharedMemoryModule(conMDLNameOfTblDef,
			sizeof(TUnionTblDef) + sizeof(TUnionTblDefRec) * num)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTblDef:: UnionConnectSharedMemoryModule! MDLName = %s ret = %d\n", conMDLNameOfTblDef, errCodeSharedMemoryModule);
		return(UnionSetUserDefinedErrorCode(errCodeSharedMemoryModule));
	}

	if ((pgunionTblDef = (PUnionTblDef)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTblDefMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTblDef:: pgunionTblDef ret = %d!\n", errCodeSharedMemoryModule);
		return(UnionSetUserDefinedErrorCode(errCodeSharedMemoryModule));
	}
	
	if ((pgunionTblDef->pTBL = (PUnionTblDefRec)((unsigned char *)pgunionTblDef + sizeof(*pgunionTblDef))) == NULL)
	{
		UnionUserErrLog("in UnionConnectTblDef:: pgunionTblDef ret = %d!\n", errCodeSharedMemoryModule);
		return(UnionSetUserDefinedErrorCode(errCodeSharedMemoryModule));
	}
	pgunionTblDefRec = pgunionTblDef->pTBL;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionTblDefMDL))
	{
		pgunionTblDef->maxNum = num;
		pgunionTblDef->realNum = 0;
		//return(UnionReloadTblDef());
		return(0);
	}
	else
		return(0);
}


int UnionDisconnectTblDef()
{
	pgunionTblDef->realNum = 0;
	pgunionTblDefRec = NULL;
	pgunionTblDef = NULL;
	return(UnionDisconnectShareModule(pgunionTblDefMDL));
}

int UnionRemoveTblDef()
{
	UnionDisconnectTblDef();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfTblDef));
}

int UnionPrintTblDefRecToFile(PUnionTblDefRec prec, FILE *fp)
{
	if ((prec == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp,"TABLE NAME = [%40s]	PRIMARYkEY FIELD NUM = [%d] TABLE FIELD NUM = [%02d]\n", prec->tableName, prec->primaryKey.fldNum, prec->fldNum);
	return(0);
}

int UnionPrintDetailTblDefRecToFile(PUnionTblDefRec prec, FILE *fp)
{
	int				i;

	if ((prec == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp, "TABLE NAME = [%40s] PRIMARYKEY FIELD NUM = [%d] TABLE FIELD NUM = [%02d]\n", prec->tableName, prec->primaryKey.fldNum, prec->fldNum);

	fprintf(fp, "TABLE PRIMARY KEY:       [");
	for(i = 0; i < prec->primaryKey.fldNum; i++)
	{
		fprintf(fp, "%s  ", prec->primaryKey.fldNameGrp[i]);
	}
	fprintf(fp, "]\n");

	fprintf(fp, "TABLE FIELDS:             tableFieldName           fieldType\n");
	for(i = 0; i < prec->fldNum; i++)
	{
		fprintf(fp, "%40s%20d\n", prec->fldDefGrp[i].name, prec->fldDefGrp[i].type);
	}
	fprintf(fp,"\n");
	return(0);
}

int UnionPrintTblDefToFile(FILE *fp)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTblDef()) < 0)
	{
		UnionUserErrLog("in UnionPrintTblDefToFile:: UnionConnectTblDef!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	if (fp == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	for (i = 0; i < pgunionTblDef->realNum; i++)
	{
		UnionPrintTblDefRecToFile((pgunionTblDefRec + i), fp);
	}
	fprintf(fp,"TABLE NUM = [%d]\n",pgunionTblDef->realNum);
	return(0);
}

int UnionPrintDetailTblDefToFile(FILE *fp, char *tableName)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTblDef()) < 0)
	{
		UnionUserErrLog("in UnionPrintDetailTblDefToFile:: UnionConnectTblDef!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	if (fp == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	
	ret = 0;
	if(strcmp(tableName, "ALL") == 0)
	{
		for (i = 0; i < pgunionTblDef->realNum; i++)
		{
			UnionPrintDetailTblDefRecToFile((pgunionTblDefRec + i), fp);
			ret++;
		}
	}
	else 
	{
		for (i = 0; i < pgunionTblDef->realNum; i++)
		{
			if(strcmp(tableName, (pgunionTblDefRec + i)->tableName) == 0)
			{
				UnionPrintDetailTblDefRecToFile((pgunionTblDefRec + i), fp);
				ret++;
			}
		}
	}
	if(ret==0)
	{
		fprintf(fp, "no table def data in image or spec table not found!!\n");
	}
	return(0);
}

PUnionTblDefRec UnionReadTblDefRec(char *tableName)
{
	int		i;
	int		ret;
	
	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionReadTblDefRec:: tableName is NULL!\n");
		return(NULL);
	}
	if ((ret = UnionConnectTblDef()) < 0)
	{
		UnionUserErrLog("in UnionReadTblDefRec:: UnionConnectTblDef!\n");
		return(NULL);
	}
	for (i = 0; i < pgunionTblDef->realNum; i++)
	{
		if (strcmp(tableName, (pgunionTblDefRec + i)->tableName) == 0)
			return(pgunionTblDefRec + i);
	}
	return(NULL);
}

// 插入一个记录
int UnionInsertTblDefRec(PUnionTblDefRec prec)
{
	int			ret;

	if (prec == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
		
	if ((ret = UnionConnectTblDef()) < 0)	
	{
		UnionUserErrLog("in UnionInsertTblDefRec:: UnionConnectTblDef!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}

	// 将变量拷贝到内存中
	if (pgunionTblDef->realNum >= pgunionTblDef->maxNum)
	{
		UnionUserErrLog("in UnionInsertTblDefRec:: rec table is full num = %d!\n", pgunionTblDef->maxNum);
		return(UnionSetUserDefinedErrorCode(errCodeRECMDL_VarTBLIsFull));
	}
	if (pgunionTblDef->realNum < 0)
	{
		UnionUserErrLog("in UnionInsertTblDefRec:: realNum = [%d]\n", pgunionTblDef->realNum);
		return(UnionSetUserDefinedErrorCode(errCodeRECMDL_RealNumError));
	}

	//memcpy(pgunionTblDef + sizeof(TunionTblDef) + (sizeof(TUnionTblDefRec) * pgunionTblDef->realNum), prec, sizeof(TUnionTblDefRec));
	memcpy(pgunionTblDefRec + pgunionTblDef->realNum, prec, sizeof(TUnionTblDefRec));
	pgunionTblDef->realNum += 1;

	return(0);
}

int UnionInsertTableDefIntoImage(PUnionObject pobject)
{
	int				len, i, offset, sltFldNum;
	TUnionTblDefRec			trec;
	char				prefixFldName[40+1];

	memset(&trec, 0, sizeof(trec));

	strncpy(trec.tableName, pobject->name, sizeof(trec.tableName) - 1);
	trec.fldNum = pobject->fldNum;
	trec.primaryKey.fldNum = pobject->primaryKey.fldNum;

	offset = 0;
	sltFldNum = 0;
	for(i = 0; i < pobject->fldNum; i++)
	{
		strcpy(trec.fldDefGrp[i].name, pobject->fldDefGrp[i].name);
		trec.fldDefGrp[i].type = pobject->fldDefGrp[i].type;

		/***
		if(strcmp(trec.fldDefGrp[i].name, "inputTeller") == 0)
		{
			continue;
		}
		if(strcmp(trec.fldDefGrp[i].name, "inputDate") == 0)
		{
			continue;
		}
		if(strcmp(trec.fldDefGrp[i].name, "inputTime") == 0)
		{
			continue;
		}
		if(strcmp(trec.fldDefGrp[i].name, "updateTeller") == 0)
		{
			continue;
		}
		if(strcmp(trec.fldDefGrp[i].name, "updateDate") == 0)
		{
			continue;
		}
		if(strcmp(trec.fldDefGrp[i].name, "updateTime") == 0)
		{
			continue;
		}
		****/

		memset(prefixFldName, 0, sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(trec.fldDefGrp[i].name, prefixFldName);
		if(sltFldNum == 0)
		{
			len = sprintf(trec.dbSelectFieldList, "%s", prefixFldName);
			offset += len;
		}
		else
		{
			if((len + strlen(prefixFldName) + 1) > sizeof(trec.dbSelectFieldList) - 1)
			{
				break;
			}
			len = sprintf(trec.dbSelectFieldList + offset, ",%s", prefixFldName);
			offset += len;
		}
		sltFldNum++;
	}

	for(i = 0; i < pobject->primaryKey.fldNum; i++)
	{
		strcpy(trec.primaryKey.fldNameGrp[i], pobject->primaryKey.fldNameGrp[i]);
	}
	UnionProgramerLog("in UnionInsertTableDefIntoImage:: tableName = [%s], fldNum = %d, pfldNum = %d dbSelectFieldList = %s\n", trec.tableName, trec.fldNum, trec.primaryKey.fldNum, trec.dbSelectFieldList);

	return(UnionInsertTblDefRec(&trec));
}

char *UnionGetTableSelectFieldFromImage(char *tableName)
{
	PUnionTblDefRec			ptblDefRec = NULL;

	if((ptblDefRec = UnionReadTblDefRec(tableName)) == NULL)
	{
		UnionAuditLog("in UnionGetTableSelectFieldFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(NULL);
	}
	return(ptblDefRec->dbSelectFieldList);
}
int UnionReadTableDefFromImage(char *tableName, PUnionObject pobject)
{
	//int				ret;
	int				i;
	PUnionTblDefRec			ptblDefRec = NULL;

	if((ptblDefRec = UnionReadTblDefRec(tableName)) == NULL)
	{
		UnionAuditLog("in UnionReadTableDefFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(-1);
	}
	strncpy(pobject->name, ptblDefRec->tableName, sizeof(pobject->name) - 1);
	pobject->fldNum = ptblDefRec->fldNum;
	pobject->primaryKey.fldNum = ptblDefRec->primaryKey.fldNum;

	for(i = 0; i < pobject->fldNum; i++)
	{
		strcpy(pobject->fldDefGrp[i].name, ptblDefRec->fldDefGrp[i].name);
		pobject->fldDefGrp[i].type = ptblDefRec->fldDefGrp[i].type;
	}
	for(i = 0; i < pobject->primaryKey.fldNum; i++)
	{
		strcpy(pobject->primaryKey.fldNameGrp[i], ptblDefRec->primaryKey.fldNameGrp[i]);
	}
	return(0);
}

int UnionReadTablePrimaryKeyFromImage(char *tableName, char *primaryKeyList)
{
	int				i, ret, offset;
	PUnionTblDefRec			ptblDefRec = NULL;
	char				prefixFldName[40+1];

	if((ptblDefRec = UnionReadTblDefRec(tableName)) == NULL)
	{
		UnionAuditLog("in UnionReadTablePrimaryKeyFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(-1);
	}

	offset = 0;
	for(i = 0; i < ptblDefRec->primaryKey.fldNum; i++)
	{
		memset(prefixFldName, 0, sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(ptblDefRec->primaryKey.fldNameGrp[i], prefixFldName);
		if(i == 0)
		{
			ret = sprintf(primaryKeyList + offset, "%s", prefixFldName);
		}
		else
		{
			ret = sprintf(primaryKeyList + offset, ",%s", prefixFldName);
		}
		offset += ret;
	}
	
	return(i);
}

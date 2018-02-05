/*	
Author:	zhangyongding
Date:	2012-12-12
*/

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( defined __linux__ )
#include <zlib.h>
#endif

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionCommBetweenMDL.h"
#include "unionXMLPackage.h"
#include "unionHighCachedAPI.h"
#include "unionMsgBufGroup.h"

int 	gunionDBErrCode = 0;

int UnionGetDBErrCode()
{
	return(UnionSetUserDefinedErrorCode(gunionDBErrCode));
}

int UnionIsFatalDBErrCode(int dbErrCode)
{
	return(0);
}

int UnionConnectDB()
{
	return(0);
}

int UnionCloseDB()
{
	return(0);
}

/*
	currentPage和numOfPerPage大于0时，分页查询
*/
int UnionSelectRealDBRecord2(char *sql,int currentPage,int numOfPerPage,select_callback callback,void *pArg)
{
	int		ret;
	int		count = 0;
	//int		timeOfHighCached = 0;
	int		timeoutOfDB = 0;
	unsigned long	len = 0;
	unsigned char	tmpBuf[204800*5];
	unsigned char	buf[204800*5];
	//char		tableName[128+1];
	char		command[64];
	//char		memcKey[1024+1];
	char		isCompressFlag[32];
	char		totalNum[32];
	
	UnionProgramerLog("in UnionSelectRealDBRecord2:: sql[%s] currentPage[%d] numOfPerPage[%d]\n",sql,currentPage,numOfPerPage);

	// 访问高速缓存服务器
	/*
	if (UnionIsUseHighCached())
	{
		if ((ret = UnionGetTableNameAndFieldNameFromSQL(sql,tableName,NULL)) < 0)
		{
			UnionUserErrLog("in UnionSelectRealDBRecord2:: UnionGetTableNameAndFieldNameFromSQL ret = [%d] sql[%s]!\n",ret,sql);
			return(ret);
		}

		memset(memcKey,0,sizeof(memcKey));		
		UnionGetKeyNameOfHighCached(tableName,sql,memcKey);
		
		if ((ret = UnionGetHighCachedValue(memcKey,(char *)buf,sizeof(buf))) <= 0)
		{
			UnionProgramerLog("in UnionSelectRealDBRecord2:: UnionGetHighCachedValue!\n");
		}
		else
		{
			buf[ret] = 0;
			if (strcmp((char *)buf,"totalNum=0") == 0)
				return(0);
			else
				goto initXMLPackage;
		}
	}
	*/
	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;
		
	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d%s;%d;%d",defLenOfCommand,conDBSvrCmdSelect,sql,currentPage,numOfPerPage);
	
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}
	tmpBuf[ret] = 0;

	if (ret < defLenOfCommand)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: ret[%d] < defLenOfCommand[%d]!\n",ret,defLenOfCommand);
		return(errCodeParameter);
	}
	
	memcpy(command,tmpBuf,defLenOfCommand);
	command[defLenOfCommand] = 0;
	
	memcpy(isCompressFlag,tmpBuf+defLenOfCommand,1);
	isCompressFlag[1] = 0;
	
	count = atoi(command);

	if (count < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: ret = [%d] sql[%s]!\n",count,sql);
		return(count);
	}
	else if (count == 0)
	{
		UnionLog("in UnionSelectRealDBRecord2:: count = [%d] sql[%s]!\n",count,sql);
		len = sprintf((char *)buf,"totalNum=0");
		goto setHighCached;
	}	

	if (isCompressFlag[0] == '1')	// 解压
	{
		len = sizeof(buf);
		if ((ret = uncompress(buf, &len, tmpBuf + defLenOfCommand + 1, ret - defLenOfCommand - 1)) != 0)
		{
			UnionUserErrLog("in UnionSelectRealDBRecord2:: uncompress ret = [%d] sql[%s]!\n",ret,sql);
			return(0 - abs(ret));
		}
	}
	else
	{
		len = ret - defLenOfCommand - 1;
		memcpy(buf,tmpBuf + defLenOfCommand + 1,len);
	}
	buf[len] = 0;

setHighCached:
	// 访问高速缓存服务器
	/*
	if (UnionIsUseHighCached())
	{
		timeOfHighCached = 0;
		if ((ret = UnionSetHighCachedValue(memcKey,(char *)buf,len,timeOfHighCached)) < 0)
		{
			UnionProgramerLog("in UnionSelectRealDBRecord2:: UnionSetHighCachedValue!\n");
		}
	}
	*/
	
	if (count == 0)
		return(0);
	
//initXMLPackage:
	if ((ret = UnionInitXMLPackage(NULL,(char *)buf,len)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: UnionInitXMLPackage[%s]!\n",buf);
		return(ret);
	}
	if ((ret = UnionReadXMLPackageValue("totalNum",totalNum,sizeof(totalNum))) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord2:: UnionReadXMLPackageValue[totalNum]!\n");
		return(ret);
	}
	return(atoi(totalNum));
}

/*
	currentPage和numOfPerPage大于0时，分页查询
*/
int UnionSelectRealDBRecord(char *sql,int currentPage,int numOfPerPage)
{
	int		ret = 0;
	long		beginTime = 0, endTime = 0;
	
	//return(UnionSelectRealDBRecord2(sql,currentPage,numOfPerPage,NULL,NULL));
	beginTime = UnionGetCurrentTimeInMicroSeconds();
	UnionLog("in UnionSelectRealDBRecord:: BEGIN \n");

	ret = UnionSelectRealDBRecord2(sql,currentPage,numOfPerPage,NULL,NULL);

	endTime = UnionGetCurrentTimeInMicroSeconds();
	UnionLog("in UnionSelectRealDBRecord:: END ret = %d use microSecond = [%ld]\n", ret, endTime - beginTime);

	if (ret == -29039)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: exchange with dbSvr timeout!!\n");
	}

	return(ret);
	
}

int UnionExecRealDBSql2(int syncFlag,char *sql)
{
	//int		i;
	int		ret;
	int		cnt = 0;
	int		timeoutOfDB = 0;
	//int		methodOfCached = 0;
	//char		tableName[128+1];
	//char		memcKey[1024+1];
	//int		tableNum = 0;
	//int		maxTableNum = 8;
	//char		tableGrp[maxTableNum][128+1];
	unsigned int	len;
	unsigned char	tmpBuf[81920];	

	UnionProgramerLog("in UnionExecRealDBSql2:: sql[%s]\n",sql);

	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;

	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d%s",defLenOfCommand,conDBSvrCmdExecSql,sql);
	
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionExecRealDBSql2:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}
	tmpBuf[ret] = 0;

	if (ret < defLenOfCommand)
		return(errCodeParameter);
	
	cnt = atoi((char *)tmpBuf);
	/*
	if (cnt > 0)
	{
		if ((ret = UnionGetTableNameAndFieldNameFromSQL(sql,tableName,NULL)) < 0)
		{
			UnionUserErrLog("in UnionExecRealDBSql2:: UnionGetTableNameAndFieldNameFromSQL ret = [%d] sql[%s]!\n",ret,sql);
			//return(ret);
		}
	
		// 拼分表定义串
		if ((tableNum = UnionSeprateVarStrIntoVarGrp(tableName,strlen(tableName),',',tableGrp,maxTableNum)) < 0)
		{
			UnionUserErrLog("in UnionExecRealDBSql2:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableName);
			return(tableNum);
		}

		for (i = 0; i < tableNum; i ++)
		{
			if ((methodOfCached = UnionReadMethodOfCachedOfTableName(tableGrp[i])) == 0)	// 缓存方法：标准
			{
				memset(memcKey,0,sizeof(memcKey));		
				UnionGetKeyNameOfHighCached(tableGrp[i],NULL,memcKey);
		
				if ((ret = UnionDeleteHighCachedKeysByAlikeName(memcKey)) < 0)
				{
					UnionUserErrLog("in UnionExecRealDBSql2:: UnionDeleteHighCachedKeysByAlikeName ret = [%d] flush all!\n",ret);
				}
			}
		}
	}
	*/
	return(cnt);
}

int UnionExecRealDBSql(char *sql)
{
	long		beginTime = 0, endTime = 0;
	int		ret = 0;
	//return(UnionExecRealDBSql2(1,sql));
	beginTime = UnionGetCurrentTimeInMicroSeconds();
	UnionLog("in UnionExecRealDBSql:: BEGIN \n");

	ret = UnionExecRealDBSql2(1,sql);

	endTime = UnionGetCurrentTimeInMicroSeconds();
	UnionLog("in UnionExecRealDBSql:: END ret = %d use microSecond = [%ld]\n", ret, endTime - beginTime);

	if (ret == -29039)
	{
		UnionUserErrLog("in UnionExecRealDBSql:: exchange with dbSvr timeout!!\n");
	}

	return(ret);
}

int UnionSelectRealDBRecordCounts(char *tableName,char *joinCondition,char *condition)
{
	int		ret;
	int		timeoutOfDB = 0;
	unsigned int	len;
	unsigned char	tmpBuf[8192];
	
	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d%s",defLenOfCommand,conDBSvrCmdSelectCounts,tableName);

	if ((joinCondition != NULL) && (strlen(joinCondition) > 0))
		len += snprintf((char *)tmpBuf + len,sizeof(tmpBuf) - len,";%s",joinCondition);
	else
		len += snprintf((char *)tmpBuf + len,sizeof(tmpBuf) - len,";");

	if ((condition != NULL) && (strlen(condition) > 0))
		len += snprintf((char *)tmpBuf + len,sizeof(tmpBuf) - len,";%s",condition);
	else
		len += snprintf((char *)tmpBuf + len,sizeof(tmpBuf) - len,";");

	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}

	if (ret < defLenOfCommand)
		return(errCodeParameter);
		
	tmpBuf[ret] = 0;
	
	//UnionProgramerLog("in UnionSelectRealDBRecord2Counts:: tmpBuf=[%s] ret = [%d]\n",tmpBuf,ret);
	return(atoi((char *)tmpBuf));
}

// 根据序列读取自增长序号
int UnionGetAddSelfNumBySequence(char *sequenceName)
{
	int		ret;
	int		timeoutOfDB = 0;
	unsigned int	len;
	unsigned char	tmpBuf[128];
	
	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;

	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d%s",defLenOfCommand,conDBSvrCmdGetSequence,sequenceName);

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionGetAddSelfNumBySequence:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}

	if (ret < defLenOfCommand)
		return(errCodeParameter);
	
	tmpBuf[ret] = 0;

	return(atoi((char *)tmpBuf));
}

int UnionSelectRealDBTree(int returnSql,char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue,char *buf,int sizeOfBuf)
{
	int		ret;
	int		timeoutOfDB = 0;
	unsigned int	len;
	unsigned char	tmpBuf[204800];//edit by yang 20150529
	unsigned char	lenBuf[128];
	
	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;
	
	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d;%d;%s;%s;%s;%d;%s;%d",defLenOfCommand,conDBSvrCmdSelectTree,returnSql,tableName,fieldName,parentFieldName,fieldType,rootValue,sizeOfBuf);

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBTree:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}

	if (ret < defLenOfCommand)
		return(errCodeParameter);

	memcpy(lenBuf,tmpBuf,defLenOfCommand);
	lenBuf[defLenOfCommand] = 0;

	memcpy(buf,tmpBuf+defLenOfCommand,ret - defLenOfCommand);
	buf[ret - defLenOfCommand] = 0;
	return(atoi((char *)lenBuf));
}

int UnionDeleteRealDBTree(char *tableName,char *fieldName,char *parentFieldName,int fieldType,char *rootValue)
{
	int		ret;
	int		timeoutOfDB = 0;
	unsigned int	len;
	unsigned char	tmpBuf[128];
	
	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) <= 0)
		timeoutOfDB = 10;
	
	len = snprintf((char *)tmpBuf,sizeof(tmpBuf),"%0*d%s;%s;%s;%d;%s",defLenOfCommand,conDBSvrCmdDeleteTree,tableName,fieldName,parentFieldName,fieldType,rootValue);

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,tmpBuf,len,tmpBuf,sizeof(tmpBuf) - 1,timeoutOfDB)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRealDBTree:: UnionExchangeWithSpecModuleVerifyReqAndRes timeout = [%d]!\n",timeoutOfDB);
		return(ret);
	}

	if (ret < defLenOfCommand)
		return(errCodeParameter);

	tmpBuf[ret] = 0;

	return(atoi((char *)tmpBuf));
}

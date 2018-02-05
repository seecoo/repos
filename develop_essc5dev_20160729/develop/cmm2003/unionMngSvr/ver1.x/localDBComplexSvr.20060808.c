// 2008/7/26
// Wolfang Wang

#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "operationControl.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// 使用3.x版本
#include "unionWorkingDir.h"
#include "mngSvrCommProtocol.h"
#include "unionMngSvrOperationType.h"
#include "mngSvrFileSvr.h"
#include "unionResID.h"
#include "unionREC.h"
#include "mngSvrTeller.h"

#ifdef _WIN32
#include "unionTBLQueryInterface.h"

#include "unionDBConf.h"
#endif

#include "unionComplexDBCommon.h"
#include "unionComplexDBObject.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectDef.h"

#include "unionDatabaseCmd.h"
//#include "allTableInfo.h"
#include "unionComplexDBUniqueKey.h"
#include "simuMngSvrLocally.h"
#include "unionComplexDBTrigger.h"
#include "unionComplexDBUnionQuery.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBRecord.h"
#include "UnionLog.h"

#ifndef _WIN32
#define conMenuDefinedQueryConditionFldSeperator      ','     	// 菜单定义的查询条件中的分隔符
#define conMenuDefinedQueryConditionReadThisFld       "this."   // 定义读取当前记录的该域
#endif

int     gunionIsFirstJudgeMngSvrLocally = 1;
int     gunionIsReadMngSvrDataLocally = 0;
char    gunionLocalDatabaseName[] = "object/objRec";


/*** del by xusj 20100607 begin ***
// 2010-3-20 wolfgang added
int UnionReadFldFromCurrentMngSvrClientReqStr(char *fldName,char *buf,int sizeOfBuf)
{
	return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,buf,sizeOfBuf));
}

// 2010-3-20 wolfgang added
int UnionReadIntTypeFldFromCurrentMngSvrClientReqStr(char *fldName,int *intValue)
{
	return(UnionReadIntTypeRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,intValue));
}
*** del by xusj 20100607 end ***/



int UnionExcuteUserSpecResSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

int UnionExcuteSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr);

int UnionIsUserSpecOperation(char *resName, int resID,int serviceID)
{
	char	varName[100];
	char	resName2[100];
	char	*ptr;

	// if (conDatabaseCmdDelete != serviceID)
	{
		memset(resName2, 0, sizeof(resName2));
		UnionReadResNameOfSpecResID(resID, resName2);

		if ((strcmp(resName, "") != 0) && (strcmp(resName, resName2) != 0))
			return 0;
	}

	sprintf(varName,"isSpecService%03d.%03d",resID,serviceID);
	// 2009/3/19,王纯军修改
	/*
	if (UnionReadIntTypeRECVar(varName) > 0)
		return(1);
	else
		return(0);
	*/
	//memset(gunionRemoteMngSvrName,0,sizeof(gunionRemoteMngSvrName));
	if (UnionReadIntTypeRECVar(varName) > 0)
	{
		return(1);
	}
	if ((ptr = UnionReadStringTypeRECVar(varName)) == NULL)
	{
			return(0);
	}
	else
	{
		UnionSetRemoteMngSvrName(ptr);
		//strcpy(gunionRemoteMngSvrName,ptr);
		return(1);
	}
	// 王纯军修改结束
}

// 判断是否是本地仿真mngSvr
int UnionIsReadMngSvrDataLocally()
{
        int     ret;

        if (!gunionIsFirstJudgeMngSvrLocally)
                return(gunionIsReadMngSvrDataLocally);
        /*
#ifdef _useLocalDatabase_
        gunionIsReadMngSvrDataLocally = 1;
        if (!UnionExistsObjectDef(gunionLocalDatabaseName))
        {
                if ((ret = UnionCreateObject(gunionLocalDatabaseName)) < 0)
                {
                        UnionUserErrLog("in UnionIsReadMngSvrDataLocally:: UnionCreateDatabase [%s]\n",gunionLocalDatabaseName);
                }
        }

#else
        gunionIsReadMngSvrDataLocally = 0;
#endif
        gunionIsFirstJudgeMngSvrLocally = 0;
        */
        if (UnionExistsDir(gunionLocalDatabaseName))
                gunionIsReadMngSvrDataLocally = 1;
        else
                gunionIsReadMngSvrDataLocally = 0;
        return(gunionIsReadMngSvrDataLocally);
}

// 读取资源标识对应的资源名称
int UnionReadResNameOfSpecResID(int resID,char *resName)
{
#ifndef _useTableDefList_
        char    tmpBuf[2048+1];
        char    fileName[512+1];
        FILE    *fp;
        int     len,ret;
        char    var[2048+1];
        char    workDir[256];
	char	*ptr;

        memset(workDir,0,sizeof(workDir));

#ifdef _WIN32
        UnionGetMainWorkingDir(workDir);
        // Mod By HuangBaoxin 2008/11/12
        sprintf(fileName,"%s\\resID.conf",workDir);
        // End Mod
        UnionLog("in UnionReadResNameOfSpecResID:: WIN fopen [%s]\n",fileName);
#else
	sprintf(fileName, "%s/confFileResID.conf", getenv("UNIONETC"));
       	if (!UnionExistsFile(fileName))
        {
 	      	sprintf(fileName, "%s/resID.conf", getenv("UNIONETC"));
 	      	if (!UnionExistsFile(fileName))
 	      		sprintf(fileName,"%s/objRec/resID.conf",getenv("UNIONOBJECTDIR"));
	}
        UnionProgramerLog("in UnionReadResNameOfSpecResID:: UNIX fopen [%s]\n",fileName);
#endif

        if ((fp = fopen(fileName,"r")) == NULL)
        {
                UnionSystemErrLog("in UnionReadResNameOfSpecResID:: fopen [%s] workingDir = [%s]\n",fileName,workDir);
                return(errCodeUseOSErrCode);
        }
        while (!feof(fp))
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
                if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) < 0)
                        continue;
                if (len == 0)
                        continue;
                if (UnionIsUnixShellRemarkLine(tmpBuf))
                        continue;
                memset(var,0,sizeof(var));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"resID",var,sizeof(var))) < 0)
                        continue;
                if (atoi(var) != resID)
                        continue;
                memset(var,0,sizeof(var));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"tableName",var,sizeof(var))) < 0)
                {
                        fclose(fp);
                        return(ret);
                }
                strcpy(resName,var);
                fclose(fp);
                return(strlen(resName));
        }
        UnionUserErrLog("in UnionReadResNameOfSpecResID:: resID [%d] not defined in file [%s]\n",resID,fileName);
        fclose(fp);
        return(errCodeEsscMDL_InvalidResID);
#else
	char	recStr[8192*10+1];
	int	lenOfRecStr;
	int	ret;
	char	uniqueKey[128+1];
	char	tableName[128+1];

	memset(recStr,0,sizeof(recStr));
	sprintf(uniqueKey,"resID=%d|",resID);
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByUniqueKey("tableList",uniqueKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadResNameOfSpecResID:: UnionSelectUniqueObjectRecordByUniqueKey from tableList resID [%d]\n",resID);
		return(lenOfRecStr);
	}
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionReadResNameOfSpecResID:: UnionReadRecFldFromRecStr tableName from [%s] of resID [%d]\n",recStr,resID);
		return(ret);
	}
	strcpy(resName,tableName);
	return(0);
#endif
}

// 数据库表操作
// serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionOperationOfDBTable(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	resName[100];
	char	*recvFileName;
        int     ret;
        char    primaryKey[512+1];
        int     recNum;
        /*
	if (serviceID == conDatabaseCmdQueryAllRec)     // 查询所有记录，对于数据库，是查询所有表
	{
		if ((ret = UnionPrintAllTableNameToFile(UnionGenerateMngSvrTempFile())) < 0)
		{
			UnionUserErrLog("in UnionOperationOfDBTable:: UnionPrintAllTableNameToFile!\n");
			return(ret);
		}
		*fileRecved = 1;
		return(0);
	} */

	memset(resName,0,sizeof(resName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"tableName",resName,sizeof(resName))) < 0)
	{
		UnionUserErrLog("in UnionOperationOfDBTable:: UnionReadRecFldFromRecStr [%s]\n","tableName");
		return(ret);
	}
	if (serviceID == conDatabaseCmdInsert)      // 创建表
	{
		memset(primaryKey,0,sizeof(primaryKey));
		if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"primaryKey",primaryKey,sizeof(primaryKey))) < 0)
		{
                	UnionUserErrLog("in UnionOperationOfDBTable:: UnionCreateTable [%s]\n",resName);
			return(ret);
		}
		if ((ret = UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(primaryKey,strlen(primaryKey),conMenuDefinedQueryConditionFldSeperator,primaryKey,sizeof(primaryKey))) < 0)
		{
			UnionUserErrLog("in UnionOperationOfDBTable:: UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator!\n");
			return(ret);
		}
		if ((ret = UnionCreateObject(resName)) < 0)
		{
			UnionUserErrLog("in UnionOperationOfDBTable:: UnionCreateObject [%s]\n",resName);
			return(ret);
		}
		else
			UnionLog("in UnionOperationOfDBTable:: UnionCreateTable [%s] [%s] OK!\n",resName,primaryKey);
		return(0);
	}
	if (serviceID == conDatabaseCmdDelete)   // 删除表
	{
		if ((ret = UnionDropObject(resName)) < 0)
		{
			UnionUserErrLog("in UnionOperationOfDBTable:: UnionDropObject [%s]\n",resName);
			return(ret);
		}
		else
			UnionLog("in UnionOperationOfDBTable:: UnionDropObject [%s] OK!\n",resName);
		return(0);
	}
	if (serviceID == conDatabaseCmdQuerySpecRec)   // 查询表
	{
                TUnionObject obj;
                if ((recNum = UnionGetObjectRecordNumber(resName)) < 0)
                {
			UnionUserErrLog("in UnionOperationOfDBTable:: UnionGetObjectRecordNumber [%s]\n",resName);
			return(recNum);
		}
                memset(&obj,0,sizeof(obj));
                if ((ret = UnionReadObjectDef(resName,&obj)) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfDBTable:: UnionReadObjectDef [%s]\n",resName);
			return(ret);
                }
                if ((ret = UnionFormFieldNameStrFromFldGrp(&obj.primaryKey, primaryKey)) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfDBTable:: UnionFormFieldNameStrFromFldGrp [%s]\n",resName);
			return(ret);
                }

                UnionConvertOneFldSeperatorInRecStrIntoAnother(primaryKey,strlen(primaryKey),'|',',',primaryKey,sizeof(primaryKey));
		sprintf(resStr,"tableName=%s|recNum=%d|primaryKey=%s",resName,recNum,primaryKey);
		return(strlen(resStr));
	}
	UnionUserErrLog("in UnionOperationOfDBTable:: 非法的服务代码[%d]!\n",serviceID);
	return(errCodeEsscMDL_InvalidService);
}

// 修改操作员登陆状态
int UnionUpdateOperationLogOnStatus(char *tellerNo,char *resName,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int logOnStatus)
{
	char    recStr[2048+1];
        int     lenOfRecStr;
        char    condition[512];
	int	offset = 0;
	int	ret;

	char    allowModifyFld[2048 + 1];
	char	logBuf[2048+1];
	int	logStatus;
	char	*pos, *pos2;

	int	isOnEnv;

	isOnEnv	= UnionIsReadComplexDBOnUnionEnv();

	memset(condition, 0, sizeof(condition));

	if (1 == isOnEnv)
	{
		if ((ret = UnionPutRecFldIntoRecStr("fld1",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
              	{
                	UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
                  	return(ret);
            	}
	}
	else
	{
		if ((ret = UnionPutRecFldIntoRecStr("idOfOperator",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
        	{
             		UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
                	return(ret);
        	}
	}

	memset(recStr, 0, sizeof(recStr));
	// 读出记录
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(resName,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: UnionSelectRecord from [%s] errCode = [%d]\n",resName,ret);
		return(lenOfRecStr);
	}

	memset(allowModifyFld, 0, sizeof(allowModifyFld));
	ret = UnionGetAllowModifyFldFromObjectRecord(resName, recStr, allowModifyFld);

	// UnionUserErrLog("in UnionUpdateOperationLogOnStatus, \nrecStr: [%s]\nallowModifyFld:[%s].\n", recStr, allowModifyFld);
	// 修改登陆状态
	if (1 == isOnEnv)
	{
      		if ((pos = strstr(allowModifyFld, "fld5")) == NULL)
     		{
			UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: cann't find fld5 from [%s].\n",allowModifyFld);
			return errCodeEsscMDL_InvalidService;
		}
	}
	else
	{
		if ((pos = strstr(allowModifyFld, "isCurrentLogon")) == NULL)
                {
			UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: cann't find isCurrentLogon from [%s].\n",allowModifyFld);
                	return errCodeEsscMDL_InvalidService;
                }
	}

	ret		= pos - allowModifyFld;
	memcpy(logBuf, allowModifyFld, ret);

	if (1 == isOnEnv)
		ret	+= sprintf(logBuf + ret, "fld5=%d|", logOnStatus);
	else
		ret	+= sprintf(logBuf + ret, "isCurrentLogon=%d|", logOnStatus);

	if ((ret = UnionUpdateUniqueObjectRecord(resName,condition,logBuf,ret)) < 0)
	{
		UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: UnionUpdateUniqueObjectRecord from [%s] errCode = [%d]\n",resName,ret);
		return(ret);
	}

       	return(0);
}

// 操作员表操作
// serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
// 操作员表操作
// serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionOperationOfOperatorTable(char *tellerNo,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	resName[100];
	char	*recvFileName;
        int     ret;
        char    remotePassword[48+1];
        char    localPassword[48+1];
        char    recStr[2048+1];
        int     lenOfRecStr;
        char    condition[512];
        char    caValue[3072 + 1];
	int	offset = 0;
	int	logonMode;	//登陆方式
	int	logStatus;
	
	char	tellerNoStr[128 + 1];
	char	authLevelList[128+1];
	char	tellerLevel[8+1];
	char	*ptr;
	int 	verifyLogonMode=0;

	memset(resName,0,sizeof(resName));
        if ((ret = UnionReadResNameOfSpecResID(resID,resName)) < 0)
        {
                UnionUserErrLog("in UnionOperationOfOperatorTable:: invalid resID [%d]\n",resID);
                return(errCodeEsscMDL_InvalidResID);
        }

        if (serviceID == conDatabaseSpecCmdOfOperatorTBL_logoff)        // 退出登录
        {
        	// modify by chenliang, 2009-5-18 13:26:26
        	// return UnionUpdateOperationLogOnStatus(tellerNo, resName, serviceID, reqStr, lenOfReqStr, resStr, 0);
        	memset(tellerNoStr, 0, sizeof(tellerNoStr));
        	strcpy(tellerNoStr, tellerNo);
        		
        	return UnionUpdateOperationLogOnStatus(tellerNoStr, resName, serviceID, reqStr, lenOfReqStr, resStr, 0);
        	// end, 2009-5-18 13:26:51
        }

        if (serviceID != conDatabaseSpecCmdOfOperatorTBL_logon)
                return(errCodeEsscMDL_InvalidService);

        // 登录
        memset(condition,0,sizeof(condition));

        // add by chenliang, 2008-12-29
        // 支持 unionEnv
        if (UnionIsReadComplexDBOnUnionEnv() == 1)
        {
                // 读出记录
                if ((ret = UnionPutRecFldIntoRecStr("fld1",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
                        return(ret);
                }

                memset(recStr,0,sizeof(recStr));
                // 读出记录
                if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(resName,condition,recStr,sizeof(recStr))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionSelectRecord from [%s] errCode = [%d]\n",resName,ret);
                        return(lenOfRecStr);
                }

                // add by chenliang, 2009-02-03
                // 读取登陆状态
                if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr, lenOfRecStr, "fld5", &logStatus)) < 0)
                {
                	UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadIntTypeRecFldFromRecStr logStatus from [%s] errCode = [%d]\n",recStr,ret);
                        return(ret);
                }
                // 判断操作员是否已经登录
                if (1 == logStatus)	// 已经登陆,返回
                {
                	UnionUserErrLog("in UnionOperationOfOperatorTable:: teller [%s] already logon.\n", tellerNo);
                	return errCodeOperatorMDL_AlreadyLogon;	// 操作员已经登录了
                }
                /*
                else	// 未登陆,设置为登陆状态
                {
                	if ((ret = UnionUpdateOperationLogOnStatus(tellerNo, resName, serviceID, reqStr, lenOfReqStr, resStr, 1)) < 0)
                	{
                		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionUpdateOperationLogOnStatus error.\n");
                		return ret;
                	}
                }
                */
                // add end. 2009-02-03

                memset(localPassword,0,sizeof(localPassword));
                if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"fld3",localPassword,sizeof(localPassword))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr password from [%s] errCode = [%d]\n",recStr,ret);
                        return(ret);
                }
                if (strcmp(reqStr,localPassword) != 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: localPassword [%s] != remotePassword [%s]\n",localPassword,reqStr);
                        return(errCodeOperatorMDL_WrongPassword);
                }

                memset(caValue,0,sizeof(caValue));
                if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"fld2",caValue,sizeof(caValue))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr userName from [%s] errCode = [%d]\n",recStr,ret);
                        return(ret);
                }

                offset          = 0;
		if ((ret = UnionPutRecFldIntoRecStr("userName",caValue,strlen(caValue),resStr+offset,sizeOfResStr)) < 0)
		{
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionPutRecFldIntoRecStr userName from [%s] errCode = [%d]\n",caValue,ret);
                        return(ret);
		}

		offset += ret;
                memset(caValue,0,sizeof(caValue));
                if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"fld4",caValue,sizeof(caValue))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr level from [%s] errCode = [%d]\n",recStr,ret);
                        return(ret);
                }
		if ((ret = UnionPutRecFldIntoRecStr("level",caValue,strlen(caValue),resStr+offset,sizeOfResStr)) < 0)
		{
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionPutRecFldIntoRecStr level from [%s] errCode = [%d]\n",caValue,ret);
                        return(ret);
		}
		offset += ret;

		// 设置为登陆状态
		if ((ret = UnionUpdateOperationLogOnStatus(tellerNo, resName, serviceID, reqStr, lenOfReqStr, resStr, 1)) < 0)
                {
                	UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionUpdateOperationLogOnStatus error.\n");
                	return ret;
                }
                return(offset);
        }
        // add end.

  	if ((ret = UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"verifyLogonMode",&verifyLogonMode)) < 0)
	{
		UnionLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr verifyLogonMode from [%s] errCode = [%d]\n",recStr,ret);
	}
	
	if (1 == verifyLogonMode)
	{
		ret = UnionVerifyAuthLoginOperator(lenOfReqStr,reqStr,sizeOfResStr,resStr);
		return ret;
	}
	
	// 读出记录
        if ((ret = UnionPutRecFldIntoRecStr("idOfOperator",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
        {
                UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
                return(ret);
        }
        memset(recStr,0,sizeof(recStr));

        // 读出记录
        if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(resName,condition,recStr,sizeof(recStr))) < 0)
        {
                memset(condition,0,sizeof(condition));
                if ((ret = UnionPutRecFldIntoRecStr("userName",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
                        return(ret);
                }

                if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(resName,condition,recStr,sizeof(recStr))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionSelectRecord from [%s] errCode = [%d]\n",resName,ret);
                        return(lenOfRecStr);
                }
        }
	if (lenOfRecStr == 0) // 没有这个操作员
	{
		UnionUserErrLog("in UnionOperationOfOperatorTable:: this operator is not found!! tellerNo=[%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}


	// add by chenliang, 2009-02-03
	// 读取登陆状态
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr, lenOfRecStr, "isCurrentLogon", &logStatus)) < 0)
	{
     		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadIntTypeRecFldFromRecStr isCurrentLogon from [%s] errCode = [%d]\n",recStr,ret);
      		return(ret);
       	}
 	// 判断操作员是否已经登录
  	if (1 == logStatus)	// 已经登陆,返回
  	{
          	UnionUserErrLog("in UnionOperationOfOperatorTable:: teller [%s] already logon.\n", tellerNo);
		// add by xusj 20090819 验证操作员的空闲时间是否超限
		if ( (ret = UnionVerifyOperatorFreeTime(tellerNo)) < 0)
		{
			UnionUserErrLog("in UnionOperationOfOperatorTable:: [%s] is too long not operator logout!!\n",tellerNo);
			return (ret);
		}
		// add by xusj end
             	return errCodeOperatorMDL_AlreadyLogon;	// 操作员已经登录了
   	}

	// 验证操作员登陆 add by xusj 20090709
	if (!UnionIsNonAuthorizationMngSvrService(resID,serviceID))
	{
		ret = UnionVerifyOperatorLogin(tellerNo, lenOfReqStr, reqStr);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionVerifyOperatorLogin error.\n");
			// 登陆失败次数+1
			// 2011-11-15 张永定修改，密码过去，登录失败次数不+1
			if (ret != errCodeOperatorMDL_OperatorPwdOverTime)
				UnionSetOperatorPasswordInputWrongTimes(tellerNo);
			return ret;
		}
		// 重置登陆失败次数
		UnionReSetOperatorPasswordInputWrongTimes(tellerNo);
	}

	strcpy(resStr,recStr);

        // 设置为登陆状态
        if ((ret = UnionUpdateOperationLogOnStatus(tellerNo, resName, serviceID, reqStr, lenOfReqStr, resStr, 1)) < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionUpdateOperationLogOnStatus error.\n");
		return ret;
	}
	//UnionLog("in UnionOperationOfOperatorTable:: resStr = [%s] \n");

	memset(tellerLevel,0,sizeof tellerLevel);
	ret = UnionReadRecFldFromRecStr(resStr, strlen(resStr),"level",tellerLevel,sizeof tellerLevel);
	if(ret < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr[level][%s]\n",resStr);	
		return ret;
	}
	memset(authLevelList,0,sizeof(authLevelList));
	if ((ptr = UnionReadStringTypeRECVar("logonDoubleAuthLevelList")) != NULL)
	{
		//strcpy(authLevelList,ptr);
		strncpy(authLevelList,ptr, sizeof(authLevelList) - 1);
		if(NULL != strstr(authLevelList,tellerLevel))
		{
			//sprintf(resStr,"%s%s|",resStr,"isDoubleAuthLevel=1");
			strcat(resStr, "isDoubleAuthLevel=1");
		}
	}
	
	// 更新登陆次数和最近登陆时间
        if ((ret = UnionUpdateOperationLogTimes(tellerNo) < 0))
	{
		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionUpdateOperationLogTimes error.\n");
		return ret;
	}

	ret = sprintf(resStr, "%s%s=%s|",resStr,"LogonRemark", UnionReadLogonRemark());
        return(ret);
}

// 判断一个资源是否是保留资源
int UnionIsReservedRes(int resID)
{
#ifndef _useTableDefList_
	char	specResName[100];

	sprintf(specResName,"isSpecRes%03d",resID);
	UnionProgramerLog("in UnionIsReservedRes:: specResName = [%s] value = [%d]\n",specResName,UnionReadIntTypeRECVar(specResName));
	return(UnionReadIntTypeRECVar(specResName));

#else
	char	recStr[8192*10+1];
	int	lenOfRecStr;
	int	ret;
	char	uniqueKey[128+1];
	int	reservedRes = 0;

	memset(recStr,0,sizeof(recStr));
	sprintf(uniqueKey,"resID=%d|",resID);
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByUniqueKey("tableList",uniqueKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionIsReservedRes:: UnionSelectUniqueObjectRecordByUniqueKey from tableList resID [%d]\n",resID);
		return(lenOfRecStr);
	}
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"reservedRes",&reservedRes)) < 0)
	{
		UnionUserErrLog("in UnionIsReservedRes:: UnionReadIntTypeRecFldFromRecStr tableName from [%s] of resID [%d]\n",recStr,resID);
		return(ret);
	}
	return(reservedRes);
#endif
}

// 与MngSvr通讯
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionReadMngSvrDataLocally(int handle,char *tellerNo,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	resName[100];
	char	*recvFileName;
        int     ret;
        char    primaryKeyVal[512+1];
        char    conditionStr[512+1];
        int     lenOfConditionStr;
        char    valueStr[2048+1];
        int     lenOfValueStr;
	//char	specResName[128+1];
        char    allowModifyFld[2048 + 1];
	int	ret1;
	int	operationType;
	int	offset = 0;

        if (lenOfReqStr >= 0)
                reqStr[lenOfReqStr] = 0;
        
	*fileRecved = 0;

	UnionSetCurrentResID(resID);
	UnionSetCurrentServiceID(serviceID);
	UnionSetCurrentTellerNo(tellerNo);

#ifdef _runningAsBackSvr_	// 2009/5/29,王纯军增加该编译开关
	// 启动成后端进程，不需要对验证命令合法和操作员是否登录
	//gunionCurrentOperationType = operationType = conMngSvrOperationTypeDbRecCmd;
	UnionSetCurrentOperationType(operationType = conMngSvrOperationTypeDbRecCmd);
#else
	// add by xusj begin 20091216 判断操作员是否被锁
	if (UnionIsOperatorLocked(tellerNo))
	{
		UnionUserErrLog("in UnionReadMngSvrDataLocally:: operator [%s] is locked!\n",tellerNo);
		return(errCodeOperatorMDL_OperatorIsLocked);
	}
	// add by xusj end 20091216 判断操作员是否被锁

        if (resID == 0) // 数据库表操作
                return(UnionOperationOfDBTable(serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	if (resID == conSpecTBLIDOfOperator)  // 操作员表
        {
                if ((ret = UnionOperationOfOperatorTable(tellerNo,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) != errCodeEsscMDL_InvalidService)
                        return(ret);
                if ((ret = UnionOperationOfOperatorGetAuthority(tellerNo,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) != errCodeEsscMDL_InvalidService)
                	return(ret);
        }
        // 判断操作是否合法
        UnionProgramerLog("in UnionReadMngSvrDataLocally:: tellerLevel = [%c]\n",UnionGetTellerLevel());
        if (!UnionIsNonAuthorizationMngSvrService(resID,serviceID))	// 判断是否要授权的命令
        {
        	if ((operationType = UnionVerifyOperation(resID,serviceID,UnionGetTellerLevel())) < 0)
        	{
                	UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionVerifyOperation resID [%d] serviceID [%d] level [%c]\n",resID,serviceID,UnionGetTellerLevel());
                	return(operationType);
        	}
        }
        else	// 2009/11/24,wolfgang wang add
        	operationType = UnionGetMngSvrOperationType(resID,serviceID);	// 2009/11/24,wolfgang wang add
        UnionLog("in UnionReadMngSvrDataLocally:: operationType = [%d]\n",operationType);
        UnionSetCurrentOperationType(operationType);
        

	//UnionLog("*** verify logon time...\n");	
	// add by xusj 20090820 判断是否超过最大空闲时间
	if (resID != conSpecTBLIDOfOperator)  // 非操作员表,2012-7-23 修改
	{
		if ( (ret = UnionVerifyOperatorFreeTime(tellerNo)) < 0)
		{
			UnionUserErrLog("in UnionReadMngSvrDataLocally:: teller is timeout!! [%s]\n",tellerNo);
                	return(ret);
                }
	}
	// add by xusj end

	// add by xusj 20110525 判断授权操作员
	if ( (ret = UnionVerifyAuthOperator(lenOfReqStr,reqStr)) < 0)
	{
                UnionUserErrLog("in UnionReadMngSvrDataLocally:: verify authTeller failuer!\n");
                return(ret);
	}
	// add by xusj end

	// added by 2012-06-27 用于前台授权的空操作
	if(ret == 1 && resID == 7 && serviceID == 108)
	{
		return(0);
	}
	// end of addication 2012-06-27

	//UnionLog("*** verify file server service...\n");	
	// 判断是否是文件处理命令	// 2008/11/10增加
	ret = UnionExcuteMngSvrFileSvrService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved);
	if (ret >= 0)
		return(ret);
	if (ret != errCodeFileSvrMDL_InvalidCmd)
        {
                UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteMngSvrFileSvrService ret = [%d]\n",ret);
                return(ret);
        }
	// 2008/11/10　增加结束
	
	//UnionLog("*** verify reserved service...\n");	
	// 2009/1/6,增加，王纯军，特殊管理的资源
	if ((UnionIsReservedRes(resID) > 0) || (operationType % 100 == conMngSvrOperationTypeReservedCmd))
	{
		UnionProgramerLog("in UnionReadMngSvrDataLocally:: %03d is spec resoure!\n",resID);
		if ((ret = UnionExcuteDBTableDesignService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
		{
			if (ret != errCodeEsscMDL_InvalidService)
			{
				UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteDBTableDesignService!\n");
				return(ret);
			}
		}
		else
			return(ret);
	}
	// 2009/1/6,增加结束

	// 2009/1/6,增加，王纯军，内存映像命令
	//UnionLog("*** verify shared memory service...\n");	
	if ((ret = UnionExcuteMngSvrSharedMemoryImageService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) >= 0)
		return(ret);
	if ((ret != errCodeEsscMDL_InvalidService) && (ret != errCodeEsscMDL_InvalidResID))
	{
		UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteMngSvrSharedMemoryImageService!\n");
		return(ret);
	}
	//  2009/1/6,增加结束，王纯军
	
	//UnionLog("*** verify type of service...\n");	
	// 2009/5/12，王纯军，增加
	switch (operationType % 100)
	{
		case	conMngSvrOperationTypeOtherSvrCmd:	// 其它服务器的命令
			if ((ret = UnionExcuteOtherSvrService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteOtherSvrService!\n");
				return(ret);
			}
			return(ret);
		case	conMngSvrOperationTypeAppSpecCmd:	// 客户自定义命令
			if ((ret = UnionExcuteUserSpecResSvrService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteUserSpecResSvrService!\n");
				return(ret);
			}
			//resStr[ret] = 0;
			//UnionLog("in UnionReadMngSvrDataLocally:: [%04d][%s]\n",ret,resStr);
			return(ret);
		default:
			break;
	}
	// 2009/5/12，王纯军，增加结束

#endif	// 2009/5/29,王纯军增加该编译开关

 	//UnionLog("*** tranditional service...\n");	
       // 读资源标识对应的表名
        memset(resName,0,sizeof(resName));
        if (resID != conResIDDynamicTBL)	// 静态表
        {
        	if ((ret = UnionReadResNameOfSpecResID(resID,resName)) < 0)
        	{
                	UnionUserErrLog("in UnionReadMngSvrDataLocally:: invalid resID [%d]\n",resID);
                	return(errCodeEsscMDL_InvalidResID);
        	}
        	offset = 0;
        }
        else
        {
        	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"resName",resName,sizeof(resName))) < 0)
        	{
        		UnionUserErrLog("in UnionReadMngSvrDataLocally:: no resName set in [%s]\n",reqStr);
        		return(ret);
        	}

        	if (conDatabaseCmdDelete == serviceID)
        	{
        		memset(resName, 0, sizeof(resName));
        	}

        	offset = ret + strlen("resName=") + 1;
	}
	UnionProgramerLog("in UnionReadMngSvrDataLocally:: resName = [%s]\n",resName);
	
	UnionSetCurrentResName(resName);	// 2009/11/9,王纯军增加
	
	//UnionLog("*** database service...\n");	
        if ((ret = UnionExcuteDBSvrOperation(resName,serviceID,reqStr+offset,lenOfReqStr-offset,resStr,sizeOfResStr,fileRecved)) < 0)
        {
		UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteDBSvrOperation [%s]\n",resName);
		return(ret);
	}
	// 执行后续操作
	if ((ret1 = UnionExcuteSuffixOperation(resID,serviceID,reqStr+offset,lenOfReqStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteSuffixOperation into [%d] [%s] errCode = [%d]\n",resID,resName,ret);
		//return(ret);
	}
	return(ret);
}

// 与MngSvr通讯
// resName,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionExcuteDBSvrOperation(char *resName,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char	*recvFileName;
        int     ret;
        int	ret2;
        char    primaryKeyVal[512+1];
        char    conditionStr[512+1];
        int     lenOfConditionStr;
        char    valueStr[2048+1];
        int     lenOfValueStr;

	char	reqStrBuf[4096];
        //char    allowModifyFld[2048 + 1];
        char    allowModifyFld[4096];
        char	*pos = NULL;

        if (lenOfReqStr >= 0)
                reqStr[lenOfReqStr] = 0;
        //UnionNullLog("resName::%s serviceID::%04d lenOfReqStr=[%04d][%s]\n",resName,serviceID,lenOfReqStr,reqStr);
	*fileRecved = 0;

	memset(reqStrBuf, 0, sizeof(reqStrBuf));
	memcpy(reqStrBuf, reqStr, lenOfReqStr);

	// 执行动作前的触发器
	if ((ret = UnionExcuteTriggerOperationOnTable(resName,serviceID,1,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteTriggerOperationOnTable into [%s] errCode = [%d]\n",resName,ret);
		return(ret);
	}

#ifndef _noUseUserSpecOperation_
	if (UnionIsUserSpecOperation(resName, UnionGetCurrentResID(),serviceID))
	{
		if ((ret = UnionExcuteAppSpecService(UnionGetCurrentResID(),serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteAppSpecService!\n");
			return(ret);
		}

		return ret;
	}
#endif

	if (serviceID == conDatabaseCmdQueryAllRec)  // 查询所有记录
        {
                // UnionLog("In UnionExcuteDBSvrOperation, 查询所有记录, and the reqStr:[%s], resName: [%s].\n", reqStr, resName);
                /*
                if ((ret = UnionBatchSelectObjectRecord(resName,reqStr,UnionGenerateMngSvrTempFile())) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionBatchSelectObjectRecord [%s]\n",resName);
                        return(ret);
                }
                */

		//UnionLog("in UnionExcuteDBSvrOperation:: call UnionBatchSelectUnionObjectRecord...\n");
                if ((ret = UnionBatchSelectUnionObjectRecord(resName,reqStr,UnionGenerateMngSvrTempFile())) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionBatchSelectUnionObjectRecord [%s]\n",resName);
                        return(ret);
                }
                *fileRecved = 1;
                return(0);
        }
	else if (serviceID == conDatabaseCmdQueryAllRecWithUnion)  // 联合查询所有记录
        {
                if ((ret = UnionBatchSelectUnionObjectRecord(resName,reqStr,UnionGenerateMngSvrTempFile())) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionBatchSelectUnionObjectRecord [%s]\n",resName);
                        return(ret);
                }
                *fileRecved = 1;
                return(0);
        }
        else if (serviceID == conDatabaseCmdInsert)  // 插入记录
        {
        	if ((lenOfReqStr = UnionAutoAppendDBRecInputAttr(reqStrBuf,lenOfReqStr,sizeof(reqStrBuf))) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionAutoAppendDBRecInputAttr into [%s] errCode = [%d]\n",resName,ret);
                        return(lenOfReqStr);
                }
        	if ((ret = UnionInsertObjectRecord(resName,reqStrBuf,lenOfReqStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionInsertObjectRecord into [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
        }
        else if (serviceID == conDatabaseCmdQuerySpecRec)  // 查询记录
        {
        	/*
                if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey(resName,reqStr,resStr,sizeOfResStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionSelectUniqueObjectRecordByPrimaryKey from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
                ret = strlen(resStr);
                */
                if ((ret = UnionExcuteUnionSelectSpecRecOnObject(resName,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteUnionSelectSpecRecOnObject from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
                return(ret);
        }
        else if (serviceID == conDatabaseCmdQuerySpecRecWithUnion)  // 联合查询指定记录
        {
                if ((ret = UnionExcuteUnionSelectSpecRecOnObject(resName,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteUnionSelectSpecRecOnObject from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
                return(ret);
        }
        else if (serviceID == conDatabaseCmdDelete)  // 删除记录
        {
        	if ((ret = UnionDeleteUniqueObjectRecord(resName,reqStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionDeleteUniqueObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
        }
        else if (serviceID == conDatabaseCmdConditionDelete)  // 条件删除记录
        {
                if ((ret = UnionBatchDeleteObjectRecord(resName,reqStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionBatchDeleteObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
        }
        else if (serviceID == conDatabaseCmdUpdate)  // 修改记录
        {
                // if (!UnionIsUseRealDB())
                {
                        memset(primaryKeyVal,0,sizeof(primaryKeyVal));
                        if ((ret = UnionGetPrimaryKeyFldFromObjectRecord(resName,reqStr,primaryKeyVal)) < 0)
                        {
                                UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionGetPrimaryKeyFldFromObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                                return(ret);
                        }
		        // 2008-09-04 add by chenliang

                        memset(allowModifyFld, 0, sizeof(allowModifyFld));
                        if((ret = UnionGetAllowModifyFldFromObjectRecord(resName, reqStr, allowModifyFld)) < 0)
                        {
                                UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionGetAllowModifyFldFromObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                                return(ret);
                        }

                        // UnionUserErrLog("resName: [%s], reqStr: [%s].\n", resName, reqStr);
                        UnionUserErrLog("primaryKeyVal: [%s], allowModifyFld: [%s].\n", primaryKeyVal, allowModifyFld);

                        // 2008-09-04 modify by chenliang
                        // if ((ret = UnionUpdateUniqueObjectRecord(resName,primaryKeyVal,reqStr,lenOfReqStr)) < 0)
        		if ((ret = UnionAutoAppendDBRecUpdateAttr(allowModifyFld,strlen(allowModifyFld), sizeof(allowModifyFld))) < 0)
                	{
                        	UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionAutoAppendDBRecUpdateAttr into [%s] errCode = [%d]\n",resName,ret);
                        	return(ret);
                	}
                        if ((ret = UnionUpdateUniqueObjectRecord(resName,primaryKeyVal,allowModifyFld,ret)) < 0)
                        {
                                UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionUpdateUniqueObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                                return(ret);
                        }
                }
                /*
                else
                {
                        if ((ret = UnionUpdateUniqueObjectRecord(resName,reqStr,reqStr,lenOfReqStr)) < 0)
                        {
                                UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionUpdateUniqueObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                                return(ret);
                        }
                }   */
        }
        else if (serviceID == conDatabaseCmdConditionUpdate)  // 条件修改记录
        {
                memset(conditionStr,0,sizeof(conditionStr));
                if ((lenOfConditionStr = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"condition",conditionStr,sizeof(conditionStr))) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionReadRecFldFromRecStr condition from [%s] errCode = [%d]\n",reqStr,lenOfConditionStr);
                        return(lenOfConditionStr);
                }
                UnionConvertOneFldSeperatorInRecStrIntoAnother(conditionStr,lenOfConditionStr,',','|',conditionStr,sizeof(conditionStr));
                memset(valueStr,0,sizeof(valueStr));
                if ((lenOfValueStr = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"value",valueStr,sizeof(valueStr))) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionReadRecFldFromRecStr value from [%s] errCode = [%d]\n",reqStr,lenOfValueStr);
                        return(lenOfValueStr);
                }
                UnionConvertOneFldSeperatorInRecStrIntoAnother(valueStr,lenOfValueStr,',','|',valueStr,sizeof(valueStr));
                if ((ret = UnionBatchUpdateObjectRecord(resName,conditionStr,valueStr,lenOfValueStr)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionBatchUpdateObjectRecord from [%s] errCode = [%d]\n",resName,ret);
                        return(ret);
                }
        }
        else if(serviceID == conDatabaseCmdQueryByRealSQL) // 根据真实SQL查询 55
        {
        	if( (pos = strstr(reqStr, "realSQL::")) == NULL )
			{
				UnionUserErrLog("in UnionExcuteDBSvrOperation:: Null pointer [pos]!\n");
				return(errCodeParameter);
			}
        	if( (ret = UnionSelectObjectRecordByRealSQL(resName, pos, UnionGenerateMngSvrTempFile())) < 0 )
        	{
        		UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionSelectUnionObjectRecordByRealSQL from [%s] errCode = [%d]\n",resName,ret);
        		return(ret);
        	}
        	*fileRecved = 1;
        	return(0);
        }
        else if(serviceID == conDatabaseCmdQueryBySpecFieldList) // 查询指定清单对应的数据 56
        {
        	if( (ret = UnionSelectObjectRecordBySpecFieldListOnObject(resName, reqStr, lenOfReqStr, UnionGenerateMngSvrTempFile())) < 0 )
        	{
        		UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionSelectObjectRecordBySpecFieldListOnObject from [%s] errCode = [%d]\n",resName,ret);
        		return(ret);
        	}
        	*fileRecved = 1;
        	return(0);
        }
        else
        {
        	//UnionUserErrLog("in UnionExcuteDBSvrOperation:: 非法的服务代码[%d]表名[%s]!\n",serviceID,resName);
        	//return(errCodeEsscMDL_InvalidService);
        	if ((ret = UnionExcuteAppSpecService(UnionGetCurrentResID(),serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
                {
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteAppSpecService!\n");
                        return(ret);
                }
        }

trigger:
	// 执行动作后的触发器
	if ((ret2 = UnionExcuteTriggerOperationOnTable(resName,serviceID,0,reqStrBuf,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteTriggerOperationOnTable into [%s] errCode = [%d]\n",resName,ret);
		return(ret2);
	}
        return(ret);
}

// 从记录"域1=域值|域2=域值|域3=域值|…"这种格式串中，拆分出关键域值"关键域1=域值|…"
int UnionGetPrimaryKeyFldFromObjectRecord(char *resName,char *record,char *priFld)
{
        int     ret;
        char    valueStr[2048];
        int     lenOfValueStr = 0;
        TUnionObject    obj;
        int     i = 0;
        int     iLen = 0;
	
        memset(&obj,0,sizeof(obj));
        if (UnionIsReadComplexDBOnUnionEnv() == 1)
        {
        	if ((ret = UnionReadSpecConfFileDef(resName,&obj)) < 0)
        	{
             		UnionUserErrLog("in UnionGetPrimaryKeyFldFromObjectRecord:: UnionReadSpecConfFileDef from [%s] errCode = [%d]\n",resName,ret);
                	return(ret);
        	}
        }
        else
        {
        	if ((ret = UnionReadObjectDef(resName,&obj)) < 0)
        	{
                	UnionUserErrLog("in UnionGetPrimaryKeyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",resName,ret);
                	return(ret);
        	}
        }
	
// 控制 Env 与 complexDB 转换
#ifndef _noUseUserSpecOperation_
        if (UnionIsReadComplexDBOnUnionEnv() == 1)
        {
                memset(valueStr, 0, sizeof(valueStr));
                if ((lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record),obj.fldDefGrp[0].name,valueStr,sizeof(valueStr))) < 0)
                {
                        UnionUserErrLog("in UnionGetPrimaryKeyFldFromObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] errCode = [%d]\n", obj.fldDefGrp[0].name, record,lenOfValueStr);
                        return(lenOfValueStr);
                }
                iLen            += sprintf(priFld + iLen, "%s=%s|", obj.fldDefGrp[0].name, valueStr);
                return iLen;
        }
#endif

        // 2008-09-04 modify by chenliang
        // memset(valueStr,0,sizeof(valueStr));

        for(i=0;i<obj.primaryKey.fldNum;i++)
        {
                // 2008-09-04 modify by chenliang
              // if ((lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record),(char *)obj.primaryKey.fldNameGrp,valueStr,sizeof(valueStr))) < 0)
              memset(valueStr, 0, sizeof(valueStr));
              if ((lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record),obj.primaryKey.fldNameGrp[i],valueStr,sizeof(valueStr))) < 0)
              {
                        UnionUserErrLog("in UnionGetPrimaryKeyFldFromObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] errCode = [%d]\n",obj.primaryKey.fldNameGrp[i],record,lenOfValueStr);
                        return(lenOfValueStr);
              }
                // 2008-09-04 modify by chenliang
              // lenOfValueStr = sprintf(priFld+iLen,"%s=%|",obj.primaryKey.fldNameGrp,valueStr);
              lenOfValueStr = sprintf(priFld+iLen,"%s=%s|",obj.primaryKey.fldNameGrp[i],valueStr);
              iLen += lenOfValueStr;
        }
        return iLen;
}

/*
function:
        判断是否是对象中的关键字段
param:
        [IN]:
        obj:            表字段信息
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
#ifdef _WIN32
int UnionIsPrimaryKey(const TUnionObject &obj, char *fldName)
#else
int UnionIsPrimaryKey(TUnionObject obj, char *fldName)
#endif
{
        int     priFldKey = obj.primaryKey.fldNum;
        int     i;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        for(i = 0; i < priFldKey; i++)
                if(strcmp(obj.primaryKey.fldNameGrp[i], fldName) == 0)
                        return 1;

        return 0;
}

/*
function:
        判断是否是对象中的唯一字段
param:
        [IN]:
        obj:            表字段信息
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是唯一字段
        0               不是唯一字段
*/
#ifdef _WIN32
int UnionIsUnique(const TUnionObject &obj, char *fldName)
#else
int UnionIsUnique(TUnionObject obj, char *fldName)
#endif
{
        int     uqeFldKey = obj.uniqueFldGrpNum;
	int	i;
	int	j;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        for(i = 0; i < uqeFldKey; i++)
        {
                for(j = 0; j < obj.uniqueFldGrp[i].fldNum; j++)
                {
                        if(strcmp(obj.uniqueFldGrp[i].fldNameGrp[j], fldName) == 0)
                                return 1;
                }
        }

        return 0;
}

/*
function：
        判断一字段名是否当前记录中存在
param:
        [IN]:
        record:         整条对象记录信息
        fldName         字段名

        [OUT]:
        NULL
return:
        0:              不存在
        1:              存在
*/
int UnionIsExistFld(char *record, char *fldName)
{
	return(UnionExistsRecFldInRecStr(record,strlen(record),fldName));
}

/*
function：
        获取允许修改的字段名称以及内容
param:
        [IN]:
        resName:        对象名
        record:         整条对象记录信息

        [OUT]:
        modFld:         保存所有允许修改的字段名以及字段名称
return:
        >=0:            所有允许修改的字段名以及字段名称的长度
        < 0:            解析出错
*/
int UnionGetAllowModifyFldFromObjectRecord(char *resName, char *record, char *modFld)
{
        int     ret;
        char    valueStr[2048];
        int     lenOfValueStr = 0;
        TUnionObject    obj;
        int     i = 0;
        char    *pos = modFld;
        int     len = 0;

        char    filterStr[2048];
        int     lenOfFilterStr;
        int     indexOfFilter;
        char    *filterPos;
	
        memset(&obj,0,sizeof(obj));
        // 根据对象名，读取该对象的定义
        if (UnionIsReadComplexDBOnUnionEnv() == 1)
        {
        	if ((ret = UnionReadSpecConfFileDef(resName,&obj)) < 0)
        	{
             		UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadSpecConfFileDef from [%s] errCode = [%d]\n",resName,ret);
                	return(ret);
        	}
        }
        else
        {
        	if ((ret = UnionReadObjectDef(resName,&obj)) < 0)
        	{
                	UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",resName,ret);
                	return(ret);
        	}
        }

// 控制 Env 与 complexDB 转换
#ifndef _noUseUserSpecOperation_
        if (UnionIsReadComplexDBOnUnionEnv() == 1)
        {
                memset(valueStr, 0, sizeof(valueStr));
                if ((lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record),obj.fldDefGrp[0].name,valueStr,sizeof(valueStr))) < 0)
                {
                        UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] errCode = [%d]\n", obj.fldDefGrp[0].name,record,lenOfValueStr);
                        return(lenOfValueStr);
                }

                memset(filterStr, 0, sizeof(filterStr));
                lenOfFilterStr          = sprintf(filterStr, "%s=%s|", obj.fldDefGrp[0].name,valueStr);

                filterPos               = strstr(record, filterStr);
                indexOfFilter           = filterPos - record;

                if (indexOfFilter < 0)
                        strcpy(modFld, record);
                else
                {
                        memcpy(pos, record, indexOfFilter);
                        pos             = pos + indexOfFilter;

                        strcpy(pos, filterPos + lenOfFilterStr);
                }

                return strlen(modFld);
        }
#endif

        // UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord, resName: [%s], obj.fldNum: [%d].\n", resName, obj.fldNum);

        for(i = 0; i < obj.fldNum; i++)
        {
                // 判断字段是否为 关键字段 或者 唯一字段
                if((UnionIsExistFld(record, obj.fldDefGrp[i].name)) && (!UnionIsPrimaryKey(obj, obj.fldDefGrp[i].name)) && (!UnionIsUnique(obj, obj.fldDefGrp[i].name)))
                {
                        memset(valueStr, 0, sizeof(valueStr));
                        lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record), obj.fldDefGrp[i].name, valueStr,sizeof(valueStr));
                        if(0 > lenOfValueStr)
                        {
                                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] errCode = [%d]\n",obj.primaryKey.fldNameGrp,record,lenOfValueStr);
                                return(lenOfValueStr);
                        }

                        len = sprintf(pos, "%s=%s|", obj.fldDefGrp[i].name, valueStr);
                        pos += len;
                }
        }

        return strlen(modFld);
}


/*
功能：
	根据文件名，读文件对应的表定义
输入参数：
	idOfObject	对象名
输出参数：
	pobjDef		读出表定义
返回值：
	>=0，成功
	<0，错误码
*/
int UnionReadSpecConfFileDef(TUnionIDOfObject idOfObject,PUnionObject pobjDef)
{
	int	ret;
	char	tblName[128+1];
	char	*ptr;
	
	if (pobjDef == NULL)
		return(errCodeParameter);
	
	memset(tblName,0,sizeof(tblName));
	if ((ret = UnionReadFileNameFromFullDir(idOfObject,strlen(idOfObject),tblName)) < 0)	
	{
		UnionAuditLog("in UnionReadSpecConfFileDef:: UnionReadFileNameFromFullDir [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	if ((ptr = strstr(tblName,".")) != NULL)
		*ptr = 0;
	/*
	if ((ret = UnionExistsObjectDef(tblName)) < 0)
	{
		UnionAuditLog("in UnionReadSpecConfFileDef:: [%s] not exists for [%s]!\n",tblName,idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	*/
	if ((ret = UnionReadObjectDef(tblName,pobjDef)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecConfFileDef:: UnionReadObjectDef [%s] for idOfObject = [%s]!\n",tblName,idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	return(ret);
}


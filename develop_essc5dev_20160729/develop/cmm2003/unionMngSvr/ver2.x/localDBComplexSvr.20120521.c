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
#include "unionTableDef.h"

#ifdef _WIN32
#include "unionTBLQueryInterface.h"

#include "unionDBConf.h"
#endif

#include "unionDatabaseCmd.h"
//#include "allTableInfo.h"
#include "simuMngSvrLocally.h"
#include "UnionLog.h"

#ifndef _WIN32
#define conMenuDefinedQueryConditionFldSeperator      ','     	// 菜单定义的查询条件中的分隔符
#define conMenuDefinedQueryConditionReadThisFld       "this."   // 定义读取当前记录的该域
#endif

int UnionIsUserSpecOperation(char *resName, int resID,int serviceID)
{
        char    varName[100];
        char    resName2[100];
        char    *ptr;

	memset(resName2, 0, sizeof(resName2));
	UnionReadResNameOfSpecResID(resID, resName2);

	if ((strcmp(resName, "") != 0) && (strcmp(resName, resName2) != 0))
	{
		return 0;
	}

	sprintf(varName,"isSpecService%03d.%03d",resID,serviceID);
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
		return(1);
	}
}

// 判断是否是本地仿真mngSvr
int UnionIsReadMngSvrDataLocally()
{
        return(0);
}

// 读取资源标识对应的资源名称
int UnionReadResNameOfSpecResID(int resID,char *resName)
{
	char	recStr[8192+1];
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

	memset(condition, 0, sizeof(condition));

	if ((ret = UnionPutRecFldIntoRecStr("idOfOperator",tellerNo,strlen(tellerNo),condition,sizeof(condition))) < 0)
       	{
		UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: UnionPutRecFldIntoRecStr [%s]!\n",tellerNo);
		return(ret);
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
	if ((pos = strstr(allowModifyFld, "isCurrentLogon")) == NULL)
	{
		UnionUserErrLog("in UnionUpdateOperationLogOnStatus:: cann't find isCurrentLogon from [%s].\n",allowModifyFld);
		return errCodeEsscMDL_InvalidService;
	}

	ret		= pos - allowModifyFld;
	memcpy(logBuf, allowModifyFld, ret);

	ret += sprintf(logBuf + ret, "isCurrentLogon=%d|", logOnStatus);

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
   	// add end. 2009-02-03

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
	// add by xusj end

	strcpy(resStr,recStr);

        // 设置为登陆状态
        if ((ret = UnionUpdateOperationLogOnStatus(tellerNo, resName, serviceID, reqStr, lenOfReqStr, resStr, 1)) < 0)
	{
		UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionUpdateOperationLogOnStatus error.\n");
		return ret;
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

        //if (resID == 0) // 数据库表操作
        //        return(UnionOperationOfDBTable(serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
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

//#ifndef _noUseUserSpecOperation_
	if (UnionIsUserSpecOperation(resName, UnionGetCurrentResID(),serviceID))
	{
		if ((ret = UnionExcuteAppSpecService(UnionGetCurrentResID(),serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionExcuteDBSvrOperation:: _noUseUserSpecOperation_ UnionExcuteAppSpecService ret = %d!\n", ret);
			return(ret);
		}

		return ret;
	}
//#endif

	
	if (serviceID == conDatabaseCmdQueryAllRec)  // 查询所有记录
        {
                // UnionLog("In UnionExcuteDBSvrOperation, 查询所有记录, and the reqStr:[%s], resName: [%s].\n", reqStr, resName);

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
                        UnionUserErrLog("in UnionExcuteDBSvrOperation:: UnionExcuteAppSpecService ret = %d!\n", ret);
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
        int     i = 0;
        int     iLen = 0;

	char		primaryKeyGrp[6][40+1];
	int		primaryKeyNum;
	
	memset(primaryKeyGrp, 0, sizeof(primaryKeyGrp));
	primaryKeyNum = UnionReadTablePrimaryKeyGrpFromImage(resName, primaryKeyGrp);
	for(i=0; i < primaryKeyNum; i++)
	{
		memset(valueStr, 0, sizeof(valueStr));
		if ((lenOfValueStr = UnionReadRecFldFromRecStr(record, strlen(record), primaryKeyGrp[i], valueStr, sizeof(valueStr))) < 0)
		{
			UnionUserErrLog("in UnionGetPrimaryKeyFldFromObjectRecord:: UnionReadRecFldFromRecStr [%s] from [%s] errCode = [%d]\n",primaryKeyGrp[i], record, lenOfValueStr);
			return(lenOfValueStr);
		}
		lenOfValueStr = sprintf(priFld+iLen,"%s=%s|", primaryKeyGrp[i], valueStr);
		iLen += lenOfValueStr;
	}
	return(iLen);
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
int UnionIsPrimaryKey(const TUnionTableDef &obj, char *fldName)
#else
int UnionIsPrimaryKey(TUnionTableDef obj, char *fldName)
#endif
{
        int     i;

        if(NULL == fldName || 0 == strlen(fldName))
                return 0;

        for(i = 0; i < obj.fieldNum; i++)
                if( 1 == (obj.fieldDef[i].isPrimaryKey) && ( 0 == strcmp(obj.fieldDef[i].fieldName, fldName)))
                        return 1;

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
        PUnionTableDef    obj;
        int     i = 0;
        char    *pos = modFld;
        int     len = 0;

        char    filterStr[2048];
        int     lenOfFilterStr;
        int     indexOfFilter;
        char    *filterPos;
	
        memset(&obj,0,sizeof(obj));
	//if ((ret = UnionReadObjectDef(resName,&obj)) < 0)
	//if ((ret = UnionReadTableDefFromImage(resName,&obj)) < 0)
	if ((obj = UnionFindTableDef(resName)) == NULL )
	{
		ret = UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists);
		UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionFindTableDef from [%s] errCode = [%d]\n",resName,ret);
		return(ret);
	}

        for(i = 0; i < obj->fieldNum; i++)
        {
                // 判断字段是否为 关键字段 或者 唯一字段
                if((UnionIsExistFld(record, obj->fieldDef[i].fieldName)) && (!UnionIsPrimaryKey(*obj, obj->fieldDef[i].fieldName)) /*&& (!UnionIsUnique(obj, obj.fldDefGrp[i].name))*/)
                {
                        memset(valueStr, 0, sizeof(valueStr));
                        lenOfValueStr = UnionReadRecFldFromRecStr(record,strlen(record), obj->fieldDef[i].fieldName, valueStr,sizeof(valueStr));
                        if(0 > lenOfValueStr)
                        {
                                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadRecFldFromRecStr  from [%s] errCode = [%d]\n", record,lenOfValueStr);
                                return(lenOfValueStr);
                        }

                        len = sprintf(pos, "%s=%s|", obj->fieldDef[i].fieldName, valueStr);
                        pos += len;
                }
        }
        return strlen(modFld);
}


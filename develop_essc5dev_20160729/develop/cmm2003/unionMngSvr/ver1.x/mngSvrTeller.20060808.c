//	Wolfgang Wang, 2006/08/08

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#ifdef _WIN32
#include "unionSvrConf.h"
#include "unionNotice.h"
#endif
#include "mngSvrTeller.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "unionTeller.h"
#include "unionOperatorType.h"
#include "unionDataTBLList.h"

char    gunionTellerNo[48+1] = "88888888";
char    gunionTellerName[40+1] = "";
char    gunionTellerLevel = '4';
int     gunionIsTellerLogonOK = 0;

/*
功能：
	判断当前操作员是否是科友开发人员
输入参数:
	无
输出参数:
	无
返回值
	1		是
	0		否
*/
int UnionCurrentTellerIsUnionDeveloper()
{
	if (gunionTellerLevel == '9')
		return(1);
	else
		return(0);
}

/*
功能：
	读取操作员级别标识
输入参数:
	level		操作员级别
	sizeOfBuf	域值缓冲大小
输出参数:
	operatorTypeID	操作员级别标识
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionReadOperatorTypemacroValueByLevel(int level,char *operatorTypeID,int sizeOfBuf)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192+1];
	char	condition[1024+1];

	//从记录拼装串
	// 拼关键字
	memset(condition,0,sizeof(condition));
	ret = UnionPutIntTypeRecFldIntoRecStr(conOperatorTypeFldNameMacroValue,level,condition+lenOfRecStr,sizeof(condition)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionPutIntTypeRecFldIntoRecStr [%s] failure!\n",conOperatorTypeFldNameMacroValue);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByUniqueKey(conTBLNameOperatorType,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionSelectUniqueObjectRecordByUniqueKey! [%s]\n",condition);
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//读指定域
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperatorTypeFldNameID,operatorTypeID,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionReadRecFldFromRecStr [%s]!\n",conOperatorTypeFldNameID);
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}

int UnionGetTellerLevelTypeID(char *tellerNo,char *tellerTypeID)
{
	int	ret;
	int	level;
	
	// 读操作员级别
	if ((ret = UnionReadTellerRecIntTypeFld(tellerNo,conTellerFldNameLevel,&level)) < 0)
	{
		UnionUserErrLog("in UnionGetTellerLevelTypeID:: UnionReadTellerRecIntTypeFld [%s]!\n",tellerNo);
		return(ret);
	}
	if (UnionReadIntTypeRECVar("isVisualControlUseTellerLevel") >= 1)
	{
		gunionTellerLevel = level % 10 + '0';	// 2010/1/26,Wolfgang Wang
		tellerTypeID[0] = gunionTellerLevel;
		tellerTypeID[1] = 0;
		return(0);	// 2010/10/30,王纯军增加此句，原来在此存在一个BUG
	}
	else
		return(UnionReadOperatorTypemacroValueByLevel(level,tellerTypeID,40+1));
}

int UnionGetTellerLevelName(char *tellerNo,char *tellerTypeID)
{
	int	ret;
	int	level;
	
	// 读操作员级别
	if ((ret = UnionReadTellerRecIntTypeFld(tellerNo,conTellerFldNameLevel,&level)) < 0)
	{
		UnionUserErrLog("in UnionGetTellerLevelTypeID:: UnionReadTellerRecIntTypeFld [%s]!\n",tellerNo);
		return(ret);
	}
	return(UnionReadOperatorTypemacroValueByLevel(level,tellerTypeID,40+1));
}

// 获得级别
char UnionGetTellerLevel()
{
        return(gunionTellerLevel);
}

// 获得登录状态
int UnionIsCurrentTellerLogonOK()
{
        return(gunionIsTellerLogonOK);
}

// 设置当前柜员登录成功
void UnionSetCurrentTellerLogonOK()
{
        gunionIsTellerLogonOK = 1;
        return;
}
/*
功能	获得当前柜员号
输入参数
	无
输出参数
	无
返回值
	指向柜员员的指针
*/
char *UnionGetTellerNo()
{
	return(gunionTellerNo);
}

/*
功能	设置当前柜员信息
输入参数
	tellerNo	当前柜员号
	tellerName	柜员名
	level           级别
输出参数
	无
返回值
	>=0	正确
	<0	失败
*/
void UnionSetTellerInfo(char *tellerNo,char *tellerName,char level)
{
        if (tellerNo != NULL)
        	strcpy(gunionTellerNo,tellerNo);
        if (tellerName != NULL)
        	strcpy(gunionTellerName,tellerName);
        gunionTellerLevel = level;
	return;
}

#ifdef _WIN32
/*
功能	柜员登录
输入参数
	id	柜员号
	passwd	柜员密码
输出参数
	无
返回值
	true	成功
        false   失败
*/
bool UnionOperatorLogon(char *id,char *passwd)
{
	char	tmpBuf[512+1];
	int	len;
	int	fileRecved;
	char	*fileName;
        char    level[100];
        int     ret;

	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionEncryptPassword(passwd,tmpBuf);
	if ((len = UnionCommunicationWithSpecMngSvr(UnionGetIPAddrOfCurrentSvr(),UnionGetPortOfCurrentSvr(),id,conSpecTBLIDOfOperator,conDatabaseSpecCmdOfOperatorTBL_logon,
						tmpBuf,strlen(tmpBuf),
						tmpBuf,sizeof(tmpBuf),
						&fileRecved)) < 0)
	{
		UnionErrorNotice("操作员[%s]登录出错,错误码[%d]原因[%s]",id,len,tmpBuf);
		return(false);
	}
        tmpBuf[len] = 0;
        if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"level",level,sizeof(level))) < 0)
        {
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"userType",level,sizeof(level))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr level from [%s] errCode = [%d]\n",tmpBuf,ret);
                        return(ret);
                }
        }
	UnionSetTellerInfo(id,NULL,level[0]);
	/*
	if (!fileRecved)
	{
		UnionErrorNotice("下载操作员权限表失败!!");
		UnionOperatorLogoff();
		return(false);
	}
	if ((fileName = UnionGetCurrentMngSvrTempFileName()) != NULL)
		RenameFile(fileName,UnionOperationControlFileName);
	*/
	return(true);
}
//---------------------------------------------------------------------------
// 退出登录
void UnionOperatorLogoff()
{
	char	tmpBuf[128+1];
	int	fileRecved = 0;
	int	len=0;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((len = UnionCommunicationWithSpecMngSvr(UnionGetIPAddrOfCurrentSvr(),UnionGetPortOfCurrentSvr(),UnionGetTellerNo(),conSpecTBLIDOfOperator,conDatabaseSpecCmdOfOperatorTBL_logoff,
		"",0,tmpBuf,sizeof(tmpBuf),&fileRecved)) < 0)
	{
		UnionErrorNotice("操作员[%s]退出登录出错,错误码[%d]原因[%s]",UnionGetTellerNo(),len,tmpBuf);
		return;
	}
	//DeleteFile(UnionOperationControlFileName);
	return;
}

#endif

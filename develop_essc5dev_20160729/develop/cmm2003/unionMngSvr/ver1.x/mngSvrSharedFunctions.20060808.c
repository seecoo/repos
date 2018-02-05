#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionResID.h"
#include "UnionLog.h"

int	gunionCurrentResID		= -1;
int	gunionCurrentServiceID		= -1;
int	gunionMngSvrIsBackuper = 0;	// 判断当前mngSvr是否是备份Svr,2010-01-22,Wolfgang Wang

// add by xusj 20100607 begin
char    gunionCurrentMngSvrClientReqStr[8192+1] = "";   // 存储当前的请求串
int     gunionLenOfCurrentMngSvrClientReqStr = 0;       // 当前请求串的长度
char	gunionCurrentMngSvrClientResStr[8192+1] = "";	// 存储当前的响应串
int	gunionLenOfCurrentMngSvrClientResStr = 0;		// 当前响应串的长度
char	gunionTellerNoOfCurrentOperation[40+1] = "";		// 2009/8/1,Wolfgang Wang added
int	gunionCurrentOperationType = -1;			// 2009/8/1,Wolfgang Wang added
char	gunionCurrentResName[40+1] = "";			// 当前资源名称,2009/11/9,王纯军增加
int	gunionMngSvrIsBackSvr = 0;	// 判断当前mngSvr是否是后台Svr,2010-01-22,Wolfgang Wang

// add by xusj begin 20100315,增加登陆提示
char	gunionLogonRemark[256+1] = "";


// add by xusj begin 20100607,增加数据事务控制
int	gunionIsDBBeginAWork=0;

int	gunionIsNoneMngSvr = 0;	// 2010/6/9,王纯军增加

// 2010/6/9,王纯军增加
int UnionSetAsNonMngSvr()
{
	gunionIsNoneMngSvr = 1;
	return(0);
}

// 2010/6/9,王纯军增加
int UnionIsNonMngSvr()
{
	return(gunionIsNoneMngSvr);
}

// 2010/6/9,王纯军增加
int UnionSetAsMngSvr()
{
	gunionIsNoneMngSvr = 0;
	return(0);
}


// 开始一个事务
int UnionDBBeginAWork()
{
	if (UnionReadIntTypeRECVar("isKmcUseDBWork") > 0)
		gunionIsDBBeginAWork = 1;
	else
		gunionIsDBBeginAWork = 0;
	return 0;
}

/***
// 结束一个事务
int UnionDBEndAWork()
{
	if (gunionIsDBBeginAWork)
		UnionRollBackWork();
	return 0;
}
***/

int UnionDBIsBeginAWork()
{
	return (gunionIsDBBeginAWork);
}
// add by xusj end 20100607,增加数据事务控制
int UnionSetLogonRemark(char *remark)
{
	strcpy(gunionLogonRemark, remark);
	return 0;
}

int UnionInitLogonRemark()
{
	memset(gunionLogonRemark, 0, sizeof gunionLogonRemark);
	return 0;
}

char *UnionReadLogonRemark()
{
	return(gunionLogonRemark);
}

int UnionGetLogonRemark(char *remark)
{
	strcpy(remark, gunionLogonRemark);
	return 0;
}
// add by xusj end 20100315,增加登陆提示
int UnionSetCurrentTellerNo(char *tellerNo)
{
	strcpy(gunionTellerNoOfCurrentOperation,tellerNo);	// 2009/8/1,Wolfgang Wang added
	return(0);
}
       
int UnionSetCurrentOperationType(int operationType)
{
	return(gunionCurrentOperationType = operationType);
}

int UnionSetCurrentResName(char *resName)	// 2009/11/9,王纯军增加
{
	strcpy(gunionCurrentResName,resName);	
	return(0);
}

// add by wolfang wang, 2010-6-2
int UnionSetCurrentMngSvrClientResStr(char *resStr,int lenOfRecStr)
{
	if (lenOfRecStr >= sizeof(gunionCurrentMngSvrClientResStr))
		gunionLenOfCurrentMngSvrClientResStr = sizeof(gunionCurrentMngSvrClientResStr) - 2;
	else
		gunionLenOfCurrentMngSvrClientResStr = lenOfRecStr;
	memcpy(gunionCurrentMngSvrClientResStr,resStr,gunionLenOfCurrentMngSvrClientResStr);
	gunionCurrentMngSvrClientResStr[gunionLenOfCurrentMngSvrClientResStr+1] = 0;
	return;
}

// 2010-3-20 wolfgang added
int UnionReadFldFromCurrentMngSvrClientResStr(char *fldName,char *buf,int sizeOfBuf)
{
	return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientResStr,gunionLenOfCurrentMngSvrClientResStr,fldName,buf,sizeOfBuf));
}

// 2010-3-20 wolfgang added
int UnionReadIntTypeFldFromCurrentMngSvrClientResStr(char *fldName,int *intValue)
{
	return(UnionReadIntTypeRecFldFromRecStr(gunionCurrentMngSvrClientResStr,gunionLenOfCurrentMngSvrClientResStr,fldName,intValue));
}

// 2013-1-14 tanhj add
// 修改请求串
int UnionUpdateCurrentMngSvrClientResStr(char *fldName,char *buf,int sizeOfBuf)
{
	//先删除掉这个原请求串中的这个域
	gunionLenOfCurrentMngSvrClientReqStr = UnionDeleteRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,fldName);
	
	//把修改的域加到请求串中
	return(UnionPutFldToCurrentMngSvrClientReqStr(fldName,buf,sizeOfBuf));
}

// added 2012-06-26
char *UnionGetCurrentMngSvrClientReqStr()
{
        return(gunionCurrentMngSvrClientReqStr);
}
// end of addication 2012-06-26
int UnionSetCurrentMngSvrClientReqStr(char *data,int lenOfData)
{
        if (lenOfData >= sizeof(gunionCurrentMngSvrClientReqStr))
                gunionLenOfCurrentMngSvrClientReqStr = sizeof(gunionCurrentMngSvrClientReqStr) - 1;
        else
        {
                gunionLenOfCurrentMngSvrClientReqStr = lenOfData;
        }
        memcpy(gunionCurrentMngSvrClientReqStr,data,gunionLenOfCurrentMngSvrClientReqStr);
        gunionCurrentMngSvrClientReqStr[gunionLenOfCurrentMngSvrClientReqStr] = 0;
	return(gunionLenOfCurrentMngSvrClientReqStr);
}

int UnionPutFldToCurrentMngSvrClientReqStr(char *fldName,char *value,int lenOfValue)
{
	gunionLenOfCurrentMngSvrClientReqStr = sprintf(gunionCurrentMngSvrClientReqStr,"%s%s=%s|",gunionCurrentMngSvrClientReqStr,fldName,value);
	return(gunionLenOfCurrentMngSvrClientReqStr);
}

int UnionReadFldFromCurrentMngSvrClientReqStr(char *fldName,char *buf,int sizeOfBuf)
{
        return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,buf,sizeOfBuf));
}

int UnionReadIntTypeFldFromCurrentMngSvrClientReqStr(char *fldName,int *intValue)
{
        return(UnionReadIntTypeRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,intValue));
}

int UnionReadLongTypeFldFromCurrentMngSvrClientReqStr(char *fldName,long *intValue)
{
        return(UnionReadLongTypeRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,intValue));
}
// add by xusj 20100607 end

// 获取当前资源号
int UnionGetCurrentResID()
{
	return gunionCurrentResID;
}

// 设置当前资源号
int UnionSetCurrentResID(int resID)
{
	gunionCurrentResID = resID;
}

// 获取当前请求号
int UnionGetCurrentServiceID()
{
	return gunionCurrentServiceID;
}

// 设置当前请求号
int UnionSetCurrentServiceID(int serviceID)
{
	gunionCurrentServiceID = serviceID;
}


//2009/11/9,王纯军增加
char *UnionGetCurrentResName()
{
	return(gunionCurrentResName);
}

// 2009/8/1,Wolfgang Wang added
char *UnionGetCurrentOperationTellerNo()
{
	return(gunionTellerNoOfCurrentOperation);
}

// 2009/8/1,Wolfgang Wang added
int UnionIsBackuperMngSvr()
{
	return(gunionMngSvrIsBackuper);
}

// 2009/8/1,Wolfgang Wang added
// 判断是否要向兄弟服务器同步该操作
int UnionIsSynchronizeComplexDBOperationToBrothers()
{
	if (gunionMngSvrIsBackuper)	// 2010-01-22,Wolfgang Wang
		return(0);
	if (strcmp(gunionTellerNoOfCurrentOperation,"backuper") == 0)
		return(0);
	if (UnionReadIntTypeRECVar("isKmcSynToBrothers")<=0) // 2010-06-07,xusj
		return(0);
	else
		return(1);
}

// 2010-01-22,Wolfgang Wang
void UnionSetAsBackupMngSvr()
{
	gunionMngSvrIsBackuper = 1;
}

// 2010-01-22,Wolfgang Wang
void UnionSetAsBackMngSvr()
{
	gunionMngSvrIsBackSvr = 1;
}
 
// 2010-01-22,Wolfgang Wang
int UnionIsBackMngSvr()
{
	return(gunionMngSvrIsBackSvr);
}

int UnionIsNonAuthorizationMngSvrService(int resID,int serviceID)
{
	//UnionLog("entering ...\n");
	if (UnionIsNonMngSvr())
		return(1);
			
	if (serviceID == conResCmdReadAllMainMenuDef)	// 读取所有主菜单
		return(1);

	if (resID == conResIDOperatorTBL)
	{
		switch (serviceID)
		{
			case	conResCmdReadMainMenuItem:	// 读取主菜单的菜单项
			case	conResCmdReadSecondaryMenuItem:	// 读取二级菜单的菜单项
			case	conResCmdReadInterfacePopupMenuItem:	// 读取界面右键菜单项
			case	conResCmdReadRecordPopupMenuItem:	// 读取记录右键菜单项
			case	conResCmdReadRecordDoubleClickMenuItem:	// 读取记录双击菜单项
			case	conResCmdReadMenuItemCmd:	// 读取菜单项的执行命令
			case	conResCmdReadPopupMenuItem:	// 读取右键菜单项的执行命令
			// 2011-11-11 张永定增加
			case	conResCmdUpdatePassword:	// 修改密码
			// 2011-7-5 张永定增加
			case	conResCmdGetOneRandomNumber:	// 获取随机数
				return(1);
		}
	}
	else if ((resID == conResIDMenuDef) || (resID == conResIDMainMenu))
	{
		switch (serviceID)
		{
			case	conResCmdGenerateMainMenu:	// 生成主菜
			case	conResCmdGenerateSpec2LevelMenu:	// 生成指定二级菜单
				return(1);
		}
	}
	else if (resID == conResIDViewList)
	{
		switch (serviceID)
		{
			case	conResCmdGenerateTBLQueryInterface:	// 生成界面视图
			case	conResCmdCreateDefaultViewOfTBL:	// 生成界面视图
				return(1);
		}
	}
	else if (resID == conResIDEnumValueDef)
	{
		switch (serviceID)
		{
			case	conResCmdDownloadEnumDef:		// 读取枚举定义文件
				return(1);
		}
	}
	// UnionLog("in UnionIsNonAuthorizationMngSvrService:: [%03d-%03d] authorizied need!\n",resID,serviceID);
	return(0);
}

int UnionFormTaskNameOfMngSvr(int port,char *taskName)
{
	char	varName[128+1];
	
	// 判断是否是后台端口
	sprintf(varName,"isBackupMngSvrPort%d",port);
	if (UnionReadIntTypeRECVar(varName) > 0)
	{
		UnionSetAsBackupMngSvr();		
		sprintf(taskName,"%s %d backup",UnionGetApplicationName(),port);
	}
	else
	{
		sprintf(varName,"isBackMngSvrPort%d",port);
		if (UnionReadIntTypeRECVar(varName) > 0)
		{
			UnionSetAsBackMngSvr();		
			sprintf(taskName,"%s %d back",UnionGetApplicationName(),port);
		}
		else
			sprintf(taskName,"%s %d",UnionGetApplicationName(),port);
	}
	return(0);
}


// Wolfgang Wang
// 2008/9/27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "unionMDLID.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#include <process.h>
#endif

TUnionMDLType	gunionMyModuleType = conMDLTypeDefault;
TUnionModuleID	gunionCurrentDynamicModuleID = -1;
long		gunionMyselfSSN = 1;

// 获取指定密码机的处理模块ID号
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsm(char *hsmIPAddr)
{
	if (hsmIPAddr == NULL)
		return(0);
	else
		return(labs(inet_addr(hsmIPAddr)));
}

// 获取指定密码机工作组的处理模块ID号
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsmGrp(char *hsmGrpID)
{
	int	i;
	char	hsmID[8+1];
	char	tmpBuf[128+1];
	
	if (hsmGrpID == NULL)
		return(1);
	
	if (UnionIsDigitString(hsmGrpID))	
		return(labs(atol(hsmGrpID)));
	//else if (strcmp(hsmGrpID,"default") == 0)
	//	return(conMDLTypeUnionHsmSvr);
	else
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,hsmGrpID);
		UnionToUpperCase(tmpBuf);
		memset(hsmID,0,sizeof(hsmID));
		//hsmID = conMDLTypeUnionHsmSvr * 1000;
		for (i = 0; i < strlen(tmpBuf); i++)
		{
			if (i == 8)
				break;
			if (UnionIsDigitChar(tmpBuf[i]))
				hsmID[i] = tmpBuf[i];
			else
				hsmID[i] = (tmpBuf[i] - 'A') % 10 + '0';
		}
		return(labs(atol(hsmID)));
	}
}

int UnionGetHsmGrpIDOutOfPort(int port,char *hsmGrpID)
{
	sprintf(hsmGrpID,"%03d",port % 1000);
	return(0);
}

// 设置模块类型
int UnionSetMyModuleType(TUnionMDLType myMDLType)
{
	if (myMDLType <= 0)
	{
		UnionUserErrLog("in UnionSetMyModuleType:: myMDLType [%d]\n",myMDLType);
		return(errCodeEsscMDL_InvalidMDLID);
	}
	gunionMyModuleType = myMDLType;
	return(0);
}

// 获取模块类型
TUnionMDLType UnionGetMyModuleType()
{
	return(gunionMyModuleType);
}

// 从模块标识号中获取模块类型
TUnionMDLType UnionGetMDLTypeOutOfMDLID(TUnionModuleID id)
{
	return(id % conMaxNumOfMDL);
}

// 从模块标识号中获取进程号
int UnionGetPIDOutOfMDLID(TUnionModuleID id)
{
	// return(id / conMaxNumOfMDL); // 2007/11/15 删除
	return(id); // 2007/11/15 增加	
}

// 从模块标识号中获取动态模块标识号
// 2007/11/15 修改这个函数
int UnionGetDynamicIDOutOfMDLID(TUnionModuleID id)
{
	return(id);
} 

// 产生模块的动态标识号
TUnionModuleID UnionGenerateMyDynamicMDLID()
{
	return(gunionCurrentDynamicModuleID = getpid());
}

// 获取当前的模块的动态标识号
TUnionModuleID UnionGetMyCurrentDynamicMDLID()
{
	return(gunionCurrentDynamicModuleID);
}

// 获取模块的静态标识号
TUnionModuleID UnionGetMyFixedMDLID()
{
	return(UnionGetMyModuleType());
}

// 获取模块的带进程号的静态标识号
TUnionModuleID UnionGetMySpecifiedFixedMDLID()
{
	return(UnionGetMyModuleType() + getpid() * conMaxNumOfMDL);
}

// 获取指定类型的模块的静态标识号
TUnionModuleID UnionGetFixedMDLIDOfMDLType(TUnionMDLType mdlType)
{
	return(mdlType % conMaxNumOfMDL);
}

// 获取指定模块的ID号
TUnionModuleID UnionGetFixedMDLIDOfSpecSvr(int specID)
{
	return(specID * conMaxNumOfMDL + 1000000000);
}

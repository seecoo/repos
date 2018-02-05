//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "commWithHsmSvr.h"

#define	defMaxDlHandleNum	1000

typedef struct
{
	char	serviceCode[4+1];
	void	*handle;
} TUnionServiceCodeDlHandle;

TUnionServiceCodeDlHandle	gunionServiceCodeDlHandle[defMaxDlHandleNum];
int				gunionCurrentDlHandleNum = 0;
int (*UnionDealServiceCode)(PUnionHsmGroupRec);

// 执行密码机指令
int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	newDl = 0;
	char	dlName[128+1];
	char	funName[128+1];
	
	for (i = 0; i < gunionCurrentDlHandleNum; i++)
	{
		if (strcmp(serviceCode,gunionServiceCodeDlHandle[i].serviceCode) == 0)
			goto dlSymTag;
	}
		
	newDl = 1;
	if (getenv("UNIONDLDIR") != NULL)
		ret = sprintf(dlName,"%s/lib%s.so",getenv("UNIONDLDIR"),serviceCode);
	else
		ret = sprintf(dlName,"%s/bin/lib%s.so",getenv("UNIONREC"),serviceCode);
	dlName[ret] = 0;
	ret = sprintf(gunionServiceCodeDlHandle[gunionCurrentDlHandleNum].serviceCode,"%s",serviceCode);
	gunionServiceCodeDlHandle[gunionCurrentDlHandleNum].serviceCode[ret] = 0;
	if ((gunionServiceCodeDlHandle[gunionCurrentDlHandleNum].handle = dlopen(dlName, RTLD_NOW)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceTask:: dlopen[%s] error[%s]!\n",dlName,dlerror());
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	if (++gunionCurrentDlHandleNum > defMaxDlHandleNum)
	{
		UnionUserErrLog("in UnionDealServiceTask:: handle number[%d] is too big!\n",gunionCurrentDlHandleNum);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

dlSymTag:
	ret = sprintf(funName,"UnionDealServiceCode%s",serviceCode);
	funName[ret] = 0;
	if ((UnionDealServiceCode = dlsym(gunionServiceCodeDlHandle[i].handle,funName)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceTask:: dlsym error funName[%s]!\n",funName);
		if (newDl)
			gunionCurrentDlHandleNum --;
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	return(ret = (*UnionDealServiceCode)(phsmGroupRec));
}

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "UnionLog.h"

#ifdef _WIN32
#include "unionRECVar.h"
#else
#include "unionREC.h"
#include "UnionTask.h"
#endif

int UnionExcuteSpecAppSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr)
{
	return(0);
}

int UnionExcuteAppSpecService(int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	UnionUserErrLog("in UnionExcuteAppSpecService:: 非法的服务代码[%d]表标识[%d]!\n",serviceID,resID);
        return(errCodeEsscMDL_InvalidService);
}

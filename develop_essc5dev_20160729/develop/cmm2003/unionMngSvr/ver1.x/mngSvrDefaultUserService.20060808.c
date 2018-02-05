// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "UnionLog.h"

int UnionExcuteUserSpecResSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	//return(UnionExcuteDBTableDesignService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
	UnionUserErrLog("in UnionExcuteUserSpecResSvrService:: resID [%03d] serviceID [%03d] not valid!\n",resID,serviceID);
	return(errCodeEsscMDL_InvalidService);
}

int UnionExcuteSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr)
{
	return(0);
}

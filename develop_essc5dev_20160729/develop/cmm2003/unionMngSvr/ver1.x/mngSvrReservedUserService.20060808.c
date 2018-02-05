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

int UnionExcuteUserSpecResSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	switch (resID)
	{
		case	conResIDDesKeyDB:
			return(UnionExcuteUnionDesKeyDBSvrOperation(serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
		case	conResIDPKDB:
			return(UnionExcuteUnionPKDBSvrOperation(serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
		default:
			return(errCodeEsscMDL_InvalidService);
	}
}


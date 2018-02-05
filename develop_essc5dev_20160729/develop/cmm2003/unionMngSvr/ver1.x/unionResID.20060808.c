// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "unionREC.h"
#include "unionVersion.h"

int gunionMyTransInfoResID = -1;

int UnionSetMyTransInfoResID(int resID)
{
	gunionMyTransInfoResID = resID;
	return(0);
}

int UnionGetMyTransInfoResID()
{
	char	varName[128+1];
	
	if (gunionMyTransInfoResID >= 0)
		return(gunionMyTransInfoResID);
	sprintf(varName,"resIDOf%s",UnionGetApplicationName());
	return(gunionMyTransInfoResID=UnionReadIntTypeRECVar(varName));
}

int UnionIsValidResType(TUnionResType type)
{
	switch (type)
	{
		case	conResTypeREC:
		case	conResTransPack:
		case	conResErrorInfo:
			return(1);
		default:
			return(0);
	}
}

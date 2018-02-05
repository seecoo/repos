// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11
// Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionFunSvrName.h"
#include "unionMDLID.h"

char gunionFunSvrName[64+1] = "";
long gunionFixedMDLIDOfFunSvr = conMDLTypeUnionFunSvr;
char gunionFunSvrStartVar[128+1] = "";
int  gunionLenOfLenFieldForApp = 2;

void UnionSetFunSvrName(char *funSvrName)
{
	strcpy(gunionFunSvrName,funSvrName);
}

char *UnionGetFunSvrName()
{
	return(gunionFunSvrName);
}

void UnionSetFixedMDLIDOfFunSvr(long fixedID)
{
	UnionProgramerLog("in UnionSetFixedMDLIDOfFunSvr:: fixedID = [%d]\n",fixedID);
	gunionFixedMDLIDOfFunSvr = fixedID;
}

long UnionGetFixedMDLIDOfFunSvr()
{
	//return(UnionGetFixedMDLIDOfSpecSvr(gunionFixedMDLIDOfFunSvr));
	return(gunionFixedMDLIDOfFunSvr);
}

void UnionSetFunSvrStartVar(char *startVar)
{
	if (startVar != NULL)
		strcpy(gunionFunSvrStartVar,startVar);
}

char *UnionGetFunSvrStartVar()
{
	return(gunionFunSvrStartVar);
}

void UnionSetLenOfLenFieldForApp(int len)
{
	gunionLenOfLenFieldForApp = len;
	return;
}

int UnionGetLenOfLenFieldForApp()
{
	return(gunionLenOfLenFieldForApp);
}


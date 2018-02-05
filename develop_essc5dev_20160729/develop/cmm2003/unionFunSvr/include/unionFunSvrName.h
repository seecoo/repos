// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11
// Version:	1.0

#ifndef _unionFunSvrName_
#define _unionFunSvrName_

void UnionSetFunSvrName(char *funSvrName);

char *UnionGetFunSvrName();

void UnionSetFixedMDLIDOfFunSvr(long fixedID);

long UnionGetFixedMDLIDOfFunSvr();

void UnionSetFunSvrStartVar(char *startVar);

char *UnionGetFunSvrStartVar();

void UnionSetLenOfLenFieldForApp(int len);

int UnionGetLenOfLenFieldForApp();

#endif

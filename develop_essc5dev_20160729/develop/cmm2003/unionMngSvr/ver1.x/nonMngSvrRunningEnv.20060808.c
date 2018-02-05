// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>

#include "transSpierBuf.h"
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"

extern PUnionTaskInstance	ptaskInstance;

int UnionTaskActionBeforeExit();

int UnionConnectRunningEnv()
{
	UnionSetAsNonMngSvr();	// …Ë÷√≥…∑«mngSvr
	return(0);
}

// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>

#include "transSpierBuf.h"
#include "unionVersion.h"

int UnionFormTaskNameOfMngSvr(int port,char *taskName)
{
	sprintf(taskName,"%s %d",UnionGetApplicationName(),port);
	return 0;
}

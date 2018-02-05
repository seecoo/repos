//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30，在5.0基础上升级
//	修改了	UnionApplyNewMsgIndexOfTransSpierBuf

#define _UnionLogMDL_3_x_
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionIndexTBL.h"
#include "transSpierBuf.h"
#include "unionErrCode.h"
#include "unionREC.h"

//#include "unionUnionTransSpierBuf.h"

int UnionGetFileNameOfTransSpierBufDef(char *fileName)
{
	sprintf(fileName,"unionTransSpierBuf");
	return(0);
}

int UnionInitTransSpierBufDef(PUnionTransSpierBufHDL pdef)
{
	int	ret = 0;
	char szValue[128+1];

	if (pdef == NULL)
		return(errCodeParameter);

	if( (ret = UnionReadUnionTransSpierBufRecFld("maxNumOfMsg", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadUnionTransSpierBufRecFld maxNumOfMsg\n");
		return (ret);
	}
	pdef->maxNumOfMsg = atoi(szValue);

	if( (ret = UnionReadUnionTransSpierBufRecFld("maxSizeOfMsg", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadUnionTransSpierBufRecFld maxSizeOfMsg\n");
		return (ret);
	}
	pdef->maxSizeOfMsg = atol(szValue);

	return(0);
}

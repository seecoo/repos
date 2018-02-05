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

int UnionGetFileNameOfTransSpierBufDef(char *fileName)
{
	sprintf(fileName,"%s/unionTransSpierBuf.Def",getenv("UNIONETC"));
	return(0);
}

int UnionInitTransSpierBufDef(PUnionTransSpierBufHDL pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTransSpierBufDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitTransSpierBufDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitTransSpierBufDef:: UnionGetEnviVarByName for [%s]\n!","maxNumOfMsg");
		goto abnormalExit;
	}
	pdef->maxNumOfMsg = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxSizeOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitTransSpierBufDef:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfMsg");
		goto abnormalExit;
	}
	pdef->maxSizeOfMsg = atol(p);
	
	UnionClearEnvi();
	return(0);

abnormalExit:	
	UnionClearEnvi();
	return(errCodeTransSpierBufMDL_InitDef);
}



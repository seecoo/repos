// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"

#include "unionServiceChanelRouter.h"

/*
功能
	获取服务通道标识
输入参数
	reqStr		服务请求
	lenOfReqStr	请求串长度
输出参数
	无
返回值
	>=0		服务通道的标识
	<0		错误代码
*/
long UnionGetServiceChanelID(char *reqStr,int lenOfReqStr)
{
	char	varName[40+1];
	int	appIDLen = 2;
	int	serviceIDLen = 3;

#ifdef _appLenIs3Chars_
	appIDLen = 3;
#endif
	if (lenOfReqStr < appIDLen+serviceIDLen)
	{
		UnionUserErrLog("in UnionGetServiceChanelID:: lenOfReqStr = [%d] error!\n",lenOfReqStr);
		return(UnionSetUserDefinedErrorCode(errCodeAPIClientReqLen));
	}
	
	memset(varName,0,sizeof(varName));
	strcpy(varName,"serviceChanelIDOf");
	memcpy(varName+17,reqStr,appIDLen+serviceIDLen);
	return(UnionReadLongTypeRECVar(varName));
}


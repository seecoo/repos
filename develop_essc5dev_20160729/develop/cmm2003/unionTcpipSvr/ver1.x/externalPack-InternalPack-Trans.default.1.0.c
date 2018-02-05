// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"

#include "externalPack-InternalPack-Trans.h"

/*
功能
	将外部请求报文转换为内部请求报文
输入参数
	exteranlReqPack		外部请求报文
	lenOfExternalReqPack	外部请求报文长度
	sizeOfInteranlReqPack	内部请求报文缓冲大小
输出参数
	interanlReqPack		内部请求报文
返回值
	>=0			内部请求报文长度
	<0			错误代码
*/
int UnionTransExternalReqPackIntoInternalReqPack(unsigned char *exteranlReqPack,int lenOfExternalReqPack,
		unsigned char *interanlReqPack,int sizeOfInteranlReqPack)
{
	if (sizeOfInteranlReqPack < lenOfExternalReqPack)
	{
		UnionUserErrLog("in UnionTransExternalReqPackIntoInternalReqPack:: sizeOfInteranlReqPack [%d] < expected [%d]!\n",
			sizeOfInteranlReqPack,lenOfExternalReqPack);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	if ((exteranlReqPack == NULL) || (interanlReqPack == NULL) || (lenOfExternalReqPack < 0))
	{
		UnionUserErrLog("in UnionTransExternalReqPackIntoInternalReqPack:: error parameter exteranlReqPack interanlReqPack = [%x] = [%x] lenOfExternalReqPack = [%d]!\n",
			 exteranlReqPack,interanlReqPack,lenOfExternalReqPack);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	memcpy(interanlReqPack,exteranlReqPack,lenOfExternalReqPack);
	return(lenOfExternalReqPack);
}

/*
功能
	将内部响应报文转换为外部响应报文
输入参数
	interanlResPack		内部响应报文
	lenOfInternalResPack	内部响应报文长度
	sizeOfExteranlResPack	外部响应报文缓冲大小
输出参数
	exteranlResPack		外部响应报文
返回值
	>=0			外部响应报文长度
	<0			错误代码
*/
int UnionTransInternalResPackIntoExternalResPack(unsigned char *interanlResPack,int lenOfInternalResPack,unsigned char *exteranlResPack,
		int sizeOfExteranlResPack)
{
	if (sizeOfExteranlResPack < lenOfInternalResPack)
	{
		UnionUserErrLog("in UnionTransInternalResPackIntoExternalResPack:: sizeOfExteranlResPack [%d] < expected [%d]!\n",
			sizeOfExteranlResPack,lenOfInternalResPack);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	if ((exteranlResPack == NULL) || (interanlResPack == NULL) || (lenOfInternalResPack < 0))
	{
		UnionUserErrLog("in UnionTransInternalResPackIntoExternalResPack:: error parameter exteranlResPack interanlResPack = [%x] = [%x] lenOfInternalResPack = [%d]!\n",
			 exteranlResPack,interanlResPack,lenOfInternalResPack);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	memcpy(exteranlResPack,interanlResPack,lenOfInternalResPack);
	return(lenOfInternalResPack);
}

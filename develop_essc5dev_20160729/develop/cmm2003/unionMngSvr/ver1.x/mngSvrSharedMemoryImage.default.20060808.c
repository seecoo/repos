// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/10
// Version:	1.0

// 文件服务器

#include <stdio.h>
#include <string.h>

#include "mngSvrServicePackage.h"
#include "unionErrCode.h"
#include "mngSvrCommProtocol.h"
#include "UnionLog.h"
#include "unionResID.h"
#include "unionTableField.h"

/* 功能
	共享内存映像命令解释器
输入参数
	handle		socket句柄
	resID		资源标识
	serviceID	命令字
	reqStr	请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件接收到
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionExcuteMngSvrSharedMemoryImageService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	UnionProgramerLog("in UnionExcuteMngSvrSharedMemoryImageService:: invalid fileSvr command [%d]\n",serviceID);
	return(errCodeEsscMDL_InvalidService);
}

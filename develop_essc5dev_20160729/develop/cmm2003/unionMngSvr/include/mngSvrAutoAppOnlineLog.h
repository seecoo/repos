// Author:	王纯军
// Date:	2010-6-2

#ifndef _mngSvrAutoAppOnlineLog_
#define _mngSvrAutoAppOnlineLog_

#include "mngSvrAutoAppOnlineLogWriter.h"

/*
功能
	根据授权记录初始化自动日志生成器
输入参数：
	serviceID	操作代码
输出参数
	无
返回值
	>=0	成功,返回域的数目
	<0	错误码
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(PUnionOperationAuthorization pauthRec);

/*
功能
	在自动生成流水表中插入请求
输入参数：
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertAutoAppOnlineLogRequestRec();

/*
功能
	在自动生成流水表中插入响应
输入参数：
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertAutoAppOnlineLogResponseRec();
#endif


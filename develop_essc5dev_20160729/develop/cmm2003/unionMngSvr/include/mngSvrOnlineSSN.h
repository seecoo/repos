// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2009/5/23
// Version:	1.0

#ifndef _mngSvrOnlineSSN_
#define _mngSvrOnlineSSN_

int UnionApplyMyMngSvrSSN();

void UnionRegisterMngSvrRequestService(int resID,int serviceID,char *tellerNo,int lenOfData,char *data);

void UnionRegisterMngSvrResponseService(int resID,int serviceID,int responseCode,int lenOfData,char *data,int fileRecved);

/*
功能：
	拼装当前流水的头
输入参数:
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormHeaderStrOfCurrentMngSvrOnlineRec(char *recStr,int sizeOfBuf);

#endif


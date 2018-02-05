// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#ifndef _mngSvrRecTransferred_
#define _mngSvrRecTransferred_

// 从字符串中读取记录域
// 返回域值的长度
int UnionReadMngSvrRecFldFromStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutMngSvrRecFldIntoStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

#endif

// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#ifndef _mngSvrRecTransferred_
#define _mngSvrRecTransferred_

// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadMngSvrRecFldFromStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutMngSvrRecFldIntoStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

#endif

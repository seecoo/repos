// Author:	������
// Date:	2010-6-2

#ifndef _mngSvrAutoAppOnlineLog_
#define _mngSvrAutoAppOnlineLog_

#include "mngSvrAutoAppOnlineLogWriter.h"

/*
����
	������Ȩ��¼��ʼ���Զ���־������
���������
	serviceID	��������
�������
	��
����ֵ
	>=0	�ɹ�,���������Ŀ
	<0	������
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRecForMngSvr(PUnionOperationAuthorization pauthRec);

/*
����
	���Զ�������ˮ���в�������
���������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInsertAutoAppOnlineLogRequestRec();

/*
����
	���Զ�������ˮ���в�����Ӧ
���������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInsertAutoAppOnlineLogResponseRec();
#endif


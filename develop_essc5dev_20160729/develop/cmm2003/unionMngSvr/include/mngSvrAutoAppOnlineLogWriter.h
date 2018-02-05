// Author:	������
// Date:	2010-6-2

#ifndef _mngSvrAutoAppOnlineLogWriter_
#define _mngSvrAutoAppOnlineLogWriter_

#include "unionOperationAuthorization.h"

// ����һ���Զ���ˮ�����ֵ������ʶ
typedef enum
{
	conAutoAppOnlineLogFldMethodReadReqStrFld = 0,	// �����󴮵���
	conAutoAppOnlineLogFldMethodReadResStrFld = 1,	// ����Ӧ������
	conAutoAppOnlineLogFldMethodReadTableFld = 2,	// �������
} TAutoAppOnlineLogFldAssignMethod;

// ����һ����ֵ����
typedef struct
{
	char					desFldName[40+1];	// ��ˮ���������
	char					oriFldName[40+1];	// Դ������
	TAutoAppOnlineLogFldAssignMethod	methodTag;		// ��ֵ������ʶ
} TUnionAutoAppOnlineLogFldDef;
typedef TUnionAutoAppOnlineLogFldDef	*PUnionAutoAppOnlineLogFldDef;

// ������ˮ��������Ŀ
#define conMaxFldNumOfAutoAppOnlineLogRec	32
// ����һ����ˮ�Զ�д�ṹ
typedef struct
{
	int				resID;			// ��ԴID
	int				serviceID;		// ����ID
	char				serviceName[40+1];	// ��������
	char				oriTableName[40+1];	// Դ������
	char				desTableName[40+1];	// Ŀ�������
	int				fldNum;			// ����Ŀ
	TUnionAutoAppOnlineLogFldDef	desTableFldGrp[conMaxFldNumOfAutoAppOnlineLogRec];
} TUnionAutoAppOnlineLogWriter;
typedef TUnionAutoAppOnlineLogWriter	*PUnionAutoAppOnlineLogWriter;

/*
����
	ƴװ��ָ�������γɵļ�¼��
���������
	pwriterDef	��������
	methodTag	������ʶ
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStr(PUnionAutoAppOnlineLogWriter pwriterDef,int methodTag,char *recStr,int sizeOfBuf);

/*
����
	ƴװ��¼����
���������
	pwriterDef	��������
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStrForRequest(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf);

/*
����
	ƴװ��¼��Ӧ��
���������
	pwriterDef	��������
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStrForResponse(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf);

/*
����
	��һ��������ȡ��ֵ����
���������
	fldDefStr	��ֵ��
	lenOfFldDefStr	��ֵ������
�������
	pfldDef		��ֵ����
����ֵ
	>=0	�ɹ�,�������������
	<0	������
*/
int UnionInitAutoAppOnlineLogFldWriterFromDefStr(char *fldDefStr,int lenOfFldDefStr,PUnionAutoAppOnlineLogFldDef pfldDef);

/*
����
	������Ȩ��¼��ʼ���Զ���־������
���������
	serviceID	��������
�������
	pwriterDef	��־����������
����ֵ
	>=0	�ɹ�,���������Ŀ
	<0	������
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRec(PUnionOperationAuthorization pauthRec,PUnionAutoAppOnlineLogWriter pwriterDef);

#endif


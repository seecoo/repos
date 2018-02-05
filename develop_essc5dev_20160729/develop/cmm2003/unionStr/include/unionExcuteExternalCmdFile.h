// Wolfgang Wang
// 2010-8-19

#ifndef _unionExcuteExternalCmdFile_
#define _unionExcuteExternalCmdFile_

// �����ⲿ���������
#define conMngSvrClientExternalCmdFunName		"UnionExcuteExternalCmd"

// �����ⲿ�����ָ��
#ifdef _WIN32_
typedef int (__stdcall *PUnionExternalCmdFun)(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
#else
typedef int (*PUnionExternalCmdFun)(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
#endif

/*
����
	ִ��һ���ⲿ����
�������
	reqStr		�ⲿ���������
	lenOfReqStr	�ⲿ��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExcuteExternalCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

/*
����
	ִ��һ�������ļ�
�������
	fileName	�����ļ�����
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		ִ�������ļ�֮��ƴ�ɵ���Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExecuteExternalCmdDefinedInSpecFile(char *fileName,char *resStr,int sizeOfResStr);

#endif

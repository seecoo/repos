//	Wolfgang Wang
//	2008/10/6

#ifndef _unionTestConf_
#define _unionTestConf_

/*
����	
	���ò���ʹ�õĿⶨ���ļ�
�������
	fileName	�ļ�����
�������
	��
����ֵ
	��
*/
void UnionSetTestLibDefFileName(char *fileName);

/*
����	
	������ɵĺ������Ա�����������
	����ļ�����Ϊ��$DIROFTEST/demo/mkfunName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunMakeFileName(char *funName,char *fileName);

/*
����	
	������ɵĺ������Գ��������
	����ļ�����Ϊ��$DIROFTEST/demo/test-funName.c
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunCProgramFileName(char *funName,char *fileName);

/*
����	
	������ɵĺ������Կ�ִ�г��������
	����ļ�����Ϊ��$DIROFTEST/demo/test-funName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetTestFunExeProgramFileName(char *funName,char *fileName);

/*
����	
	��ö����˿��ļ����ļ�������
	����ļ�����Ϊ��$DIROFTEST/conf/libFile.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetLibFileDefFileName(char *fileName);

/*
����	
	��ö�����ͷ�ļ�Ŀ¼�����ļ�������
	����ļ�����Ϊ��$DIROFTEST/conf/includeFileDir.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetIncludeFileDirDefFileName(char *fileName);

/*
����	
	��ö�����ͷ�ļ����ļ�������
	����ļ�����Ϊ��$DIROFAPITEST/conf/includeFile.def
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetIncludeFileDefFileName(char *fileName);

/*
����	
	��ö������������ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.input
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarInputValueListIndexFileName(char *funName,char *fileName);

/*
����	
	��ö������������ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.output
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarOutputValueListIndexFileName(char *funName,char *fileName);

/*
����	
	��ö���������ֵ����ֵ����������ļ�
	����ļ�����Ϊ��$DIROFTEST/funVarValueListIndex/funName.return
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFunVarReturnValueListIndexFileName(char *funName,char *fileName);

/*
����	
	��ò��������ļ�ȫ��
	����ļ�����Ϊ��$DIROFTEST/testdata/fileName
�������
	��
�������
	�ļ�����
����ֵ
	��
*/
void UnionGetFullFileNameOfTestData(char *fileName,char *fullFileName);

#endif

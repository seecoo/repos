//	Wolfgang Wang
//	2008/10/6

#ifndef _unionGenerateMakeFile_
#define _unionGenerateMakeFile_


/*
����	
	����ͷ�ļ�Ŀ¼�Ķ������ӵ�makefile��
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddIncludeDirDefToMakeFile(FILE *fp);

/*
����	
	�����ļ��������ӵ�makefile��
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddLibDefToMakeFile(FILE *fp);


/*
����	
	Ϊһ�����Գ������һ�������ļ�
�������
	cFileName		���Գ����ļ�����
	exeFileName		���ɵĿ�ִ�г�������
	makeFileName		�����ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMakeFileForTestProgramForFun(char *cFileName,char *exeFileName,char *makeFileName);

#endif


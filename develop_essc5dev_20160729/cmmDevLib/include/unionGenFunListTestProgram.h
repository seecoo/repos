//	Wolfgang Wang
//	2008/2/25

#ifndef _unionGenFunListTestProgram_
#define _unionGenFunListTestProgram_

/*
����
	����һ�����ú����ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateCallFunName(int funIndex,char *funName,FILE *outFp);

/*
����
	����һ�������������ֵ�ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateInputVarSetFunName(int funIndex,char *funName,FILE *outFp);

/*
����
	����һ����ʾ��������ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateOutputVarDisplayFunName(int funIndex,char *funName,FILE *outFp);

/*
����
	����һ����ʾ����ֵ�ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateReturnVarDisplayFunName(int funIndex,char *funName,FILE *outFp);

/*
����	
	Ϊһ��������������ǰ׺
�������
	funIndex	�������
�������
	funVarPrefix	����ǰ׺
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarNamePrefixOfFunGrp(int funIndex,char *funVarPrefix);

/*
����	
	����һ��Ϊһ�麯����ָ���ͺ��������ͷſռ�ĺ���
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�ĺ���
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
����	
	���������弰Ϊ������������ı�������д�뵽���Գ�����
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
����	
	����һ����ȡ����ֵ����������ļ�
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarValueTagListIndexFileOfFunGrp(char funGrp[][128+1],int numOfFun);

/*
����	
	����һ���Ժ���������������и�ֵ�ĺ���
�������
	funIndex	�����ں���ϵ���е�������
	funName		��������
	testDataFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunInputVarOfFunGrpAssignmentFun(int funIndex,char *funName,char *testDataFileName,FILE *fp);

/*
����	
	����һ���Ժ����������������չʾ�ĺ���
�������
	funIndex	���������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunOutputVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp);

/*
����	
	����һ���Ժ����ķ��ز�������չʾ�ĺ���
�������
	funIndex	���������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunReturnVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp);

/*
����	
	����һ��Ϊ�������ɵ��ú����ĺ���
�������
	funIndex	�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCallFunOfFunGrpToTestProgram(int funIndex,char *funName,FILE *fp);

/*
����	
	���ļ��ж�ȡһ��Ҫ���Եĺ�������
�������
	testFileName	���������ļ�����
	numOfFun	�����嵥�п��Էŵĺ����������Ŀ
�������
	funGrp		���������嵥
����ֵ
	>=0		�����ĺ�������Ŀ
	<0		�������
*/
int UnionGetFunGrpListFromFile(char *testDataFileName,char funGrp[][128+1],int numOfFun);

/*
����	
	����һ��������
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMainFunOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
����	
	Ϊһ�麯���Զ�����һ�鴦����
�������
	testFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAutoGenerateFunForFunGrp(char *testFileName,FILE *fp);

/*
����	
	Ϊһ���������в���һ�����Գ���
�������
	cFileName	�ļ�����
	testDataFileName	���������ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAutoGenerateFunForFunGrpToSpecCFile(char *testDataFileName,char *cFileName);

#endif

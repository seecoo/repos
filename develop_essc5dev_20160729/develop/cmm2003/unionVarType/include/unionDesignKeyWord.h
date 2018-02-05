//	Wolfgang Wang
//	2008/10/6

#ifndef _unionDesignKeyWord_
#define _unionDesignKeyWord_

// ���������ʶ
#define conDesginKeyWordTagConst		1
// ���������ʶ
#define conDesginKeyWordTagFun			10
// ȫ�ֱ��������ʶ
#define conDesginKeyWordTagGlobalVar		11
// ���Ͷ����ƫ�ƣ��Ӹ�ƫ�Ʊ���50�������Ͷ�����
#define conDesginKeyWordTagVarTypeOffset	100

/*
����	
	���һ���������ڳ����ȫ��
�������
	funName		��������
�������
	fullProgramName	�����ȫ��
	incFileName	����Ӧ������ͷ�ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetFullProgramFileNameOfFun(char *funName,char *fullProgramName,char *incFileName);

/*
����	
	��ʾһ�����͵Ķ���
�������
	typeTag		ָ��������
	keyWord		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayKeyWordDefOfSpecTypeToFp(int typeTag,char *keyWord,FILE *fp);

/*
����	
	��ʾһ�����͵Ķ���
�������
	keyWord		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayKeyWordDefToFp(char *keyWord,FILE *fp);

/*
����	
	��ʾһ�����͵Ķ���
�������
	keyWord		��������
	fileName	��һ�����Ͷ���д�뵽�ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputKeyWordDefToSpecFile(char *keyWord,char *fileName);

/*
����	
	��ʾһ�����͵����ж���
�������
	nameOfType	��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayAllDefOfSpecKeyWordToFp(char *keyWord,FILE *fp);

#endif

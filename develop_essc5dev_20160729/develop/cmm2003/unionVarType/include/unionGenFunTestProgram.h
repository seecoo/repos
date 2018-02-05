//	Wolfgang Wang
//	2008/2/25

#ifndef _unionGenFunTestProgram_
#define _unionGenFunTestProgram_

#include "unionFunDef.h"

/*
����	
	���õ�ǰ���õı����Ƿ��Ƕ�����
�������
	isBinary	�����Ƿ��Ƕ����Ƶı�־
�������
	��
����ֵ
	��
*/
void UnionSetCurrentVarValueAsBinary(int isBinary);

/*
����	
	���õ�ǰ���õı����ĳ���
�������
	varValueLen	�����ĳ���
�������
	��
����ֵ
	��
*/
void UnionSetCurrentVarValueLenName(char *varValueLen);

/*
����	
	���̶��Ĳ��Դ���д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	���������
*/
int UnionAddFixedCodesToFile(FILE *fp);


/*
����	
	д�����������Ϣ
�������
	fp		���Գ�����
�������
	��
����ֵ
	��
*/
void UnionAddAutoGenerateInfoToTestProgram(FILE *fp);

/*
����	
	����һ�����ַ������ͽ��и�ֵ�����
�������
	localVarPrefix	����ǰ׺
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	varValueLenTag	����ֵ�ĳ���
	isBinary	����ֵ�Ƿ��Ƕ�����,1,��,0,��
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateStrAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp);

/*
����	
	����һ����ָ�����ͽ��и�ֵ�����
�������
	localVarPrefix	����ǰ׺
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	varValueLenTag	����ֵ�ĳ���
	isBinary	����ֵ�Ƿ��Ƕ�����,1,��,0,��
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGeneratePointerAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp);

/*
����	
	����һ���Ի����������ͽ��и�ֵ�����
�������
	varPrefix	����ǰ׺
	baseType	��������
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAssignmentCSentenceToFile(char *varPrefix,int baseType,char *varValueTag,int valueIsVarName,char *value,FILE *fp);

/*
����	
	��ͷ�ļ�д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	ͷ�ļ�������
*/
int UnionAddIncludeFileToTestProgram(FILE *fp);

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�Ĵ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp);

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�ĺ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp);

/*
����	
	����һ��Ϊָ���ͺ��������ͷſռ�Ĵ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp);

/*
����	
	����һ��Ϊָ���ͺ��������ͷſռ�ĺ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp);

/*
����	
	����һ��Ϊ�������ɵ��ú���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCallFunOfFunToTestProgram(char *funName,FILE *fp);

/*
����	
	����һ����ȡ����ֵ����������ļ�
�������
	pdef		��������ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarValueTagListIndexFile(PUnionFunDef pdef);

/*
����	
	���������弰Ϊ������������ı�������д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddFunDefAndFunVarListToTestProgram(char *funName,FILE *fp);

/*
����	
	����һ���Ժ���������������и�ֵ�ĺ���
�������
	funName		��������
	testDataFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunInputVarAssignmentFun(char *funName,char *testDataFileName,FILE *fp);

/*
����	
	����һ���Ի����������ͽ��д�ӡ�����
�������
	varPrefix	����ǰ׺
	baseType	��������
	varValueTag	��������
	value		����ֵ
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintOutputVarCSentenceToFp(char *varPrefix,int baseTypeTag,char *varValueTag,FILE *fp);

/*
����	
	����һ���Ժ����������������չʾ�ĺ���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunOutputVarDisplayFun(char *funName,FILE *fp);

/*
����	
	����һ���Ժ����ķ��ز�������չʾ�ĺ���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunReturnVarDisplayFun(char *funName,FILE *fp);

/*
����	
	Ϊһ��������
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMainFunToTestProgram(FILE *fp);

/*
����	
	Ϊһ����������һ�����Գ���
�������
	funName		��������
	fileName	�ļ�����
	testDataFileName	���������ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateTestProgramForFun(char *funName,char *testDataFileName,char *fileName);

/*
����	
	��ȡ�̶�����Ķ����ļ�
�������
	��
�������
	fileName	����̶�����Ķ����ļ�
����ֵ
	>=0		��ȷ
	<0		������
*/
int UnionFileNameOfFixedCodesConfFile(char *filename);

/*
����	
	���ù̶�����Ķ����ļ�
�������
	fileName	����̶�����Ķ����ļ�
�������
	��
����ֵ
	>=0		��ȷ
	<0		������
*/
int UnionSetFileNameOfFixedCodesConfFile(char *filename);

#endif

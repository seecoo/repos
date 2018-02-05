// wolfang wang
// 2008/10/3

#ifndef _unionVarTypeDefFile_
#define _unionVarTypeDefFile_

#include "unionVarTypeDef.h"

/*
����	
	���һ���ؼ������ڵĳ�������
�������
	keyWord		��������
�������
	programName	��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetProgramFileNameOfKeyWord(char *keyWord,char *nameOfProgram);

/* �ж��Ƿ����һ�����͵Ķ���
�������
	fileName	�ļ�����
	nameOfType	ָ��������
�������
	��
����ֵ��
	>=1 	����
	0	������	
	<0	����
	
*/
int UnionExistsVarTypeDefInSpecFile(char *fileName,char *nameOfType);

/* �ж�ȱʡ�����ļ����Ƿ����һ�����͵Ķ���
�������
	nameOfType	ָ��������
�������
	��
����ֵ��
	>=1 	����
	0	������	
	<0	����
	
*/
int UnionExistsVarTypeDefInDefaultDefFile(char *nameOfType);

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromSpecFile(char *fileName,char *nameOfType,PUnionVarTypeDef pdef);

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromDefaultFile(char *nameOfType,PUnionVarTypeDef pdef);

/* �����ͱ�ʶ�Ķ���д�뵽ָ�����ļ���
�������
	pdef		���ͱ�ʶ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteVarTypeDefIntoSpecFile(PUnionVarTypeDef pdef,char *fileName);

/* �����ͱ�ʶ�Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		���ͱ�ʶ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteVarTypeDefIntoDefaultFile(PUnionVarTypeDef pdef);

#endif

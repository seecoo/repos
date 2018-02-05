//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionStructDefFile_
#define _unionStructDefFile_

#include "unionVarDef.h"
#include "unionStructDef.h"

/* ��ȱʡ�����ļ���ָ�����ƵĽṹ�Ķ���
�������
	nameOfType	�ṹ����
�������
	pstructDef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadStructDefFromDefaultDefFile(char *nameOfType,PUnionStructDef pstructDef);

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
�������
	pstructDef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadStructDefFromSpecFile(char *fileName,PUnionStructDef pstructDef);

/* ���ṹ�Ķ���д�뵽ָ�����ļ���
�������
	pdef		�ṹ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteStructDefIntoSpecFile(PUnionStructDef pdef,char *fileName);

/* ���ṹ�Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		�ṹ����
	programFileName	�ṹ���ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteStructDefIntoDefaultFile(PUnionStructDef pdef,char *programFileName);

#endif

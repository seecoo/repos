//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionArrayDefFile_
#define _unionArrayDefFile_

#include "unionArrayDef.h"

/* ��ȱʡ�ļ���ָ�����Ƶ���������Ͷ���
�������
	nameOfType	�������͵�����
�������
	typeNameOfArray	�������͵�����
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionReadTypeOfArrayDefFromDefaultDefFile(char *nameOfType,char *typeNameOfArray);

/* ��ȱʡ�����ļ���ָ�����Ƶ�����Ķ���
�������
	nameOfType	��������
�������
	pdef	���������鶨��
����ֵ��
	>=0 	����������Ĵ�С
	<0	�������	
	
*/
int UnionReadArrayDefFromDefaultDefFile(char *nameOfType,PUnionArrayDef pdef);

/* ��ָ���ļ���ָ�����Ƶ�����Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������鶨��
����ֵ��
	>=0 	����������Ĵ�С
	<0	�������	
	
*/
int UnionReadArrayDefFromSpecFile(char *fileName,PUnionArrayDef pdef);

#endif

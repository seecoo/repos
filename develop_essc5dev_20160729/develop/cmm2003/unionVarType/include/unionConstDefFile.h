// wolfang wang
// 2008/10/3

#ifndef _unionConstDefFile_
#define _unionConstDefFile_

#include "unionConstDef.h"

/*
����	
	��ȱʡ�����ļ���ȡ������ֵ
�������
	constName	��������
	previousName	constName��Ӧ��ֵ
�������
	constValue	����ֵ
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadConstValueFromDefaultDefFile(char *constName,char *previousName,char *constValue);

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadConstDefFromSpecFile(char *fileName,PUnionConstDef pdef);

#endif

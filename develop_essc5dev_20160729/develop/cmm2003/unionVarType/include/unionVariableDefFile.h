// wolfang wang
// 2008/10/3

#ifndef _unionVariableDefFile_
#define _unionVariableDefFile_

#include "unionVarDef.h"
#include "unionVariableDef.h"

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVariableDefFromSpecFile(char *fileName,PUnionVariableDef pdef);

/* ��ȱʡ�����ļ���ָ�����Ƶ�ȫ�ֱ����Ķ���
�������
	nameOfVariable	ȫ�ֱ�������
�������
	pdef	������ȫ�ֱ�������
����ֵ��
	>=0 	������ȫ�ֱ����Ĵ�С
	<0	�������	
	
*/
int UnionReadVariableDefFromDefaultDefFile(char *nameOfVariable,PUnionVariableDef pdef);

#endif

// wolfang wang
// 2008/10/3

#ifndef _unionFunDefFile_
#define _unionFunDefFile_

#include "unionFunDef.h"

/*
����	
	��ȱʡ�����ļ���ȡһ����������
�������
	funName		��������
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromDefaultDefFile(char *funName,PUnionFunDef pdef);

/*
����	
	���ļ���ȡһ����������
�������
	fileName	�ļ�����
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromSpecFile(char *fileName,PUnionFunDef pdef);

#endif

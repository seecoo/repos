// wolfang wang
// 2008/10/3

#ifndef _unionModuleDefFile_
#define _unionModuleDefFile_

#include "unionModuleDef.h"

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
nameOfModulee	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadModuleDefFromSpecFile(char *fileName,char *nameOfModule,PUnionModuleDef pdef);

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
nameOfModulee	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadModuleDefFromDefaultFile(char *nameOfModule,PUnionModuleDef pdef);

#endif

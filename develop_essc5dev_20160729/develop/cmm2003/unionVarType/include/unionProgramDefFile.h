// wolfang wang
// 2008/10/3

#ifndef _unionProgramDefFile_
#define _unionProgramDefFile_

#include "unionProgramDef.h"

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
nameOfPrograme	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadProgramDefFromSpecFile(char *fileName,char *nameOfProgram,PUnionProgramDef pdef);

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
nameOfPrograme	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadProgramDefFromDefaultFile(char *nameOfProgram,PUnionProgramDef pdef);

#endif

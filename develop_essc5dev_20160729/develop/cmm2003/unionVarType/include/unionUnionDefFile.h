//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionUnionDefFile_
#define _unionUnionDefFile_

#include "unionVarDef.h"
#include "unionUnionDef.h"

/* ��ȱʡ�����ļ���ָ�����ƵĽṹ�Ķ���
�������
	nameOfType	�ṹ����
�������
	punionDef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadUnionDefFromDefaultDefFile(char *nameOfType,PUnionUnionDef punionDef);

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
�������
	punionDef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadUnionDefFromSpecFile(char *fileName,PUnionUnionDef punionDef);


#endif

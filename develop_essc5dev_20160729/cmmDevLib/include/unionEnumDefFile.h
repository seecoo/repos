//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionEnumDefFile_
#define _unionEnumDefFile_

#include "unionEnumDef.h"

/* ��ȱʡ�����ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	nameOfType	ö������
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
*/
int UnionReadEnumDefFromDefaultDefFile(char *nameOfType,PUnionEnumDef pdef);

/* ��ָ���ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	fileName	�ļ�����
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
*/
int UnionReadEnumDefFromSpecFile(char *fileName,PUnionEnumDef pdef);

#endif

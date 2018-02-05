//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionSimpleTypeDefFile_
#define _unionSimpleTypeDefFile_

#include "unionSimpleTypeDef.h"

/* ��ָ���ļ���ָ�����Ƶļ����͵Ķ���
�������
	fileName	�ļ�����
�������
	pdef	�����ļ����Ͷ���
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionReadSimpleTypeDefFromSpecFile(char *fileName,PUnionSimpleTypeDef pdef);

/* ��ȱʡ�����ļ���ָ�����Ƶļ����͵Ķ���
�������
	nameOfType	�ṹ����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadSimpleTypeDefFromDefaultDefFile(char *nameOfType,PUnionSimpleTypeDef pdef);

/* �������͵Ķ���д�뵽ָ�����ļ���
�������
	pdef		�����Ͷ���
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSimpleTypeDefIntoSpecFile(PUnionSimpleTypeDef pdef,char *fileName);

/* �������͵Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		�����Ͷ���
	programFileName	���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSimpleTypeDefIntoDefaultFile(PUnionSimpleTypeDef pdef,char *programFileName);

/* �������͵Ķ���д�뵽ȱʡ���ļ���
�������
	nameOfType	�����Ͷ���
	name		��������
	remark		˵��
	programFileName	���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSpecSimpleTypeDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName);


#endif

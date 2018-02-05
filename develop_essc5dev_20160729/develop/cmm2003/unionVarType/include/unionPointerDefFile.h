//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionPointerDefFile_
#define _unionPointerDefFile_

#include "unionPointerDef.h"

/* ��ȱʡ�ļ���ָ�����Ƶ�ָ��Ķ���
�������
	nameOfType	ָ�����͵�����
�������
	pdef	������ָ�붨��
����ֵ��
	>=0 	������ָ��Ĵ�С
	<0	�������	
	
*/
int UnionReadPointerDefFromDefaultDefFile(char *nameOfType,PUnionPointerDef pdef);

/* ��ָ���ļ���ָ�����Ƶ�ָ��Ķ���
�������
	fileName	�ļ�����
�������
	pdef	������ָ�붨��
����ֵ��
	>=0 	������ָ��Ĵ�С
	<0	�������	
	
*/
int UnionReadPointerDefFromSpecFile(char *fileName,PUnionPointerDef pdef);

/* ��ָ�����͵Ķ���д�뵽ָ�����ļ���
�������
	pdef		ָ�����Ͷ���
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWritePointerDefIntoSpecFile(PUnionPointerDef pdef,char *fileName);

/* ��ָ�����͵Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		ָ�����Ͷ���
	programFileName	ָ���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWritePointerDefIntoDefaultFile(PUnionPointerDef pdef,char *programFileName);

/* ��ָ�����͵Ķ���д�뵽ȱʡ���ļ���
�������
	nameOfType	ָ�����Ͷ���
	name		��������
	remark		˵��
	programFileName	ָ���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSpecPointerDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName);

#endif

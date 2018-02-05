//	Author: Wolfgang Wang
//	Date: 2008/3/6

#ifndef _unionComplexDBObjectFileName_
#define _unionComplexDBObjectFileName_

#include "unionComplexDBCommon.h"

/* **** ��һ����	����Ĵ���	Ŀ¼���ļ����� **** */

// ���ܣ����һ���������ִ��ǰ�Ĵ�������������
/*
���������
	idOfObject	���������
���������
	fileName	�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectInsertBeforeTriggerFileName(TUnionIDOfObject idOfObject,char *fileName);

// ���ܣ����һ������һ��������ļ�����
/*
���������
	idOfObject	���������
���������
	fileName	������Ĵ����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefCreateFileName(TUnionIDOfObject idOfObject,char *fileName);

/* **** �ڶ�����	����Ķ���	Ŀ¼���ļ����� **** */

// ���ܣ����һ��������Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
���������
	objectDir	������Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefDirName(TUnionIDOfObject idOfObject,char *objectDir);

// ���ܣ����һ������Ķ����ļ�����
/*
���������
	idOfObject	���������
���������
	defFileName	����Ķ����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectDefFileName(TUnionIDOfObject idOfObject,char *defFileName);

// ���ܣ����һ��������ⲿ�ؼ��ֶ����嵥�ļ�����
/*
���������
	idOfObject	���������
���������
	defFileName	������ⲿ�ؼ��ֶ����嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectChildrenDefFileName(TUnionIDOfObject idOfObject,char *defFileName);

/* **** ��������	����ļ�¼	Ŀ¼���ļ����� **** */

// ���ܣ�������ж����¼�Ĵ洢Ŀ¼
/*
���������
	��
���������
	objectDir	�����¼�Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetAllObjectRecDirName(char *objectDir);

// ���ܣ�������ж����¼�Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
���������
	objectDir	�����¼�Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetAllObjectRecDirName(char *objectDir);

// ���ܣ����һ�������¼�Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
���������
	objectDir	����Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecMainDirName(TUnionIDOfObject idOfObject,char *objectDir);

// ���ܣ����һ������ļ�ֵ�����ļ�����
/* ���������
	idOfObject	���������
���������
	objectDir	��ֵ�����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjecRecPrimaryKeyIndexFileName(TUnionIDOfObject idOfObject,char *fileName);

// ���ܣ����һ�������Ψһֵ�ļ�����
/*
���������
	idOfObject	���������
	fldGrpName	Ψһֵ������
���������
	objectDir	��ֵ�����ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecUniqueKeyIndexFileName(TUnionIDOfObject idOfObject,char *fldGrpName,char *fileName);

// ���ܣ����һ������ʵ���Ĵ洢Ŀ¼
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	dir		����ʵ���Ĵ洢Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecDirName(TUnionIDOfObject idOfObject,char *primaryKey,char *dir);

// ���ܣ����һ������ʵ����ֵ�ļ�����
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	fileName	����ʵ����ֵ�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecValueFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName);

// ���ܣ����һ������ʵ�����õĶ���ʵ���嵥�ļ�����
/*
���������
	idOfObject	���������
	primaryKey	����ʵ���Ĺؼ���
���������
	fileName	���õĶ���ʵ���嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecParentsListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName);

// ���ܣ����һ��ʹ�øö���ʵ���Ķ���ʵ���嵥�ļ�����
/*
���������
	objectName�������������
	primaryKey��������ʵ���Ĺؼ���
	foreignObjectName���ⲿ���������
���������
	fileName	ʹ�øö���ʵ���Ķ���ʵ���嵥�ļ�����
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetObjectRecChildListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *foreignObjectName,char *fileName);

// ���ܣ����˵� ������Ŀ¼ �ַ���
/*
���������
	absolutePath������·��
���������
	relativelyPath:	���·��
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionFilterMainWorkingDirStr(char *absolutePath, char *relativelyPath);

// ���ܣ���ֵ�ļ������·����Ϊ����·��
/*
���������
	valueFileName		ֵ�ļ�����
���������
	fullValueFileName:	ֵ�ļ�����·��
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionPatchMainDirToObjectValueFileName(char *valueFileName, char *fullValueFileName);

// ���ܣ���ö����������Ŀ¼
/*
���������
	��
���������
	mainDir		�����������Ŀ¼
����ֵ��
	>=0���ɹ�
	<0��������
*/
int UnionGetMainObjectDir(char *mainDir);

// ���ܣ����һ�����������������ļ������ж��ļ��Ƿ����
/*
���������
	idOfObject	���������
	operationTag	������ʶ
	beforeOrAfter	�ǲ���֮ǰִ�л���֮��ִ��
���������
	fileName	�ļ�����
����ֵ��
	>0���ļ�����
	=0���ļ�������
	<0��������
*/
int UnionGetObjectTriggerFileName(TUnionIDOfObject idOfObject,char *operationTag,char *beforeOrAfter,char *fileName);

#endif

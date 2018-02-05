//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _unionComplexDBObject_
#define _unionComplexDBObject_

#include "unionComplexDBObjectDef.h"

int UnionConnectDatabase();

int UnionCloseDatabase();

/*
���ܣ�����Ԥ�ȶ����SQL�ļ�����һ������
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObject(TUnionIDOfObject idOfObject);

/*
���ܣ�
	����һ����������Ѵ��ڣ���ɾ��
���������
	prec	����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectAnyway(PUnionObject prec);

/*
���ܣ�ɾ��һ������
���������
	idOfObject�������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionDropObject(TUnionIDOfObject idOfObject);

#endif

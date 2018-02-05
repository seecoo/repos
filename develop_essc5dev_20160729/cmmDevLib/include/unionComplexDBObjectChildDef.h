//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBObjectChildDef_
#define _unionComplexDBObjectChildDef_

/*
���ܣ����һ��������ⲿ���ö���Ǽ��ļ��Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectForeignObjDefFile(TUnionIDOfObject idOfObject);

/*
���ܣ�����һ��������ⲿ���ö���Ǽ��ļ�
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectForeignObjDefFile(TUnionIDOfObject idOfObject);

/*
���ܣ�����Ƿ����������������˱�����
���������
	idOfObject������ID
���������
	��
����ֵ��
	>0���������������ñ�����
	0��û�������������ñ�����
	<0��ʧ�ܣ�������
*/
int UnionExistForeignObjectInfo(TUnionIDOfObject idOfObject);

/*
���ܣ��Ǽ����ñ�������ⲿ�����������Ϣ
���������
	idOfObject��������ID
	foreignObjectGrp�����ñ�������ⲿ����ؼ��ֶ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionRegisterForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp);

/*
���ܣ�ȡ�����ñ�������ⲿ����������Ϣ�ĵǼ�
���������
	idOfObject��������ID
	foreignObjectGrp�����ñ�������ⲿ����ؼ��ֶ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCancelRegisterOfForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp);

#endif

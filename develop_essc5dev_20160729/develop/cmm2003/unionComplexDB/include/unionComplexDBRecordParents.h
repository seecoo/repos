//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _recordParents_
#define _recordParents_

/*
���ܣ����һ������ʵ�����ڲ������ļ��Ƿ����
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ�����һ������ʵ�����ڲ������ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ���һ������ʵ�����ڲ������ļ���ɾ��һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp��������ı����������������ʽΪ"��������1.��������2.��������3����������N"��
		��������������˳���뱾�������ļ��ж���ı�������˳��һ��
	idOfForeignObject���ⲿ�����ID
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDelObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey);

/*
���ܣ���һ������ʵ�����ڲ������ļ�������һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp��������ı����������������ʽΪ"��������1.��������2.��������3����������N"��
		��������������˳���뱾�������ļ��ж���ı�������˳��һ��
	idOfForeignObject���ⲿ�����ID
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey);

/*
���ܣ�ȡ��һ��ʵ�����õĶ���ʵ���Ĺؼ���
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	localFldGrp�������������ƣ���ʽΪ"��1������.��2������.��3�����ơ���N������"��
		�������˳����������ļ����ⲿ�ؼ��ֵı�����������˳��һ��
	idOfForeignObject���ⲿ���������
	sizeOfPrimaryKey��foreignPrimaryKey�Ĵ洢�ռ��С
���������
	foreignPrimaryKey�����������õ��ⲿ����ʵ���Ĺؼ�����ֵ��
		��ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
����ֵ��
	>=0���ɹ�������foreignPrimaryKey�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionObtainForeignObjectRecPrimaryKey(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey,int sizeOfPrimaryKey);

#endif

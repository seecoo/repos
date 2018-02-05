//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _recordChild_
#define _recordChild_

/*
���ܣ����һ������ʵ�����ⲿ�����ļ��Ƿ����
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject);

/*
���ܣ���һ������ʵ�����ⲿ�����ļ�������һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
	foreignPrimaryKey���ⲿ����ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey);

/*
���ܣ���һ������ʵ�����ⲿ�����ļ���ɾ��һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
	foreignPrimaryKey���ⲿ����ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey);

/*
���ܣ����һ�������ʵ���Ƿ����������ʵ������
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��	
���������
	��
����ֵ��
	1����
	0������
	<0��ʧ�ܣ�������
*/
int UnionExistForeignObjectRecordDef(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey);

#endif

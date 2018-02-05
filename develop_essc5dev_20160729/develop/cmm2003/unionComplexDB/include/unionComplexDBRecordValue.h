//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _recordValue_
#define _recordValue_

/*
���ܣ����һ������ʵ����ֵ�ļ��Ƿ����
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
int UnionExistsObjectRecValueFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ�����һ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	record���¼�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord���¼�¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ���ɾ��һ������ʵ����ֵ�ļ�
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
int UnionDeleteObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ��޸�һ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	record��Ҫ�޸ĵļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��Ҫ�޸ĵļ�¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ������ʵ����ֵ�ļ�
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	sizeOfRecord��record�Ĵ洢�ռ��С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);

#endif

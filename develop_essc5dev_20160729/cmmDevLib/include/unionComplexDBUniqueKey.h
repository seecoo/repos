//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _uniqueKey_
#define _uniqueKey_

/*
���ܣ����һ�������ĳΨһֵ�����ļ��Ƿ����
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName);

/*
���ܣ�����һ�������ĳΨһֵ�����ļ�
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName);

/*
���ܣ���һ�������ĳΨһֵ�����ļ�������һ���¼�¼��Ψһֵ
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey���¼�¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	storePosition���¼�¼�Ĵ洢λ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition);

/*
���ܣ���һ�������ĳΨһֵ�����ļ���ɾ��һ����¼��Ψһֵ
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey��Ҫɾ����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey);

/*
���ܣ����ݶ���ʵ����Ψһֵ���ʵ���Ĵ洢λ��
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	sizeofStorePosition��storePosition�Ĵ洢�ռ��С
���������
	storePosition����¼�Ĵ洢λ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecStoreStationByUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition,int sizeofStorePosition);

/*
���ܣ��ж϶���ʵ����Ψһֵ�Ƿ��ظ�
���������
	idOfObject������ID
	fldGrpName��Ψһֵ�����ƣ���ʽΪ"Ψһֵ��1������.Ψһֵ��2������.Ψһֵ��3�����ơ�Ψһֵ��N������"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
	uniqueKey����¼��Ψһֵ����ʽΪ"Ψһֵ��1��ֵ.Ψһֵ��2��ֵ.Ψһֵ��3��ֵ��Ψһֵ��N��ֵ"��
		Ψһֵ�������˳����������ļ��ж����Ψһֵ��˳��һ��
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ�������
*/
int UnionObjectRecUniqueKeyIsRepeat(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey);

#endif

//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _primaryKey_
#define _primaryKey_

/*
���ܣ����һ������ļ�ֵ�ļ��Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject);

/*
���ܣ�����һ������ļ�ֵ�ļ�
���������
	idOfObject������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionCreateObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject);

/*
���ܣ���һ������ļ�ֵ�ļ�������һ���¼�¼�ļ�ֵ
���������
	idOfObject������ID
	primaryKey���¼�¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	storePosition���¼�¼�Ĵ洢λ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition);

/*
���ܣ���һ������ļ�ֵ�ļ���ɾ��һ����¼�ļ�ֵ
���������
	idOfObject������ID
	primaryKey��Ҫɾ����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ����ݶ���ʵ���ļ�ֵ���ʵ���Ĵ洢λ��
���������
	idOfObject������ID
	primaryKey����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
	sizeofStorePosition��storePosition�Ĵ洢�ռ��С
���������
	storePosition����¼�Ĵ洢λ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecStoreStationByPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition,int sizeofStorePosition);

/*
���ܣ��ж϶���ʵ���ļ�ֵ�Ƿ��ظ�
���������
	idOfObject������ID
	primaryKey����¼�ļ�ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳����������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ�������
*/
int UnionObjectRecPaimaryKeyIsRepeat(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ���ö����ʵ����Ŀ
���������
	idOfObject������ID
���������
	��
����ֵ��
	>=0��ʵ����
	<0��ʧ�ܣ�������
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject);

// Mary add begin, 20081112
/*
���ܣ���ȡָ��������м�¼
���������
	tableName��	����
	maxRecNum	������Ի��������¼����
���������
	recKey		�ؼ��֣���һ���ؼ����ɶ���򹹳ɣ�ʹ��.�ֿ�������
����ֵ��
	>=0��		��¼��
	<0��		�������
*/
int UnionReadAllRecOfSpecTBL(char *tableName,char recKey[][128+1],int maxRecNum);
// Mary add end, 20081112

/*
function:
        �ж��Ƿ��Ƕ����еĹؼ��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
*/
int UnionIsPrimaryKeyFld(TUnionIDOfObject idOfObject, char *fldName);

/*
function:
        �ж��Ƿ��Ƕ����еĹؼ��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
*/
int UnionIsPrimaryKeyFldOfObject(PUnionObject pobject, char *fldName);

/*
function:
        �ж��Ƿ��Ƕ����е�Ψһֵ�ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               ��Ψһֵ�ֶ�
        0               ����Ψһֵ�ֶ�
*/
int UnionIsUniqueFld(TUnionIDOfObject idOfObject, char *fldName);

/*
function:
        �ж��Ƿ��Ƕ����е�Ψһ�ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               ��Ψһֵ�ֶ�
        0               ����Ψһֵ�ֶ�
*/
int UnionIsUniqueFldOfObject(PUnionObject pobject, char *fldName);
#endif

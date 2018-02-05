#include "UnionLog.h"
#include "UnionStr.h"

#include "unionDesKeyDB.h"
#include "desKeyDBRecord.h"

/*
���ܣ�����һ����Կ
���������
	idOfObject������
	record����Կ����ʽ��"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertDesKeyDBRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord)
{
	int		ret;
	TUnionDesKey	tDesKey;
	
	// ����¼��ת���ɽṹ����ʽ
	memset(&tDesKey, 0, sizeof(TUnionDesKey));
	if ((ret = UnionReadDesKeyFromRecStr(record, lenOfRecord, &tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyDBRecord, translate [%s] to TUnionDesKey failed.\n", record);
		return ret;
	}
	
	// ��ת���ɽṹ���Ժ�ļ�¼���� DES ��Կ����
	if ((ret = UnionInsertDesKeyIntoKeyDB(&tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyDBRecord, insert [%s] failed.\n", record);
		return ret;
	}
	
	UnionSuccessLog("in UnionInsertDesKeyDBRecord, insert [%s] success.\n", record);
	return 0;
}

/*
���ܣ����ݹؼ���ɾ��һ����Կ
���������
	idOfObject������ID
	primaryKey���ؼ��֣�"fld1=��ֵ|"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniqueDesKeyDBRecord(TUnionIDOfObject idOfObject,char *primaryKey)
{
	int			ret;
	char			fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	
	if ((ret = UnionConnectExistingDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueDesKeyDBRecord, UnionConnectExistingDesKeyDB failed.\n");
		return ret;
	}
	
	memset(fullName, 	0, sizeof(fullName));
	if ((ret = UnionReadRecFldFromRecStr(primaryKey, strlen(primaryKey),"fullName",fullName,sizeof(fullName))) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueDesKeyDBRecord:: UnionReadRecFldFromRecStr fullName!\n");
		goto exitflag;
	}
	
	if ((ret = UnionDeleteDesKeyFromKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueDesKeyDBRecord, UnionDeleteDesKeyFromKeyDB: [%s] failed.\n", primaryKey);
		goto exitflag;
	}

exitflag:
	UnionDisconnectDesKeyDB();
	if (ret < 0)
		return ret;
	else
		return 0;
}

/*
���ܣ��޸�һ����Կ
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateUniqueDesKeyDBRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int			ret;
	TUnionDesKey		tDesKey;
	char			caRecord[4096];
	
	memset(caRecord, 0, sizeof(caRecord));
	ret			= sprintf(caRecord, "%s%s", primaryKey, record);
	
	// ����¼��ת���ɽṹ����ʽ
	memset(&tDesKey, 0, sizeof(TUnionDesKey));
	if ((ret = UnionReadDesKeyFromRecStr(caRecord, ret, &tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueDesKeyDBRecord, translate [%s] to TUnionDesKey failed.\n", record);
		return ret;
	}
	
	// �޸ļ�¼
	if ((ret = UnionUpdateAllDesKeyFieldsInKeyDB(&tDesKey)) < 0)
	// if ((ret = UnionUpdateDesKeyAttrInKeyDB(&tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueDesKeyDBRecord, update [%s] failed.\n", record);
		return ret;	
	}
	
	UnionSuccessLog("in UnionUpdateUniqueDesKeyDBRecord, update [%s] success.\n", record);
	return 0;
}

/*
���ܣ����ݹؼ��ֲ�ѯһ����Կ
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"fld1=��ֵ|fld2=��ֵ|��|fldN=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueDesKeyDBRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	int			ret;
	TUnionDesKey		tDesKey;
	
	// ��ʼ�� DES ��Կ�ṹ��
	memset(&tDesKey,	0, sizeof(TUnionDesKey));
	if ((ret = UnionReadRecFldFromRecStr(primaryKey,strlen(primaryKey),"fullName",tDesKey.fullName,sizeof(tDesKey.fullName))) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadRecFldFromRecStr fullName!\n");
		return(ret);
	}
	
	// ��ȡDES��Կ��¼
	if ((ret = UnionReadDesKeyFromKeyDB(&tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadDesKeyFromKeyDB failed!\n");
		return(ret);
	}
	
	// �� DES ��Կ��¼�ṹ��ת���ɼ�¼��
	if ((ret = UnionPutDesKeyIntoRecStr(&tDesKey, record, sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadDesKeyFromRecStr failed!\n");
		return(ret);
	}
	
	UnionSuccessLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey, select [%s] success.\n", record);
	return ret;
}

/*
���ܣ�����Ψһֵ��ѯһ����Կ
���������
	idOfObject������ID
	uniqueKey������ʵ����Ψһֵ����ʽΪ"Ψһֵ��1=��ֵ|Ψһֵ��2=��ֵ|��Ψһֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueDesKeyDBRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
	return UnionSelectUniqueDesKeyDBRecordByPrimaryKey(idOfObject, uniqueKey, record, sizeOfRecord);	
}

/*
���ܣ�����ɾ����Կ
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ������Կ��Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeleteDesKeyDBRecord(TUnionIDOfObject idOfObject,char *condition)
{
	return 0;
}

/*
���ܣ������޸���Կ
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ���Կ��ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ���Կ��Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdateDesKeyDBRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord)
{
	return 0;
}

/*
���ܣ�������ѯ��Կ
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ��������Կ��ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ������Կ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectDesKeyDBRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	int			ret;
	
	if ((ret = UnionConnectExistingDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectDesKeyDBRecord, UnionConnectExistingDesKeyDB failed.\n");
		return ret;
	}
	
	if ((ret = UnionOutputAllDesKeyToRecStrFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectDesKeyDBRecord, UnionConnectExistingDesKeyDB failed.\n");
		goto exitflag;	
	}

exitflag:
	UnionDisconnectDesKeyDB();
	if (ret < 0)
		return ret;
	else
		return 0;
}

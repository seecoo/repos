#include "UnionLog.h"
#include "UnionStr.h"

#include "unionPKDB.h"
#include "pkDBRecord.h"

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
int UnionInsertPKDBRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord)
{
	int		ret;
	TUnionPK	tPK;
	
	// ����¼��ת���ɽṹ����ʽ
	memset(&tPK, 0, sizeof(TUnionPK));
	if ((ret = UnionReadPKFromRecStr(record, lenOfRecord, &tPK)) < 0)
	{
		UnionUserErrLog("in UnionInsertPKDBRecord, translate [%s] to TUnionPK failed.\n", record);
		return ret;
	}
	
	// ��ת���ɽṹ���Ժ�ļ�¼���� DES ��Կ����
	if ((ret = UnionInsertPKIntoKeyDB(&tPK)) < 0)
	{
		UnionUserErrLog("in UnionInsertPKDBRecord, insert [%s] failed.\n", record);
		return ret;
	}
	
	UnionSuccessLog("in UnionInsertPKDBRecord, insert [%s] success.\n", record);
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
int UnionDeleteUniquePKDBRecord(TUnionIDOfObject idOfObject,char *primaryKey)
{
	int			ret;
	char			fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	
	if ((ret = UnionConnectExistingPKDB()) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniquePKDBRecord, UnionConnectExistingPKDB failed.\n");
		return ret;
	}
	
	memset(fullName, 	0, sizeof(fullName));
	if ((ret = UnionReadRecFldFromRecStr(primaryKey, strlen(primaryKey),"fullName",fullName,sizeof(fullName))) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniquePKDBRecord:: UnionReadRecFldFromRecStr fullName!\n");
		goto exitflag;
	}
	
	if ((ret = UnionDeletePKFromKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniquePKDBRecord, UnionDeletePKFromKeyDB: [%s] failed.\n", primaryKey);
		goto exitflag;
	}

exitflag:
	UnionDisconnectPKDB();
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
int UnionUpdateUniquePKDBRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int			ret;
	TUnionPK		tPK;
	char			caRecord[4096];
	
	memset(caRecord, 0, sizeof(caRecord));
	ret			= sprintf(caRecord, "%s%s", primaryKey, record);
	
	// ����¼��ת���ɽṹ����ʽ
	memset(&tPK, 0, sizeof(TUnionPK));
	if ((ret = UnionReadPKFromRecStr(caRecord, ret, &tPK)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniquePKDBRecord, translate [%s] to TUnionPK failed.\n", record);
		return ret;
	}
	
	// �޸ļ�¼
	if ((ret = UnionUpdateAllPKFieldsInKeyDB(&tPK)) < 0)
	// if ((ret = UnionUpdatePKAttrInKeyDB(&tPK)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniquePKDBRecord, update [%s] failed.\n", record);
		return ret;	
	}
	
	UnionSuccessLog("in UnionUpdateUniquePKDBRecord, update [%s] success.\n", record);
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
int UnionSelectUniquePKDBRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	int			ret;
	TUnionPK		tPK;
	
	// ��ʼ�� DES ��Կ�ṹ��
	memset(&tPK,	0, sizeof(TUnionPK));
	if ((ret = UnionReadRecFldFromRecStr(primaryKey,strlen(primaryKey),"fullName",tPK.fullName,sizeof(tPK.fullName))) < 0)
	{
		UnionUserErrLog("in UnionSelectUniquePKDBRecordByPrimaryKey:: UnionReadRecFldFromRecStr fullName!\n");
		return(ret);
	}
	
	// ��ȡDES��Կ��¼
	if ((ret = UnionReadPKFromKeyDB(&tPK)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniquePKDBRecordByPrimaryKey:: UnionReadPKFromKeyDB failed!\n");
		return(ret);
	}
	
	// �� DES ��Կ��¼�ṹ��ת���ɼ�¼��
	if ((ret = UnionPutPKIntoRecStr(&tPK, record, sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniquePKDBRecordByPrimaryKey:: UnionReadPKFromRecStr failed!\n");
		return(ret);
	}
	
	UnionSuccessLog("in UnionSelectUniquePKDBRecordByPrimaryKey, select [%s] success.\n", record);
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
int UnionSelectUniquePKDBRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
	return UnionSelectUniquePKDBRecordByPrimaryKey(idOfObject, uniqueKey, record, sizeOfRecord);	
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
int UnionBatchDeletePKDBRecord(TUnionIDOfObject idOfObject,char *condition)
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
int UnionBatchUpdatePKDBRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord)
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
int UnionBatchSelectPKDBRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	int			ret;
	
	if ((ret = UnionConnectExistingPKDB()) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectPKDBRecord, UnionConnectExistingPKDB failed.\n");
		return ret;
	}
	
	if ((ret = UnionOutputAllPKToRecStrFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectPKDBRecord, UnionConnectExistingPKDB failed.\n");
		goto exitflag;	
	}

exitflag:
	UnionDisconnectPKDB();
	if (ret < 0)
		return ret;
	else
		return 0;
}

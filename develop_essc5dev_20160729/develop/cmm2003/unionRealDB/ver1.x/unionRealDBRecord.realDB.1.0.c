//	Author: zhangyongding
//	Date: 2008-12-3

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "unionResID.h"
#include "UnionStr.h"
#include "unionRec0.h"
#include "unionErrCode.h"
#include "unionRecFile.h"

#include "unionRealBaseDB.h"
#include "unionRealDBTransformStr.h"

#include "unionTableDef.h"

/*
���ܣ�����һ�������ʵ��
���������
	idOfObject������
	record��
		��ʽ1��"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
		��ʽ2��"SQL::(��1,��2,��,��N) values (��ֵ1,��ֵ2,��,��ֵN)"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecord(char * idOfObject,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		value[30000+1];
	char		tmpBuf1[1024+1];
	char		tmpBuf2[1024+1];
	//TUnionRec	rec;
	int		ret1;
	
	memset(value,0,sizeof(value));
	if ((ret = UnionIsSQLString(record,lenOfRecord,value,sizeof(value))) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		// 2009/11/9������������
		if ((lenOfRecord = UnionAutoAppendFldOfSpecTBL(idOfObject,record,lenOfRecord)) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionAutoAppendFldOfSpecTBL idOfObject = [%s]!\n",idOfObject);
			return(lenOfRecord);
		}
		// 2009/11/9�����������ӽ���
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,record,lenOfRecord,conTransformTypeInsert,value,sizeof(value))) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	ret = UnionInsertRealDBRecord(idOfObject,value,strlen(value));
	if (ret < 0)
	{
                if (ret != errCodeObjectMDL_PrimaryKeyIsRepeat)
        		UnionUserErrLog("in UnionInsertObjectRecord:: UnionInsertRealDBRecord!\n");
                else
                        UnionLog("in UnionInsertObjectRecord:: UnionInsertRealDBRecord!\n");
		return(ret);
	}
	// 2009/8/21��Wolfgang Wang added
#ifndef _withoutSynchronize_
	if ((ret1 = UnionSynchronizeComplexDBInsertToBrothers(idOfObject,record,lenOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionSynchronizeComplexDBInsertToBrothers!\n");
		//return(ret);
	}
#endif
	return(ret);
}

/*
���ܣ����ݹؼ���ɾ��һ�������ʵ��
���������
	idOfObject������ID
	primaryKey��
		��ʽ1��"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
		��ʽ2��"SQL::��ֵ��1=��ֵ and ��ֵ��2=��ֵ and ��ֵ��N=��ֵ"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniqueObjectRecord(char * idOfObject,char *primaryKey)
{
	int		i;
	int		ret;	
	char		condition[2048+1];
	//TUnionRec	rec;
	int		ret1;
	
	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(primaryKey,strlen(primaryKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,primaryKey,strlen(primaryKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	ret = UnionDeleteRealDBRecord(idOfObject,condition);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionDeleteRealDBRecord!\n");
		return(ret);
	}
	// 2009/8/21��Wolfgang Wang added
#ifndef _withoutSynchronize_
	if ((ret1 = UnionSynchronizeComplexDBDeleteToBrothers(idOfObject,primaryKey)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionSynchronizeComplexDBDeleteToBrothers!\n");
		//return(ret);
	}
#endif
	return(ret);
}

/*
���ܣ��ж�һ������Ƿ���Ҫ�޸�
���������
        fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
����ֵ��
        1:      ��Ҫ�޸�
        0��     ����Ҫ�޸�
*/
int UnionIsForeignKeyShouldUpdate(PUnionObjectFldGrp fldGrp,char *record)
{
        int     i;
        int     len = strlen(record);
        char	caValue[4096];
        int     iRet;

        if (fldGrp == NULL || record == NULL)
	{
		UnionUserErrLog("in UnionIsForeignKeyShouldUpdate:: parameter is error!\n");
		return(0);
	}
        /*
        UnionLog("In UnionIsForeignKeyShouldUpdate, the record is: [%s].\n", record);

        for (i=0;i<fldGrp->fldNum;i++)
        {
                UnionLog("In UnionIsForeignKeyShouldUpdate, the [%d] is: [%s].\n", i, fldGrp->fldNameGrp[i]);
        } */

        for (i=0;i<fldGrp->fldNum;i++)
        {
                iRet = UnionReadRecFldFromRecStr(record, len, fldGrp->fldNameGrp[i], caValue,sizeof(caValue));
                if (iRet > 0)
                {
                        UnionLog("In UnionIsForeignKeyShouldUpdate, return 1.\n");
                        return 1;
                }
        }
        
        return 0;
}

/*
���ܣ��޸�һ�������ʵ��
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
int UnionUpdateUniqueObjectRecord(char * idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		condition[1024+1];
	char		field[8192*4+1];
	//TUnionRec	rec;
	int		ret1;
	
        UnionProgramerLog("in UnionUpdateUniqueObjectRecord::primaryKey=[%s],lenOfRecord = [%04d] record=[%s]\n", primaryKey,lenOfRecord,record);
        
	// ����ֵ��
	memset(field,0,sizeof(field));
	if ((ret = UnionIsSQLString(record,lenOfRecord,field,sizeof(field))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,record,lenOfRecord,conTransformTypeUpdate,field,sizeof(field))) < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	// ����������
	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(primaryKey,strlen(primaryKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,primaryKey,strlen(primaryKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	UnionProgramerLog("in UnionUpdateUniqueObjectRecord:: field = [%s] condition = [%s]\n",field,condition);
	if ((ret = UnionUpdateRealDBRecord(idOfObject,field,condition)) < 0)
	{
		if (ret == -100 || ret == 1403)
			ret = errCodeDatabaseMDL_RecordNotFound;
                UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: field=[%s], condition=[%s]\n", field, condition);
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionUpdateRealDBRecord! ret = [%d]\n",ret);
		return(ret);
	}
	
#ifndef _withoutSynchronize_
	// 2009/8/21��Wolfgang Wang added
	if ((ret1 = UnionSynchronizeComplexDBUpdateToBrothers(idOfObject,primaryKey,record,lenOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionSynchronizeComplexDBUpdateToBrothers!\n");
		//return(ret);
	}
#endif
	return(ret);
}

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ����ֻѡ��ָ������
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
	fldGrp		Ҫѡ�������嵥��ÿ����֮����,�ָ���
	lenOfFldGrp	���嵥�ĳ���
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectFldGrpByPrimaryKey(char * idOfObject,char *primaryKey,char *fldGrp,int lenOfFldGrp,char *record,int sizeOfRecord)
{
	int		i;
	int		ret;	
	char		condition[2048+1];
	//TUnionRec	rec;

	int		expireTime = -1;
	char		sqlKey[128+1];

	// added 2013-05-29
        // ʹ��cache�ȴ�cache�ж�����
	expireTime = UnionGetDBMemcachedTableExpireTime(idOfObject);
	memset(sqlKey, 0, sizeof(sqlKey));
	if(expireTime != -1)
	{
		UnionGetDBSelectStatementKey(idOfObject, fldGrp, primaryKey, sqlKey);
		if((ret = UnionReadUniqueDDBRecFromMemcached(sqlKey, sizeOfRecord, record)) > 0)
		{
			UnionAuditLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionReadUniqueDDBRecFromMemcached table = [%s] conditon = [%s] ret = [%d] OK!\n", idOfObject, primaryKey, ret);
			return(ret);
		}
	}
	// end of addition 2013-05-29

	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(primaryKey,strlen(primaryKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,primaryKey,strlen(primaryKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	
	if ((ret = UnionSelectRealDBRecord(idOfObject,fldGrp,condition,record,sizeOfRecord,NULL)) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionSelectRealDBRecord!\n");
		return(ret);
	}

	// added 2013-05-29
	// ʹ��cache������ݸ��µ�cache
	if(expireTime != -1)
	{
		UnionWriteUniqueDBRecIntoMemcached(sqlKey, expireTime, ret, record);
		//UnionAuditLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionWriteUniqueDBRecIntoMemcached table = [%s] conditon = [%s] len = [%d] !!\n", idOfObject, primaryKey, ret);
	}
	// end of addition 2013-05-29

	return(ret);
}

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueObjectRecordByPrimaryKey(char * idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	int		i;
	int		ret;	
	char		condition[2048+1];
	//TUnionRec	rec;

	int                     expireTime = -1;
        char                    sqlKey[128+1];

        // added 2013-05-29
        // ʹ��cache�ȴ�cache�ж�����
	expireTime = UnionGetDBMemcachedTableExpireTime(idOfObject);
	memset(sqlKey, 0, sizeof(sqlKey));
	if(expireTime != -1)
	{
		UnionGetDBSelectStatementKey(idOfObject, "*", primaryKey, sqlKey);
		if((ret = UnionReadUniqueDDBRecFromMemcached(sqlKey, sizeOfRecord, record)) > 0)
		{
			UnionAuditLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionReadUniqueDDBRecFromMemcached table = [%s] conditon = [%s] ret = [%d] OK!\n", idOfObject, primaryKey, ret);
			return(ret);
		}
	}
        // end of addition 2013-05-29

	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(primaryKey,strlen(primaryKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,primaryKey,strlen(primaryKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	if ((ret = UnionSelectRealDBRecord(idOfObject,"*",condition,record,sizeOfRecord,NULL)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionSelectRealDBRecord! tableName[%s]primaryKey[%s]\n",idOfObject,primaryKey);
		return(ret);
	}

	// added 2013-05-29
        // ʹ��cache������ݸ��µ�cache
	if(expireTime != -1)
	{
		UnionWriteUniqueDBRecIntoMemcached(sqlKey, expireTime, ret, record);
		//UnionAuditLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionWriteUniqueDBRecIntoMemcached table = [%s] conditon = [%s] len = [%d] !!\n", idOfObject, primaryKey, ret);
	}
	// end of addition 2013-05-29

	/*
	if (ret == 100)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionSelectRealDBRecord from [%s] on [%s]!\n",idOfObject,primaryKey);
		return(errCodeObjectMDL_ObjectRecordNotExist);
	}
	*/		
	return(ret);
}

/*
���ܣ�����Ψһֵ��ѯһ�������ʵ��
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
int UnionSelectUniqueObjectRecordByUniqueKey(char * idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
	int		i;
	int		ret;	
	char		condition[2048+1];
	//TUnionRec	rec;

	int                     expireTime = -1;
	char                    sqlKey[128+1];

        // added 2013-05-29
        // ʹ��cache�ȴ�cache�ж�����
	expireTime = UnionGetDBMemcachedTableExpireTime(idOfObject);
	memset(sqlKey, 0, sizeof(sqlKey));
	if(expireTime != -1)
	{
		UnionGetDBSelectStatementKey(idOfObject, "*", uniqueKey, sqlKey);
		if((ret = UnionReadUniqueDDBRecFromMemcached(sqlKey, sizeOfRecord, record)) > 0)
		{
			UnionAuditLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionReadUniqueDDBRecFromMemcached table = [%s] conditon = [%s] ret = [%d] OK!\n", idOfObject, uniqueKey, ret);
			return(ret);
		}
	}
        // end of addition 2013-05-29

	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(uniqueKey,strlen(uniqueKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,uniqueKey,strlen(uniqueKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	
	if ((ret = UnionSelectRealDBRecord(idOfObject,"*",condition,record,sizeOfRecord,NULL)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByUniqueKey:: UnionSelectRealDBRecord!\n");
		return(ret);
	}

	// added 2013-05-29
        // ʹ��cache������ݸ��µ�cache
	if(expireTime != -1)
	{
		UnionWriteUniqueDBRecIntoMemcached(sqlKey, expireTime, ret, record);
		//UnionAuditLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionWriteUniqueDBRecIntoMemcached table = [%s] conditon = [%s] len = [%d] !!\n", idOfObject, uniqueKey, ret);
	}
	// end of addition 2013-05-29	

	return(ret);
}

/*
���ܣ�����ɾ��һ�������ʵ��
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ����ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeleteObjectRecord(char * idOfObject,char *condition)
{
	int		i;
	int		ret;	
	char		tmpBuf[2048+1];
	//TUnionRec	rec;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionIsSQLString(condition,strlen(condition),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

        ret = UnionDeleteRealDBRecord(idOfObject,tmpBuf);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionBatchDeleteObjectRecord:: UnionDeleteRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

/*
���ܣ������޸�һ�������ʵ��
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ�ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdateObjectRecord(char * idOfObject,char *condition,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		tmpBuf[1024+1];
	char		field[1024+1];
	//TUnionRec	rec;

	// ����ֵ��
	memset(field,0,sizeof(field));
	if ((ret = UnionIsSQLString(record,lenOfRecord,field,sizeof(field))) < 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,record,lenOfRecord,conTransformTypeUpdate,field,sizeof(field))) < 0)
		{
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	// ����������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionIsSQLString(condition,strlen(condition),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

        ret = UnionUpdateRealDBRecord(idOfObject,field,tmpBuf);
	if (ret < 0)
	{
		if (ret == -100 || ret == -1403)
			ret = errCodeDatabaseMDL_RecordNotFound;
                UnionUserErrLog("in UnionBatchUpdateObjectRecord:: UnionUpdateRealDBRecord field=[%s], condition=[%s]!", field,tmpBuf, ret);
		return(ret);
	}
	return(ret);
}

/*
���ܣ������޸�һ�������ʵ��,�������string��������渽��һ���ַ���
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	numOfField: ��Ҫ׷���ַ�������ĸ���
	fieldGrp����Ҫ׷���ַ�����������
	strGrp��׷�ӵ��ַ�������
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ�ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchAppandStringToFieldOfObject(char * idOfObject, char *condition, int numOfField, char fieldGrp[][128+1], char strGrp[][128+1])
{
	int		i;
	int		ret;	
	char		tmpBuf[1024+1];
	char		field[1024+1];
	//TUnionRec	rec;

	memset(field, 0, sizeof field);

	if (numOfField <= 0)
		return 0;

	sprintf(field," %s=concat(trim(%s),%s)", fieldGrp[0], strGrp[0]);
	for(i=1; i<numOfField; i++)
	{
		sprintf(field,"%s AND %s=concat(trim(%s),%s)", field, fieldGrp[i], strGrp[i]);
	}

	// ����������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionIsSQLString(condition,strlen(condition),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionBatchAppandStringToFieldOfObject:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionBatchAppandStringToFieldOfObject:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

        ret = UnionUpdateRealDBRecord(idOfObject,field,tmpBuf);
	if (ret < 0)
	{
		if (ret == -100 || ret == 1403)
			ret = errCodeDatabaseMDL_RecordNotFound;
                UnionUserErrLog("in UnionBatchAppandStringToFieldOfObject:: field=[%s], tmpBuf=[%s]!", field,tmpBuf);
		UnionUserErrLog("in UnionBatchAppandStringToFieldOfObject:: UnionUpdateRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

/*
���ܣ�������ѯһ�������ʵ��
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectObjectRecord(char * idOfObject,char *condition,char *fileName)
{
	int		i;
	int		ret;	
	char		tmpBuf[2048+1];
	//TUnionRec	rec;
	int		oriConLen;
		
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionIsSQLString(condition,strlen(condition),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	else	// ��SQL���
	{
		// ��SQL����е���������ת��
		if ((ret = UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(idOfObject,tmpBuf,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject!\n");
			return(ret);
		}
	}		
	// 2009/11/9������������
	oriConLen = strlen(tmpBuf);
	UnionIsAddKeyWordANDToCondition(tmpBuf); // add by xusj 20091216

	if ((ret = UnionAutoAppendBranchIDToQueryCondition(idOfObject,oriConLen,tmpBuf + oriConLen,sizeof(tmpBuf)-oriConLen)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAutoAppendBranchIDToQueryCondition idOfObject = [%s]!\n",idOfObject);
		return(ret);
	}
	UnionLog("in UnionBatchSelectObjectRecord:: tableName = [%s] condition = [%s]\n",idOfObject, tmpBuf);
	// 2009/11/9�����������ӽ���
	if ((ret = UnionSelectRealDBRecord(idOfObject,"*",tmpBuf,NULL,0,fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionSelectRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

/*
���ܣ���ѯ���������ļ�¼��
���������
        idOfObject������ID
        condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
����ֵ��
        >=0���ɹ������ز�ѯ���ļ�¼����Ŀ
        <0��ʧ�ܣ�������
*/
int UnionSelectObjectRecordCounts(char * idOfObject,char *condition)
{
        int             i;
        int             ret;
        char            tmpBuf[2048+1];
        //TUnionRec       rec;
        int             oriConLen;

        memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionIsSQLString(condition,strlen(condition),tmpBuf,sizeof(tmpBuf))) < 0)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionIsSQLString!\n");
                return(ret);
        }
        else if (ret == 0)
        {
                if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,tmpBuf,sizeof(tmpBuf))) < 0)
                {
                        UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionTransformRecStrToSQLStr!\n");
                        return(ret);
                }
        }
        else    // ��SQL���
        {
                // ��SQL����е���������ת��
                if ((ret = UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(idOfObject,tmpBuf,tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject!\n");
                        return(ret);
                }
        }
        // 2009/11/9������������
        oriConLen = strlen(tmpBuf);
        UnionIsAddKeyWordANDToCondition(tmpBuf); // add by xusj 20091216
        if ((ret = UnionAutoAppendBranchIDToQueryCondition(idOfObject,oriConLen, tmpBuf + oriConLen,sizeof(tmpBuf)-oriConLen)) < 0)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionAutoAppendBranchIDToQueryCondition idOfObject = [%s]!\n",idOfObject);
                return(ret);
        }
        UnionLog("condition = [%s]\n",tmpBuf);
        // 2009/11/9�����������ӽ���
        if ((ret = UnionSelectRealDBRecordCounts(idOfObject,tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionSelectRealDBRecordCounts!\n");
                return(ret);
        }
        return(ret);
}

/*
���ܣ����ݹؼ��ֶ�ȡָ������
���������
	idOfObject��	����ID
	primaryKey��	����ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	fldName		����
	sizeOfBuf	���ܲ�ѯ��Ĵ�С
���������
	fldValue	��ֵ
����ֵ��
	>=0��		�ɹ���������ֵ�ĳ���
	<0��		ʧ�ܣ�������
*/
int UnionSelectSpecFldOfObjectByPrimaryKey(char * idOfObject,char *primaryKey,char *fldName,char *fldValue,int sizeOfBuf)
{
	int	ret;
	char	recStr[8192*2+1];
	int	lenOfRecStr;
	
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject,primaryKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecFldOfObjectByPrimaryKey:: UnionSelectUniqueObjectRecordByPrimaryKey primaryKey = [%s] tableName = [%s]\n",primaryKey,idOfObject);
		return(lenOfRecStr);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecFldOfObjectByPrimaryKey:: UnionReadRecFldFromRecStr [%s] from [%s] of tableName = [%s]\n",fldName,recStr,idOfObject);
		return(ret);
	}
	return(ret);
}

/*
���ܣ�ƴװһ���ؼ���������
���������
	idOfObject��	Ҫ���Ķ���ID	
	primaryKeyValueList	�ؼ�ֵ�ִ�,������ֵ��.�ֿ�
	lenOfValueList		�ؼ�ֵ�ִ�����
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	recStr		�ؼ���������
����ֵ��
	0��	�ؼ����������ĳ���
	<0��	ʧ�ܣ����ش�����
*/
int UnionFormObjectPrimarKeyCondition(char * idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf)
{
	char	primaryKeyList[512+1];
	int	len;
	int	fldNum;
	char	fldGrp[16][128+1];
	char	valueGrp[16][128+1];
	int	ret;
	int	offset = 0;
	int	index;
	char	caNewValue[4096];
	int	iNewValueLen;
		
	memset(primaryKeyList,0,sizeof(primaryKeyList));
	if ((len = UnionFormTablePrimaryKeyList(idOfObject, primaryKeyList)) < 0)
	{
                UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionFormTablePrimaryKeyList [%s]!\n",idOfObject);
                return(UnionSetUserDefinedErrorCode(len));
        }

	/*
	if ((len = UnionReadObjectPrimarKeyList(idOfObject,primaryKeyList,sizeof(primaryKeyList))) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionReadObjectPrimarKeyList [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(len));
	}
	*/
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKeyList = [%04d][%s]\n",len,primaryKeyList);
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(primaryKeyList,len,',',fldGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",primaryKeyList,idOfObject);
		return(UnionSetUserDefinedErrorCode(fldNum));
	}
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKeyValueList = [%04d][%s]\n",lenOfValueList,primaryKeyValueList);
	if ((ret = UnionSeprateVarStrIntoVarGrp(primaryKeyValueList,lenOfValueList,'.',valueGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",primaryKeyValueList,idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	if (fldNum != ret)
	{
		UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: fldNum [%d] of primaryKey in [%s] != that [%d] in [%s] of [%s]!\n",fldNum,primaryKeyList,ret,primaryKeyValueList,idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	for (index = 0; index < fldNum; index++)
	{
		// Mary add begin, 20081112
		memset(caNewValue,0,sizeof(caNewValue));
		iNewValueLen=UnionTranslateRecFldStrIntoStr(valueGrp[index],strlen(valueGrp[index]),caNewValue,sizeof(caNewValue));
		if (iNewValueLen <= 0)
		{
			UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionTranslateStrIntoRecFldStr for [%s] fail! return=[%d]\n",valueGrp[index],iNewValueLen);
			return(iNewValueLen);
		}
		if (strlen(valueGrp[index]) != iNewValueLen)
			UnionAuditLog("in UnionFormObjectPrimarKeyCondition:: caValue=[%s] iValueLen=[%d] caNewValue=[%s] iNewValueLen=[%d]\n",valueGrp[index],strlen(valueGrp[index]),caNewValue,iNewValueLen);
		if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],caNewValue,iNewValueLen,recStr+offset,sizeOfBuf-offset)) < 0)
		// Mary add end, 20081112
		//if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],valueGrp[index],strlen(valueGrp[index]),recStr+offset,sizeOfBuf-offset)) < 0)	Mary delete, 20081112
		{
			UnionUserErrLog("in UnionFormObjectPrimarKeyCondition:: UnionPutRecFldIntoRecStr [%s] of [%s]!\n",primaryKeyValueList,idOfObject);
			return(UnionSetUserDefinedErrorCode(ret));
		}
		offset += ret;
	}
	//UnionDebugLog("in UnionFormObjectPrimarKeyCondition:: primaryKey = [%s]\n",recStr);
	return(offset);
}

/*
���ܣ�
	��һ����¼��ƴװ�ؼ���
���������
	idOfObject��		����ID
	record��		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��		record�ĳ���
	sizeOfPrimaryKey	�ؼ�ֵ�������С
���������
	primaryKey		�ؼ�ֵ������.�ֿ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormPrimaryKeyStrOfSpecObjectRec(char * idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey)
{
	PUnionTableDef		sObject = NULL;
	int			iRet;
	int			fldNum;
	int			index;
	char			tmpBuf[4096+1];
	int			offset = 0;
	int			primaryKeyNum;
	int			i;
	char 			primaryKeyGrp[6][40+1];

	
	if (idOfObject == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	if ((sObject = UnionFindTableDef(idOfObject)) == NULL)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: UnionFindTableDef fail! \n");
		return(-1);
	}

	/*
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	*/

	primaryKeyNum = 0;
        for(i = 0; i < sObject->fieldNum; i++) 
        {
                if((sObject->fieldDef[i]).isPrimaryKey == 0)
                {       
                        continue;
                }
                if(primaryKeyNum  + 1 > 6)
                {
                        UnionAuditLog("in UnionFormPrimaryKeyStrOfSpecObjectRec :: primaryKeyNum = [%d] is too many!\n", primaryKeyNum);
                        return(UnionSetUserDefinedErrorCode(errCodeParameter));
                }
                strcpy(primaryKeyGrp[primaryKeyNum], (sObject->fieldDef[i]).fieldName);
                primaryKeyNum++;
        }
	
	for(i = 0; i < primaryKeyNum; i++)
        {
                if(i == 0)
                {
                        offset = sprintf(primaryKey + offset, "%s", primaryKeyGrp[i]);
                }
                else
                {
                        offset += sprintf(primaryKey + offset, ".%s", primaryKeyGrp[i]);
                }
        }

	/*	
	for (index = 0; index < sObject.primaryKey.fldNum; index++)
	{
		memset(&tmpBuf,0,sizeof(tmpBuf));
		if ((iRet = UnionReadRecFldFromRecStr(record,lenOfRecord,sObject.primaryKey.fldNameGrp[index],tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: UnionReadRecFldFromRecStr [%s] from [%s]\n",sObject.primaryKey.fldNameGrp[index],record);
			return(iRet);
		}
		if (offset + 1 + iRet >= sizeOfPrimaryKey)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyStrOfSpecObjectRec:: smallBuffer!\n");
			return(errCodeSmallBuffer);
		}
		if (index != 0)
		{
			memcpy(primaryKey+offset,".",1);
			offset++;
		}
		memcpy(primaryKey+offset,tmpBuf,iRet);
		offset += iRet;
	}
	*/
	return(offset);
}

/*
���ܣ�
	��һ����¼��ƴװ�ؼ���
���������
	idOfObject��		����ID
	record��		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��		record�ĳ���
	sizeOfPrimaryKey	�ؼ�ֵ�������С
���������
	primaryKey		�ؼ�ֵ����fld1=value|fld2=value|
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormPrimaryKeyRecStr(char * idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey)
{
	PUnionTableDef		sObject = NULL;
	int			iRet;
	int			fldNum;
	int			index;
	char			tmpBuf[4096+1];
	int			offset = 0;
	int			primaryKeyNum;
	char            	primaryKeyGrp[6][40+1];
	
	if (idOfObject == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStr:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ȡ������
	if ((sObject = UnionFindTableDef(idOfObject)) == NULL)
	{
		UnionUserErrLog("in UnionFormPrimaryKeyRecStr:: UnionFindTableDef fail!\n");
		return(-1);
	}
	
	primaryKeyNum = UnionReadTablePrimaryKeyGrpFromImage(idOfObject, primaryKeyGrp);
	for (index = 0; index < primaryKeyNum; index++)
	{
		memset(&tmpBuf,0,sizeof(tmpBuf));
		if ((iRet = UnionReadRecFldFromRecStr(record,lenOfRecord,primaryKeyGrp[index],tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyRecStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",primaryKeyGrp[index],record);
			return(iRet);
		}
		if (offset + 1 + iRet + strlen(primaryKeyGrp[index]) + 2 >= sizeOfPrimaryKey)
		{
			UnionUserErrLog("in UnionFormPrimaryKeyRecStr:: smallBuffer!\n");
			return(errCodeSmallBuffer);
		}
		offset = sprintf(primaryKey,"%s%s=%s|", primaryKey, primaryKeyGrp[index], tmpBuf);
	}
	return(offset);
}

int UnionBatchSelectUnionObjectRecord(char * idOfObject,char *condition,char *fileName)
{
	return(UnionBatchSelectObjectRecord(idOfObject,condition,fileName));
}

/*
���ܣ���������ṹƴ�ɸ�ʽ��"����1.����2.����3��"
���������
	fldGrp��������ṹ
���������
	fieldNameStr����ʽ������ʽΪ"����1.����2.����3��"
����ֵ��
	>=0���ɹ������ظ�ʽ���ĳ���
	<0��ʧ�ܣ�������
int UnionFormFieldNameStrFromFldGrp(char **primaryKeyGrp, int primaryKeyNum, char *fieldNameStr)
{
	int	i,iLen=0;
	
	if (primaryKeyGrp == NULL || fieldNameStr == NULL)
	{
		UnionUserErrLog("in UnionFormFieldNameStrFromFldGrp:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (i=0;i<primaryKeyNum;i++)
	{
		sprintf(fieldNameStr + iLen,"%s%c", primaryKeyGrp[i],SeparableSignOfFieldNameGroup);
		iLen += (strlen(primaryKeyGrp[i])+1);
	}
	
	fieldNameStr[iLen-1] = '\0';

        // UnionLog("In UnionFormFieldNameStrFromFldGrp, fieldNameStr is: [%s], and the (iLen-1) is: [%d].\n", fieldNameStr, iLen-1);
	return(iLen-1);
}
*/

/*
���ܣ�
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationID	������ʶ
	isBeforeOperation	�ǲ���֮ǰִ�л���֮��ִ�У�1 ��ʾ����֮ǰ
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr)
{
	return(0);
}

/*
���ܣ�
	ִ��һ��������ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr)
{
	return(UnionSelectUniqueObjectRecordByUniqueKey(resName,recStr,resStr,sizeOfResStr));
}

/*
���ܣ�������ʵ��SQL�����в�ѯ
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"realSQL:: SQL���"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectRecordByRealSQL(char * idOfObject,char *condition,char *fileName)
{
	int		ret;
	char		tmpBuf[2048+1];

	if( condition == NULL || fileName == NULL )
	{
		UnionUserErrLog("in UnionSelectObjectRecordByRealSQL:: NULL pointer!\n");
		return (errCodeParameter);
	}

	if( strncmp(condition, "realSQL::", 9) != 0 )
	{
		UnionUserErrLog("in UnionSelectObjectRecordByRealSQL:: condition=[%s]!\n", condition);
		return (errCodeParameter);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	if( (ret = UnionTransSQL(condition+9, idOfObject, tmpBuf, sizeof(tmpBuf))) < 0 )
	{
		UnionUserErrLog("in UnionSelectObjectRecordByRealSQL:: UnionTransSQL condition=[%s] idOfObject=[%s]!\n", 
			condition, idOfObject);
		return (ret);
	}
	UnionAuditLog("in UnionSelectObjectRecordByRealSQL:: tmpBuf = [%s]\n",tmpBuf);

	if( (ret = UnionSelectDBRecordByRealSQL(idOfObject, tmpBuf, NULL, 0, fileName)) < 0 )
	{
		UnionUserErrLog("in UnionSelectObjectRecordByRealSQL:: UnionSelectDBRecordByRealSQL!\n");
		return(ret);
	}

	return(ret);
}

/*
 ���ܣ���ѯָ���嵥��Ӧ������
 ���������
  	idOfObject��������
  	reqStr������
  	fileName���ļ���
 ���봮��fldList=����1,����2,...|displayTag=x|condition=...|expandFldName=yyy|valueList=value1,value2,...|
 1��Ҫ�ӱ��ж�ȡ�����壬���磺fldList=����1������2��...|
 2����������ֵ��ķָ��������磺displayTag=xX
 	��x=1ʱ����ʾÿ����¼�������¸�ʽ��ʾ
 		����1=��ֵ1|����2=��ֵ2|...|
 	��x<>1ʱ��x�Ƿָ�����ʾÿ���������¸�ʽ��ʾ
 		��ֵ1x��ֵ2x..��ֵN
 3���̶���ѯ�����Σ����磺condition=...|
 4�����Ӳ�ѯ�����Σ�expandFldName=yyy|valueList=value1,value2,...|
 	����expandFldName=yyy|�������˸�������������
 	valueList=value1,value2,...|�������˸��Ӹ���ֵ��
 */
int UnionSelectObjectRecordBySpecFieldListOnObject(char * idOfObject,char *reqStr,int lenOfReqStr, char *fileName)
{
	int		ret;
	char		condition[2048+1];
	char		fldList[2048+1];
	char		expandFldName[40+1];
	char		valueList[2048+1];
	char		desValueList[2048+1];
	char		desFileName[512+1];
	char		tmpBuf[2048+1];
	char		SQLStr[4096+1];
	char		displayTag[10+1];
	char		conditionStr[2048+1];
	
	// �Ӵ��ж�ȡ��ֵ
	// ��ȡ����
	memset(fldList,0,sizeof(fldList));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fldList",fldList,sizeof(fldList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr fldList from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// ��ȡ��ֵ��ķָ���
	memset(displayTag,0,sizeof(displayTag));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"displayTag",displayTag,sizeof(displayTag))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr displayTag from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// ��ȡ����
	memset(condition,0,sizeof(condition));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"condition",condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr condition from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// ��ȡ��������������
	memset(expandFldName,0,sizeof(expandFldName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"expandFldName",expandFldName,sizeof(expandFldName))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr expandFldName from [%s] errCode = [%d]\n",reqStr,ret);
		return(ret);
	}
	// ��ȡ���Ӹ���ֵ
	memset(valueList,0,sizeof(valueList));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"valueList",valueList,sizeof(valueList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr valueList from [%s] errCode = [%d]\n",reqStr,ret);
		return(ret);
	}
	// ת��������
	memset(conditionStr,0,sizeof(conditionStr));
	if ((ret = UnionIsSQLString(condition,strlen(condition),conditionStr,sizeof(conditionStr))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,condition,strlen(condition),conTransformTypeCondition,conditionStr,sizeof(conditionStr))) < 0)
		{
			UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	// ת����ֵ�嵥��
	memset(desValueList,0,sizeof(desValueList));
	if ((ret = UnionTransValueListForFormatStr(idOfObject, expandFldName, valueList, ',', desValueList, sizeof(desValueList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionTransformRecStrToSQLStr!\n");
		return(ret);
	}
	// ��SQL��䴮
	if (strlen(condition) <= 0)
		strcpy(conditionStr,"1=1");
	if (strlen(fldList) <= 0)
		strcpy(fldList, "*");
	
	if ((ret > 0) || (strlen(desValueList) > 0))
		sprintf(SQLStr, "realSQL::select %s from %s where %s and %s in (%s)", fldList, idOfObject, conditionStr, expandFldName, desValueList);
	else
		sprintf(SQLStr, "realSQL::select %s from %s where %s", fldList, idOfObject, conditionStr);
	
	UnionAuditLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: SQLStr=[%s]\n",SQLStr);
	
	if (( ret = UnionSelectObjectRecordByRealSQL(idOfObject, SQLStr, fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionSelectObjectRecordByRealSQL!\n");
		return(ret);
	}
	if ((strlen(displayTag) <= 0) || (displayTag[0] == '1') || (ret == 0))
	{
		return ret;
	}
	
	memset(desFileName,0,sizeof(desFileName));
	sprintf(desFileName,"%s.txt",fileName);
	UnionAuditLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: fileName[%s],desFileName=[%s]\n",fileName,desFileName);

	if ((ret = UnionTransRecStrFileToSpecFormatFile(fileName, desFileName, displayTag[0])) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionTransRecStrFileToSpecFormatFile!\n");
		return(ret);
	}
	//�����ļ�
	if ((ret = UnionRenameFile(desFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionRenameFile!\n");
		return(ret);
	}
	
	return ret;
}

/*
 ���ܣ�
 	��¼���ļ�ת��Ϊָ����ʽ���ļ�
 ���봮��ʽ��
 	����1=��ֵ1|����2=��ֵ2|...����n=��ֵn|
 �������ʽ��
 	��ֵ1,��ֵ2,...,��ֵn
*/
int UnionTransRecStrFileToSpecFormatFile(char *oriFile, char *desFile, char speratorTag)
{
	char		caRecord[2048+1];
	FILE		*oriFp;
	FILE		*desFp;
	TUnionRec	tRec;
	int		lineNum = 0;
	int		ret;
	int		lenStr;
	int		recLen;
	char		tmpBuf[2048+1];
	int		i;
	
	if ((oriFp = fopen(oriFile, "r")) == NULL)
	{
		UnionSystemErrLog("in UnionTransRecStrFileToSpecFormatFile:: fopen[%s] is error 1!\n",oriFile);
		return(errCodeUseOSErrCode);
	}
	if ((desFp = fopen(desFile, "w")) == NULL)
	{
		UnionSystemErrLog("in UnionTransRecStrFileToSpecFormatFile:: fopen[%s] is error 2!\n",desFile);
		fclose(oriFp);
		return(errCodeUseOSErrCode);
	}
	lineNum = 0;
	while(!feof(oriFp))
	{
		memset(caRecord,0,sizeof(caRecord));
		recLen = UnionReadOneDataLineFromTxtFile(oriFp,caRecord,sizeof(caRecord));
		if (recLen < 0)
		{
			if (recLen == errCodeFileEnd)
				break;
			UnionUserErrLog("in UnionTransRecStrFileToSpecFormatFile:: UnionReadOneDataLineFromTxtFile fail! return=[%d] fileName = [%s]\n",recLen,oriFile);
			fclose(oriFp);
			fclose(desFp);
			return(recLen);
		}
		if (recLen == 0)	// ���ļ�����
			break;
		
		if ((ret = UnionReadRecFromRecStr(caRecord,recLen,&tRec)) < 0)
		{
			UnionUserErrLog("in UnionTransRecStrFileToSpecFormatFile:: UnionReadRecFromRecStr! return=[%d]\n",recLen,oriFile);
			fclose(oriFp);
			fclose(desFp);
			return ret;
		}
		memset(tmpBuf,0,sizeof(tmpBuf));
		lenStr = 0;
		for(i = 0; i< tRec.fldNum; i++)
		{
			sprintf(tmpBuf+lenStr,"%s%c",tRec.fldValue[i],speratorTag);
			lenStr += strlen(tRec.fldValue[i])+1;
		}
		tmpBuf[lenStr-1] = '\0';
		fputs(tmpBuf,desFp);
		fputs("\n",desFp);
		fflush(desFp);
		lineNum ++;
	}
	fclose(oriFp);
	fclose(desFp);
	return lineNum;
}

/*
���ܣ�����һ�������ʵ��
���������
	idOfObject������
	record��
		��ʽ1��"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
		��ʽ2��"SQL::(��1,��2,��,��N) values (��ֵ1,��ֵ2,��,��ֵN)"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecordWithoutSynchronize(char * idOfObject,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		value[30000+1];
	char		tmpBuf1[1024+1];
	char		tmpBuf2[1024+1];
	//TUnionRec	rec;
	int		ret1;
	
	memset(value,0,sizeof(value));
	if ((ret = UnionIsSQLString(record,lenOfRecord,value,sizeof(value))) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		// 2009/11/9������������
		if ((lenOfRecord = UnionAutoAppendFldOfSpecTBL(idOfObject,record,lenOfRecord)) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionAutoAppendFldOfSpecTBL idOfObject = [%s]!\n",idOfObject);
			return(lenOfRecord);
		}
		// 2009/11/9�����������ӽ���
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,record,lenOfRecord,conTransformTypeInsert,value,sizeof(value))) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	ret = UnionInsertRealDBRecord(idOfObject,value,strlen(value));
	if (ret < 0)
	{
                if (ret != errCodeObjectMDL_PrimaryKeyIsRepeat)
        		UnionUserErrLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionInsertRealDBRecord!\n");
                else
                        UnionLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionInsertRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

/*
���ܣ��޸�һ�������ʵ��
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
int UnionUpdateUniqueObjectRecordWithoutSynchronize(char * idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		condition[1024+1];
	char		field[8192*4+1];
	//TUnionRec	rec;
	int		ret1;
	
        UnionProgramerLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize::primaryKey=[%s],lenOfRecord = [%04d] record=[%s]\n", primaryKey,lenOfRecord,record);
        
	// ����ֵ��
	memset(field,0,sizeof(field));
	if ((ret = UnionIsSQLString(record,lenOfRecord,field,sizeof(field))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,record,lenOfRecord,conTransformTypeUpdate,field,sizeof(field))) < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}

	// ����������
	memset(condition,0,sizeof(condition));
	if ((ret = UnionIsSQLString(primaryKey,strlen(primaryKey),condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: UnionIsSQLString!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		if ((ret = UnionTransformRecStrToSQLStr(idOfObject,primaryKey,strlen(primaryKey),conTransformTypeCondition,condition,sizeof(condition))) < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: UnionTransformRecStrToSQLStr!\n");
			return(ret);
		}
	}
	UnionProgramerLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: field = [%s] condition = [%s]\n",field,condition);
	if ((ret = UnionUpdateRealDBRecord(idOfObject,field,condition)) < 0)
	{
                UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: field=[%s], condition=[%s]", field, condition);
		UnionUserErrLog("in UnionUpdateUniqueObjectRecordWithoutSynchronize:: UnionUpdateRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

// ��ȡ��ļ�¼�� 
// condition: "dbFld1 = value1 and  dbFld2 = value2..."
int UnionGetTableRecords(char *tableName, char *condition)
{
	int			ret;
	char			sql[2048];
	char			tmpBuf[4096];
	char			retStr[128];

	memset(sql, 0, sizeof(sql));

	if(condition == NULL || strlen(condition) == 0)
	{
        	sprintf(sql, "select count(*) from %s", tableName);
	}
	else
	{
        	sprintf(sql, "select count(*) from %s where %s", tableName, condition);
	}

	UnionProgramerLog("in UnionGetTableRecords:: excute sql = [%s]!\n", sql);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	if ((ret = UnionOpenRealDBSql(sql, tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecords:: UnionOpenRealDBSql sql=[%s] return=%d Error\n", sql, ret);
		return(ret);
	}
        UnionRTrim(tmpBuf);

	memset(retStr, 0, sizeof(retStr));
	if ((ret = UnionReadRecFldFromRecStr(tmpBuf, strlen(tmpBuf), "FIELD0", retStr, sizeof(retStr) - 1) < 0))
	{
		UnionUserErrLog("in UnionGetTableRecords:: UnionReadRecFldFromRecStr fld = %s ret = %d error !\n","FIELD0", ret);
		return(ret);
	}
	
	UnionProgramerLog("in UnionGetTableRecords:: UnionReadRecFldFromRecStr retStr = [%s]!\n", retStr);

	return(atoi(retStr));

	
}



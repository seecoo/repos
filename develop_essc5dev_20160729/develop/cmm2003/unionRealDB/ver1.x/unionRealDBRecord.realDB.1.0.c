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
功能：插入一个对象的实例
输入参数：
	idOfObject：表名
	record：
		格式1："域1=域值|域2=域值|…|域N=域值"
		格式2："SQL::(域1,域2,…,域N) values (域值1,域值2,…,域值N)"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
		// 2009/11/9，王纯军增加
		if ((lenOfRecord = UnionAutoAppendFldOfSpecTBL(idOfObject,record,lenOfRecord)) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecord:: UnionAutoAppendFldOfSpecTBL idOfObject = [%s]!\n",idOfObject);
			return(lenOfRecord);
		}
		// 2009/11/9，王纯军增加结束
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
	// 2009/8/21，Wolfgang Wang added
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
功能：根据关键字删除一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：
		格式1："键值域1=域值|键值域2=域值|…键值域N=域值|"
		格式2："SQL::键值域1=域值 and 键值域2=域值 and 键值域N=域值"
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
	// 2009/8/21，Wolfgang Wang added
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
功能：判断一个外键是否需要修改
输入参数：
        fldGrp：域名组结构
	record：记录串，格式为"域1=域值|域2=域值|域3=域值|…"
返回值：
        1:      需要修改
        0：     不需要修改
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
功能：修改一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
        
	// 更新值串
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

	// 更新条件串
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
	// 2009/8/21，Wolfgang Wang added
	if ((ret1 = UnionSynchronizeComplexDBUpdateToBrothers(idOfObject,primaryKey,record,lenOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionSynchronizeComplexDBUpdateToBrothers!\n");
		//return(ret);
	}
#endif
	return(ret);
}

/*
功能：根据关键字查询一个对象的实例（只选择指定的域）
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
        // 使用cache先从cache中读数据
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
	// 使用cache则把数据更新到cache
	if(expireTime != -1)
	{
		UnionWriteUniqueDBRecIntoMemcached(sqlKey, expireTime, ret, record);
		//UnionAuditLog("in UnionSelectObjectFldGrpByPrimaryKey:: UnionWriteUniqueDBRecIntoMemcached table = [%s] conditon = [%s] len = [%d] !!\n", idOfObject, primaryKey, ret);
	}
	// end of addition 2013-05-29

	return(ret);
}

/*
功能：根据关键字查询一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
        // 使用cache先从cache中读数据
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
        // 使用cache则把数据更新到cache
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
功能：根据唯一值查询一个对象的实例
输入参数：
	idOfObject：对象ID
	uniqueKey：对象实例的唯一值，格式为"唯一值域1=域值|唯一值域2=域值|…唯一值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
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
        // 使用cache先从cache中读数据
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
        // 使用cache则把数据更新到cache
	if(expireTime != -1)
	{
		UnionWriteUniqueDBRecIntoMemcached(sqlKey, expireTime, ret, record);
		//UnionAuditLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionWriteUniqueDBRecIntoMemcached table = [%s] conditon = [%s] len = [%d] !!\n", idOfObject, uniqueKey, ret);
	}
	// end of addition 2013-05-29	

	return(ret);
}

/*
功能：批量删除一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：删除条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	无
返回值：
	>0：成功，返回被删除的实例数目
	<0：失败，错误码
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
功能：批量修改一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：修改条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值|"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	>=0：成功，返回被修改的实例数目
	<0：失败，错误码
*/
int UnionBatchUpdateObjectRecord(char * idOfObject,char *condition,char *record,int lenOfRecord)
{
	int		i;
	int		ret;	
	char		tmpBuf[1024+1];
	char		field[1024+1];
	//TUnionRec	rec;

	// 更新值串
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

	// 更新条件串
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
功能：批量修改一个对象的实例,将对象的string类型域后面附加一个字符串
输入参数：
	idOfObject：对象ID
	condition：修改条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	numOfField: 需要追加字符串的域的个数
	fieldGrp：需要追加字符串的域数组
	strGrp：追加的字符串数组
输出参数：
	无
返回值：
	>=0：成功，返回被修改的实例数目
	<0：失败，错误码
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

	// 更新条件串
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
功能：批量查询一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	fileName：存储了查询出来的实例的值的文件名
返回值：
	>=0：成功，返回查询出的记录的数目
	<0：失败，错误码
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
	else	// 是SQL语句
	{
		// 对SQL语句中的域名进行转换
		if ((ret = UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(idOfObject,tmpBuf,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject!\n");
			return(ret);
		}
	}		
	// 2009/11/9，王纯军增加
	oriConLen = strlen(tmpBuf);
	UnionIsAddKeyWordANDToCondition(tmpBuf); // add by xusj 20091216

	if ((ret = UnionAutoAppendBranchIDToQueryCondition(idOfObject,oriConLen,tmpBuf + oriConLen,sizeof(tmpBuf)-oriConLen)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionAutoAppendBranchIDToQueryCondition idOfObject = [%s]!\n",idOfObject);
		return(ret);
	}
	UnionLog("in UnionBatchSelectObjectRecord:: tableName = [%s] condition = [%s]\n",idOfObject, tmpBuf);
	// 2009/11/9，王纯军增加结束
	if ((ret = UnionSelectRealDBRecord(idOfObject,"*",tmpBuf,NULL,0,fileName)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionSelectRealDBRecord!\n");
		return(ret);
	}
	return(ret);
}

/*
功能：查询满足条件的记录数
输入参数：
        idOfObject：对象ID
        condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
返回值：
        >=0：成功，返回查询出的记录的数目
        <0：失败，错误码
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
        else    // 是SQL语句
        {
                // 对SQL语句中的域名进行转换
                if ((ret = UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(idOfObject,tmpBuf,tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject!\n");
                        return(ret);
                }
        }
        // 2009/11/9，王纯军增加
        oriConLen = strlen(tmpBuf);
        UnionIsAddKeyWordANDToCondition(tmpBuf); // add by xusj 20091216
        if ((ret = UnionAutoAppendBranchIDToQueryCondition(idOfObject,oriConLen, tmpBuf + oriConLen,sizeof(tmpBuf)-oriConLen)) < 0)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionAutoAppendBranchIDToQueryCondition idOfObject = [%s]!\n",idOfObject);
                return(ret);
        }
        UnionLog("condition = [%s]\n",tmpBuf);
        // 2009/11/9，王纯军增加结束
        if ((ret = UnionSelectRealDBRecordCounts(idOfObject,tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecordCounts:: UnionSelectRealDBRecordCounts!\n");
                return(ret);
        }
        return(ret);
}

/*
功能：根据关键字读取指定的域
输入参数：
	idOfObject：	对象ID
	primaryKey：	对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	fldName		域名
	sizeOfBuf	接受查询域的大小
输出参数：
	fldValue	域值
返回值：
	>=0：		成功，返回域值的长度
	<0：		失败，错误码
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
功能：拼装一个关键字条件串
输入参数：
	idOfObject：	要读的对象ID	
	primaryKeyValueList	关键值字串,两个域值以.分开
	lenOfValueList		关键值字串长度
	sizeOfBuf	关键字清单的大小
输出参数：
	recStr		关键字条件串
返回值：
	0：	关键字条件串的长度
	<0：	失败，返回错误码
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
功能：
	从一个记录串拼装关键字
输入参数：
	idOfObject：		对象ID
	record：		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：		record的长度
	sizeOfPrimaryKey	关键值串缓冲大小
输出参数：
	primaryKey		关键值串，以.分开
返回值：
	0：成功
	<0：失败，错误码
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
	
	// 读取对象定义
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
功能：
	从一个记录串拼装关键字
输入参数：
	idOfObject：		对象ID
	record：		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：		record的长度
	sizeOfPrimaryKey	关键值串缓冲大小
输出参数：
	primaryKey		关键值串，fld1=value|fld2=value|
返回值：
	0：成功
	<0：失败，错误码
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
	
	// 读取对象定义
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
功能：从域名组结构拼成格式串"域名1.域名2.域名3…"
输入参数：
	fldGrp：域名组结构
输出参数：
	fieldNameStr：格式串，格式为"域名1.域名2.域名3…"
返回值：
	>=0：成功，返回格式串的长度
	<0：失败，错误码
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
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationID	操作标识
	isBeforeOperation	是操作之前执行还是之后执行，1 表示操作之前
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr)
{
	return(0);
}

/*
功能：
	执行一个表的联合查询操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr)
{
	return(UnionSelectUniqueObjectRecordByUniqueKey(resName,recStr,resStr,sizeOfResStr));
}

/*
功能：根据真实的SQL语句进行查询
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"realSQL:: SQL语句"
输出参数：
	fileName：存储了查询出来的实例的值的文件名
返回值：
	>=0：成功，返回查询出的记录的数目
	<0：失败，错误码
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
 功能：查询指定清单对应的数据
 输入参数：
  	idOfObject：对象名
  	reqStr：请求串
  	fileName：文件名
 输入串：fldList=域名1,域名2,...|displayTag=x|condition=...|expandFldName=yyy|valueList=value1,value2,...|
 1、要从表中读取的域定义，形如：fldList=域名1，域名2，...|
 2、读出的域值间的分隔符，形如：displayTag=xX
 	当x=1时，表示每条记录按照以下格式显示
 		域名1=域值1|域名2=域值2|...|
 	当x<>1时，x是分隔符表示每条按照以下格式显示
 		域值1x域值2x..域值N
 3、固定查询条件段，形如：condition=...|
 4、附加查询条件段，expandFldName=yyy|valueList=value1,value2,...|
 	其中expandFldName=yyy|，定义了附加条件的域名
 	valueList=value1,value2,...|，定义了附加赋的值。
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
	
	// 从串中读取域值
	// 读取域定义
	memset(fldList,0,sizeof(fldList));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fldList",fldList,sizeof(fldList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr fldList from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// 读取域值间的分隔符
	memset(displayTag,0,sizeof(displayTag));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"displayTag",displayTag,sizeof(displayTag))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr displayTag from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// 读取条件
	memset(condition,0,sizeof(condition));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"condition",condition,sizeof(condition))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr condition from [%s] errCode = [%d]\n",reqStr,ret);
		//return(ret);
	}
	// 读取附加条件的域名
	memset(expandFldName,0,sizeof(expandFldName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"expandFldName",expandFldName,sizeof(expandFldName))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr expandFldName from [%s] errCode = [%d]\n",reqStr,ret);
		return(ret);
	}
	// 读取附加赋的值
	memset(valueList,0,sizeof(valueList));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"valueList",valueList,sizeof(valueList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionReadRecFldFromRecStr valueList from [%s] errCode = [%d]\n",reqStr,ret);
		return(ret);
	}
	// 转换条件域串
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
	// 转换域值清单串
	memset(desValueList,0,sizeof(desValueList));
	if ((ret = UnionTransValueListForFormatStr(idOfObject, expandFldName, valueList, ',', desValueList, sizeof(desValueList))) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionTransformRecStrToSQLStr!\n");
		return(ret);
	}
	// 打SQL语句串
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
	//拷贝文件
	if ((ret = UnionRenameFile(desFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordBySpecFieldListOnObject:: UnionRenameFile!\n");
		return(ret);
	}
	
	return ret;
}

/*
 功能：
 	记录串文件转换为指定格式的文件
 输入串格式：
 	域名1=域值1|域名2=域值2|...域名n=域值n|
 输出串格式：
 	域值1,域值2,...,域值n
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
		if (recLen == 0)	// 读文件结束
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
功能：插入一个对象的实例
输入参数：
	idOfObject：表名
	record：
		格式1："域1=域值|域2=域值|…|域N=域值"
		格式2："SQL::(域1,域2,…,域N) values (域值1,域值2,…,域值N)"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
		// 2009/11/9，王纯军增加
		if ((lenOfRecord = UnionAutoAppendFldOfSpecTBL(idOfObject,record,lenOfRecord)) < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecordWithoutSynchronize:: UnionAutoAppendFldOfSpecTBL idOfObject = [%s]!\n",idOfObject);
			return(lenOfRecord);
		}
		// 2009/11/9，王纯军增加结束
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
功能：修改一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
        
	// 更新值串
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

	// 更新条件串
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

// 获取表的记录数 
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



#include "UnionLog.h"
#include "UnionStr.h"

#include "unionDesKeyDB.h"
#include "desKeyDBRecord.h"

/*
功能：插入一把密钥
输入参数：
	idOfObject：表名
	record：密钥，格式："域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionInsertDesKeyDBRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord)
{
	int		ret;
	TUnionDesKey	tDesKey;
	
	// 将记录串转化成结构体形式
	memset(&tDesKey, 0, sizeof(TUnionDesKey));
	if ((ret = UnionReadDesKeyFromRecStr(record, lenOfRecord, &tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyDBRecord, translate [%s] to TUnionDesKey failed.\n", record);
		return ret;
	}
	
	// 将转化成结构体以后的记录插入 DES 密钥库中
	if ((ret = UnionInsertDesKeyIntoKeyDB(&tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyDBRecord, insert [%s] failed.\n", record);
		return ret;
	}
	
	UnionSuccessLog("in UnionInsertDesKeyDBRecord, insert [%s] success.\n", record);
	return 0;
}

/*
功能：根据关键字删除一把密钥
输入参数：
	idOfObject：对象ID
	primaryKey：关键字，"fld1=域值|"
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteUniqueDesKeyDBRecord(TUnionIDOfObject idOfObject,char *primaryKey)
{
	int			ret;
	char			fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	
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
功能：修改一把密钥
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
int UnionUpdateUniqueDesKeyDBRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	int			ret;
	TUnionDesKey		tDesKey;
	char			caRecord[4096];
	
	memset(caRecord, 0, sizeof(caRecord));
	ret			= sprintf(caRecord, "%s%s", primaryKey, record);
	
	// 将记录串转化成结构体形式
	memset(&tDesKey, 0, sizeof(TUnionDesKey));
	if ((ret = UnionReadDesKeyFromRecStr(caRecord, ret, &tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueDesKeyDBRecord, translate [%s] to TUnionDesKey failed.\n", record);
		return ret;
	}
	
	// 修改记录
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
功能：根据关键字查询一把密钥
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"fld1=域值|fld2=域值|…|fldN=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
*/
int UnionSelectUniqueDesKeyDBRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	int			ret;
	TUnionDesKey		tDesKey;
	
	// 初始化 DES 密钥结构体
	memset(&tDesKey,	0, sizeof(TUnionDesKey));
	if ((ret = UnionReadRecFldFromRecStr(primaryKey,strlen(primaryKey),"fullName",tDesKey.fullName,sizeof(tDesKey.fullName))) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadRecFldFromRecStr fullName!\n");
		return(ret);
	}
	
	// 读取DES密钥记录
	if ((ret = UnionReadDesKeyFromKeyDB(&tDesKey)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadDesKeyFromKeyDB failed!\n");
		return(ret);
	}
	
	// 将 DES 密钥记录结构体转化成记录串
	if ((ret = UnionPutDesKeyIntoRecStr(&tDesKey, record, sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey:: UnionReadDesKeyFromRecStr failed!\n");
		return(ret);
	}
	
	UnionSuccessLog("in UnionSelectUniqueDesKeyDBRecordByPrimaryKey, select [%s] success.\n", record);
	return ret;
}

/*
功能：根据唯一值查询一把密钥
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
int UnionSelectUniqueDesKeyDBRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
	return UnionSelectUniqueDesKeyDBRecordByPrimaryKey(idOfObject, uniqueKey, record, sizeOfRecord);	
}

/*
功能：批量删除密钥
输入参数：
	idOfObject：对象ID
	condition：删除条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	无
返回值：
	>0：成功，返回被删除的密钥数目
	<0：失败，错误码
*/
int UnionBatchDeleteDesKeyDBRecord(TUnionIDOfObject idOfObject,char *condition)
{
	return 0;
}

/*
功能：批量修改密钥
输入参数：
	idOfObject：对象ID
	condition：修改条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	record：要修改的密钥的值，格式为"域1=域值|域2=域值|…|域N=域值|"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	>=0：成功，返回被修改的密钥数目
	<0：失败，错误码
*/
int UnionBatchUpdateDesKeyDBRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord)
{
	return 0;
}

/*
功能：批量查询密钥
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	fileName：存储了查询出来的密钥的值的文件名
返回值：
	>=0：成功，返回查询出的密钥的数目
	<0：失败，错误码
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

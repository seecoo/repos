#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionRec0.h"
#include "unionErrCode.h"

#include "unionWorkingDir.h"

#include "unionRecFile.h"

#include "unionComplexDBAPIOnUnionEnv.h"
#include "unionComplexDBObjectDef.h"
/*
#include "unionComplexDBCommon.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectFileName.h"
*/

char *UnionGetEnviVarOfTheIndexByIndex(int varIndex,int varValueIndex);
int UnionReadSpecConfFileDef(TUnionIDOfObject idOfObject,PUnionObject pobjDef);

/*
功能：
	将一个记录字串中的域名，替换为fldx的格式，其中x是域顺序号，从1开始计数
输入参数：
	idOfObject	对象名
	recStr		记录串
	lenOfRecStr	记录串长度
	sizeOfBuf	缓冲的大小
	ifNonExistsSetDefault	如果记录域未定义是否采用缺省值
			1	采用
			0	不采用
输出参数：
	newRecStr	替换后的记录串
返回值：
	>=0，成功	新记录串的长度
	<0，错误码
*/
int UnionConvertRecStrIntoUnionEnvFormatOnObjDef(PUnionObject pobjDef,char *recStr,int lenOfRecStr,char *newRecStr,int sizeOfBuf,int ifNonExistsSetDefault)
{
	int			ret;
	int			fldDefIndex;
	TUnionRec		fldValueGrp;
	int			fldValueIndex;
	PUnionObjectFldDef	pfldDef;
	int			offset = 0;
	char			fldName[40+1];
	
	if (pobjDef == NULL)
		return(lenOfRecStr);

	// 读取对象域赋值
	memset(&fldValueGrp,0,sizeof(fldValueGrp));
	if ((ret = UnionReadRecFromRecStr(recStr,lenOfRecStr,&fldValueGrp)) < 0)
	{
		UnionUserErrLog("in UnionConvertRecStrIntoUnionEnvFormatOnObjDef:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecStr,recStr);
		return(ret);
	}
	
	// 依域的定义顺序找到对该域的赋值
	for (fldDefIndex = 0; fldDefIndex < pobjDef->fldNum; fldDefIndex++)
	{
		sprintf(fldName,"fld%d",fldDefIndex+1);			
		pfldDef = &(pobjDef->fldDefGrp[fldDefIndex]);
		for (fldValueIndex = 0; fldValueIndex < fldValueGrp.fldNum; fldValueIndex++)
		{
			if (strcmp(fldValueGrp.fldName[fldValueIndex],pfldDef->name) == 0)	// 找到针对该域的赋值
			{
				if ((ret = UnionPutRecFldIntoRecStr(fldName,fldValueGrp.fldValue[fldValueIndex],strlen(fldValueGrp.fldValue[fldValueIndex]),
						newRecStr+offset,sizeOfBuf-offset)) < 0)
				{
					UnionUserErrLog("in UnionConvertRecStrIntoUnionEnvFormatOnObjDef:: UnionPutRecFldIntoRecStr fldName = [%s] [%s]\n",fldName,pfldDef->name);
					return(ret);
				}
				offset += ret;
				break;
			}
		}
		if (fldValueIndex < fldValueGrp.fldNum)
			continue;
		// 没有找到针对该域的赋值
		if (!ifNonExistsSetDefault)	// 不对该域赋值
			continue;
		// 对该域赋缺省值
		if ((ret = UnionPutRecFldIntoRecStr(fldName,pfldDef->defaultValue,strlen(pfldDef->defaultValue),newRecStr+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionConvertRecStrIntoUnionEnvFormatOnObjDef:: UnionPutRecFldIntoRecStr fldName = [%s] [%s]\n",fldName,pfldDef->name);
			return(ret);
		}
		offset += ret;
	}
	recStr[offset] = 0;
	return(offset);
}

/*
功能：
	将一个记录字串中的域名，从fldx的格式，替换为真实域名，其中x从1开始计数
输入参数：
	pobjDef	对象名
	recStr		记录串
	lenOfRecStr	记录串长度
	sizeOfBuf	缓冲的大小
输出参数：
	newRecStr	替换后的记录串
返回值：
	>=0，成功	新记录串的长度
	<0，错误码
*/
int UnionConvertRecStrFromUnionEnvFormatOnObjDef(PUnionObject pobjDef,char *recStr,int lenOfRecStr,char *newRecStr,int sizeOfBuf)
{
	int			ret;
	int			fldDefIndex;
	TUnionRec		fldValueGrp;
	int			fldValueIndex;
	PUnionObjectFldDef	pfldDef;
	int			offset = 0;
	char			fldName[40+1];

	if (pobjDef == NULL)
		return(lenOfRecStr);

	// 读取对象域赋值
	memset(&fldValueGrp,0,sizeof(fldValueGrp));
	if ((ret = UnionReadRecFromRecStr(recStr,lenOfRecStr,&fldValueGrp)) < 0)
	{
		UnionUserErrLog("in UnionConvertRecStrFromUnionEnvFormatOnObjDef:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecStr,recStr);
		return(ret);
	}
	// 依域的值顺序找到对该域的赋值
	for (fldValueIndex = 0; fldValueIndex < fldValueGrp.fldNum; fldValueIndex++)
	{
		for (fldDefIndex = 0; fldDefIndex < pobjDef->fldNum; fldDefIndex++)
		{
			sprintf(fldName,"fld%d",fldDefIndex+1);			
			pfldDef = &(pobjDef->fldDefGrp[fldDefIndex]);
			if (strcmp(fldValueGrp.fldName[fldValueIndex],fldName) == 0)	// 找到针对该域的赋值
			{
				if ((ret = UnionPutRecFldIntoRecStr(pfldDef->name,
						fldValueGrp.fldValue[fldValueIndex],
						strlen(fldValueGrp.fldValue[fldValueIndex]),
						newRecStr+offset,sizeOfBuf-offset)) < 0)
				{
					UnionUserErrLog("in UnionConvertRecStrFromUnionEnvFormatOnObjDef:: UnionPutRecFldIntoRecStr fldName = [%s] [%s]\n",fldName,pfldDef->name);
					return(ret);
				}
				offset += ret;
				break;
			}
		}
		if (fldDefIndex < pobjDef->fldNum)
			continue;
		// 没有找到针对该域的赋值
		UnionAuditLog("in UnionConvertRecStrFromUnionEnvFormatOnObjDef:: fld = [%s] not valid field of [%s]\n",fldValueGrp.fldName[fldValueIndex],pobjDef->name);
	}
	recStr[offset] = 0;
	return(offset);
}

/*
功能：
	将一个记录字串中的域名，替换为fldx的格式，其中x是域顺序号，从1开始计数
输入参数：
	idOfObject	对象名
	recStr		记录串
	lenOfRecStr	记录串长度
	sizeOfBuf	缓冲的大小
	ifNonExistsSetDefault	如果记录域未定义是否采用缺省值
			1	采用
			0	不采用
输出参数：
	newRecStr	替换后的记录串
返回值：
	>=0，成功	新记录串的长度
	<0，错误码
*/
int UnionConvertRecStrIntoUnionEnvFormat(char *idOfObject,char *recStr,int lenOfRecStr,char *newRecStr,int sizeOfBuf,int ifNonExistsSetDefault)
{
	int			ret;
	TUnionObject		objDef;
	
	if ((ret = UnionReadSpecConfFileDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionConvertRecStrIntoUnionEnvFormat:: UnionReadSpecConfFileDef idOfObject = [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(UnionConvertRecStrIntoUnionEnvFormatOnObjDef(&objDef,recStr,lenOfRecStr,newRecStr,sizeOfBuf,ifNonExistsSetDefault));
}

/*
功能：
	将一个记录字串中的域名，从fldx的格式，替换为真实域名，其中x从1开始计数
输入参数：
	idOfObject	对象名
	recStr		记录串
	lenOfRecStr	记录串长度
	sizeOfBuf	缓冲的大小
输出参数：
	newRecStr	替换后的记录串
返回值：
	>=0，成功	新记录串的长度
	<0，错误码
*/
int UnionConvertRecStrFromUnionEnvFormat(char *idOfObject,char *recStr,int lenOfRecStr,char *newRecStr,int sizeOfBuf)
{
	int			ret;
	TUnionObject		objDef;

	if ((ret = UnionReadSpecConfFileDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionConvertRecStrFromUnionEnvFormat:: UnionReadSpecConfFileDef idOfObject = [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	return(UnionConvertRecStrFromUnionEnvFormatOnObjDef(&objDef,recStr,lenOfRecStr,newRecStr,sizeOfBuf));
}

// 功能：获得所有对象Env记录的存储目录
/*
输入参数：
	无
输出参数：
	objectDir	对象记录的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetAllObjectEnvRecDirName(char *objectDir)
{
	if (objectDir == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
#ifndef _WIN32
	sprintf(objectDir,"%s",getenv("UNIONETC"));
#else
        char	mainDir[512+1];
        memset(mainDir,0,sizeof(mainDir));
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s", mainDir);
#endif
        return 0;
}

int UnionGetEnvObjTblName(char *objTBLName, char *idOfObject)
{
	char			recDir[512];
	
	if ((NULL == objTBLName) || (NULL == idOfObject))
	{
		UnionLog("in UnionGetEnvObjTblName, bad parameter.\n");
		return errCodeParameter;
	}
	
	// 获取对象表名称
	memset(recDir, 0, sizeof(recDir));
	UnionGetAllObjectEnvRecDirName(recDir);
	
	return sprintf(objTBLName, "%s/%s", recDir, idOfObject);
}

/*
功能	将一个记录域写入到Env记录字符串
输入参数
	prec		记录
	sizeOfRecStr	记录串缓冲的大小
输出参数
	recStr		打成的记录串
返回值
	>=0		记录串的长度
	<0		错误码
*/
int UnionPutRecIntoEnvStr(PUnionRec prec,char *recStr,int sizeOfRecStr, int begin)
{
        int	lenOfFldValue;
	int	offset = 0;
	int	index;
        int     j;
	int	thisFldLen;
        char    curFldName[512];

	if ((recStr == NULL) || (sizeOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionPutRecIntoEnvStr:: null pointer or sizeOfRecStr = [%d]!\n",sizeOfRecStr);
		return(errCodeParameter);
	}
	for (index = begin; (index < prec->fldNum + begin) && (index < conMaxNumOfFldPerRec); index++)
	{
                memset(curFldName, 0, sizeof(curFldName));
                sprintf(curFldName, "fld%d", index + 1);

                for (j = 0; (j < prec->fldNum) && (j < conMaxNumOfFldPerRec); j++)
                {
                        if (strcmp(prec->fldName[j], curFldName) == 0)
                                break;
                }

                if (j > prec->fldNum)
                {
                        UnionUserErrLog("in UnionPutRecIntoEnvStr:: out of bound.\n");
			return(errCodeParameter);
                }

                lenOfFldValue   = strlen(prec->fldValue[j]);
                if ((thisFldLen = 2 + lenOfFldValue) + offset >= sizeOfRecStr)
		{
			UnionUserErrLog("in UnionPutRecIntoEnvStr:: sizeOfRecStr [%d] < expected [%d] when put fldIndex [%d]\n",sizeOfRecStr,thisFldLen + offset,index);
			return(errCodeSmallBuffer);
		}
                
		sprintf(recStr+offset,"[%s]",prec->fldValue[j]);
		offset          += thisFldLen;
	}
	if (prec->fldNum > conMaxNumOfFldPerRec)
	{
		UnionAuditLog("in UnionPutRecIntoRecStr:: fldNum [%d] too much, and [%d] fld not put into the recStr!\n",prec->fldNum,prec->fldNum-conMaxNumOfFldPerRec);
	}
	return(offset);
}

/*
// 将[域值1][域值2]... 组串成: fld1=域值1|fld2=域值2|…|fldN=域值N
int UnionPutEnvStrIntoRec(PUnionRec prec,char *recStr,int sizeOfRecStr)
{
}
*/


int UnionReadEnvCondition(char *caCondition, int sizeOfConStr, char *primaryKey)
{
	TUnionRec		sRecord;
        int                     iRet;
	
	// 读取对象域赋值
	memset(&sRecord,0,sizeof(TUnionRec));	
	if ((iRet = UnionReadRecFromRecStr(primaryKey,strlen(primaryKey),&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionReadEnvCondition:: UnionReadRecFromRecStr from [%s]\n",primaryKey);
		return(iRet);
	}
	
	iRet                    = UnionPutRecIntoEnvStr(&sRecord, caCondition, sizeOfConStr, 0);

        if ('[' == caCondition[0])
        {
                strcpy(caCondition, caCondition + 1);
                iRet            -= 1;
        }

        if (']' == caCondition[iRet - 1])
        {
                caCondition[iRet - 1]   = 0;
        }

        return iRet;
}

/*
功能：插入一个对象的实例
输入参数：
	idOfObject：表名
	record：格式1："域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionInsertObjectRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord)
{
	TUnionRec		sRecord;
        int                     iRet;
        int                     iRecLen;
	char			objFileName[512];
	char			caRecord[4096+1];
	char			varName[128];
	
	if (idOfObject == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 2009/5/29，王纯军增加
	if ((lenOfRecord = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,record,lenOfRecord,record,lenOfRecord+100,1)) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] recStr::[%04d][%s]\n",idOfObject,lenOfRecord,record);
		return(UnionSetUserDefinedErrorCode(lenOfRecord));
	}
	// 2009/5/29，王纯军增加结束
	
	// 读取对象域赋值
	memset(&sRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
	
	memset(varName, 0, sizeof(varName));
	if ((iRet = UnionReadRecFldFromRecStr(record, lenOfRecord, "fld1", varName, sizeof(varName))) < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionReadRecFldFromRecStr [fld1] from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
	
	// 将格式 “fld1=域值|fld2=域值|…|域N=域值”转换为 [域值][域值][域值][域值]...
	// 将记录结构写成记录串
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionPutRecIntoEnvStr(&sRecord,caRecord,sizeof(caRecord), 1);
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecord:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRecLen);
		return(iRecLen);
	}
	
	UnionLog("in UnionInsertObjectRecord: \nrecord: [%s], \ncaRecord: [%s].\n", record, caRecord);

	// 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);
	
	// 写记录
	return UnionInsertEnviVar(objFileName, varName, "%s", caRecord);
}

/*
功能：根据关键字删除一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey："fld1=域值|"
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char			objFileName[512];
	char			caPrimaryKey[4096+1];
	int			ret;
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
		
	// 2009/5/29，王纯军增加
	if ((ret = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,primaryKey,strlen(primaryKey),primaryKey,strlen(primaryKey)+100,0)) < 0)
	{
		UnionUserErrLog("in UnionDeleteUniqueObjectRecord:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] primaryKey::[%s]\n",idOfObject,primaryKey);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	// 2009/5/29，王纯军增加结束

	memset(caPrimaryKey, 0, sizeof(caPrimaryKey));
	UnionReadEnvCondition(caPrimaryKey, sizeof(caPrimaryKey), primaryKey);
	
	// 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);
	
	return UnionDeleteEnviVar(objFileName, caPrimaryKey);
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
int UnionUpdateUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	TUnionRec		sRecord;
	TUnionObject	objDef;
	TUnionRec		ssRecord;
	char			pRecord[4096 + 1];
	char			pRec[4096 + 1];
	char			fldFlag[10];
	char			fldValue[2048 + 1];
	char			*pos;
	char			szTmpPrimaryKey[512+1];
	char			caPrimaryKey[512];
	char			objFileName[512];
	char			caRecord[4096+1];
        int                     iRet;
        int                     iRecLen;
        int			i, offset;
	
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// add by wangk 2009-7-31
	memset(szTmpPrimaryKey, 0, sizeof(szTmpPrimaryKey));
	strcpy(szTmpPrimaryKey, primaryKey);
	// add end wangk 2009-7-31

	// 2009/5/29，王纯军增加
	if ((iRet = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,primaryKey,strlen(primaryKey),primaryKey,strlen(primaryKey)+100,0)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] primaryKey::[%s]\n",idOfObject,primaryKey);
		return(UnionSetUserDefinedErrorCode(iRet));
	}
	// 2009/5/29，王纯军增加结束
	
	memset(caPrimaryKey, 0, sizeof(caPrimaryKey));
	UnionReadEnvCondition(caPrimaryKey, sizeof(caPrimaryKey), primaryKey);
	
	// 2009/5/29，王纯军增加
	if ((lenOfRecord = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,record,lenOfRecord,record,lenOfRecord+100,0)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] recStr::[%04d][%s]\n",idOfObject,lenOfRecord,record);
		return(UnionSetUserDefinedErrorCode(lenOfRecord));
	}
	// 2009/5/29，王纯军增加结束
	
	// 读取对象域赋值
	memset(&sRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
	
	// add by chenliang, 2009-2-23 15:39:17
	// modify by wangk 2009-7-31
	memset(pRecord, 0, sizeof(pRecord));
	if ((iRet = UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject, szTmpPrimaryKey, pRecord, sizeof(pRecord))) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: select [%s] from [%s] failed, ret: [%d].\n", szTmpPrimaryKey, idOfObject,iRet);
		return iRet;
	}
	
	UnionNullLog("in UnionUpdateUniqueObjectRecord, in [%s] the [%s]'s ori record: [%s].\n", idOfObject, szTmpPrimaryKey, pRecord);

	if ((iRet = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,pRecord,strlen(pRecord),pRecord,strlen(pRecord)+100,0)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] recStr::[%04zu][%s]\n",idOfObject,strlen(pRecord),pRecord);
		return(UnionSetUserDefinedErrorCode(iRet));
	}

	memset(&ssRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(pRecord,strlen(pRecord),&ssRecord)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadRecFromRecStr from [%zu] [%s]\n",strlen(pRecord),pRecord);
		return(iRet);
	}

	// add by wangk 2009-7-31
	memset(&objDef, 0, sizeof(TUnionObject));
	if ((iRet = UnionReadSpecConfFileDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadSpecConfFileDef idOfObject = [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(iRet));
	}
	// add end wangk 2009-7-31

	memset(pRec, 0, sizeof(pRec));
	for (i = 2, offset = 0; i <= objDef.fldNum; i++)
	{
		memset(fldFlag, 0, sizeof(fldFlag));
		sprintf(fldFlag, "fld%d", i);
		
		// 需要被修改字段中存在
		if ((pos = strstr(record, fldFlag)) != NULL)
		{
			memset(fldValue, 0, sizeof(fldValue));
			if ((iRet = UnionReadRecFldFromRecStr(record, lenOfRecord, fldFlag, fldValue, sizeof(fldValue))) < 0)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: read [%s]'s from [%s] failed, ret: [%d].\n", fldFlag, record, iRet);
				return iRet;
			}
			
			if ((iRet = UnionPutRecFldIntoRecStr(fldFlag, fldValue, strlen(fldValue), pRec + offset, sizeof(pRec) - offset)) < 0)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecFldIntoRecStr [%s] of [%s] sizeof(pRec) = [%zu] offset = [%d] varValue = [%s] valueLen = [%zu]\n",
                                        	fldFlag,pRec,sizeof(pRec),offset,fldValue,strlen(fldValue));
                        	return iRet;
			}
			
			//flag	= 0;	// modify by wangk 2009-7-31
			offset	+= iRet;
			continue;
		}
		else if ((pos = strstr(pRecord, fldFlag)) != NULL)
		{
			memset(fldValue, 0, sizeof(fldValue));
			if ((iRet = UnionReadRecFldFromRecStr(pRecord, strlen(pRecord), fldFlag, fldValue, sizeof(fldValue))) < 0)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: read [%s]'s from [%s] failed, ret: [%d].\n", fldFlag, pRecord, iRet);
				return iRet;
			}
			
			if ((iRet = UnionPutRecFldIntoRecStr(fldFlag, fldValue, strlen(fldValue), pRec + offset, sizeof(pRec) - offset)) < 0)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecFldIntoRecStr [%s] of [%s] sizeof(pRec) = [%zu] offset = [%d] varValue = [%s] valueLen = [%zu]\n",
                                        	fldFlag,pRec,sizeof(pRec),offset,fldValue,strlen(fldValue));
                        	return iRet;
			}
			
			//flag	= 0;	// modify by wangk 2009-7-31
			offset	+= iRet;
			continue;
		}
		else
		{
			//if (flag = 1)	// modify by wangk 2009-7-31
				//break;
			
			if ((iRet = UnionPutRecFldIntoRecStr(fldFlag, "", 0, pRec + offset, sizeof(pRec) - offset)) < 0)
			{
				UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecFldIntoRecStr [%s] of [%s] sizeof(pRec) = [%zu] offset = [%d] varValue = [%s] valueLen = [%zu]\n",
                                        	fldFlag,pRec,sizeof(pRec),offset,fldValue,strlen(fldValue));
                        	return iRet;
			}
		
			//flag		= 1;	// modify by wangk 2009-7-31
				
			offset	+= iRet;
			continue;
		}
	}
	/*
	memset(pRecord, 0, sizeof(pRecord));
	for (i = 0, offset = 0; i < ssRecord.fldNum; i++)
	{
		flag		= 0;
		if (strcmp("fld1", ssRecord.fldName[i]) == 0)
			continue;
		
		for (j = 0; j < sRecord.fldNum; j++)
		{
			if (strcmp(ssRecord.fldName[i], sRecord.fldName[j]) == 0)
			{
				if ((iRet = UnionPutRecFldIntoRecStr(sRecord.fldName[j], sRecord.fldValue[j], strlen(sRecord.fldValue[j]), pRecord + offset, sizeof(pRecord) - offset)) < 0)
				{
					UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecFldIntoRecStr [%s] of [%s] sizeof(pRecord) = [%d] offset = [%d] varValue = [%s] valueLen = [%d]\n",
                                        		sRecord.fldName[j],pRecord,sizeof(pRecord),offset,sRecord.fldValue[j],strlen(sRecord.fldValue[j]));
                        		return iRet;
				}
				
				offset	+= iRet;
				flag	= 1;
				break;
			}
		}
		
		if (1 == flag)
			continue;
		
		if ((iRet = UnionPutRecFldIntoRecStr(ssRecord.fldName[i], ssRecord.fldValue[i], strlen(ssRecord.fldValue[i]), pRecord + offset, sizeof(pRecord) - offset)) < 0)
		{
			UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecFldIntoRecStr [%s] of [%s] sizeof(pRecord) = [%d] offset = [%d] varValue = [%s] valueLen = [%d]\n",
                                        ssRecord.fldName[i],pRecord,sizeof(pRecord),offset,ssRecord.fldValue[i],strlen(ssRecord.fldValue[i]));
                        return iRet;
		}

		offset	+= iRet;
	}
	*/
	
	UnionNullLog("in UnionUpdateUniqueObjectRecord, the des record: [%s].\n", pRec);
	///*
	// 读取对象域赋值
	memset(&sRecord,0,sizeof(TUnionRec));
	if ((iRet = UnionReadRecFromRecStr(pRec,strlen(pRec),&sRecord)) < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionReadRecFromRecStr from [%zu] [%s]\n",strlen(pRecord),pRecord);
		return(iRet);
	}
	//*/
	// add end, 2009-2-23 15:39:22
	
	// 将格式 “域1=域值|域2=域值|…|域N=域值”转换为 [域值][域值][域值][域值]...
	// 将记录结构写成记录串
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=UnionPutRecIntoEnvStr(&sRecord,caRecord,sizeof(caRecord), 1);
	if (iRecLen < 0)
	{
		UnionUserErrLog("in UnionUpdateUniqueObjectRecord:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRecLen);
		return(iRecLen);
	}
	
	// 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);

	return UnionUpdateEnviVar(objFileName, caPrimaryKey, "%s", caRecord);
}

/*
功能：根据关键字查询一个对象的实例
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
int UnionSelectUniqueObjectRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	char			caPrimaryKey[512];
	char			objFileName[512];
        char                    caValue[2048];
        int                     offset;
        int                     i;
        int                     ret;
	
	if ((NULL == idOfObject) || (NULL == primaryKey) || (NULL == record) || (0 >= sizeOfRecord))
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
        
	// 2009/5/29，王纯军增加
	if ((ret = UnionConvertRecStrIntoUnionEnvFormat(idOfObject,primaryKey,strlen(primaryKey),primaryKey,strlen(primaryKey)+100,0)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] primaryKey::[%s]\n",idOfObject,primaryKey);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	// 2009/5/29，王纯军增加结束

	memset(caPrimaryKey, 0, sizeof(caPrimaryKey));
	UnionReadEnvCondition(caPrimaryKey, sizeof(caPrimaryKey), primaryKey);

	// 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);

        UnionLog("in UnionSelectUniqueObjectRecordByPrimaryKey, primaryKey: [%s], caPrimaryKey: [%s], objFileName: [%s].\n", primaryKey, caPrimaryKey, objFileName);

        for (i = 0, offset = 0; ; i++)
        {
                memset(caValue, 0, sizeof(caValue));
                ret             = UnionReadEnviVarValueOfName(objFileName, caPrimaryKey, i, caValue);

                if (ret >= 0)
                        offset  += sprintf(record + offset, "fld%d=%s|", i + 1, caValue);
                else
                {
                        // UnionUserErrLog("ret: [%d], caPrimaryKey: [%s].\n", ret, caPrimaryKey);
                        break;
                }
        }
	// 2009/5/29，王纯军增加
	record[offset] = 0;
	if ((ret = UnionConvertRecStrFromUnionEnvFormat(idOfObject,record,offset,record,sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: UnionConvertRecStrIntoUnionEnvFormat idOfObject = [%s] recStr::[%04d][%s]\n",idOfObject,offset,record);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset = ret;
	record[offset] = 0;
	// 2009/5/29，王纯军增加结束
        UnionLog("in UnionSelectUniqueObjectRecordByPrimaryKey, the record: [%04d][%s].\n", offset,record);
        
        if (0 >= offset)
        {
                UnionUserErrLog("in UnionSelectUniqueObjectRecordByPrimaryKey:: read record error.\n");
                return offset;
        }

        return offset;
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
int UnionSelectUniqueObjectRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord)
{
        return UnionSelectUniqueObjectRecordByPrimaryKey(idOfObject, uniqueKey, record, sizeOfRecord);
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
int UnionBatchDeleteObjectRecord(TUnionIDOfObject idOfObject,char *condition)
{
        return 0;
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
int UnionBatchUpdateObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord)
{
        return 0;
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
int UnionBatchSelectObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
        char			objFileName[512];
        char                    caRecord[3072];
        char                    *caValue;
        int                     ret;
        int                     recordNum;
        int                     i, j;
        int                     offset;
        
        char			fldFlg[512 + 1];
        char			specFldVal[2048 + 1];
        char			caRecordCpy[3072];
        char			*pos;
        int			lenOfCondition;
        int			off;
 	TUnionObject		objDef;
 	
	if ((ret = UnionReadSpecConfFileDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionReadSpecConfFileDef idOfObject = [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	UnionLog("in UnionBatchSelectObjectRecord:: fldNum = [%d] name = [%s] for [%s]\n",objDef.fldNum,objDef.name,idOfObject);
	
        if ((NULL == idOfObject) || (NULL == condition) || (NULL == fileName))
	{
		UnionUserErrLog("in UnionBatchSelectObjectRecord:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

        // 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);

        ret                     = UnionInitEnvi(objFileName);
        if (0 > ret)
        {
                UnionUserErrLog("in UnionBatchSelectObjectRecord:: file: [%s] exists error!\n", objFileName);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        }
        
        lenOfCondition		= strlen(condition);
        recordNum               = UnionGetEnviVarNum();
        
        UnionNullLog("in UnionBatchSelectObjectRecord, recordNum is: [%d], condition: [%s].\n", recordNum, condition);
        
        for (i = 0; i < recordNum; i++)
        {
                memset(caRecord, 0, sizeof(caRecord));
                memset(caRecordCpy, 0, sizeof(caRecordCpy));
                for (j = 0, offset = 0, off = 0; ; j++)
                {
                        caValue = UnionGetEnviVarOfTheIndexByIndex(i, j);

			memset(fldFlg, 0, sizeof(fldFlg));
			sprintf(fldFlg, "fld%d", j + 1);
			
                        if (NULL != caValue)
                        {
                                offset  += sprintf(caRecord + offset, "fld%d=%s|", j + 1, caValue);
                        }
                        else
                                break;
			
			memset(specFldVal, 0, sizeof(specFldVal));
			/*
			if ((ret = UnionReadRecFldFromRecStr(condition, lenOfCondition, fldFlg, specFldVal, sizeof(specFldVal))) < 0)
			{
				strcpy(specFldVal, caValue);
			}
			*/
			
			if ((pos = strstr(condition, fldFlg)) != NULL)
			{
				UnionReadRecFldFromRecStr(condition, lenOfCondition, fldFlg, specFldVal, sizeof(specFldVal));
			}
			else
			{
				strcpy(specFldVal, caValue);
			}
			
			off		+= sprintf(caRecordCpy + off, "fld%d=%s|", j + 1, specFldVal);
                }
                
                // UnionNullLog("in UnionBatchSelectObjectRecord, [%d]\n[%s]\n[%s].\n", i + 1, caRecord, caRecordCpy);

                // offset  = sprintf(caRecord + offset, "\n");
		// 2009/5/29，王纯军增加
		caRecord[offset] = 0;
		if ((ret = UnionConvertRecStrFromUnionEnvFormatOnObjDef(&objDef,caRecord,offset,caRecord,sizeof(caRecord))) < 0)
		{
			UnionUserErrLog("in UnionBatchSelectObjectRecord:: UnionConvertRecStrFromUnionEnvFormatOnObjDef idOfObject = [%s] recStr::[%04d][%s]\n",idOfObject,offset,caRecord);
			UnionClearEnvi();
			return(UnionSetUserDefinedErrorCode(ret));
		}
		offset = ret;
		// 2009/5/29，王纯军增加结束
		//if (strcmp(caRecord, caRecordCpy) == 0)	2009/5/29，王纯军删除
                	UnionAppendRecStrToFile(fileName, caRecord, offset);
        }

        UnionClearEnvi();
        return i;
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
int UnionFormObjectPrimarKeyCondition(TUnionIDOfObject idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf)
{
        return 0;
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
int UnionFormPrimaryKeyStrOfSpecObjectRec(TUnionIDOfObject idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey)
{
        return 0;
}

/*
功能：获得对象的实例数目
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>=0：实例数
	<0：失败，错误码
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject)
{
	char			objFileName[512];
	
	// 获取对象表名称
	memset(objFileName, 0, sizeof(objFileName));
	UnionGetEnvObjTblName(objFileName, idOfObject);
	
	return (UnionGetTotalRecNumInFile(objFileName));	
}

/* 2009/5/29,王纯军删除
int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject)
{
	return(0);
}

int UnionExistsObjectDef(TUnionIDOfObject idOfObject)
{
	return(0);
}
 2009/5/29,王纯军删除 结束
*/

int UnionCreateObject(TUnionIDOfObject idOfObject)
{
	return(0);
}

int UnionDropObject(TUnionIDOfObject idOfObject)
{
	return(0);
}

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
*//*
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr)
{
	return(0);
}
*/
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

int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	return(UnionBatchSelectObjectRecord(idOfObject,condition,fileName));
}

// add by wangk 2009-8-7
int UnionCloseDatabase()
{
	return (0);
}
// add end

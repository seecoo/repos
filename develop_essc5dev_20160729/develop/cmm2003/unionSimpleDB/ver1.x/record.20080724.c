// Author:	ChenJiaMei
// Date:	2008-7-16

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
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionRecFile.h"
#include "unionRec0.h"
#include "unionFldGrp.h"
#include "unionErrCode.h"

#include "table.h"
#include "database.h"
#include "recInfo.h"
#include "allTableInfo.h"
#include "record.h"

// 说明：本文件中的程序基于文件数据库的操作

/*
功能：	从一个关键字定义串中读出各个字段
输入参数：
	defStr		关键字定义串
	lenOfDefStr	关键字定义串长度
        maxFldNum       关键字数组的大小
输出参数：
	primaryKeyGrp	关键字数组
返回值：
	>=0：		关键字包括的域数目
	<0：		错误代码
*/
int UnionReadPrimaryKeyFldGrpFromDefStr(char *defStr,int lenOfDefStr,char primaryKeyGrp[][128+1],int maxFldNum)
{
	int	defStrOffset = 0;
	int	fldIndex = 0;
	int	fldStrOffset = 0;
	int	fldNum;

	if (defStr == NULL || lenOfDefStr <= 0)
	{
		UnionUserErrLog("in UnionReadPrimaryKeyFldGrpFromDefStr:: parameter error!\n");
		return(errCodeParameter);
	}
	for (;defStrOffset < lenOfDefStr;defStrOffset++)
	{
		if (defStr[defStrOffset] == '|')
		{
			fldIndex++;
			fldStrOffset=0;
			continue;
		}
		if (primaryKeyGrp == NULL)	// 不输出关键字
			continue;
		if (fldIndex >= maxFldNum)
		{
			UnionUserErrLog("in UnionReadPrimaryKeyFldGrpFromDefStr:: fldIndex [%d] out of range!\n",fldIndex);
			return(errCodeSmallBuffer);
		}
		
		if (fldStrOffset == 0)
			memset(primaryKeyGrp[fldIndex],0,sizeof(primaryKeyGrp[fldIndex]));
		if (fldStrOffset >= sizeof(primaryKeyGrp[fldIndex]))
		{
			UnionUserErrLog("in UnionReadPrimaryKeyFldGrpFromDefStr:: len of fldStrOffset [%d] of index [%d] too long!\n",fldStrOffset,fldIndex);
			return(errCodeSmallBuffer);
		}
		primaryKeyGrp[fldIndex][fldStrOffset]=defStr[defStrOffset];
		fldStrOffset++;
	}
	if (defStr[lenOfDefStr-1] != '|')	// 最后一个域没有带|结束符
		return(fldIndex+1);
	else
		return(fldIndex);
}

// 根据表格，记录条件，获得记录文件的名称
int UnionGetRecordFileNameFromCondition(char *tableName,char *condition,int lenOfCondition,char *fileName,char *keyStr,int sizeOfKeyStr)
{
	int	ret;

	if ((ret = UnionFormRecordFileName(tableName,condition,lenOfCondition,keyStr,sizeOfKeyStr)) < 0)
	{
		UnionUserErrLog("in UnionGetRecordFileNameFromCondition:: UnionFormRecordFileName from [%s]\n",condition);
		return(ret);
	}
	if ((ret = UnionGetRecordFileName(tableName,keyStr,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetRecordFileNameFromCondition:: UnionGetRecordFileName from [%s] of [%s]\n",condition,tableName);
		return(ret);
	}
	return(ret);
}

/*
功能：	从记录信息文件中读出关键字各字段
输入参数：
	tableName	表名
        maxFldNum       关键字数组的大小
输出参数：
	primaryKeyGrp	关键字数组
返回值：
	>=0：		关键字包括的域数目
	<0：		错误代码
*/
int UnionReadPrimaryKeyFldGrpFromRecInfoFile(char *tableName,char primaryKeyGrp[][128+1],int maxFldNum)
{
	int	ret;
	char	defStr[1024+1];
	
	memset(defStr,0,sizeof(defStr));
	if ((ret = UnionGetPrimaryKeyFromTableRecInfo(tableName,defStr,sizeof(defStr))) < 0)
	{
		UnionUserErrLog("in UnionReadPrimaryKeyFldGrpFromRecInfoFile:: UnionGetPrimaryKeyFromTableRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionReadPrimaryKeyFldGrpFromDefStr(defStr,ret,primaryKeyGrp,maxFldNum));
}

// 根据表格，记录名，获得记录文件的名称
int UnionGetRecordFileName(char *tableName,char *recNullFileName,char *fileName)
{
        char    dir[512+1];
        int     ret;

        memset(dir,0,sizeof(dir));
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
        {
                UnionUserErrLog("in UnionGetRecordFileName:: UnionGetDirOfTable!\n");
                return(ret);
        }
        sprintf(fileName,"%s/%s",dir,recNullFileName);
        return(0);
}

// 判断是否存在记录文件
int UnionExistsRecordFile(char *recordFileName)
{
	return(UnionExistsFile(recordFileName));
}

// 根据记录和键值组成记录的文件名称
int UnionFormRecordFileName(char *tableName,char *record,int lenOfRecord,char *fileName,int sizeOfFileName)
{
	int		ret;
	char		primaryKeyGrp[conMaxNumOfPrimaryKeyFld][128+1];
	int		offset = 0;
	int		index;
	int		fldNum;
	
	if ((fldNum = UnionReadPrimaryKeyFldGrpFromRecInfoFile(tableName,primaryKeyGrp,conMaxNumOfPrimaryKeyFld)) < 0)
	{
		UnionUserErrLog("in UnionFormRecordFileName:: UnionReadPrimaryKeyFldGrpFromRecInfoFile for [%s]\n",tableName);
		return(fldNum);
	}
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionReadRecFldFromRecStr(record,lenOfRecord,primaryKeyGrp[index],fileName+offset,sizeOfFileName-offset)) < 0)
                {
			UnionUserErrLog("in UnionFormRecordFileName:: UnionReadRecFldFromRecStr keyIndex [%d] keyTag [%s]\n",index,primaryKeyGrp[index]);
			return(ret);
		}
                offset += ret;
                if (offset + 1 >= sizeOfFileName)
                {
			UnionUserErrLog("in UnionFormRecordFileName:: sizeOfFileName [%d] too small for [%s]\n",sizeOfFileName,tableName);
			return(errCodeSmallBuffer);
		}
                fileName[offset] = '.';
                offset++;
        }
        if (offset == 0)
        {
                UnionUserErrLog("in UnionFormRecordFileName:: no primary key for [%s]\n",tableName);
                return(errCodeSQLPrimaryKeyIsNull);
        }
        strcpy(fileName+offset,"rec");
        return(offset+3);
}

/*
功能：在表中插入一条记录
输入参数：
	tableName：表名
	record：要插入的记录，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionInsertRecord(char *tableName,char *record,int lenOfRecord)
{
	char	fileName[512],keyStr[512+1];
	int	ret;

	if (tableName == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionInsertRecord:: parameters error!\n");
		return(errCodeParameter);
	}

	// 组成记录文件名称
	memset(fileName,0,sizeof(fileName));
        memset(keyStr,0,sizeof(keyStr));
	ret=UnionGetRecordFileNameFromCondition(tableName,record,lenOfRecord,fileName,keyStr,sizeof(keyStr));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionInsertRecord:: UnionGetRecordFileNameFromCondition fail!return=[%d]\n",ret);
		return(ret);
	}

	// 判断该记录的文件是否存在
	if (UnionExistsRecordFile(fileName))
	{
		UnionUserErrLog("in UnionInsertRecord:: record file [%s] already exists!\n",fileName);
		return(errCodeDatabaseMDL_RecordAlreadyExist);
	}

	// 创建记录文件
	if ((ret = UnionWriteRecStrIntoFile(fileName,record,lenOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionInsertRecord:: UnionWriteRecStrIntoFile [%s] fail!\n",fileName);
		return(ret);
	}

	// 修改记录信息文件
	ret=UnionAddToRecInfo(tableName,keyStr);
	if (ret < 0)
	{
		UnionDeleteRecFile(fileName);	// 删除文件
		UnionUserErrLog("in UnionInsertRecord:: UnionAddToRecInfo for [%s] [%s] fail! return=[%d]\n",tableName,fileName,ret);
		return(ret);
	}

	return(0);
}

/*
功能：删除表中的一条记录
输入参数：
	tableName：表名
	key：要删除记录的键值，格式为"关键字域1=域值|关键字域2=域值|…|关键字域N=域值"
	lenOfKey：键值的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDeleteRecord(char *tableName,char *key,int lenOfKey)
{
	char	fileName[512];
	int	ret;
        char    keyStr[512];
	
	if (tableName == NULL || key == NULL || lenOfKey <= 0)
	{
		UnionUserErrLog("in UnionDeleteRecord:: parameters error!\n");
		return(errCodeParameter);
	}
	
	// 组成记录文件名称
	memset(fileName,0,sizeof(fileName));
        memset(keyStr,0,sizeof(keyStr));
	ret=UnionGetRecordFileNameFromCondition(tableName,key,lenOfKey,fileName,keyStr,sizeof(keyStr));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteRecord:: UnionGetRecordFileNameFromCondition fail!return=[%d]\n",ret);
		return(ret);
	}

	// 判断该记录的文件是否存在
	if (!UnionExistsRecordFile(fileName))
	{
		UnionUserErrLog("in UnionDeleteRecord:: record file [%s] does not exists!\n",fileName);
		UnionDeleteFromRecInfo(tableName,keyStr);	// 删除记录信息表中可能存在的垃圾
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	// 删除记录文件
	if ((ret = UnionDeleteRecFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRecord:: UnionDeleteRecFile!\n",fileName);
		UnionDeleteFromRecInfo(tableName,keyStr);	// 删除记录信息表中可能存在的垃圾
		return(ret);
	}

	// 修改记录信息文件
	ret=UnionDeleteFromRecInfo(tableName,keyStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteRecord:: UnionDeleteFromRecInfo for [%s] [%s] fail! return=[%d]\n",tableName,fileName,ret);
		return(ret);
	}

	return(0);
}

/*
功能：修改表中的一条记录，关键字不能修改
输入参数：
	tableName：表名
	record：要修改的记录，格式为"域1=域值|域2=域值|…|域N=域值"，
		必须带关键字域且关键字域不能修改，其余域可以只包括要修改的域
	lenOfRecord：记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionUpdateRecord(char *tableName,char *record,int lenOfRecord)
{
	char		recStr[2048+1];
	int		lenOfRecStr;
	TUnionRec	oriRec,newRec;
	char		fileName[512+1];
	char		keyStr[512+1];
	int		index;
        int             ret;
	char		tmpVar[512+1];
	
	if (tableName == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: parameters error!\n");
		return(errCodeParameter);
	}
	
	// 读取记录
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectRecord(tableName,record,lenOfRecord,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionSelectRecord [%s]\n",record);
		return(lenOfRecStr);
	}
	// 将要更改的域读入到结构中
	memset(&newRec,0,sizeof(newRec));
	if ((ret = UnionReadRecFromRecStr(record,lenOfRecord,&newRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(ret);
	}
	// 将原来的域读入到结构中
	memset(&oriRec,0,sizeof(oriRec));
	if ((ret = UnionReadRecFromRecStr(recStr,lenOfRecStr,&oriRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecStr,recStr);
		return(ret);
	}
	
	// 将更改的域写入到串中
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionPutRecIntoRecStr(&newRec,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionPutRecIntoRecStr for [%s]!\n",tableName);
		return(ret);
	}
	
	// 将未改的域置入到记录串中
	for (index = 0; index < oriRec.fldNum; index++)
	{
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,oriRec.fldName[index],tmpVar,sizeof(tmpVar))) >= 0)
			continue;	// 是更改了的域
		if ((ret = UnionPutRecFldIntoRecStr(oriRec.fldName[index],oriRec.fldValue[index],strlen(oriRec.fldValue[index]),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
		{
			UnionUserErrLog("in UnionUpdateRecord:: UnionPutRecFldIntoRecStr [%s] for [%s]!\n",oriRec.fldName[index],tableName);
			return(ret);
		}
		lenOfRecStr += ret;
	}
	memset(fileName,0,sizeof(fileName));
	memset(keyStr,0,sizeof(keyStr));
	if ((ret = UnionGetRecordFileNameFromCondition(tableName,recStr,lenOfRecStr,fileName,keyStr,sizeof(keyStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionGetRecordFileNameFromCondition of [%s]!\n",tableName);
		return(ret);
	}
		
	if ((ret = UnionWriteRecStrIntoFile(fileName,recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRecord:: UnionWriteRecStrIntoFile to [%s]!\n",tableName);
		return(ret);
	}
	return(ret);
}

/*
功能：根据关键字查询一条记录
输入参数：
	tableName：表名
	key：记录关键字，格式为"关键字域1=域值|关键字域2=域值|…|关键字域N=域值"
	lenOfKey：关键字的长度
        sizeOfRecord    记录缓冲的大小
输出参数：
	record：查找出的记录
返回值：
	>=0：成功，返回记录的长度
	<0：失败
*/
int UnionSelectRecord(char *tableName,char *key,int lenOfKey,char *record,int sizeOfRecord)
{
	char	fileName[512];
        char    keyStr[512];
	int	ret;

	if (tableName == NULL || key == NULL || record == NULL || lenOfKey <= 0)
	{
		UnionUserErrLog("in UnionSelectRecord:: parameters error!\n");
		return(errCodeParameter);
	}

	// 组成记录文件名称
	memset(fileName,0,sizeof(fileName));
        memset(keyStr,0,sizeof(keyStr));
	ret=UnionGetRecordFileNameFromCondition(tableName,key,lenOfKey,fileName,keyStr,sizeof(keyStr));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSelectRecord:: UnionGetRecordFileNameFromCondition fail!return=[%d]\n",ret);
		return(ret);
	}
	if ((ret = UnionReadRecStrFromFile(fileName,record,sizeOfRecord)) < 0)
	{
		UnionUserErrLog("in UnionSelectRecord:: UnionReadRecStrFromFile %s fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	return(strlen(record));
}

/*
功能：将所有记录写入到文件中
输入参数：
	tableName：表名
	fileName	写入到的文件名称
输出参数：
	无
返回值：
	>=0：		记录数
	<0：		错误代码
*/
int UnionPrintAllRecordToFile(char *tableName,char *condition,char *fileName)
{
	char	recInfoFileName[512+1];
	TUnionRecFileHDL	fromFp;
	FILE	*toFp;
	int	ret;
	int	fileOpened = 0;
	int	len;
	char	recFileName[256+1];
	char	recStr[1024+1];
	int	lenOfRecStr;
	int	recNum = 0;
	int	outputNum = 0;
	
	if (tableName == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionPrintAllRecordToFile:: parameters error!\n");
		return(errCodeParameter);
	}
	// 获得记录索引文件并打开
	if ((ret = UnionGetFileNameOfRecInfo(tableName,recInfoFileName)) < 0)
	{
		UnionUserErrLog("in UnionPrintAllRecordToFile:: UnionGetFileNameOfRecInfo fail! [%s] return=[%d]\n",tableName,ret);
		return(ret);
	}
	if ((fromFp = UnionOpenRecFileHDL(recInfoFileName)) == NULL)
	{
		UnionUserErrLog("in UnionPrintAllRecordToFile:: UnionOpenRecFileHDL [%s]!\n",recInfoFileName);
		return(errCodeUseOSErrCode);
	}
	// 打开输出文件
        if (strcmp(fileName,"stderr") == 0)
                toFp = stderr;
        else if (strcmp(fileName,"stdout") == 0)
                toFp = stdout;
        else
        {
                fileOpened = 1;
                if ((toFp = fopen(fileName,"w")) == NULL)
                {
                        UnionSystemErrLog("in UnionPrintAllRecordToFile:: fopen [%s]!\n",fileName);
                        fclose(fromFp);
                        return(errCodeUseOSErrCode);
                }
        }
        for (;;)
	{
		memset(recFileName,0,sizeof(recFileName));
		if (recNum == 0)
			len = UnionReadRecOfIndexFromFile(fromFp,2,recFileName,sizeof(recFileName));
		else
			len = UnionReadNextRecFromFile(fromFp,recFileName,sizeof(recFileName));
		if (len <= 0)
			break;
		recNum++;
		if ((ret = UnionGetRecordFileName(tableName,recFileName,recInfoFileName)) < 0)
		{
			UnionUserErrLog("in UnionPrintAllRecordToFile:: UnionGetRecordFileName! [%s] [%s]\n",tableName,recFileName);
			continue;
		}
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadRecStrFromFile(recInfoFileName,recStr,sizeof(recStr))) < 0)
		{
			UnionUserErrLog("in UnionPrintAllRecordToFile:: UnionReadRecStrFromFile! [%s] [%s]\n",tableName,recFileName);
			continue;
		}
                if (!UnionIsRecStrFitSpecConditon(recStr,lenOfRecStr,condition,strlen(condition)))
                        continue;
		fprintf(toFp,"%s\n",recStr);
		outputNum++;
	}
	UnionCloseRecFileHDL(fromFp);
	if (fileOpened)
		fclose(toFp);
	UnionNullLog("in UnionPrintAllRecordToFile:: [%d] records found, print to file = [%d]\n",recNum,outputNum);
	return(outputNum);
}



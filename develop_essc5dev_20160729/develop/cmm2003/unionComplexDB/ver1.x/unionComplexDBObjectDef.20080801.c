//	Author: ChenJiaMei
//	Date: 2008-8-4

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionComplexDBObjectSql.h"
#include "unionREC.h"
#include "unionComplexDBObjectDef.h"

/*
功能：检查一个对象的定义是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectDef(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象定义文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefFileName(idOfObject,caFileName);

	return(UnionExistsFile(caFileName));
}

/*
function:
        判断是否是对象中的字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsFldOfObject(PUnionObject pobject, char *fldName)
{
        int             ret;
        int             i;
        char            *ptr,*ptr1; //add by xusj 20091125
        char            tmpBuf[128+1]; //add by xusj 20091125
        memset(tmpBuf, 0, sizeof tmpBuf);

        if (NULL == fldName || pobject == 0)
                return 0;

        /***del by xusj begin 20091125***
        for(i = 0; i < pobject->fldNum; i++)
                if(strcmp(pobject->fldDefGrp[i].name, fldName) == 0)
                        return 1;
        ***del by xusj end****/

        /***add by xusj begin 20091125***/
        for(i = 0; i < pobject->fldNum; i++)
        {
                if(strcmp(pobject->fldDefGrp[i].name, fldName) == 0)
                        return 1;
        }
        for(i = 0; i < pobject->fldNum; i++)
        {
                if( (ptr=strstr(fldName,pobject->fldDefGrp[i].name)) != NULL)
                {
		 	if ( (ptr1=strchr(fldName,'(')) != NULL ) // 类似 trim(field)情况
			{
				if (strncmp(ptr1+1, pobject->fldDefGrp[i].name, strlen(pobject->fldDefGrp[i].name)) == 0)
				{
					strncpy(tmpBuf,fldName,(ptr-fldName));
					if (!UnionIsTBLFldWithoutPrefix(pobject->name))
						strcat(tmpBuf,UnionGetPrefixOfDBField());
					strcat(tmpBuf,ptr);
					strcpy(fldName, tmpBuf);
					return 0;
				}
			}
                }
        }
	/***add by xusj end 20091125***/
        return 0;
}

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	pobject：		指向对象定义的指针
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameInNameGrp(PUnionObject pobject,char defFldNameGrp[][128+1],int numOfFld,char realFldNameGrpInDB[][128+1])
{
	int	index;
	int	isTableName=0;
	int	num = 0;
	char	tmpBuf[128+1];
	
	for (index = 0; index < numOfFld; index++)
	{
		strcpy(tmpBuf,defFldNameGrp[index]);
		if (!UnionIsFldOfObject(pobject,tmpBuf) || isTableName == 2)
		{
                        if (isTableName == 1)
                                isTableName=2;
                        else
                                isTableName=0;
			strcpy(realFldNameGrpInDB[index],tmpBuf);
			if (strcmp(tmpBuf,"from") == 0)
				isTableName = 1;
		}
		else
		{
			++num;
			UnionChargeFieldNameToDBFieldName(tmpBuf,realFldNameGrpInDB[index]);
		}
	}
	return(num);
}

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	idOfObject		对象名称
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObjectInNameGrp(TUnionIDOfObject idOfObject,char defFldNameGrp[][128+1],int numOfFld,char realFldNameGrpInDB[][128+1])
{
	TUnionObject	objDef;
	int		ret;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionConvertObjectFldNameOfSpecObjectInNameGrp:: UnionReadObjectDef [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	return(UnionConvertObjectFldNameInNameGrp(&objDef,defFldNameGrp,numOfFld,realFldNameGrpInDB));
}

/*
功能：判断一个对象的定义是否合法
输入参数：
	objectDef：对象定义
输出参数：
	无
返回值：
	1：合法
	0：不合法
	<0：失败，错误码
*/
int UnionIsValidObjectDef(TUnionObject objectDef)
{
	int	iRet,i,j;
	TUnionObjectFldGrp	sFldGrpTmp;
	TUnionObject		sForeignObj;
	
	// 判断对象名称是否合法
	if (UnionIsValidObjectFldName(objectDef.name) == 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: name of object [%s] is invalid\n",objectDef.name);
		return(0);
	}
	
	// 判断域定义是否合法
	if (objectDef.fldNum <= 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: fldNum of object [%d] is invalid\n",objectDef.fldNum);
		return(0);
	}
	for (i=0;i<objectDef.fldNum;i++)
	{
		// 判断域定义是否合法
		iRet=UnionIsValidObjectFieldDef(objectDef.fldDefGrp[i]);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionIsValidObjectFieldDef fail! return=[%d]\n",iRet);
			return(iRet);
		}
		if (iRet == 0)
		{
			UnionAuditLog("in UnionIsValidObjectDef:: field [%s] of object [%s] is valid\n",(objectDef.fldDefGrp[i]).name,objectDef.name);
			return(0);
		}
	}
	
	// 判断关键字定义是否合法
	if ((iRet = UnionCheckFldGrpInObject(&(objectDef.primaryKey),&objectDef)) < 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: primaryKey fld group does not defined in object [%s]!\n",objectDef.name);
		return(0);
	}
	
	// 判断唯一值定义是否合法
	for (i = 0; i < objectDef.uniqueFldGrpNum; i++)
	{
		if ((iRet = UnionCheckFldGrpInObject(&(objectDef.uniqueFldGrp[i]),&objectDef)) < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: unique fld group does not defined in object [%s]!\n",objectDef.name);
			return(0);
		}
		iRet=UnionFldGrpIsRepeat(&(objectDef.primaryKey),&(objectDef.uniqueFldGrp[i]));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionFldGrpIsRepeat fail! return=[%d]\n",iRet);
			return(iRet);
		}
		else if (iRet == 1)
		{
			UnionAuditLog("in UnionIsValidObjectDef:: unique fld group [%d] is repeat of primary key!\n",i);
			return(0);
		}
	}
	
	// 判断外部关键字中的本地域组是否合法
	for (i = 0; i < objectDef.foreignFldGrpNum; i++)
	{
		if ((iRet = UnionCheckFldGrpInObject(&(objectDef.foreignFldGrp[i].localFldGrp),&objectDef)) < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: foreign key fld group does not defined in object [%s]!\n",objectDef.name);
			return(0);
		}
	}
	
	// 判断外部关键字中的外部域组是否合法
	for (i = 0; i < objectDef.foreignFldGrpNum; i++)
	{
		// 读外部对象定义
		memset(&sForeignObj,0,sizeof(TUnionObject));
		iRet=UnionReadObjectDef(objectDef.foreignFldGrp[i].objectName,&sForeignObj);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionReadObjectDef for [%s] fail! return=[%d]\n",objectDef.foreignFldGrp[i].objectName,iRet);
			return(iRet);
		}
		// 判断外键定义是否外部对象的关键字
		iRet=UnionFldGrpIsCompleteRepeat(&(objectDef.foreignFldGrp[i].foreignFldGrp),&(sForeignObj.primaryKey));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionFldGrpIsCompleteRepeat for [%d] foreign primary fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		if (iRet == 0)
		{
			// 判断外键定义是否外部对象的唯一值
			for (j=0;j<sForeignObj.uniqueFldGrpNum;j++)
			{
				iRet=UnionFldGrpIsCompleteRepeat(&(objectDef.foreignFldGrp[i].foreignFldGrp),&(sForeignObj.uniqueFldGrp[j]));
				if (iRet < 0)
				{
					UnionUserErrLog("in UnionIsValidObjectDef:: UnionFldGrpIsCompleteRepeat for [%d][%d] foreign unique fail! return=[%d]\n",i,j,iRet);
					return(iRet);
				}
				if (iRet == 1)
					break;
			}
			if (j == sForeignObj.uniqueFldGrpNum)
			{
				UnionUserErrLog("in UnionIsValidObjectDef:: foreign key is not primary and unique key!\n");
				return(0);
			}
		}
	}

	return(1);
}

/*
功能：将对象定义，写入到对象定义文件中
输入参数：
	pobject：指向对象定义的指针
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionStoreObjectDefAnyway(PUnionObject pobject)
{
	char	caFileName[512];
	int	ret;
	
	if (pobject == NULL)
	{
		UnionUserErrLog("in UnionStoreObjectDefAnyway:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象定义文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefFileName(pobject->name,caFileName);

	if ((ret = UnionPrintObjectDefToSpecFile(pobject,caFileName)) < 0)
	{
		UnionUserErrLog("in UnionStoreObjectDefAnyway:: UnionPrintObjectDefToSpecFile!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	return(ret);
}

/*
功能：将对象定义，写入到指定文件中
输入参数：
	pobject：	指向对象定义的指针
	fileName	文件名
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionPrintObjectDefToSpecFile(PUnionObject pobject,char *fileName)
{
	// TUnionRecFileHDL	hFileHdl;
	char	caRecord[512],caBuf[512],caTmpBuf[128];
	int	iRet,iRecLen,i;
	
	if (pobject == NULL)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	if (UnionExistsFile(fileName))
	{
		if ((iRet = UnionDeleteFile(fileName)) < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionDeleteFile!\n");
			return(UnionSetUserDefinedErrorCode(iRet));
		}
	}
		
	// 写对象定义
	// 写对象名称
	iRet=UnionAppendRecStrToFile(fileName,"# 对象名称定义",14);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for 对象名称定义 fail! return=[%d]\n",iRet);
		return(iRet);
	}
	memset(caRecord,0,sizeof(caRecord));
	sprintf(caRecord,"%s%s",ObjectDefFlagOfObjectName,pobject->name);
	iRet=UnionAppendRecStrToFile(fileName,caRecord,strlen(caRecord));
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	// 写域定义
	iRet=UnionAppendRecStrToFile(fileName,"\n# 域定义",8);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for 域定义 fail! return=[%d]\n",iRet);
		return(iRet);
	}
	for (i=0;i<pobject->fldNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		strcpy(caRecord,ObjectDefFlagOfFldDefGrp);
		iRecLen=ObjectDefFlagLenOfFldDefGrp;
		// 域名称
		iRet=UnionPutRecFldIntoRecStr("name",pobject->fldDefGrp[i].name,strlen(pobject->fldDefGrp[i].name),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] name fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 域类型
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].type);
		iRet=UnionPutRecFldIntoRecStr("type",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] type fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 域大小
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].size);
		iRet=UnionPutRecFldIntoRecStr("size",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] size fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		/*
		// 是否唯一值
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].isUnique);
		iRet=UnionPutRecFldIntoRecStr("isUnique",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] isUnique fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		*/
		// 是否允许空值
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].nullPermitted);
		iRet=UnionPutRecFldIntoRecStr("nullPermitted",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] nullPermitted fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 缺省值
		if (strlen(pobject->fldDefGrp[i].defaultValue) > 0)
		{
			iRet=UnionPutRecFldIntoRecStr("defaultValue",pobject->fldDefGrp[i].defaultValue,strlen(pobject->fldDefGrp[i].defaultValue),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] defaultValue fail!\n",i);
				return(iRet);
			}
			iRecLen += iRet;
		}
		// 说明
		if (strlen(pobject->fldDefGrp[i].remark) > 0)
		{
			iRet=UnionPutRecFldIntoRecStr("remark",pobject->fldDefGrp[i].remark,strlen(pobject->fldDefGrp[i].remark),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] remark fail!\n",i);
				return(iRet);
			}
			iRecLen += iRet;
		}
		iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	// 写关键字
	iRet=UnionAppendRecStrToFile(fileName,"\n# 关键字定义",13);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for 关键字定义 fail! return=[%d]\n",iRet);
		return(iRet);
	}
	memset(caRecord,0,sizeof(caRecord));
	strcpy(caRecord,ObjectDefFlagOfPrimaryKey);
	iRecLen=ObjectDefFlagLenOfPrimaryKey;
	memset(caBuf,0,sizeof(caBuf));
	iRet=UnionFormFieldNameStrFromFldGrp(&pobject->primaryKey,caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionFormFieldNameStrFromFldGrp for primary key fail! return=[%d]\n",iRet);
		return(iRet);
	}
	strcat(caRecord,caBuf);
	iRecLen += iRet;
	iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	// 写唯一值组
	iRet=UnionAppendRecStrToFile(fileName,"\n# 唯一值定义",13);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for 唯一值定义 fail! return=[%d]\n",iRet);
		return(iRet);
	}
	for (i=0;i<pobject->uniqueFldGrpNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		strcpy(caRecord,ObjectDefFlagOfUniqueFldGrp);
		iRecLen=ObjectDefFlagLenOfUniqueFldGrp;
	
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&pobject->uniqueFldGrp[i],caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionFormFieldNameStrFromFldGrp for unique key fail! return=[%d]\n",iRet);
			return(iRet);
		}
		strcat(caRecord,caBuf);
		iRecLen += iRet;
		iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	// 写外部关键字
	iRet=UnionAppendRecStrToFile(fileName,"\n# 外部关键字定义",17);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for 外部关键字定义 fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	for (i=0;i<pobject->foreignFldGrpNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		strcpy(caRecord,ObjectDefFlagOfForeignFldGrp);
		iRecLen=ObjectDefFlagLenOfForeignFldGrp;
	
		// 本地域组
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(pobject->foreignFldGrp[i]).localFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionFormFieldNameStrFromFldGrp for foreign key localFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("localFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] localFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部对象名称
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(pobject->foreignFldGrp[i]).objectName,strlen((pobject->foreignFldGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部域组
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(pobject->foreignFldGrp[i]).foreignFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionFormFieldNameStrFromFldGrp for foreign key foreignFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("foreignFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] foreignFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		iRet=UnionAppendRecStrToFile(fileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
功能：将对象定义，写入到对象定义文件中
输入参数：
	pobject：指向对象定义的指针
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionStoreObjectDef(PUnionObject pobject)
{
	// 检查对象定义文件是否存在
	if (pobject == NULL)
		return(errCodeParameter);
	if (UnionExistsObjectDef(pobject->name) > 0)
	{
		UnionUserErrLog("in UnionStoreObjectDef:: [%s] already exists!\n",pobject->name);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	return(UnionStoreObjectDefAnyway(pobject));
}


/*
功能：从对象定义文件，读一个对象定义
输入参数：
	idOfObject：要读的对象ID
输出参数：
	pobject：指向对象定义的指针
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject)
{
	//TUnionRecFileHDL	hFileHdl;
	char			*ptr = NULL;
	FILE	*hFileHdl;
	char	caFileName[512],caRecord[512],caBuf[512];
	int	iRet,iRecLen;
	
	if (idOfObject == NULL || pobject == NULL)
	{
		UnionUserErrLog("in UnionReadObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// 获得对象定义文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefFileName(idOfObject,caFileName);
	
	// 检查对象定义文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionReadObjectDef:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	// UnionLog("in UnionReadObjectDef:: the caFileName is: [%s].\n", caFileName);
	// 读对象定义
	//hFileHdl=UnionOpenRecFileHDL(caFileName);
	//if (hFileHdl == NULL)
	if ((hFileHdl = fopen(caFileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionReadObjectDef:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	
	memset(pobject,0,sizeof(TUnionObject));
	while(!feof(hFileHdl))
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		//iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		iRecLen = UnionReadOneDataLineFromTxtFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			if (iRecLen == errCodeFileEnd)
				break;
			UnionUserErrLog("in UnionReadObjectDef:: UnionReadNextRecFromFile fail! return=[%d] fileName = [%s]\n",iRecLen,caFileName);
			//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// 读文件结束
			break;
		
		// UnionLog("in UnionReadObjectDef:: caRecord is: [%s].\n", caRecord);
		
		if (memcmp(caRecord,ObjectDefFlagOfObjectName,ObjectDefFlagLenOfObjectName) == 0)	// 对象名称定义
		{
			strcpy(pobject->name,&caRecord[ObjectDefFlagLenOfObjectName]);
			if (strcmp(pobject->name,idOfObject) != 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: [%s] error!\n",caRecord);
				UnionUserErrLog("in UnionReadObjectDef:: pobject->name: [%s] does not equal to idOfObject: [%s]!\n",pobject->name,idOfObject);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectNameError));
			}
		}
		else if (memcmp(caRecord,ObjectDefFlagOfFldDefGrp,ObjectDefFlagLenOfFldDefGrp) == 0)	// 域定义
		{
			// 域名称
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"name",(pobject->fldDefGrp[pobject->fldNum]).name,sizeof((pobject->fldDefGrp[pobject->fldNum]).name));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for name from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfFldDefGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// 域类型
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"type",&(pobject->fldDefGrp[pobject->fldNum].type));
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for type fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// 大小
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"size",&(pobject->fldDefGrp[pobject->fldNum].size));
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for size fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].size = UnionGetDefaultSizeOfTBLFieldType(pobject->fldDefGrp[pobject->fldNum].type);
			}
			/*
			// 是唯一值
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"isUnique",&(pobject->fldDefGrp[pobject->fldNum]).isUnique);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for isUnique fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			*/
			// 是否允许空值
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"nullPermitted",(int *)&(pobject->fldDefGrp[pobject->fldNum].nullPermitted));
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for nullPermitted fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].nullPermitted = 1;
			}

			// 缺省值定义
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"defaultValue",(pobject->fldDefGrp[pobject->fldNum]).defaultValue,sizeof((pobject->fldDefGrp[pobject->fldNum]).defaultValue));
			// 说明
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"remark",(pobject->fldDefGrp[pobject->fldNum]).remark,sizeof((pobject->fldDefGrp[pobject->fldNum]).remark));
			pobject->fldNum++;
		}
		else if (memcmp(caRecord,ObjectDefFlagOfPrimaryKey,ObjectDefFlagLenOfPrimaryKey) == 0)	// 关键字定义
		{
			iRet=UnionFormFldGrpFromFieldNameStr(&caRecord[ObjectDefFlagLenOfPrimaryKey],iRecLen-ObjectDefFlagLenOfPrimaryKey,&pobject->primaryKey);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfPrimaryKey]);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
		}
		else if (memcmp(caRecord,ObjectDefFlagOfUniqueFldGrp,ObjectDefFlagLenOfUniqueFldGrp) == 0)	// 唯一值定义
		{
			iRet=UnionFormFldGrpFromFieldNameStr(&caRecord[ObjectDefFlagLenOfUniqueFldGrp],iRecLen-ObjectDefFlagLenOfUniqueFldGrp,&pobject->uniqueFldGrp[pobject->uniqueFldGrpNum]);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfUniqueFldGrp]);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			pobject->uniqueFldGrpNum++;
		}
		else if (memcmp(caRecord,ObjectDefFlagOfForeignFldGrp,ObjectDefFlagLenOfForeignFldGrp) == 0)	// 外部关键字定义
		{
			// 本地域组
			memset(caBuf,0,sizeof(caBuf));
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"localFldGrp",caBuf,sizeof(caBuf));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for localFldGrp from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			iRet=UnionFormFldGrpFromFieldNameStr(caBuf,iRet,&(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).localFldGrp);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",caBuf);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// 外部对象名称
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"foreignObject",(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName,sizeof((pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for foreignObject from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// 外部域组
			memset(caBuf,0,sizeof(caBuf));
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"foreignFldGrp",caBuf,sizeof(caBuf));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for foreignFldGrp from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			iRet=UnionFormFldGrpFromFieldNameStr(caBuf,iRet,&(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).foreignFldGrp);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",caBuf);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			pobject->foreignFldGrpNum++;
		}
	}
	
	//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	fclose(hFileHdl);

	return(0);
}

/*
功能：从对象定义文件，读一个对象定义的关键字清单
输入参数：
	idOfObject：要读的对象ID	
	sizeOfBuf	关键字清单的大小
输出参数：
	primaryKeyList	关键字清单，两个域之间以,分开
返回值：
	0：关键字串的长度
	<0：失败，返回错误码
*/
int UnionReadObjectPrimarKeyList(TUnionIDOfObject idOfObject,char *primaryKeyList,int sizeOfBuf)
{
	TUnionObject	objDef;
	int		ret;
	int		index;
	int		offset = 0;
	int		fldLen;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(idOfObject,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionReadObjectPrimarKeyList:: UnionReadObjectDef [%s]!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	
	for (index = 0; index < objDef.primaryKey.fldNum; index++)
	{
		fldLen = strlen(objDef.primaryKey.fldNameGrp[index]);
		if (fldLen + offset + 1 >= sizeOfBuf)
		{
			UnionUserErrLog("in UnionReadObjectPrimarKeyList:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
			return(UnionSetUserDefinedErrorCode(errCodeParameter));
		}
		if (index > 0)
		{
			memcpy(primaryKeyList+offset,",",1);
			offset++;
		}
		memcpy(primaryKeyList+offset,objDef.primaryKey.fldNameGrp[index],fldLen);
		offset += fldLen;
	}
	return(offset);
}


/*
功能：删除一个对象定义
输入参数：
	idOfObject：对象的ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionDropObjectDef(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionDropObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象定义文件是否存在
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectDef:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionDropObjectDef:: object [%s] defined file does not exists!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 删除本对象的定义文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectDef:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// 删除本对象的外部关键字定义清单文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectChildrenDefFileName(idOfObject,caBuf);
	if (!UnionExistsFile(caBuf))
		return(0);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectDef:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能	
	将一个域组写成串
输入参数
	flag		域组的标识
	pfldGrp		域组
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateFldGrpIntoSQLToFp(char *flag,PUnionObjectFldGrp pfldGrp,FILE *fp)
{
	FILE	*outFp = stdout;
	char	flagTag[128];
	int	fldNum;
	char	prefixFldName[128+1];
	
	if (pfldGrp == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	if (flag == NULL)
		strcpy(flagTag,"");
	else
		strcpy(flagTag,flag);	
	for (fldNum = 0; fldNum < pfldGrp->fldNum; fldNum++)
	{
		// 2009/6/23 张永定增加
		memset(prefixFldName,0,sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(pfldGrp->fldNameGrp[fldNum],prefixFldName);
		
		if (fldNum == 0)
		{
			if (strlen(flagTag) == 0)
				fprintf(outFp,"	(%s",prefixFldName);
			else
				fprintf(outFp,"	%s (%s",flagTag,prefixFldName);
		}
		else
			fprintf(outFp,",%s",prefixFldName);
	}
	if (pfldGrp->fldNum > 0)
	{
		//if ((flag == NULL) || (strlen(flag) == 0))
			fprintf(outFp,") ");
		//else
		//	fprintf(outFp,"),\n");
	}
	return(0);
}

/*
功能	
	获得数据库域的最大长度
输入参数
	fldSize		域长度
输入出数
	无
返回值
	域的合法长度
*/
int UnionGetValidFldSizeOfDatabase(int fldSize)
{
	int	maxFldSize;
	
	if ((maxFldSize = UnionReadIntTypeRECVar("maxFldSizeOfDatabase")) <= 0)
		return(fldSize);
	if (fldSize >= maxFldSize)
		return(maxFldSize);
	else
		return(fldSize);
}

/*
功能	
	从一个表定义创建SQL语句
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateSQLFromObjectDefToFp(PUnionObject prec,FILE *fp)
{
	FILE				*outFp = stdout;
	int				ret;
	int				fldNum;
	PUnionObjectFldDef		pfldDef;
	char				typeName[40+1];
	int				index;
	PUnionDefOfRelatedObject	prelation;
	int				dhNeeded;
	char				prefixFldName[128+1];
	int				mustBeNotNullFld = 0;
	
	if (prec == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	// 写创建语句
	fprintf(outFp,"-- 生成表 %s\n",prec->name);
	fprintf(outFp,"create table %s\n",prec->name);
	fprintf(outFp,"(\n");
	// 创建域
	for (fldNum = 0; fldNum < prec->fldNum; fldNum++)
	{
		pfldDef = &(prec->fldDefGrp[fldNum]);
		memset(typeName,0,sizeof(typeName));
		UnionGetFldTypeName(pfldDef->type,typeName);
		if (strlen(pfldDef->remark) > 0)
			fprintf(outFp,"	-- %s \n",pfldDef->remark);

		// 2009/6/23 张永定增加
		memset(prefixFldName,0,sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(pfldDef->name,prefixFldName);

		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size >= 254)
					fprintf(outFp,"	%s		varchar",prefixFldName);
				else
					fprintf(outFp,"	%s		%s",prefixFldName,typeName);
				fprintf(outFp,"(%d)",UnionGetValidFldSizeOfDatabase(pfldDef->size));
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	%s		%s",prefixFldName,typeName);
				fprintf(outFp,"(%d)",UnionGetValidFldSizeOfDatabase(pfldDef->size));
				break;
			case	conObjectFldType_Bool:
			case	conObjectFldType_Int:
			case	conObjectFldType_Long:
				fprintf(outFp,"	%s		%s",prefixFldName,"integer");
				break;
			case	conObjectFldType_Double:
				if (UnionReadIntTypeRECVar("isDBUseOracle") > 0)
					fprintf(outFp," %s              number(12,2)",prefixFldName);
				else
					fprintf(outFp,"	%s		%s",prefixFldName,typeName);
				break;
			default:
				fprintf(outFp,"	%s		%s",prefixFldName,typeName);
				break;
		}
		mustBeNotNullFld = 0;
		if ((UnionIsPrimaryKeyFldOfObject(prec,pfldDef->name)) || (UnionIsUniqueFldOfObject(prec,pfldDef->name)))
		{
			// 是关键字或唯一值，必须不为空域
			mustBeNotNullFld = 1;
			if (pfldDef->nullPermitted)
				pfldDef->nullPermitted = 0;
		}
		if (strlen(pfldDef->defaultValue) > 0)
		{
			// 定义了缺省值
			if (mustBeNotNullFld)	// 域值必须不为空
				fprintf(outFp," not null");
			else
			{
				switch (pfldDef->type)
				{
					case	conObjectFldType_String:
					case	conObjectFldType_Bit:
						fprintf(outFp," default '%s'",pfldDef->defaultValue);
						break;
					default:
						fprintf(outFp," default %s",pfldDef->defaultValue);
						break;
				}
			}
		}
		else	// 没定义缺省值
		{
			if (!pfldDef->nullPermitted)
				fprintf(outFp," not null");
		}
		fprintf(outFp,",");
		fprintf(outFp,"\n");
	}

	// 创关键字
	fprintf(outFp,"	-- 定义关键字 \n");
	if ((ret = UnionCreateFldGrpIntoSQLToFp("primary key ",&(prec->primaryKey),outFp)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLFromObjectDefToFp:: UnionCreateFldGrpIntoSQLToFp for primary key of [%s]\n",prec->name);
		return(ret);
	}
	dhNeeded = 1;
	for (index = 0; index < prec->uniqueFldGrpNum; index++)
	{
		if (dhNeeded)
		{
			fprintf(outFp,",\n");
			dhNeeded = 0;
		}
		fprintf(outFp,"	-- 定义唯一值\n");
		if ((ret = UnionCreateFldGrpIntoSQLToFp("unique ",&(prec->uniqueFldGrp[index]),outFp)) < 0)
		{
			UnionUserErrLog("in UnionCreateSQLFromObjectDefToFp:: UnionCreateFldGrpIntoSQLToFp for the [%dth] unique of [%s]\n",index,prec->name);
			return(ret);
		}
		dhNeeded = 1;
	}
	// 创外部关键字
	for (index = 0; index < prec->foreignFldGrpNum; index++)
	{
		prelation = &(prec->foreignFldGrp[index]);
		if (strlen(prelation->objectName) == 0)
			continue;
		if (dhNeeded)
		{
			fprintf(outFp,",\n");
			dhNeeded = 0;
		}
		fprintf(outFp,"	-- 定义外部键值 \n");
		fprintf(outFp,"	foreign key ");
		if ((ret = UnionCreateFldGrpIntoSQLToFp("",&(prelation->localFldGrp),outFp)) < 0)
		{
			UnionUserErrLog("in UnionCreateSQLFromObjectDefToFp:: UnionCreateFldGrpIntoSQLToFp for the [%dth] foreign key of [%s]\n",index,prec->name);
			return(ret);
		}
		fprintf(outFp,"references ");
		if ((ret = UnionCreateFldGrpIntoSQLToFp(prelation->objectName,&(prelation->foreignFldGrp),outFp)) < 0)
		{
			UnionUserErrLog("in UnionCreateSQLFromObjectDefToFp:: UnionCreateFldGrpIntoSQLToFp for the [%dth] foreign key of [%s]\n",index,prec->name);
			return(ret);
		}
		dhNeeded = 1;
	}
	fprintf(outFp,"\n");
	fprintf(outFp,");\n");
	return(0);
}

/*
功能：
	转换一个对象域的名称
输入参数：
	objectName	对象名称
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObject(char *objectName,char *oriFldName,char *defName)
{
	int	ret;
	TUnionObject	object;
	
	memset(&object,0,sizeof(object));
	if ((ret = UnionReadObjectDef(objectName,&object)) < 0)
	{
		UnionUserErrLog("in UnionConvertObjectFldNameOfSpecObject:: UnionReadObjectDef [%s]!\n",objectName);
		return(ret);
	}
	return(UnionConvertObjectFldName(&object,oriFldName,defName));
}

/*
功能：
	转换一个对象域的名称
输入参数：
	pobject：	指向对象定义的指针
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldName(PUnionObject pobject,char *oriFldName,char *defName)
{
	int	fldNum;
	char	tmpBuf3[100];
	char	tmpBuf1[100];
	char	tmpBuf2[100];		
	char	*ptr;
	int	offset;
	char	prefix[100];
	
	if ((pobject == NULL) || (oriFldName == NULL) || (defName == NULL))
	{
		UnionUserErrLog("in UnionConvertObjectFldName:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	strcpy(tmpBuf3,oriFldName);
	UnionToUpperCase(tmpBuf3);
	if ((ptr = UnionGetPrefixOfDBField()) == NULL)
		strcpy(tmpBuf1,tmpBuf3);
	else
	{
		offset = strlen(ptr);
		strcpy(prefix,ptr);
		UnionToUpperCase(prefix);
		if (strncmp(tmpBuf3,prefix,offset) == 0)
			strcpy(tmpBuf1,tmpBuf3+offset);
		else
			strcpy(tmpBuf1,tmpBuf3);
	}
	for (fldNum = 0; fldNum < pobject->fldNum; fldNum++)
	{
		strcpy(tmpBuf2,pobject->fldDefGrp[fldNum].name);
		UnionToUpperCase(tmpBuf2);
		if (strcmp(tmpBuf1,tmpBuf2) == 0)
		{
			strcpy(defName,pobject->fldDefGrp[fldNum].name);
			return(strlen(defName));
		}
	}
	//UnionAuditLog("in UnionConvertObjectFldName:: fldName [%s] not valid fld of object [%s]\n",oriFldName,pobject->name);
	strcpy(defName,oriFldName);
	return(strlen(defName));
	//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}


/*
功能：
	获得一个对象的域定义的类型
输入参数：
	pobject：	指向对象定义的指针
	fldName		域名称
输出参数：
	fldTypeName	域的c语言类型
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionGetTypeOfSpecFldFromObjectDef(PUnionObject pobject,char *fldName,char *fldTypeName)
{
	int	fldNum;
		
	if ((pobject == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (fldNum = 0; fldNum < pobject->fldNum; fldNum++)
	{
		if (strcmp(fldName,pobject->fldDefGrp[fldNum].name) == 0)
		{
			if (fldTypeName == NULL)
				return(pobject->fldDefGrp[fldNum].type);
			switch (pobject->fldDefGrp[fldNum].type)
			{
				case	conObjectFldType_String:
					if (pobject->fldDefGrp[fldNum].size > 1)
						strcpy(fldTypeName,"char *");
					else
						strcpy(fldTypeName,"char");
					break;
				case	conObjectFldType_Int:
					strcpy(fldTypeName,"int");
					break;
				case	conObjectFldType_Double:
					strcpy(fldTypeName,"double");
					break;
				case	conObjectFldType_Bit:
					strcpy(fldTypeName,"unsigned char *");
					break;
				case	conObjectFldType_Bool:
					strcpy(fldTypeName,"int");
					break;
				case	conObjectFldType_Long:
					strcpy(fldTypeName,"long");
					break;
				default:
					UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: invalid type [%d] of fld [%s] of object [%s]\n",
						pobject->fldDefGrp[fldNum].type,fldName,pobject->name);
					return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldTypeInvalid));
			}
			return(pobject->fldDefGrp[fldNum].type);
		}
	}
	UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: fldName [%s] not valid fld of object [%s]\n",fldName,pobject->name);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}

/*
功能：
	获得一个对象的域定义的说明
输入参数：
	pobject：	指向对象定义的指针
	fldName		域名称
输出参数：
	remark		域说明
返回值：
	>=0：	成功
	<0：	失败，返回错误码
*/
int UnionGetRemarkOfSpecFldFromObjectDef(PUnionObject pobject,char *fldName,char *remark)
{
	int	fldNum;
		
	if ((pobject == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionGetRemarkOfSpecFldFromObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (fldNum = 0; fldNum < pobject->fldNum; fldNum++)
	{
		if (strcmp(fldName,pobject->fldDefGrp[fldNum].name) == 0)
		{
			strcpy(remark,pobject->fldDefGrp[fldNum].remark);
			return(0);
		}
	}
	UnionUserErrLog("in UnionGetRemarkOfSpecFldFromObjectDef:: fldName [%s] not valid fld of object [%s]\n",fldName,pobject->name);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}


/*
功能	
	根据表定义，将关键字写成参数说明
输入参数
	pobjectDef	表定义
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPrimaryKeyRemarkAsVarFunToFp(PUnionObject pobjectDef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	ret;
	char	remark[256+1];
	int	fldNum;
	
	if (pobjectDef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
		
	for (fldNum = 0; fldNum < pobjectDef->primaryKey.fldNum; fldNum++)
	{
		if ((ret = UnionGetRemarkOfSpecFldFromObjectDef(pobjectDef,pobjectDef->primaryKey.fldNameGrp[fldNum],remark)) < 0)
		{
			UnionUserErrLog("in UnionGenerateRecPrimaryKeyRemarkAsVarFunToFp:: UnionGetRemarkOfSpecFldFromObjectDef!\n");
			return(ret);
		}
		fprintf(outFp,"	%s %s\n",pobjectDef->primaryKey.fldNameGrp[fldNum],remark);
	}
	return(0);
}

/*
功能	
	根据表定义，将关键字写成函数参数
输入参数
	pobjectDef	表定义
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPrimaryKeyAsVarFunToFp(PUnionObject pobjectDef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	type;
	char	fldTypeName[256+1];
	int	fldNum;
	
	if (pobjectDef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
		
	for (fldNum = 0; fldNum < pobjectDef->primaryKey.fldNum; fldNum++)
	{
		if ((type = UnionGetTypeOfSpecFldFromObjectDef(pobjectDef,pobjectDef->primaryKey.fldNameGrp[fldNum],fldTypeName)) < 0)
		{
			UnionUserErrLog("in UnionGenerateRecPrimaryKeyAsVarFunToFp:: UnionGetTypeOfSpecFldFromObjectDef!\n");
			return(type);
		}
		if (fldNum >= 1)
			fprintf(outFp,",");
		if (fldTypeName[strlen(fldTypeName)-1] == '*')
			fprintf(outFp,"%s%s",fldTypeName,pobjectDef->primaryKey.fldNameGrp[fldNum]);
		else
			fprintf(outFp,"%s %s",fldTypeName,pobjectDef->primaryKey.fldNameGrp[fldNum]);
	}
	return(0);
}

/*
功能	
	根据表定义，将关键字拼串写入到文件中
输入参数
	funName		函数名称
	pobjectDef	表定义
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPrimaryKeyIntoRecStrToFp(char *funName,PUnionObject pobjectDef,FILE *fp)
{
	int	fldNum;
	int	type;
	FILE	*outFp = stdout;
	char	fldTypeName[128+1];
	
	if (pobjectDef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"	// 拼关键字\n");
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	for (fldNum = 0; fldNum < pobjectDef->primaryKey.fldNum; fldNum++)
	{
		if ((type = UnionGetTypeOfSpecFldFromObjectDef(pobjectDef,pobjectDef->primaryKey.fldNameGrp[fldNum],fldTypeName)) < 0)
		{
			UnionUserErrLog("in UnionGenerateUpdateSpecFldFunFromTBLDef:: UnionGetTypeOfSpecFldFromObjectDef!\n");
			return(type);
		}
		if (strcmp(fldTypeName,"char") == 0)
			fprintf(outFp,"	ret = UnionPutCharTypeRecFldIntoRecStr(\"%s\",%s,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);\n",
					pobjectDef->primaryKey.fldNameGrp[fldNum],pobjectDef->primaryKey.fldNameGrp[fldNum]);
		else if (strcmp(fldTypeName,"int") == 0)
			fprintf(outFp,"	ret = UnionPutIntTypeRecFldIntoRecStr(\"%s\",%s,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);\n",
					pobjectDef->primaryKey.fldNameGrp[fldNum],pobjectDef->primaryKey.fldNameGrp[fldNum]);
		else if (strcmp(fldTypeName,"double") == 0)
			fprintf(outFp,"	ret = UnionPutDoubleTypeRecFldIntoRecStr(\"%s\",%s,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);\n",
					pobjectDef->primaryKey.fldNameGrp[fldNum],pobjectDef->primaryKey.fldNameGrp[fldNum]);
		else
			fprintf(outFp,"	ret = UnionPutRecFldIntoRecStr(\"%s\",%s,strlen(%s),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr);\n",
				pobjectDef->primaryKey.fldNameGrp[fldNum],pobjectDef->primaryKey.fldNameGrp[fldNum],pobjectDef->primaryKey.fldNameGrp[fldNum]);
		fprintf(outFp,"	if (ret < 0)\n");
		fprintf(outFp,"	{\n");
		fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionPutRecFldIntoRecStr [%s] failure!\\n\");\n",funName,pobjectDef->primaryKey.fldNameGrp[fldNum]);
		fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
		fprintf(outFp,"	}\n");
		fprintf(outFp,"	lenOfRecStr += ret;\n");
	}
	return(0);
}

/*
功能	
	根据表定义，将关键字写成参数说明
输入参数
	tblName	表名
	fp文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(char *tblName,FILE *fp)
{
	TUnionObject	objDef;
	int		ret;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp:: UnionReadObjectDef [%s]!\n",tblName);
		return(ret);
	}
	
	return(UnionGenerateRecPrimaryKeyRemarkAsVarFunToFp(&objDef,fp));
}

/*
功能	
	根据表定义，将关键字写成函数参数
输入参数
	tblName	表名
	fp	文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(char *tblName,FILE *fp)
{
	TUnionObject	objDef;
	int		ret;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSpecRecPrimaryKeyAsVarFunToFp:: UnionReadObjectDef [%s]!\n",tblName);
		return(ret);
	}
	
	return(UnionGenerateRecPrimaryKeyAsVarFunToFp(&objDef,fp));
}

/*
功能	
	根据表定义，将关键字拼串写入到文件中
输入参数
	funName		函数名称
	tblName	表名
	fp	文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(char *funName,char *tblName,FILE *fp)
{
	TUnionObject	objDef;
	int		ret;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp:: UnionReadObjectDef [%s]!\n",tblName);
		return(ret);
	}
	
	return(UnionGenerateRecPrimaryKeyIntoRecStrToFp(funName,&objDef,fp));
}

/*
功能	
	读出一张表的关键字
输入参数
	tblName	表名
输入出数
	primaryKey	读出的关键字串，2个关键字之间以,分隔
返回值
	>=0	成功,关键字串的长度
	<0	错误码
*/
int UnionReadPrimaryKeyOfSpecObject(char *tblName,char *primaryKey)
{
	TUnionObject	objDef;
	int		ret;
	int		fldNum;
	int		offset = 0;
	char		*ptr,prefix[128+1];
	int		lenOfPrefix;
	
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionReadPrimaryKeyOfSpecObject:: UnionReadObjectDef [%s]!\n",tblName);
		return(ret);
	}
//	if ((ptr = UnionGetPrefixOfDBField()) == NULL)
	if (((ptr = UnionGetPrefixOfDBField()) == NULL) || UnionIsTBLFldWithoutPrefix(tblName))
		strcpy(prefix,"");
	else
		strcpy(prefix,ptr);
	lenOfPrefix = strlen(prefix);
	for (fldNum = 0; fldNum < objDef.primaryKey.fldNum; fldNum++)
	{
		if (fldNum > 0)
		{
			strcpy(primaryKey+offset,",");
			offset++;
		}
		sprintf(primaryKey+offset,"%s%s",prefix,objDef.primaryKey.fldNameGrp[fldNum]);
		offset = offset + lenOfPrefix + strlen(objDef.primaryKey.fldNameGrp[fldNum]);
	}

	if (fldNum == 0)
		sprintf(primaryKey,"%s%s",primaryKey, "1");

	return(offset);
}

int UnionConvertTBLFldTypeIntoName(int type,char *typeName)
{
	switch (type)
	{
		case	conObjectFldType_String:
			strcpy(typeName,"char");
			break;
		case	conObjectFldType_Int:
			strcpy(typeName,"int");
			break;
		case	conObjectFldType_Double:
			strcpy(typeName,"double");
			break;
		case	conObjectFldType_Bit:
			strcpy(typeName,"unsigned char");
			break;
		case	conObjectFldType_Bool:
			strcpy(typeName,"int");
			break;
		case	conObjectFldType_Long:
			strcpy(typeName,"long");
			break;
		default:
			strcpy(typeName,"unknown");
			break;
	}
	return(strlen(typeName));
}


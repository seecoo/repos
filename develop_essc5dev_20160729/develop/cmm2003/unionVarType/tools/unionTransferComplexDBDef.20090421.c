//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionTransferComplexDBDef.h"
#include "UnionLog.h"

/*
功能	
	判断一个域定义是否存在
输入参数
	pfldRec	tableField的记录定义
输出参数
	无
返回值
	errCodeNameIsUsed	同名的存在，但定义不同
	1	存在
	其它	不存在
*/
int UnionExistSpecTableFieldRec(PUnionTableField pfldRec)
{
	int			ret;
	TUnionTableField 	fldRec;
	
	memset(&fldRec,0,sizeof(fldRec));
	strcpy(fldRec.ID,pfldRec->ID);
	if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
	{
		UnionUserErrLog("in UnionExistSpecTableFieldRec:: UnionReadTableFieldRec [%s]!\n",pfldRec->ID);
		return(ret);
	}
	if ((strcmp(pfldRec->fieldName,fldRec.fieldName) != 0) || 
		(strcmp(pfldRec->defaultValue,fldRec.defaultValue) != 0) ||
		(strcmp(pfldRec->valueConstraint,fldRec.valueConstraint) != 0) ||
		(strcmp(pfldRec->value,fldRec.value) != 0))
		return(errCodeNameIsUsed);
	if ((pfldRec->fieldType != fldRec.fieldType) || 
		(pfldRec->size != fldRec.size) ||
		(pfldRec->nullPermitted != fldRec.nullPermitted))
		return(errCodeNameIsUsed);
	return(1);
}

/*
功能	
	获得一个表域定义的标识
输入参数
	tableName	表名
	fldName		域名称
输出参数
	fldID		域标识
返回值
	>=0	成功
	<0	错误码
*/
int UnionFormTableFieldRecFldID(char *tableName,char *fldName,char *fldID)
{
	if ((fldName == NULL) || (fldID == NULL))
		return(errCodeParameter);

	if ((tableName == NULL) || (strlen(tableName) == 0))
		strcpy(fldID,fldName);
	else
		sprintf(fldID,"%s_%s",tableName,fldName);
	if (strlen(fldID) > 40)
		fldID[40] = 0;
	return(0);
}

/*
功能	
	将一个complexDB定义的域转换为tableField的记录定义
输入参数
	tableName	表名
	pfldDef		complexDB定义的域
输出参数
	pfldRec		tableField的记录定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionTransferComplexDBFldDefToTableFieldRec(char *tableName,PUnionObjectFldDef pfldDef,PUnionTableField pfldRec)
{
	int			ret;
		
	memset(pfldRec,0,sizeof(*pfldRec));
	strcpy(pfldRec->fieldName,pfldDef->name);
	UnionFormTableFieldRecFldID(tableName,pfldDef->name,pfldRec->ID);
	pfldRec->fieldType = pfldDef->type;
	pfldRec->size = pfldDef->size;
	strcpy(pfldRec->defaultValue,pfldDef->defaultValue);
	pfldRec->nullPermitted = pfldDef->nullPermitted;
	strcpy(pfldRec->remark,pfldDef->remark);
	return(0);
}

/*
功能	
	将一个fldGrp定义写入到清单串中
输入参数
	tableName	表名
	pfldGrpDef	域定义
输出参数
	fldListStr	域清单串
返回值
	>=0	成功,串长度
	<0	错误码
*/
int UnionPutObjectFldGrpDefIntoStr(char *tableName,PUnionObjectFldGrp pfldGrpDef,char *fldListStr)
{
	int	offset;
	int	index;
	char	fldID[128+1];
	
	for (index = 0,offset = 0; index < pfldGrpDef->fldNum; index++)
	{
		memset(fldID,0,sizeof(fldID));
		UnionFormTableFieldRecFldID(tableName,pfldGrpDef->fldNameGrp[index],fldID);
		if (index > 0)
		{
			sprintf(fldListStr+offset,",");
			offset++;
		}
		strcpy(fldListStr+offset,fldID);
		offset += strlen(fldID);
	}
	return(offset);
}

/*
功能	
	根据资源名称，读以complexDB定义的资源ID号
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0	资源ID
	<0	错误码
*/
int UnionReadResIDOfSpecResNameForComplexDB(char *resName)
{
        char    tmpBuf[2048+1];
        char    fileName[512+1];
        FILE    *fp;
        int     len,ret;
        char    var[2048+1];
        char    workDir[256];
	char	*ptr;

        memset(workDir,0,sizeof(workDir));
#ifdef _WIN32
        UnionGetMainWorkingDir(workDir);
        // Mod By HuangBaoxin 2008/11/12
        sprintf(fileName,"%s\\resID.conf",workDir);
        // End Mod
        UnionLog("in UnionReadResIDOfSpecResNameForComplexDB:: WIN fopen [%s]\n",fileName);
#else
	sprintf(fileName, "%s/confFileResID.conf", getenv("UNIONETC"));
       	if (!UnionExistsFile(fileName))
        {
 	      	sprintf(fileName, "%s/resID.conf", getenv("UNIONETC"));
 	      	if (!UnionExistsFile(fileName))
 	      		sprintf(fileName,"%s/objRec/resID.conf",getenv("UNIONOBJECTDIR"));
	}
        UnionProgramerLog("in UnionReadResIDOfSpecResNameForComplexDB:: UNIX fopen [%s]\n",fileName);
#endif
        if ((fp = fopen(fileName,"r")) == NULL)
        {
                UnionSystemErrLog("in UnionReadResIDOfSpecResNameForComplexDB:: fopen [%s] workingDir = [%s]\n",fileName,workDir);
                return(errCodeUseOSErrCode);
        }
        while (!feof(fp))
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
                if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) < 0)
                        continue;
                if (len == 0)
                        continue;
                if (UnionIsUnixShellRemarkLine(tmpBuf))
                        continue;
                memset(var,0,sizeof(var));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"tableName",var,sizeof(var))) < 0)
                {
                        fclose(fp);
                        return(ret);
                }
                if (strcmp(var,resName) != 0)
                	continue;
                memset(var,0,sizeof(var));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"resID",var,sizeof(var))) < 0)
                {
                        fclose(fp);
                        return(ret);
                }
                fclose(fp);
                return(atoi(var));
        }
        UnionUserErrLog("in UnionReadResIDOfSpecResNameForComplexDB:: resID not defined in file [%s] for [%s]\n",fileName,resName);
        fclose(fp);
        return(errCodeEsscMDL_InvalidResID);
}

/*
功能	
	将一个complexDB定义的对象转换为tableList的记录定义
输入参数
	
	pobjDef		complexDB定义的对象
输出参数
	ptblRec		tableList的记录定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionTransferComplexDBObjectDefToTableListRec(PUnionObject pobjDef,PUnionTableList ptblRec)
{
	int				ret;
	int				index;
	char				fldID[128+1];
	int				offset = 0;
	int				grpIndex;
	char				*list1,*list2;
	char				*foreignTableName;
	PUnionDefOfRelatedObject	pforeignDef;
	
	if ((pobjDef == NULL) || (ptblRec == NULL))
		return(errCodeParameter);

	memset(ptblRec,0,sizeof(*ptblRec));
	strcpy(ptblRec->tableName,pobjDef->name);
	//ptblRec->tableTypeID = 0;
	strcpy(ptblRec->tableAlais,pobjDef->name);
	ptblRec->resID = UnionReadResIDOfSpecResNameForComplexDB(pobjDef->name);
	
	// 域清单
	for (index = 0,offset = 0; index < pobjDef->fldNum; index++)
	{
		memset(fldID,0,sizeof(fldID));
		UnionFormTableFieldRecFldID(ptblRec->tableName,pobjDef->fldDefGrp[index].name,fldID);
		if (index > 0)
		{
			sprintf(ptblRec->fieldList+offset,",");
			offset++;
		}
		strcpy(ptblRec->fieldList+offset,fldID);
		offset += strlen(fldID);
	}
	// 关键字域
	UnionPutObjectFldGrpDefIntoStr(pobjDef->name,&(pobjDef->primaryKey),ptblRec->primaryKey);
	// 唯一值域
	for (grpIndex = 0; grpIndex < 4; grpIndex++)
	{
		if (grpIndex >= pobjDef->uniqueFldGrpNum)
			break;
		switch (grpIndex)
		{
			case	0:
				list1 = ptblRec->unique1;
				break;
			case	1:
				list1 = ptblRec->unique2;
				break;
			case	2:
				list1 = ptblRec->unique3;
				break;
			case	3:
				list1 = ptblRec->unique4;
				break;
		}
		UnionPutObjectFldGrpDefIntoStr(pobjDef->name,&(pobjDef->uniqueFldGrp[grpIndex]),list1);
	}
	
	// 外键域
	for (grpIndex = 0; grpIndex < 4; grpIndex++)
	{
		if (grpIndex >= pobjDef->foreignFldGrpNum)
			break;
		switch (grpIndex)
		{
			case	0:
				list1 = ptblRec->myKeyGrp1;
				list2 = ptblRec->foreignKey1;
				foreignTableName = ptblRec->foreignTable1;
				break;
			case	1:
				list1 = ptblRec->myKeyGrp2;
				list2 = ptblRec->foreignKey2;
				foreignTableName = ptblRec->foreignTable2;
				break;
			case	2:
				list1 = ptblRec->myKeyGrp3;
				list2 = ptblRec->foreignKey3;
				foreignTableName = ptblRec->foreignTable3;
				break;
			case	3:
				list1 = ptblRec->myKeyGrp4;
				list2 = ptblRec->foreignKey4;
				foreignTableName = ptblRec->foreignTable4;
				break;
		}
		pforeignDef = &(pobjDef->foreignFldGrp[grpIndex]);
		UnionPutObjectFldGrpDefIntoStr(pobjDef->name,&(pforeignDef->localFldGrp),list1);
		strcpy(foreignTableName,pforeignDef->objectName);
		UnionPutObjectFldGrpDefIntoStr(foreignTableName,&(pforeignDef->foreignFldGrp),list2);
	}
	return(0);
}
/*
功能	
	将一个complexDB定义的对象插入到tabelList中
输入参数
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertComplexDBObjectDefIntoTableList(char *tableName)
{
	int			ret;
	int			index;
	TUnionObject		objDef;
	TUnionTableList 	tblRec;
	TUnionTableField	fldRec;
	
	// 读取表定义
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tableName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionReadObjectDef [%s]!\n",tableName);
		return(ret);
	}
	// 将表定义转换为tableList记录
	memset(&tblRec,0,sizeof(tblRec));
	if ((ret = UnionTransferComplexDBObjectDefToTableListRec(&objDef,&tblRec)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionTransferComplexDBObjectDefToTableListRec [%s]!\n",tableName);
		return(ret);
	}
	// 在表中插入表记录
	if ((ret = UnionInsertTableListRec(&tblRec)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionInsertTableListRec [%s]!\n",tableName);
		return(ret);
	}
	// 在表中插入表记录的域定义		
	for (index = 0; index < objDef.fldNum; index++)
	{
		if ((ret = UnionTransferComplexDBFldDefToTableFieldRec(objDef.name,&(objDef.fldDefGrp[index]),&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionTransferComplexDBFldDefToTableFieldRec [%s] of [%s]!\n",
					objDef.fldDefGrp[index].name,tableName);
			return(ret);
		}
		if ((ret = UnionInsertTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionInsertTableFieldRec [%s] of [%s]!\n",
					objDef.fldDefGrp[index].name,tableName);
			return(ret);
		}
	}
	return(0);
}


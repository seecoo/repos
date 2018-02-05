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
����	
	�ж�һ�������Ƿ����
�������
	pfldRec	tableField�ļ�¼����
�������
	��
����ֵ
	errCodeNameIsUsed	ͬ���Ĵ��ڣ������岻ͬ
	1	����
	����	������
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
����	
	���һ��������ı�ʶ
�������
	tableName	����
	fldName		������
�������
	fldID		���ʶ
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	��һ��complexDB�������ת��ΪtableField�ļ�¼����
�������
	tableName	����
	pfldDef		complexDB�������
�������
	pfldRec		tableField�ļ�¼����
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	��һ��fldGrp����д�뵽�嵥����
�������
	tableName	����
	pfldGrpDef	����
�������
	fldListStr	���嵥��
����ֵ
	>=0	�ɹ�,������
	<0	������
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
����	
	������Դ���ƣ�����complexDB�������ԴID��
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0	��ԴID
	<0	������
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
����	
	��һ��complexDB����Ķ���ת��ΪtableList�ļ�¼����
�������
	
	pobjDef		complexDB����Ķ���
�������
	ptblRec		tableList�ļ�¼����
����ֵ
	>=0	�ɹ�
	<0	������
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
	
	// ���嵥
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
	// �ؼ�����
	UnionPutObjectFldGrpDefIntoStr(pobjDef->name,&(pobjDef->primaryKey),ptblRec->primaryKey);
	// Ψһֵ��
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
	
	// �����
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
����	
	��һ��complexDB����Ķ�����뵽tabelList��
�������
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInsertComplexDBObjectDefIntoTableList(char *tableName)
{
	int			ret;
	int			index;
	TUnionObject		objDef;
	TUnionTableList 	tblRec;
	TUnionTableField	fldRec;
	
	// ��ȡ����
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDef(tableName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionReadObjectDef [%s]!\n",tableName);
		return(ret);
	}
	// ������ת��ΪtableList��¼
	memset(&tblRec,0,sizeof(tblRec));
	if ((ret = UnionTransferComplexDBObjectDefToTableListRec(&objDef,&tblRec)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionTransferComplexDBObjectDefToTableListRec [%s]!\n",tableName);
		return(ret);
	}
	// �ڱ��в�����¼
	if ((ret = UnionInsertTableListRec(&tblRec)) < 0)
	{
		UnionUserErrLog("in UnionInsertComplexDBObjectDefIntoTableList:: UnionInsertTableListRec [%s]!\n",tableName);
		return(ret);
	}
	// �ڱ��в�����¼������		
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


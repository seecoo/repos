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
���ܣ����һ������Ķ����Ƿ����
���������
	idOfObject������ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectDef(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ���������ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefFileName(idOfObject,caFileName);

	return(UnionExistsFile(caFileName));
}

/*
function:
        �ж��Ƿ��Ƕ����е��ֶ�
param:
        [IN]:
        idOfObject:     ����ID
        fldName:        �ֶ���
        [OUT]:
        NULL
return:
        1               �ǹؼ��ֶ�
        0               ���ǹؼ��ֶ�
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
		 	if ( (ptr1=strchr(fldName,'(')) != NULL ) // ���� trim(field)���
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
���ܣ�
	���������������ת��Ϊʵ�����ݿ��е�����
���������
	pobject��		ָ��������ָ��
	defFldNameGrp		����ʱ���õ�����
���������
	realFldNameGrpInDB	ʵ�����ݿ��е�����	
����ֵ��
	>=0��	����ת��������������
	<0��	ʧ�ܣ����ش�����
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
���ܣ�
	���������������ת��Ϊʵ�����ݿ��е�����
���������
	idOfObject		��������
	defFldNameGrp		����ʱ���õ�����
���������
	realFldNameGrpInDB	ʵ�����ݿ��е�����	
����ֵ��
	>=0��	����ת��������������
	<0��	ʧ�ܣ����ش�����
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
���ܣ��ж�һ������Ķ����Ƿ�Ϸ�
���������
	objectDef��������
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ�������
*/
int UnionIsValidObjectDef(TUnionObject objectDef)
{
	int	iRet,i,j;
	TUnionObjectFldGrp	sFldGrpTmp;
	TUnionObject		sForeignObj;
	
	// �ж϶��������Ƿ�Ϸ�
	if (UnionIsValidObjectFldName(objectDef.name) == 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: name of object [%s] is invalid\n",objectDef.name);
		return(0);
	}
	
	// �ж������Ƿ�Ϸ�
	if (objectDef.fldNum <= 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: fldNum of object [%d] is invalid\n",objectDef.fldNum);
		return(0);
	}
	for (i=0;i<objectDef.fldNum;i++)
	{
		// �ж������Ƿ�Ϸ�
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
	
	// �жϹؼ��ֶ����Ƿ�Ϸ�
	if ((iRet = UnionCheckFldGrpInObject(&(objectDef.primaryKey),&objectDef)) < 0)
	{
		UnionAuditLog("in UnionIsValidObjectDef:: primaryKey fld group does not defined in object [%s]!\n",objectDef.name);
		return(0);
	}
	
	// �ж�Ψһֵ�����Ƿ�Ϸ�
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
	
	// �ж��ⲿ�ؼ����еı��������Ƿ�Ϸ�
	for (i = 0; i < objectDef.foreignFldGrpNum; i++)
	{
		if ((iRet = UnionCheckFldGrpInObject(&(objectDef.foreignFldGrp[i].localFldGrp),&objectDef)) < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: foreign key fld group does not defined in object [%s]!\n",objectDef.name);
			return(0);
		}
	}
	
	// �ж��ⲿ�ؼ����е��ⲿ�����Ƿ�Ϸ�
	for (i = 0; i < objectDef.foreignFldGrpNum; i++)
	{
		// ���ⲿ������
		memset(&sForeignObj,0,sizeof(TUnionObject));
		iRet=UnionReadObjectDef(objectDef.foreignFldGrp[i].objectName,&sForeignObj);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionReadObjectDef for [%s] fail! return=[%d]\n",objectDef.foreignFldGrp[i].objectName,iRet);
			return(iRet);
		}
		// �ж���������Ƿ��ⲿ����Ĺؼ���
		iRet=UnionFldGrpIsCompleteRepeat(&(objectDef.foreignFldGrp[i].foreignFldGrp),&(sForeignObj.primaryKey));
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionIsValidObjectDef:: UnionFldGrpIsCompleteRepeat for [%d] foreign primary fail! return=[%d]\n",i,iRet);
			return(iRet);
		}
		if (iRet == 0)
		{
			// �ж���������Ƿ��ⲿ�����Ψһֵ
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
���ܣ��������壬д�뵽�������ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
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
	
	// ��ö������ļ�����
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
���ܣ��������壬д�뵽ָ���ļ���
���������
	pobject��	ָ��������ָ��
	fileName	�ļ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
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
		
	// д������
	// д��������
	iRet=UnionAppendRecStrToFile(fileName,"# �������ƶ���",14);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for �������ƶ��� fail! return=[%d]\n",iRet);
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
	
	// д����
	iRet=UnionAppendRecStrToFile(fileName,"\n# ����",8);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for ���� fail! return=[%d]\n",iRet);
		return(iRet);
	}
	for (i=0;i<pobject->fldNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		strcpy(caRecord,ObjectDefFlagOfFldDefGrp);
		iRecLen=ObjectDefFlagLenOfFldDefGrp;
		// ������
		iRet=UnionPutRecFldIntoRecStr("name",pobject->fldDefGrp[i].name,strlen(pobject->fldDefGrp[i].name),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] name fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// ������
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].type);
		iRet=UnionPutRecFldIntoRecStr("type",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] type fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// ���С
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
		// �Ƿ�Ψһֵ
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
		// �Ƿ������ֵ
		memset(caTmpBuf,0,sizeof(caTmpBuf));
		sprintf(caTmpBuf,"%d",pobject->fldDefGrp[i].nullPermitted);
		iRet=UnionPutRecFldIntoRecStr("nullPermitted",caTmpBuf,strlen(caTmpBuf),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] nullPermitted fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// ȱʡֵ
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
		// ˵��
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
	
	// д�ؼ���
	iRet=UnionAppendRecStrToFile(fileName,"\n# �ؼ��ֶ���",13);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for �ؼ��ֶ��� fail! return=[%d]\n",iRet);
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
	
	// дΨһֵ��
	iRet=UnionAppendRecStrToFile(fileName,"\n# Ψһֵ����",13);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for Ψһֵ���� fail! return=[%d]\n",iRet);
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
	
	// д�ⲿ�ؼ���
	iRet=UnionAppendRecStrToFile(fileName,"\n# �ⲿ�ؼ��ֶ���",17);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionAppendRecStrToFile for �ⲿ�ؼ��ֶ��� fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	for (i=0;i<pobject->foreignFldGrpNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		strcpy(caRecord,ObjectDefFlagOfForeignFldGrp);
		iRecLen=ObjectDefFlagLenOfForeignFldGrp;
	
		// ��������
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
		// �ⲿ��������
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(pobject->foreignFldGrp[i]).objectName,strlen((pobject->foreignFldGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionPrintObjectDefToSpecFile:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// �ⲿ����
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
���ܣ��������壬д�뵽�������ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionStoreObjectDef(PUnionObject pobject)
{
	// ���������ļ��Ƿ����
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
���ܣ��Ӷ������ļ�����һ��������
���������
	idOfObject��Ҫ���Ķ���ID
���������
	pobject��ָ��������ָ��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
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

	// ��ö������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefFileName(idOfObject,caFileName);
	
	// ���������ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionReadObjectDef:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	// UnionLog("in UnionReadObjectDef:: the caFileName is: [%s].\n", caFileName);
	// ��������
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
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
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
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		// UnionLog("in UnionReadObjectDef:: caRecord is: [%s].\n", caRecord);
		
		if (memcmp(caRecord,ObjectDefFlagOfObjectName,ObjectDefFlagLenOfObjectName) == 0)	// �������ƶ���
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
		else if (memcmp(caRecord,ObjectDefFlagOfFldDefGrp,ObjectDefFlagLenOfFldDefGrp) == 0)	// ����
		{
			// ������
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"name",(pobject->fldDefGrp[pobject->fldNum]).name,sizeof((pobject->fldDefGrp[pobject->fldNum]).name));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for name from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfFldDefGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// ������
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"type",&(pobject->fldDefGrp[pobject->fldNum].type));
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for type fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// ��С
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"size",&(pobject->fldDefGrp[pobject->fldNum].size));
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for size fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].size = UnionGetDefaultSizeOfTBLFieldType(pobject->fldDefGrp[pobject->fldNum].type);
			}
			/*
			// ��Ψһֵ
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"isUnique",&(pobject->fldDefGrp[pobject->fldNum]).isUnique);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for isUnique fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			*/
			// �Ƿ������ֵ
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"nullPermitted",(int *)&(pobject->fldDefGrp[pobject->fldNum].nullPermitted));
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionReadObjectDef:: UnionReadIntTypeRecFldFromRecStr for nullPermitted fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].nullPermitted = 1;
			}

			// ȱʡֵ����
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"defaultValue",(pobject->fldDefGrp[pobject->fldNum]).defaultValue,sizeof((pobject->fldDefGrp[pobject->fldNum]).defaultValue));
			// ˵��
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"remark",(pobject->fldDefGrp[pobject->fldNum]).remark,sizeof((pobject->fldDefGrp[pobject->fldNum]).remark));
			pobject->fldNum++;
		}
		else if (memcmp(caRecord,ObjectDefFlagOfPrimaryKey,ObjectDefFlagLenOfPrimaryKey) == 0)	// �ؼ��ֶ���
		{
			iRet=UnionFormFldGrpFromFieldNameStr(&caRecord[ObjectDefFlagLenOfPrimaryKey],iRecLen-ObjectDefFlagLenOfPrimaryKey,&pobject->primaryKey);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfPrimaryKey]);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
		}
		else if (memcmp(caRecord,ObjectDefFlagOfUniqueFldGrp,ObjectDefFlagLenOfUniqueFldGrp) == 0)	// Ψһֵ����
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
		else if (memcmp(caRecord,ObjectDefFlagOfForeignFldGrp,ObjectDefFlagLenOfForeignFldGrp) == 0)	// �ⲿ�ؼ��ֶ���
		{
			// ��������
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
			// �ⲿ��������
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"foreignObject",(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName,sizeof((pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionReadObjectDef:: UnionReadRecFldFromRecStr for foreignObject from [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// �ⲿ����
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
���ܣ��Ӷ������ļ�����һ��������Ĺؼ����嵥
���������
	idOfObject��Ҫ���Ķ���ID	
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	primaryKeyList	�ؼ����嵥��������֮����,�ֿ�
����ֵ��
	0���ؼ��ִ��ĳ���
	<0��ʧ�ܣ����ش�����
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
���ܣ�ɾ��һ��������
���������
	idOfObject�������ID
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
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
	
	// ���������ļ��Ƿ����
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
	
	// ɾ��������Ķ����ļ�
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectDef:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// ɾ����������ⲿ�ؼ��ֶ����嵥�ļ�
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
����	
	��һ������д�ɴ�
�������
	flag		����ı�ʶ
	pfldGrp		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
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
		// 2009/6/23 ����������
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
����	
	������ݿ������󳤶�
�������
	fldSize		�򳤶�
�������
	��
����ֵ
	��ĺϷ�����
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
����	
	��һ�����崴��SQL���
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
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
	
	// д�������
	fprintf(outFp,"-- ���ɱ� %s\n",prec->name);
	fprintf(outFp,"create table %s\n",prec->name);
	fprintf(outFp,"(\n");
	// ������
	for (fldNum = 0; fldNum < prec->fldNum; fldNum++)
	{
		pfldDef = &(prec->fldDefGrp[fldNum]);
		memset(typeName,0,sizeof(typeName));
		UnionGetFldTypeName(pfldDef->type,typeName);
		if (strlen(pfldDef->remark) > 0)
			fprintf(outFp,"	-- %s \n",pfldDef->remark);

		// 2009/6/23 ����������
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
			// �ǹؼ��ֻ�Ψһֵ�����벻Ϊ����
			mustBeNotNullFld = 1;
			if (pfldDef->nullPermitted)
				pfldDef->nullPermitted = 0;
		}
		if (strlen(pfldDef->defaultValue) > 0)
		{
			// ������ȱʡֵ
			if (mustBeNotNullFld)	// ��ֵ���벻Ϊ��
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
		else	// û����ȱʡֵ
		{
			if (!pfldDef->nullPermitted)
				fprintf(outFp," not null");
		}
		fprintf(outFp,",");
		fprintf(outFp,"\n");
	}

	// ���ؼ���
	fprintf(outFp,"	-- ����ؼ��� \n");
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
		fprintf(outFp,"	-- ����Ψһֵ\n");
		if ((ret = UnionCreateFldGrpIntoSQLToFp("unique ",&(prec->uniqueFldGrp[index]),outFp)) < 0)
		{
			UnionUserErrLog("in UnionCreateSQLFromObjectDefToFp:: UnionCreateFldGrpIntoSQLToFp for the [%dth] unique of [%s]\n",index,prec->name);
			return(ret);
		}
		dhNeeded = 1;
	}
	// ���ⲿ�ؼ���
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
		fprintf(outFp,"	-- �����ⲿ��ֵ \n");
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
���ܣ�
	ת��һ�������������
���������
	objectName	��������
	oriFldName	������
���������
	defName		����ʱ���õ�����
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
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
���ܣ�
	ת��һ�������������
���������
	pobject��	ָ��������ָ��
	oriFldName	������
���������
	defName		����ʱ���õ�����
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
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
���ܣ�
	���һ����������������
���������
	pobject��	ָ��������ָ��
	fldName		������
���������
	fldTypeName	���c��������
����ֵ��
	>=0��	����
	<0��	ʧ�ܣ����ش�����
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
���ܣ�
	���һ������������˵��
���������
	pobject��	ָ��������ָ��
	fldName		������
���������
	remark		��˵��
����ֵ��
	>=0��	�ɹ�
	<0��	ʧ�ܣ����ش�����
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
����	
	���ݱ��壬���ؼ���д�ɲ���˵��
�������
	pobjectDef	����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	���ݱ��壬���ؼ���д�ɺ�������
�������
	pobjectDef	����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	���ݱ��壬���ؼ���ƴ��д�뵽�ļ���
�������
	funName		��������
	pobjectDef	����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
	fprintf(outFp,"	// ƴ�ؼ���\n");
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
����	
	���ݱ��壬���ؼ���д�ɲ���˵��
�������
	tblName	����
	fp�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	���ݱ��壬���ؼ���д�ɺ�������
�������
	tblName	����
	fp	�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	���ݱ��壬���ؼ���ƴ��д�뵽�ļ���
�������
	funName		��������
	tblName	����
	fp	�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
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
����	
	����һ�ű�Ĺؼ���
�������
	tblName	����
�������
	primaryKey	�����Ĺؼ��ִ���2���ؼ���֮����,�ָ�
����ֵ
	>=0	�ɹ�,�ؼ��ִ��ĳ���
	<0	������
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


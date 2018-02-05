//	Author: Wolfgang Wang
//	Date: 2008/3/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectSql.h"

// ���ܣ� ���һ����������ռ�ڴ��С
/*
���������
	��
���������
	��
����ֵ��
	���һ�����嶨����ռ�ڴ�Ĵ�С
*/
int UnionGetSizeOfObjectDef()
{
	return(sizeof(TUnionObject));
}

// ���ܣ���ָ�����ж�һ������Ķ���
/*
���������
	str		�ַ���
	lenOfStr	�ַ�������
���������
	pfldGrp		���������鶨�壬����Ԥ�ȷ���ռ�
����ֵ��
	�ɹ������ض������ַ�����ռ���ַ���
	ʧ�ܣ����ش�����
*/
int UnionGetObjectFldGrpDefFromStr(char *str,int lenOfStr,PUnionObjectFldGrp pfldGrp)
{
	int		ret;
	char		*ptr;
	int		index;
        char            oriChar;
	char		tmpBuf[2048+1];

	if ((str == NULL) || (pfldGrp == NULL) || (lenOfStr <= 0) || (lenOfStr >= sizeof(tmpBuf)))
	{
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	memset(pfldGrp,0,sizeof(*pfldGrp));
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,str,lenOfStr);
	for (index = 0; index <lenOfStr; index++)
	{
		if (tmpBuf[index] == ' ')
			continue;
		else
			break;
	}
	if (index >= lenOfStr)
	{
		UnionUserErrLog("in UnionGetObjectFldGrpDefFromStr:: [%s] not valid fldGrp define line!\n",str);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	if (tmpBuf[index] != '(')
	{
		UnionUserErrLog("in UnionGetObjectFldGrpDefFromStr:: [%s] not valid fldGrp define line!\n",str);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
        ++index;
	ptr = tmpBuf + index;
	pfldGrp->fldNum = 0;
	for (; index < lenOfStr; index++)
	{
		if ((tmpBuf[index] != ',') && (tmpBuf[index] != ')'))
			continue;
                oriChar = tmpBuf[index];
		if (pfldGrp->fldNum >= conMaxFldNumPerFldGrp)
		{
			UnionUserErrLog("in UnionGetObjectFldGrpDefFromStr:: [%s] defined too many flds!\n",str);
			return(UnionSetUserDefinedErrorCode(errCodeParameter));
		}
		tmpBuf[index] = 0;
		if (strlen(ptr) >= sizeof(pfldGrp->fldNameGrp[pfldGrp->fldNum]))
		{
			UnionUserErrLog("in UnionGetObjectFldGrpDefFromStr:: fldName [%s] in [%s] too long!\n",ptr,str);
			return(UnionSetUserDefinedErrorCode(errCodeParameter));
		}
		strcpy(pfldGrp->fldNameGrp[pfldGrp->fldNum],ptr);
		pfldGrp->fldNum++;
		ptr = tmpBuf + index + 1;
                tmpBuf[index] = oriChar;
		if (tmpBuf[index] == ')')
			return(index+1);
                //++index;
	}
	UnionUserErrLog("in UnionGetObjectFldGrpDefFromStr:: [%s] not valid fldGrp define line!\n",str);
	return(UnionSetUserDefinedErrorCode(errCodeParameter));
}		

// ���ܣ���ָ���ַ����ж�������
/*
���������
	fldStr		���崮
	lenOfFldStr	���崮����
���������
	pfldDef		���������壬����Ԥ�ȷ���ռ�
����ֵ��
	�ɹ���0
	ʧ�ܣ����ش�����
*/
int UnionGetObjectFldDefFromStr(char *fldStr,int lenOfFldStr,PUnionObjectFldDef pfldDef)
{
	int	ret;
	char	*ptr;
	char	fldName[256+1],fldType[256+1];
	char	tmpBuf[2048+1];
	
	if ((fldStr == NULL) || (lenOfFldStr <= 0) || (pfldDef == NULL) || (lenOfFldStr >= sizeof(tmpBuf)))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	
	memset(pfldDef,0,sizeof(*pfldDef));
	// ��������
	memset(fldName,0,sizeof(fldName));
	sscanf(fldStr,"%s",fldName);
	if ((strlen(fldName) == 0) || (strlen(fldName) >= sizeof(pfldDef->name)))
	{
		UnionUserErrLog("in UnionGetObjectFldDefFromStr:: fldName in [%s] not valid!\n",fldStr);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	strcpy(pfldDef->name,fldName);
	// ��������
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,fldStr,lenOfFldStr);
	// �ж������ͼ�����
	ptr = strstr(tmpBuf,pfldDef->name);
	UnionToUpperCase(tmpBuf);
	memset(fldType,0,sizeof(fldType));
	sscanf(ptr+strlen(pfldDef->name),"%s",fldType);
	if ((strlen(fldType) > 0) && (fldType[strlen(fldType)-1] == ','))
		fldType[strlen(fldType)-1] = 0;
	if (strcmp(fldType,"INTEGER") == 0)
	{
		pfldDef->type = conObjectFldType_Int;
		pfldDef->size = 12;
	}
	else if (strcmp(fldType,"BOOL") == 0)
	{
		pfldDef->type = conObjectFldType_Bool;
		pfldDef->size = 1;
	}
	else if ((strcmp(fldType,"DOUBLE") == 0) || (strcmp(fldType,"DECIMAL") == 0))
	{
		pfldDef->type = conObjectFldType_Double;
		pfldDef->size = 20;
	}
	else if (strncmp(fldType,"CHAR(",5) == 0)
	{
		pfldDef->type = conObjectFldType_String;
		if (fldType[strlen(fldType)-1] != ')')
		{
			UnionUserErrLog("in UnionGetObjectFldDefFromStr:: fldType in [%s] is invalid!\n",fldType,fldStr);
			return(UnionSetUserDefinedErrorCode(errCodeParameter));
		}
		fldType[strlen(fldType)-1] = 0;
		pfldDef->size = atoi(fldType+strlen("CHAR("));
		fldType[strlen(fldType)] = ')';
	}
	else
	{
		UnionUserErrLog("in UnionGetObjectFldDefFromStr:: fldType = [%s] in str [%s] is invalid!\n",fldType, fldStr);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// ���Ƿ������ֵ
	ptr = strstr(ptr,fldType) + strlen(fldType);
	if (strstr(ptr,"NOT NULL") == NULL)
		pfldDef->nullPermitted = 1;
	else
		pfldDef->nullPermitted = 0;
	/*
	// ���Ƿ���Ψһֵ
	if (strstr(ptr,"UNIQUE") == NULL)
		pfldDef->isUnique = 0;
	else
		pfldDef->isUnique = 1;
	*/
	return(0);
}

/*
���ܣ��Ӷ��󴴽��ļ����һ������Ķ���
���������
	idOfObject������ID
���������
	pobject�������Ķ����壬����Ԥ�ȷ���ռ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectDefFromCreateSqlFile(TUnionIDOfObject idOfObject,PUnionObject pobject)
{
	/* Mary delete, 2008-9-5	
	char			fileName[512];
	int			ret;
	char			tmpBuf[2048+1];
	char			defLine[2048+1];
	FILE			*fp;
	char			*ptr;
	char			varName[256+1];
	int			lenOfLine;
	int			lenOfLocalKeyDef;
	int			finishOK = 0;
        unsigned int            index;
        int			primaryKeyDefined = 0;
        char                    tmpChar;
	*/
	TUnionRecFileHDL	hFileHdl;			// Mary add, 2008-9-5	
	int			tmp;
	char	caFileName[512],caRecord[512],caBuf[512];	// Mary add, 2008-9-5
	int	iRet,iRecLen;					// Mary add, 2008-9-5
	
	if ((pobject == NULL) || (idOfObject == NULL))
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: pobject is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// ��ö����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	if ((iRet = UnionGetObjectDefCreateFileName(idOfObject,caFileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectDefCreateFileName [%s]\n",idOfObject);
		return(iRet);
	}

	// Mary add begin, 2008-9-5
	// ���������ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	// ��������
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	
	memset(pobject,0,sizeof(TUnionObject));
	while(1)
	{
		// ��ȡ�ļ��е�һ�У��Ѿ�ȥ����ע����
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// ���ļ�����
			break;
		
		if (memcmp(caRecord,ObjectDefFlagOfObjectName,ObjectDefFlagLenOfObjectName) == 0)	// �������ƶ���
		{
			strcpy(pobject->name,&caRecord[ObjectDefFlagLenOfObjectName]);
			if (strcmp(pobject->name,idOfObject) != 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: pobject->name [%s] defined in file does not equal to idOfObject [%s]!\n",pobject->name,idOfObject);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectNameError));
			}
		}
		else if (memcmp(caRecord,ObjectDefFlagOfFldDefGrp,ObjectDefFlagLenOfFldDefGrp) == 0)	// ����
		{
			// ������
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"name",(pobject->fldDefGrp[pobject->fldNum]).name,sizeof((pobject->fldDefGrp[pobject->fldNum]).name));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadRecFldFromRecStr for name fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// ������
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"type",&(pobject->fldDefGrp[pobject->fldNum].type));
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadIntTypeRecFldFromRecStr for type fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// ��С
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"size",&(pobject->fldDefGrp[pobject->fldNum].size));
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadIntTypeRecFldFromRecStr for size fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].size = UnionGetDefaultSizeOfTBLFieldType(pobject->fldDefGrp[pobject->fldNum].type);
			}
			/*
			// ��Ψһֵ
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"isUnique",&(pobject->fldDefGrp[pobject->fldNum]).isUnique);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadIntTypeRecFldFromRecStr for isUnique fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			*/
			// �Ƿ������ֵ
                        // iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"nullPermitted", &(pobject->fldDefGrp[pobject->fldNum].nullPermitted));
			iRet=UnionReadIntTypeRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfFldDefGrp],iRecLen-ObjectDefFlagLenOfFldDefGrp,"nullPermitted", &tmp);
			if (iRet < 0)
			{
				//UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadIntTypeRecFldFromRecStr for nullPermitted fail! return=[%d]\n",iRet);
				//UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				//return(iRet);
				pobject->fldDefGrp[pobject->fldNum].nullPermitted = 1;
			}
#ifdef _WIN32
                        (pobject->fldDefGrp[pobject->fldNum]).nullPermitted = static_cast<bool>(tmp);
#else
						(pobject->fldDefGrp[pobject->fldNum]).nullPermitted = tmp;
#endif

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
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfPrimaryKey]);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
		}
		else if (memcmp(caRecord,ObjectDefFlagOfUniqueFldGrp,ObjectDefFlagLenOfUniqueFldGrp) == 0)	// Ψһֵ����
		{
			iRet=UnionFormFldGrpFromFieldNameStr(&caRecord[ObjectDefFlagLenOfUniqueFldGrp],iRecLen-ObjectDefFlagLenOfUniqueFldGrp,&pobject->uniqueFldGrp[pobject->uniqueFldGrpNum]);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",&caRecord[ObjectDefFlagLenOfUniqueFldGrp]);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
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
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadRecFldFromRecStr for name fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}

                        iRet=UnionFormFldGrpFromFieldNameStr(caBuf,iRet,&(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).localFldGrp);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",caBuf);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// �ⲿ��������
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"foreignObject",(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName,sizeof((pobject->foreignFldGrp[pobject->foreignFldGrpNum]).objectName));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadRecFldFromRecStr for name fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			// �ⲿ����
			memset(caBuf,0,sizeof(caBuf));
			iRet=UnionReadRecFldFromRecStr(&caRecord[ObjectDefFlagLenOfForeignFldGrp],iRecLen-ObjectDefFlagLenOfForeignFldGrp,"foreignFldGrp",caBuf,sizeof(caBuf));
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadRecFldFromRecStr for name fail! return=[%d]\n",iRet);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			iRet=UnionFormFldGrpFromFieldNameStr(caBuf,iRet,&(pobject->foreignFldGrp[pobject->foreignFldGrpNum]).foreignFldGrp);
			if (iRet < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionFormFldGrpFromFieldNameStr for [%s] fail! return=[%d]\n",caBuf);
				UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
				return(iRet);
			}
			pobject->foreignFldGrpNum++;
		}
	}
	
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);

	return(0);
	// Mary add end, 2008-9-5
	
/* Mary delete, 2008-9-5
	// �򿪶����ļ�
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		ret = UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		UnionSystemErrLog("in UnionCreateObjectDefFromCreateSqlFile:: fopen [%s]\n",fileName);
		return(ret);
	}

	memset(pobject,0,sizeof(*pobject));
	// ���ļ��ж���������	
	if ((lenOfLine = UnionReadNextRecFromFile(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadNextRecFromFile [%s] !\n",fileName);
		goto errorExit;
	}
	// ����������
        strcpy(defLine,tmpBuf);
        UnionToUpperCase(tmpBuf);
	if ((ptr = strstr(tmpBuf,"CREATE TABLE")) == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: CREATE TABLE not defined in file [%s] !\n",fileName);
		goto errorExit;
	}
	memset(varName,0,sizeof(varName));
	sscanf(defLine+strlen("CREATE TABLE"),"%s",varName);
	if (strlen(varName) >= sizeof(pobject->name))
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: objectname too long defined in file [%s] !\n",fileName);
		goto errorExit;
	}
	strcpy(pobject->name,varName);
	
	// ���ļ��ж�����
	if ((ret = UnionReadNextRecFromFile(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadNextRecFromFile [%s] !\n",fileName);
		goto errorExit;
	}
	// ��������ʼλ��
	if ((ptr = strstr(tmpBuf,"(")) == NULL)
	{
		UnionUserErrLog("( not defined in file [%s] !\n",fileName);
		goto errorExit;
	}
	// ������
	pobject->fldNum = 0;
	pobject->uniqueFldGrpNum = 0;
	pobject->foreignFldGrpNum = 0;
	while (!feof(fp))
	{
		if ((lenOfLine = UnionReadNextRecFromFile(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionReadNextRecFromFile [%s] !\n",fileName);
			goto errorExit;
		}
		if (strncmp(tmpBuf,");",2) == 0)	// �Ƕ������
		{
			finishOK = 1;
			break;
		}
		strcpy(defLine,tmpBuf);
		UnionToUpperCase(tmpBuf);
		// �ж��Ƿ��ǹؼ���
		if ((ptr = strstr(tmpBuf,"PRIMARY KEY")) != NULL)
		{	// �ǹؼ���
			if (primaryKeyDefined)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: more than one primary key defined in file [%s]!\n",fileName);
				goto errorExit;
			}
			if ((ret = UnionGetObjectFldGrpDefFromStr(defLine+strlen("PRIMARY KEY"),lenOfLine-strlen("PRIMARY KEY"),&(pobject->primaryKey))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			primaryKeyDefined = 1;
			continue;	
		}
		// �ж��Ƿ���Ψһֵ
		if (((ptr = strstr(tmpBuf,"UNIQUE")) != NULL) && (strstr(ptr+strlen("UNIQUE"),"(") != NULL))
		{
                        // ��Ψһֵ��
			if (pobject->uniqueFldGrpNum >= conMaxUniqueFldGrpNumPerObject)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: too many unique field groups defined in file [%s] !\n",fileName);
				goto errorExit;
			}
			if ((ret = UnionGetObjectFldGrpDefFromStr(defLine+strlen("UNIQUE"),lenOfLine-strlen("UNIQUE"),
				&(pobject->uniqueFldGrp[pobject->uniqueFldGrpNum]))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			++pobject->uniqueFldGrpNum;
			continue;	
		}
		// �ж��Ƿ������
		if ((ptr = strstr(tmpBuf,"FOREIGN KEY")) != NULL)
		{	// �����
			if (pobject->foreignFldGrpNum >= conMaxParentFldGrpNumPerObject)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: too many foreign key groups defined in file [%s] !\n",fileName);
				goto errorExit;
			}			
			// ����������
			if ((lenOfLocalKeyDef = UnionGetObjectFldGrpDefFromStr(defLine+strlen("FOREIGN KEY"),lenOfLine-strlen("FOREIGN KEY"),
				&(pobject->foreignFldGrp[pobject->foreignFldGrpNum].localFldGrp))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			// ���ⲿ������
			memset(varName,0,sizeof(varName));
			sscanf(defLine+strlen("FOREIGN KEY")+lenOfLocalKeyDef,"%s",varName);
			if ((strlen(varName) == 0) || (strlen(varName) >= sizeof(pobject->foreignFldGrp[pobject->foreignFldGrpNum].objectName)))
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: objectname define in [%s] in file [%s] too long!\n",defLine,fileName);
				goto errorExit;
			}
			strcpy(pobject->foreignFldGrp[pobject->foreignFldGrpNum].objectName,varName);
			// ���ⲿ��ֵ
			ptr = strstr(defLine+strlen("FOREIGN KEY")+lenOfLocalKeyDef,varName);
			ptr = ptr + strlen(varName);
			if ((ret = UnionGetObjectFldGrpDefFromStr(ptr,strlen(ptr),
				&(pobject->foreignFldGrp[pobject->foreignFldGrpNum].foreignFldGrp))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			++pobject->foreignFldGrpNum;
			continue;	
		}
		// �ж��Ƿ������(����)
		if ((ptr = strstr(tmpBuf,"REFERENCES")) != NULL)
		{	// �����
			if (pobject->foreignFldGrpNum >= conMaxParentFldGrpNumPerObject)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: too many foreign key groups defined in file [%s] !\n",fileName);
				goto errorExit;
			}			
			// ����������
			if ((lenOfLocalKeyDef = UnionGetObjectFldGrpDefFromStr(defLine,lenOfLine,
				&(pobject->foreignFldGrp[pobject->foreignFldGrpNum].localFldGrp))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			// ���ⲿ������
                        tmpChar = ptr[strlen("REFERENCES")];
                        ptr[strlen("REFERENCES")] = 0;
			memset(varName,0,sizeof(varName));
			sscanf(defLine+strlen(tmpBuf),"%s",varName);
			if ((strlen(varName) == 0) || (strlen(varName) >= sizeof(pobject->foreignFldGrp[pobject->foreignFldGrpNum].objectName)))
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: objectname define in [%s] in file [%s] too long!\n",defLine,fileName);
				goto errorExit;
			}
			strcpy(pobject->foreignFldGrp[pobject->foreignFldGrpNum].objectName,varName);
                        ptr[strlen("REFERENCES")] = tmpChar;
			// ���ⲿ��ֵ
                        UnionToUpperCase(varName);
			ptr = strstr(ptr+strlen("REFERENCES"),varName);
                        tmpChar = ptr[strlen(varName)];
                        ptr[strlen(varName)] = 0;
			if ((ret = UnionGetObjectFldGrpDefFromStr(defLine+strlen(tmpBuf),lenOfLine-strlen(tmpBuf),
				&(pobject->foreignFldGrp[pobject->foreignFldGrpNum].foreignFldGrp))) < 0)
			{
				UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldGrpDefFromStr [%s] in file [%s] !\n",defLine,fileName);
				goto errorExit;
			}
			++pobject->foreignFldGrpNum;
			continue;	
		}
		// ������
		if (pobject->fldNum >= conMaxFldNumPerObject)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: fldNum [%d] in file [%s] out of range!\n",pobject->fldNum,fileName);
			goto errorExit;
		}
		if ((ret = UnionGetObjectFldDefFromStr(defLine,lenOfLine,&(pobject->fldDefGrp[pobject->fldNum]))) < 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionGetObjectFldDefFromStr [%s] in file [%s]!\n",tmpBuf,fileName);
			goto errorExit;
		}
		pobject->fldNum++;
	}
	fclose(fp);
	if (!finishOK)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: object defined without end flag in file [%s]!\n",fileName);
		goto errorExit;
	}
	// ���ؼ����ǲ��ǺϷ�������
	if ((ret = UnionCheckFldGrpInObject(&(pobject->primaryKey),pobject)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: primaryKey fld group defined in file [%s] error!\n",fileName);
		goto errorExit;
	}
	// ���Ψһֵ���ǲ��ǺϷ�������
	for (index = 0; index < pobject->uniqueFldGrpNum; index++)
	{
		if ((ret = UnionCheckFldGrpInObject(&(pobject->uniqueFldGrp[index]),pobject)) < 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: unique fld group [%d] defined in file [%s] error!\n",index,fileName);
			goto errorExit;
		}
	}
	// ����ⲿ�ؼ����ǲ��ǺϷ�������
	for (index = 0; index < pobject->foreignFldGrpNum; index++)
	{
		if ((ret = UnionCheckFldGrpInObject(&(pobject->foreignFldGrp[index].localFldGrp),pobject)) < 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: foreign key fld group [%d] defined in file [%s] error!\n",index,fileName);
			goto errorExit;
		}
		//pobject->foreignFldGrp[index].foreignFldGrp.maxValueLen = pobject->foreignFldGrp[index].localFldGrp.maxValueLen;
	}
	// ���ⲿ����Ǽ����ֵ
	// ���������
	
	// �洢����
	if ((ret = UnionStoreObjectDef(pobject)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromCreateSqlFile:: UnionStoreObjectDef [%s]!\n",pobject->name);
		// ȡ�����ⲿ����Ǽǵ����ֵ
		// ���������
		
		return(ret);;
	}
	
	// ������������ļ�
	// ���������
	
	return(0);	
errorExit:
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ReadObjectDefinition));
*/
}

// �ж��Ƿ�Ϸ����������
/*
���������
	varName		�������
���������
	��
����ֵ��
	1	�Ϸ�������
	0	�Ƿ�������
*/
int UnionIsValidObjectFldName(char *varName)
{
	int	len;
	int	index;
	
	if (varName == NULL)
		return(0);
	for (index = 0,len = strlen(varName); index < len;index++)
	{
		if ((varName[index] >= 'A') && (varName[index] <= 'Z'))
			continue;
		else if ((varName[index] >= 'a') && (varName[index] <= 'z'))
			continue;
                else if((varName[index] == '[') || (varName[index] == ']'))
                        continue;
		else if ((varName[index] >= '0') && (varName[index] <= '9'))
		{
			if (index == 0)
				return(0);
			else
				continue;
		}
		else if ((varName[index] == '_') || (varName[index] == '.'))
		{
			if ((index == 0) || (index == len - 1))
				return(0);
			else
				continue;
		}
		else
			return(0);
	}
	return(1);
}

// ���ܣ���������е����Ƿ��ڶ����ж�����
/*
���������
	pfldGrp		Ҫ��������
	pobject		�ڸö����м��
���������
	��
����ֵ��
	�ɹ���>= 0
	ʧ�ܣ�<0��������
*/
int UnionCheckFldGrpInObject(PUnionObjectFldGrp pfldGrp,PUnionObject pobject)
{
	unsigned int	index1,index2;
	int	len = 0;

	if ((pobject == NULL) || (pfldGrp == NULL))
	{
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	for (index1 = 0; index1 < pfldGrp->fldNum; index1++)
	{
		for (index2 = 0; index2 < pobject->fldNum; index2++)
		{
			if (strcmp(pobject->fldDefGrp[index2].name,pfldGrp->fldNameGrp[index1]) == 0)
			{
				len += pobject->fldDefGrp[index2].size;
				break;
			}
		}
		if (index2 == pobject->fldNum)
		{
			UnionUserErrLog("in UnionCheckFldGrpInObject:: fld [%s] in fldGrp not defined in object [%s]\n",
				pfldGrp->fldNameGrp[index1],pobject->name);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ReadObjectDefinition));
		}
	}
	//pfldGrp->maxValueLen = len + pfldGrp->fldNum;
	return(0);
}

/*
���ܣ��ж�һ����ֵ�Ƿ�����һ����ֵ������
���������
	pfldGrpChild��Ҫ��������
	pfldGrpParent���ڸ������м��
���������
	��
����ֵ��
	1����
	0������
	<0��ʧ�ܣ����ش�����
*/
int UnionIsChildFldGrpOfFldGrp(PUnionObjectFldGrp pfldGrpChild,PUnionObjectFldGrp pfldGrpParent)
{
	unsigned int	index1,index2;
	
	if (pfldGrpChild == NULL || pfldGrpParent == NULL)
	{
		UnionUserErrLog("in UnionIsChildFldGrpOfFldGrp:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	for (index1 = 0; index1 < pfldGrpChild->fldNum; index1++)
	{
		for (index2 = 0; index2 < pfldGrpParent->fldNum; index2++)
		{
			if (strcmp(pfldGrpChild->fldNameGrp[index1],pfldGrpParent->fldNameGrp[index2]) == 0)
				break;
		}
		if (index2 == pfldGrpParent->fldNum)
		{
			//UnionAuditLog("in UnionIsChildFldGrpOfFldGrp:: fld [%s] in pfldGrpChild not defined in pfldGrpParent\n",
			//	pfldGrpChild->fldNameGrp[index1]);
			return(0);
		}
	}
	
	return(1);
}

/*
���ܣ��ж����������Ƿ��ظ�
���������
	fldGrp1������1
	fldGrp2������2
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ����ش�����
*/
int UnionFldGrpIsRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2)
{
	int	iRet;

	if (fldGrp1 == NULL || fldGrp2 == NULL)
	{
		UnionUserErrLog("in UnionFldGrpIsRepeat:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	iRet=UnionIsChildFldGrpOfFldGrp(fldGrp1,fldGrp2);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionFldGrpIsRepeat:: UnionIsChildFldGrpOfFldGrp fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
		return(0);
	
	iRet=UnionIsChildFldGrpOfFldGrp(fldGrp2,fldGrp1);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionFldGrpIsRepeat:: UnionIsChildFldGrpOfFldGrp again fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	if (iRet > 0)
		return(1);
	else
		return(0);
}

/*
���ܣ��ж����������Ƿ���ȫһ�£�����������������˳���ж�
���������
	fldGrp1������1
	fldGrp2������2
���������
	��
����ֵ��
	1���ظ�
	0�����ظ�
	<0��ʧ�ܣ����ش�����
*/
int UnionFldGrpIsCompleteRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2)
{
	int	iRet,iKeyLen1,iKeyLen2;
	char	caKeyField1[1024],caKeyField2[1024];

	if (fldGrp1 == NULL || fldGrp2 == NULL)
	{
		UnionUserErrLog("in UnionFldGrpIsCompleteRepeat:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caKeyField1,0,sizeof(caKeyField1));
	iKeyLen1=UnionFormFieldNameStrFromFldGrp(fldGrp1,caKeyField1);
	if (iKeyLen1 < 0)
	{
		UnionUserErrLog("in UnionFldGrpIsCompleteRepeat:: UnionFormFieldNameStrFromFldGrp for fldGrp1 fail! return=[%d]\n",iKeyLen1);
		return(iKeyLen1);
	}
	UnionLog("in UnionFldGrpIsCompleteRepeat:: field group1 = [%s] length = [%d]\n",caKeyField1,iKeyLen1);
	
	memset(caKeyField2,0,sizeof(caKeyField2));
	iKeyLen2=UnionFormFieldNameStrFromFldGrp(fldGrp2,caKeyField2);
	if (iKeyLen2 < 0)
	{
		UnionUserErrLog("in UnionFldGrpIsCompleteRepeat:: UnionFormFieldNameStrFromFldGrp for fldGrp2 fail! return=[%d]\n",iKeyLen2);
		return(iKeyLen2);
	}
	
	UnionLog("in UnionFldGrpIsCompleteRepeat:: field group2 = [%s] length = [%d]\n",caKeyField2,iKeyLen2);
	
	if (iKeyLen1 == iKeyLen2 && strcmp(caKeyField1,caKeyField2) == 0)
		return(1);
	else
		return(0);
}

/*
���ܣ�
	���һ�������ȱʡ���ͳ���
���������
	type	����
���������
	��
����ֵ��
	����
*/
int UnionGetDefaultSizeOfTBLFieldType(int type)
{
	switch(type)
	{
		case conObjectFldType_Int:
			return(12);
        	case conObjectFldType_Double:
        		return(20);
	        case conObjectFldType_Bool:
	        	return(1);
	        default:
        	       	return(1);
	}
}


/*
���ܣ��ж������Ƿ�Ϸ�
���������
	fldDef������
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ����ش�����
*/
int UnionIsValidObjectFieldDef(TUnionObjectFldDef fldDef)
{
	int	iLen;
	
	// �ж϶��������Ƿ�Ϸ�
	if (UnionIsValidObjectFldName(fldDef.name) == 0)
	{
		UnionAuditLog("in UnionIsValidObjectFieldDef:: name of field [%s] is invalid\n",fldDef.name);
		return(0);
	}
	
	switch(fldDef.type)
	{
	case conObjectFldType_String:
		break;
	case conObjectFldType_Int:
		/*
		if (fldDef.size != 12)
		{
			UnionAuditLog("in UnionIsValidObjectFieldDef:: field size [%d] of INT is invalid!\n",fldDef.size);
        		return(0);
        	}
        	*/
        	break;
        case conObjectFldType_Double:
        	/*
        	if (fldDef.size != 20)
		{
			UnionAuditLog("in UnionIsValidObjectFieldDef:: field size [%d] of DOUBLE is invalid!\n",fldDef.size);
        		return(0);
        	}
        	*/
        	break;
        case conObjectFldType_Bit:
        	break;
        case conObjectFldType_Bool:
        	/*
        	if (fldDef.size != 1)
		{
			UnionAuditLog("in UnionIsValidObjectFieldDef:: field size [%d] of BOOL is invalid!\n",fldDef.size);
        		return(0);
        	}
        	*/
        	break;
        default:
        	UnionAuditLog("in UnionIsValidObjectFieldDef:: field type [%d] is invalid!\n",fldDef.type);
        	return(0);
	}

/*
	if (fldDef.isUnique != 0 && fldDef.isUnique != 1)
	{
		UnionAuditLog("in UnionIsValidObjectFieldDef:: field isUnique [%d] is invalid!\n",fldDef.isUnique);
        	return(0);
	}
*/	
	if (fldDef.nullPermitted != 0 && fldDef.nullPermitted != 1)
	{
		UnionAuditLog("in UnionIsValidObjectFieldDef:: field nullPermitted [%d] is invalid!\n",fldDef.nullPermitted);
        	return(0);
	}
	
	switch(fldDef.type)
	{
	case conObjectFldType_String:
        case conObjectFldType_Bit:
        	break;
        default:
        	return(1);
	}

	iLen=strlen(fldDef.defaultValue);
	if (iLen > 0 && iLen > fldDef.size)
	{
		UnionAuditLog("in UnionIsValidObjectFieldDef:: defaultValue length[%d] > fieldDef.size[%d]!\n",iLen,fldDef.size);
		return(0);
	}
	
	return(1);
}

// Mary add begin, 2008-9-5
/*
���ܣ���һ��������Ĵ�д����󴴽��ļ�
���������
	idOfObject������ID
	objSqlStr�������崮
	objSqlStrLen�������崮�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateObjectSqlFileByString(TUnionIDOfObject idOfObject,char *objSqlStr,int objSqlStrLen)
{
	char	caFileName[512];
	int	iRet;
	
	if ((idOfObject == NULL) || (objSqlStr == NULL) || (objSqlStrLen <= 0))
	{
		UnionUserErrLog("in UnionCreateObjectSqlFileByString:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// ��ö��󴴽��ļ�����
	memset(caFileName,0,sizeof(caFileName));
	if ((iRet = UnionGetObjectDefCreateFileName(idOfObject,caFileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectSqlFileByString:: UnionGetObjectDefCreateFileName [%s]\n",idOfObject);
		return(iRet);
	}

	// �����󴴽��ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) != 0)
	{
		UnionUserErrLog("in UnionCreateObjectSqlFileByString:: [%s] already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}

	// ��һ��������Ĵ�д����󴴽��ļ�
	iRet=UnionWriteRecStrIntoFile(caFileName,objSqlStr,objSqlStrLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectSqlFileByString:: UnionWriteRecStrIntoFile [%s] fail! return=[%d]\n",idOfObject,iRet);
		return(iRet);
	}
	
	return(0);
}
// Mary add end, 2008-9-5
/*
���ܣ��������壬д�뵽�����ļ���
���������
	pobject��ָ��������ָ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ����ش�����
*/
int UnionCreateComplexDBTBLCreateSqlFile(PUnionObject pobject)
{
	char	caFileName[512];
	int	ret;
	
	if (pobject == NULL)
	{
		UnionUserErrLog("in UnionCreateComplexDBTBLCreateSqlFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö������ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectDefCreateFileName(pobject->name,caFileName);

	if ((ret = UnionPrintObjectDefToSpecFile(pobject,caFileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateComplexDBTBLCreateSqlFile:: UnionPrintObjectDefToSpecFile!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	return(ret);
}

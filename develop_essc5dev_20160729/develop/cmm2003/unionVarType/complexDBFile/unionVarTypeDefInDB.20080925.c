// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarTypeDefFile.h"
#include "UnionLog.h"

/*
����	
	���һ���ؼ������ڵĳ�������
�������
	keyWord		��������
�������
	programName	��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetProgramFileNameOfKeyWord(char *keyWord,char *nameOfProgram)
{
	int	ret;
	TUnionVarTypeDef	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarTypeDefFromDefaultFile(keyWord,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetProgramFileNameOfKeyWord:: UnionReadVarTypeDefFromDefaultFile!\n");
		return(ret);
	}
	if (nameOfProgram != NULL)
		strcpy(nameOfProgram,def.nameOfProgram);
	return(0);
}

/* �ж��Ƿ����һ�����͵Ķ���
�������
	fileName	�ļ�����
	nameOfType	ָ��������
�������
	��
����ֵ��
	>=1 	����
	0	������	
	<0	����
	
*/
int UnionExistsVarTypeDefInSpecFile(char *fileName,char *nameOfType)
{
	char			lineStr[1024+1];
	int			lineLen;
	int			ret;
	FILE			*fp;
	TUnionVarTypeDef	def;
	
	if (fileName == NULL)
	{
		UnionUserErrLog("in UnionExistsVarTypeDefInSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionExistsVarTypeDefInDefaultDefFile(nameOfType));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExistsVarTypeDefInSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionExistsVarTypeDefInSpecFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (strcmp(nameOfType,def.nameOfType) == 0)
		{
			fclose(fp);
			return(1);
		}
	}
	fclose(fp);
	return(0);
}

/* �ж�ȱʡ�����ļ����Ƿ����һ�����͵Ķ���
�������
	nameOfType	ָ��������
�������
	��
����ֵ��
	>=1 	����
	0	������	
	<0	����
	
*/
int UnionExistsVarTypeDefInDefaultDefFile(char *nameOfType)
{
	char       record[1024+1];
        char       condition[1024+1];
        int        ret;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "nameOfType=");
        strcat(condition, nameOfType);
        strcat(condition, "|");

        ret = UnionSelectUniqueObjectRecordByPrimaryKey("keyWord", condition, record, sizeof(record));
        if (ret == errCodeObjectMDL_ObjectDefinitionNotExists)
        {
        	return 0;
        }
        return ret;

}	

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromSpecFile(char *fileName,char *nameOfType,PUnionVarTypeDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionReadVarTypeDefFromDefaultFile(nameOfType,pdef));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadVarTypeDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionReadVarTypeDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(nameOfType,pdef->nameOfType) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadVarTypeDefFromSpecFile:: [%s] not defined in [%s]\n",nameOfType,fileName);
	return(errCodeCDPMDL_VarTypeNotDefined);
}

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromDefaultFile(char *nameOfType,PUnionVarTypeDef pdef)
{
	char       record[1024+1];
        char       condition[1024+1];
        int        ret;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "nameOfType=");
        strcat(condition, nameOfType);
        strcat(condition, "|");
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("keyWord", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadVarTypeDefFromDefaultFile:: UnionReadVarTypeDefFromDefaultFile!\n");
                return(ret);
        }
        
        /* ���ؼ������Ƹ�ֵ */
        strcpy(pdef->nameOfType, nameOfType);

        /* ���ؼ������͸�ֵ */
        UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "typeTag", &(pdef->typeTag) );

        /* ���ؼ�������ģ�鸳ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "nameOfProgram", pdef->nameOfProgram, sizeof (pdef->nameOfProgram));

        return(0);
}

/* �����ͱ�ʶ�Ķ���д�뵽ָ�����ļ���
�������
	pdef		���ͱ�ʶ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteVarTypeDefIntoSpecFile(PUnionVarTypeDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((ret = UnionExistsVarTypeDefInSpecFile(fileName,pdef->nameOfType)) < 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: UnionExistsVarTypeDefInSpecFile [%s]\n",pdef->nameOfType);
		return(ret);
	}
	if (ret > 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: [%s] already exists!\n",pdef->nameOfType);		
		return(errCodeRECMDL_VarAlreadyExists);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteVarTypeDefIntoDefaultFile(pdef));
	if ((fp = fopen(fileName,"a+")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteVarTypeDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintVarTypeDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: UnionPrintVarTypeDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* �����ͱ�ʶ�Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		���ͱ�ʶ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteVarTypeDefIntoDefaultFile(PUnionVarTypeDef pdef)
{
	char	record[1024+1];

	memset(record, 0, sizeof record);

	if (NULL == pdef)
		return (errCodeParameter);

	sprintf(record, "%s=%s|%s=%d|%s=%s|", "nameOfType",pdef->nameOfType,"typeTag",pdef->typeTag,"nameOfProgram",pdef->nameOfProgram);

	return (UnionInsertObjectRecord("keyWord", record, strlen(record)));
}

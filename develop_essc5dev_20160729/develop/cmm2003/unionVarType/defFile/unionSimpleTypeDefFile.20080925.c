//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionSimpleTypeDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"
#include "unionVarTypeDefFile.h"

/* ��ָ���ļ���ָ�����Ƶļ����͵Ķ���
�������
	fileName	�ļ�����
�������
	pdef	�����ļ����Ͷ���
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionReadSimpleTypeDefFromSpecFile(char *fileName,PUnionSimpleTypeDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSimpleTypeDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadSimpleTypeDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadSimpleTypeDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_SimpleTypeNotDefined);
}

/* ��ȱʡ�����ļ���ָ�����Ƶļ����͵Ķ���
�������
	nameOfType	�ṹ����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadSimpleTypeDefFromDefaultDefFile(char *nameOfType,PUnionSimpleTypeDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	if ((ret = UnionReadSimpleTypeDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromDefaultDefFile:: UnionReadSimpleTypeDefFromSpecFile [%s] from [%s]\n",nameOfType,fileName);
		return(ret);
	}
	return(ret);
}


/* �������͵Ķ���д�뵽ָ�����ļ���
�������
	pdef		�����Ͷ���
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSimpleTypeDefIntoSpecFile(PUnionSimpleTypeDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteSimpleTypeDefIntoDefaultFile(pdef,""));
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteSimpleTypeDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintSimpleTypeDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteSimpleTypeDefIntoSpecFile:: UnionPrintSimpleTypeDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* �������͵Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		�����Ͷ���
	programFileName	���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSimpleTypeDefIntoDefaultFile(PUnionSimpleTypeDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionSimpleTypeDef	def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	// �Ǽ����Ͷ���
	memset(&varTypeDef,0,sizeof(varTypeDef));
	strcpy(varTypeDef.nameOfType,pdef->name);
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// δ���������
	{
		varTypeDef.typeTag = conVarTypeTagSimpleType;
		UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecSimpleTypeDefFromCHeaderFile:: UnionWriteSimpleTypeDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// �Ѷ��������
	{
		// ��ǰ�����Ͳ��Ǽ���������
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagSimpleType)
		{
			UnionUserErrLog("in UnionWriteSimpleTypeDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWriteSimpleTypeDefIntoSpecFile(pdef,fileName));
}

/* �������͵Ķ���д�뵽ȱʡ���ļ���
�������
	nameOfType	�����Ͷ���
	name		��������
	remark		˵��
	programFileName	���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����ļ����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSpecSimpleTypeDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName)
{
	TUnionSimpleTypeDef	def;
	
	memset(&def,0,sizeof(def));
	if ((nameOfType == NULL) || (strlen(nameOfType) == 0) || (name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	strcpy(def.nameOfType,nameOfType);
	strcpy(def.name,name);
	if (remark != NULL)
		strcpy(def.remark,remark);
	return(UnionWriteSimpleTypeDefIntoDefaultFile(&def,programFileName));
}

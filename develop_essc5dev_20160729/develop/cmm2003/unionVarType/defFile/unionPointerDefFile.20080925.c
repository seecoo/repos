//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionPointerDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"


/* ��ȱʡ�ļ���ָ�����Ƶ�ָ��Ķ���
�������
	nameOfType	ָ�����͵�����
�������
	pdef	������ָ�붨��
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionReadPointerDefFromDefaultDefFile(char *nameOfType,PUnionPointerDef pdef)
{
	char			fileName[256+1];

	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	return(UnionReadPointerDefFromSpecFile(fileName,pdef));
}

/* ��ָ���ļ���ָ�����Ƶ�ָ��Ķ���
�������
	fileName	�ļ�����
�������
	pdef	������ָ�붨��
����ֵ��
	>=0 	������ָ��Ĵ�С
	<0	�������	
	
*/
int UnionReadPointerDefFromSpecFile(char *fileName,PUnionPointerDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadPointerDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadPointerDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadPointerDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadPointerDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_PointerNotDefined);
}

/* ��ָ�����͵Ķ���д�뵽ָ�����ļ���
�������
	pdef		ָ�����Ͷ���
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWritePointerDefIntoSpecFile(PUnionPointerDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWritePointerDefIntoDefaultFile(pdef,""));
	UnionPrintPointerDefToFp(pdef,stdout);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWritePointerDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintPointerDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWritePointerDefIntoSpecFile:: UnionPrintPointerDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* ��ָ�����͵Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		ָ�����Ͷ���
	programFileName	ָ���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWritePointerDefIntoDefaultFile(PUnionPointerDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionPointerDef	def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);


	// ��ָ�����Ͷ���
	memset(&varTypeDef,0,sizeof(varTypeDef));
	strcpy(varTypeDef.nameOfType,pdef->name);
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// δ����ָ������
	{
		varTypeDef.typeTag = conVarTypeTagPointer;
		UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecPointerDefFromCHeaderFile:: UnionWritePointerDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// �Ѷ���ָ������
	{
		// ��ǰ�����Ͳ���ָ����������
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagPointer)
		{
			UnionUserErrLog("in UnionWritePointerDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWritePointerDefIntoSpecFile(pdef,fileName));
}

/* ��ָ�����͵Ķ���д�뵽ȱʡ���ļ���
�������
	nameOfType	ָ�����Ͷ���
	name		��������
	remark		˵��
	programFileName	ָ���������ڵĳ�������
�������
	��
����ֵ��
	>=0 	������ָ�����͵Ĵ�С
	<0	�������	
	
*/
int UnionWriteSpecPointerDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName)
{
	TUnionPointerDef	def;
	
	memset(&def,0,sizeof(def));
	if ((nameOfType == NULL) || (strlen(nameOfType) == 0) || (name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	strcpy(def.nameOfType,nameOfType);
	strcpy(def.name,name);
	if (remark != NULL)
		strcpy(def.remark,remark);
	return(UnionWritePointerDefIntoDefaultFile(&def,programFileName));
}



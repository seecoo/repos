//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionDesignKeyWord.h"
#include "unionVarTypeDef.h"
#include "unionModuleDef.h"
#include "unionProgramDef.h"
#include "unionErrCode.h"
#include "UnionLog.h"

/*
����	
	���һ���������ڳ����ȫ��
�������
	funName		��������
�������
	fullProgramName	�����ȫ��
	incFileName	����Ӧ������ͷ�ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetFullProgramFileNameOfFun(char *funName,char *fullProgramName,char *incFileName)
{
	char	nameOfProgram[128+1];
	char	nameOfModule[128+1];
	char	devDir[256+1];
	char	version[128+1];
	int	ret;
	
	if ((ret = UnionGetProgramFileNameOfKeyWord(funName,nameOfProgram)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetProgramFileNameOfKeyWord [%s]\n",funName);
		return(ret);
	}
	if ((ret = UnionGetModuleNameAndVersionOfProgram(nameOfProgram,nameOfModule,version)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetModuleNameAndVersionOfProgram [%s]\n",nameOfProgram);
		return(ret);
	}
	if ((ret = UnionGetDevDirAndIncFileConfOfModule(nameOfModule,devDir,incFileName)) < 0)
	{
		UnionUserErrLog("in UnionGetFullProgramFileNameOfFun:: UnionGetDevDirAndIncFileConfOfModule [%s]\n",nameOfModule);
		return(ret);
	}
	if (fullProgramName != NULL)
		sprintf(fullProgramName,"%s/%s.%s.c",devDir,nameOfProgram,version);
	return(0);
}
	
/*
����	
	��ʾһ�����͵Ķ���
�������
	typeTag		ָ��������
	keyWord		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayKeyWordDefOfSpecTypeToFp(int typeTag,char *keyWord,FILE *fp)
{
	int	ret;
	int	tag;

	if (keyWord == NULL)
		return(errCodeParameter);
		
	switch (typeTag)
	{
		case	conDesginKeyWordTagFun:	// ����
			ret = UnionPrintSpecFunDefToFp(keyWord,fp);
			break;
		case	conDesginKeyWordTagConst:	// ����
			ret = UnionPrintSpecConstDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagSimpleType:	// ������
			ret = UnionPrintSpecSimpleTypeDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagStruct:	// �ṹ
			ret = UnionPrintSpecStructDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagUnion:	// ����
			ret = UnionPrintSpecUnionDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagArray:	// ����
			ret = UnionPrintSpecArrayDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagPointer:	// ָ��
			ret = UnionPrintSpecPointerDefToFp(keyWord,fp);
			break;
		case	conVarTypeTagEnum:	// ö��
			ret = UnionPrintSpecEnumDefToFp(keyWord,fp);
			break;
		case	conDesginKeyWordTagGlobalVar:	// ȫ�ֱ���
			ret = UnionPrintSpecVariableDefToFp(keyWord,fp);
			break;
		default:
			UnionUserErrLog("in UnionDisplayKeyWordDefOfSpecTypeToFp:: invalid keyWord [%s]\n",keyWord,fp);
			return(errCodeCDPMDL_InvalidKeyWord);
	}
	return(ret);
}

/*
����	
	��ʾһ�����͵Ķ���
�������
	keyWord		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayKeyWordDefToFp(char *keyWord,FILE *fp)
{
	return(UnionDisplayAllDefOfSpecKeyWordToFp(keyWord,fp));
}

/*
����	
	��ʾһ�����͵Ķ���
�������
	keyWord		��������
	fileName	��һ�����Ͷ���д�뵽�ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputKeyWordDefToSpecFile(char *keyWord,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionOutputKeyWordDefToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionDisplayAllDefOfSpecKeyWordToFp(keyWord,fp)) < 0)
	{
		UnionUserErrLog("in UnionOutputKeyWordDefToSpecFile:: UnionDisplayAllDefOfSpecKeyWordToFp!\n");
	}
	fclose(fp);
	return(ret);
}

/*
����	
	��ʾһ�����͵����ж���
�������
	keyWord		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionDisplayAllDefOfSpecKeyWordToFp(char *keyWord,FILE *fp)
{
	FILE			*indexFp;
	char			fileName[256+1];
	char			lineStr[1024+1];
	int			lineLen;
	int			ret;
	TUnionVarTypeDef	def;
	int			keyWordType;
	int			totalNum = 0;
	
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);	
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	keyWordType = UnionConvertUnionKeyWordIntoTag(keyWord);
	while (!feof(indexFp))
	{
		memset(&def,0,sizeof(def));
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (keyWordType > 0)
		{
			if (keyWordType != def.typeTag)
				continue;
			if ((ret = UnionDisplayKeyWordDefOfSpecTypeToFp(def.typeTag,def.nameOfType,fp)) < 0)
			{
				UnionUserErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionDisplayKeyWordDefOfSpecTypeToFp [%s]!\n",def.nameOfType);
				printf("[%s] not defined!\n",def.nameOfType);
				continue;
			}
			totalNum++;
		}
		else if (strcmp(keyWord,def.nameOfType) == 0)
		{
			if ((ret = UnionDisplayKeyWordDefOfSpecTypeToFp(def.typeTag,def.nameOfType,fp)) < 0)
			{
				UnionUserErrLog("in UnionDisplayAllDefOfSpecKeyWordToFp:: UnionDisplayKeyWordDefOfSpecTypeToFp [%s]!\n",def.nameOfType);
				printf("[%s] not defined!\n",def.nameOfType);
				continue;
			}
			totalNum++;
			break;
		}
	}
	fclose(indexFp);
	return(0);
}	
	

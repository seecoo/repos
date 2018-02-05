// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// ��������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: nameOfType must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	memset(typeTag,0,sizeof(typeTag));
	// ������ʶ
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagTypeTag,typeTag,sizeof(typeTag))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagTypeTag,str);
		return(ret);
	}
	pdef->typeTag = UnionConvertCVarDefKeyWordIntoTag(typeTag);
	// ģ������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarTypeDefTagProgramName,pdef->nameOfProgram,sizeof(pdef->nameOfProgram))) < 0)
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarTypeDefTagProgramName,str);
		return(ret);
	}
	return(0);
}

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	pdef		��������
	sizeOfBuf	���崮�Ĵ�С
�������
	str		���崮
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPutVarTypeDefIntoStr(PUnionVarTypeDef pdef,char *str,int sizeOfBuf)
{
	int	ret;
	char	keyWord[128+1];
	
	// ��������
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagNameOfType,pdef->nameOfType,strlen(pdef->nameOfType),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagNameOfType,str);
		return(ret);
	}
	memset(keyWord,0,strlen(keyWord));
	UnionConvertTypeTagIntoCVarDefKeyWord(pdef->typeTag,keyWord);
	// ������ʶ
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagTypeTag,keyWord,strlen(keyWord),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagTypeTag,str);
		return(ret);
	}
	// ģ������
	if ((ret = UnionPutRecFldIntoRecStr(conVarTypeDefTagProgramName,pdef->nameOfProgram,strlen(pdef->nameOfProgram),str,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutVarTypeDefIntoStr:: UnionPutRecFldIntoRecStr [%s] into [%s]!\n",conVarTypeDefTagProgramName,str);
		return(ret);
	}
	return(0);
}

/*
����	
	��һ�������ڲ��ؼ��ֱ�ʶת��Ϊ��ʶֵ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertUnionKeyWordIntoTag(char *keyWord)
{
	int	tag;
	
	if ((tag == UnionConvertCVarDefTypeIntoTag(keyWord)) > 0)
		return(tag);
	if (strcmp(keyWord,conDesginKeyWordTagStrPointer) == 0)
		return(conVarTypeTagPointer);
	else if (strcmp(keyWord,conDesginKeyWordTagStrStruct) == 0)
		return(conVarTypeTagStruct);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnion) == 0)
		return(conVarTypeTagUnion);
	else if (strcmp(keyWord,conDesginKeyWordTagStrArray) == 0)
		return(conVarTypeTagArray);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFun) == 0)
		return(conDesginKeyWordTagFun);
	else if (strcmp(keyWord,conDesginKeyWordTagStrConst) == 0)
		return(conDesginKeyWordTagConst);
	else if (strcmp(keyWord,conDesginKeyWordTagStrGlobalVar) == 0)
		return(conDesginKeyWordTagGlobalVar);
	else 
		return(errCodeCDPMDL_InvalidKeyWord);
}

/*
����	
	��һ��c�����Ͷ���Ĺؼ���ת��Ϊ�����ڲ���ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertCVarDefTypeIntoTag(char *keyWord)
{
	if (strcmp(keyWord,conDesginKeyWordTagStrInt) == 0)
		return(conVarTypeTagInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrLong1) == 0))
		return(conVarTypeTagLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrChar) == 0)
		return(conVarTypeTagChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrDouble) == 0)
		return(conVarTypeTagDouble);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFloat) == 0)
		return(conVarTypeTagFloat);
	else if (strcmp(keyWord,conDesginKeyWordTagStrEnum) == 0)
		return(conVarTypeTagEnum);
	else if (strcmp(keyWord,conDesginKeyWordTagStrVoid) == 0)
		return(conVarTypeTagVoid);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrFile) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrFile1) == 0))
		return(conVarTypeTagFile);
	else if (strcmp(keyWord,conDesginKeyWordTagStrString) == 0)
		return(conVarTypeTagString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedString) == 0)
		return(conVarTypeTagUnsignedString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedInt) == 0)
		return(conVarTypeTagUnsignedInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong1) == 0))
		return(conVarTypeTagUnsignedLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedChar) == 0)
		return(conVarTypeTagUnsignedChar);
	else return(errCodeCDPMDL_NotBaseVarType);
}

/*
����	
	�ж�һ�������Ƿ���һ��������
�������
	nameOfType	��������
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsBaseType(char *nameOfType)
{
	return(UnionIsBaseTypeTag(UnionGetTypeTagOfSpecNameOfType(nameOfType)));
}

/*
����	
	���ָ�����͵ı������ڲ���ʶ
�������
	nameOfType	��������
�������
	��
����ֵ
	�������͵��ڲ���ʶ
*/
int UnionGetTypeTagOfSpecNameOfType(char *nameOfType)
{
	TUnionVarTypeDef	typeTagDef;
	char			fileName[256+1];
	int			ret;
	int			typeTag;
	
	if ((typeTag = UnionConvertCVarDefTypeIntoTag(nameOfType)) != errCodeCDPMDL_NotBaseVarType)
		return(typeTag);
	// ��ȡ�������͵ı�ʶ
	UnionGetDefaultFileNameOfVarTypeTagDef(nameOfType,fileName);
	memset(&typeTagDef,0,sizeof(typeTagDef));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,nameOfType,&typeTagDef)) < 0)
	{
		UnionUserErrLog("in UnionGetTypeTagOfSpecNameOfType:: UnionReadVarTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(typeTagDef.typeTag);
}

/*
����	
	���ָ�����͵ı�������ԭʼ����
�������
	oriNameOfType	��������
�������
	finalNameOfType	������������
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionGetFinalTypeNameOfSpecNameOfType(char *oriNameOfType,char *finalNameOfType)
{
	TUnionVarTypeDef	typeTagDef;
	char			fileName[256+1];
	int			ret;
	int			typeTag;
	
	if ((typeTag = UnionConvertCVarDefTypeIntoTag(oriNameOfType)) != errCodeCDPMDL_NotBaseVarType)
	{
		strcpy(finalNameOfType,oriNameOfType);
		return(0);
	}
	// ��ȡ�������͵ı�ʶ
	UnionGetDefaultFileNameOfVarTypeTagDef(oriNameOfType,fileName);
	memset(&typeTagDef,0,sizeof(typeTagDef));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,oriNameOfType,&typeTagDef)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeNameOfSpecNameOfType:: UnionReadVarTypeDefFromSpecFile [%s]!\n",oriNameOfType);
		return(ret);
	}
	if (typeTagDef.typeTag != conVarTypeTagSimpleType)
	{
		strcpy(finalNameOfType,typeTagDef.nameOfType);
		return(0);
	}
	return(UnionGetFinalTypeNameOfSpecNameOfSimpleType(typeTagDef.nameOfType,finalNameOfType));
}

/*
����	
	�ж��Ƿ���һ����������
�������
	keyTag		���ͱ�ʶ
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsBaseTypeTag(TUnionVarTypeTag keyTag)
{
	switch (keyTag)
	{
		case	conVarTypeTagInt:
		case	conVarTypeTagLong:
		case	conVarTypeTagChar:
		case	conVarTypeTagUnsignedInt:
		case	conVarTypeTagUnsignedLong:
		case	conVarTypeTagUnsignedChar:
		case	conVarTypeTagDouble:
		case	conVarTypeTagFloat:
		case	conVarTypeTagVoid:
		case	conVarTypeTagEnum:
		case	conVarTypeTagFile:
		case	conVarTypeTagString:
		case	conVarTypeTagUnsignedString:
			return(1);
		case	conVarTypeTagSimpleType:
		case	conVarTypeTagStruct:
		case	conVarTypeTagUnion:
		case	conVarTypeTagArray:
		case	conVarTypeTagPointer:
			return(0);
		default:
			return(errCodeCDPMDL_InvalidVarType);
	}
}

/*
����	
	��һ�������ڲ���ʶת��Ϊc�����Ͷ���Ĺؼ���
�������
	keyTag		���ͱ�ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertTypeTagIntoCVarDefKeyWord(TUnionVarTypeTag keyTag,char *keyWord)
{
	switch (keyTag)
	{
		case	conVarTypeTagInt:
			strcpy(keyWord,conDesginKeyWordTagStrInt);
			return(0);
		case	conVarTypeTagLong:
			strcpy(keyWord,conDesginKeyWordTagStrLong);
			return(0);
		case	conVarTypeTagChar:
			strcpy(keyWord,conDesginKeyWordTagStrChar);
			return(0);
		case	conVarTypeTagUnsignedInt:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedInt);
			return(0);
		case	conVarTypeTagUnsignedLong:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedLong);
			return(0);
		case	conVarTypeTagUnsignedChar:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedChar);
			return(0);
		case	conVarTypeTagDouble:
			strcpy(keyWord,conDesginKeyWordTagStrDouble);
			return(0);
		case	conVarTypeTagFloat:
			strcpy(keyWord,conDesginKeyWordTagStrFloat);
			return(0);
		case	conVarTypeTagFile:
			strcpy(keyWord,conDesginKeyWordTagStrFile);
			return(0);
		case	conVarTypeTagString:
			strcpy(keyWord,conDesginKeyWordTagStrString);
			return(0);
		case	conVarTypeTagUnsignedString:
			strcpy(keyWord,conDesginKeyWordTagStrUnsignedString);
			return(0);
		case	conVarTypeTagEnum:
			strcpy(keyWord,conDesginKeyWordTagStrEnum);
			return(0);
		case	conVarTypeTagSimpleType:
			strcpy(keyWord,conDesginKeyWordTagStrSimpleType);
			return(0);
		case	conVarTypeTagStruct:
			strcpy(keyWord,conDesginKeyWordTagStrStruct);
			return(0);
		case	conVarTypeTagUnion:
			strcpy(keyWord,conDesginKeyWordTagStrUnion);
			return(0);
		case	conVarTypeTagArray:
			strcpy(keyWord,conDesginKeyWordTagStrArray);
			return(0);
		case	conVarTypeTagPointer:
			strcpy(keyWord,conDesginKeyWordTagStrPointer);
			return(0);
		case	conVarTypeTagVoid:
			strcpy(keyWord,conDesginKeyWordTagStrVoid);
			return(0);
		default:
			strcpy(keyWord,conDesginKeyWordTagStrVoid);
			return(0);
	}
}

/*
����	
	��һ��c�����Ͷ���Ĺؼ���ת��Ϊ�����ڲ���ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertCVarDefKeyWordIntoTag(char *keyWord)
{
	if (strcmp(keyWord,conDesginKeyWordTagStrInt) == 0)
		return(conVarTypeTagInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrLong1) == 0))
		return(conVarTypeTagLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrChar) == 0)
		return(conVarTypeTagChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedInt) == 0)
		return(conVarTypeTagUnsignedInt);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrUnsignedLong1) == 0))
		return(conVarTypeTagUnsignedLong);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedChar) == 0)
		return(conVarTypeTagUnsignedChar);
	else if (strcmp(keyWord,conDesginKeyWordTagStrDouble) == 0)
		return(conVarTypeTagDouble);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFloat) == 0)
		return(conVarTypeTagFloat);
	else if ((strcmp(keyWord,conDesginKeyWordTagStrFile) == 0) || (strcmp(keyWord,conDesginKeyWordTagStrFile) == 0))
		return(conVarTypeTagFile);
	else if (strcmp(keyWord,conDesginKeyWordTagStrString) == 0)
		return(conVarTypeTagString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnsignedString) == 0)
		return(conVarTypeTagUnsignedString);
	else if (strcmp(keyWord,conDesginKeyWordTagStrEnum) == 0)
		return(conVarTypeTagEnum);
	else if (strcmp(keyWord,conDesginKeyWordTagStrSimpleType) == 0)
		return(conVarTypeTagSimpleType);
	else if (strcmp(keyWord,conDesginKeyWordTagStrStruct) == 0)
		return(conVarTypeTagStruct);
	else if (strcmp(keyWord,conDesginKeyWordTagStrUnion) == 0)
		return(conVarTypeTagUnion);
	else if (strcmp(keyWord,conDesginKeyWordTagStrArray) == 0)
		return(conVarTypeTagArray);
	else if (strcmp(keyWord,conDesginKeyWordTagStrPointer) == 0)
		return(conVarTypeTagPointer);
	else if (strcmp(keyWord,conDesginKeyWordTagStrVoid) == 0)
		return(conVarTypeTagVoid);
	else if (strcmp(keyWord,conDesginKeyWordTagStrFun) == 0)
		return(conDesginKeyWordTagFun);
	else if (strcmp(keyWord,conDesginKeyWordTagStrConst) == 0)
		return(conDesginKeyWordTagConst);
	else if (strcmp(keyWord,conDesginKeyWordTagStrGlobalVar) == 0)
		return(conDesginKeyWordTagGlobalVar);
	else return(conVarTypeTagVoid);
}

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefToFp(PUnionVarTypeDef pdef,FILE *fp)
{
	FILE	*outFp;
	char	keyWord[100+1];
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	memset(keyWord,0,sizeof(keyWord));
	UnionConvertTypeTagIntoCVarDefKeyWord(pdef->typeTag,keyWord);
	fprintf(outFp,"nameOfType=%s|typeTag=%s|nameOfProgram=%s|\n",pdef->nameOfType,keyWord,pdef->nameOfProgram);
	return(0);
}

/* ��ָ�����ͱ�ʶ�Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefToFpInDefFormat(PUnionVarTypeDef pdef,FILE *fp)
{
	return(UnionPrintVarTypeDefToFp(pdef,fp));
}

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefInFileToFp(char *fileName,char *nameOfType,FILE *fp)
{
	TUnionVarTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarTypeDefFromSpecFile(fileName,nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarTypeDefInFileToFp:: UnionReadVarTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintVarTypeDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
	nameOfType	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputVarTypeDefInFile(char *nameOfType,char *fileName)
{
	return(UnionPrintVarTypeDefInFileToFp(fileName,nameOfType,stdout));

}

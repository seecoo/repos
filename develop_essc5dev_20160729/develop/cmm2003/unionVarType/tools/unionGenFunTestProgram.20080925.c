//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionFunDef.h"
#include "unionVarValue.h"
#include "unionErrCode.h"
#include "unionGenFunTestProgram.h"
#include "unionGenFunListTestProgram.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionFunDefFile.h"
#include "unionTestConf.h"

#ifndef conDefaultArraySize
#define	conDefaultArraySize	3
#endif

char gunionConfFileNameOfFixedCodes[128] = "";
char gunionCurrentVarValueLen[128] = "";
int gunionVarIsBinary = 0;

/*
����	
	���õ�ǰ���õı����Ƿ��Ƕ�����
�������
	isBinary	�����Ƿ��Ƕ����Ƶı�־
�������
	��
����ֵ
	��
*/
void UnionSetCurrentVarValueAsBinary(int isBinary)
{
	gunionVarIsBinary = isBinary;
	return;
}

/*
����	
	���õ�ǰ���õı����ĳ���
�������
	varValueLen	�����ĳ���
�������
	��
����ֵ
	��
*/
void UnionSetCurrentVarValueLenName(char *varValueLen)
{
	strcpy(gunionCurrentVarValueLen,varValueLen);
	return;
}
/*
����	
	���ù̶�����Ķ����ļ�
�������
	fileName	����̶�����Ķ����ļ�
�������
	��
����ֵ
	>=0		��ȷ
	<0		������
*/
int UnionSetFileNameOfFixedCodesConfFile(char *filename)
{
	strcpy(gunionConfFileNameOfFixedCodes,filename);
	return(0);
}

/*
����	
	��ȡ�̶�����Ķ����ļ�
�������
	��
�������
	fileName	����̶�����Ķ����ļ�
����ֵ
	>=0		��ȷ
	<0		������
*/
int UnionFileNameOfFixedCodesConfFile(char *filename)
{
	if (strlen(gunionConfFileNameOfFixedCodes) == 0)
		return(0);
	sprintf(filename,"%s/conf/%s.def",getenv("DIROFTEST"),gunionConfFileNameOfFixedCodes);
	return(strlen(filename));
}

/*
����	
	д�����������Ϣ
�������
	fp		���Գ�����
�������
	��
����ֵ
	��
*/
void UnionAddAutoGenerateInfoToTestProgram(FILE *fp)
{
	char		systemDateTime[14+1];
	FILE		*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"// This is a group of automatically created c program-language codes by Union Tech Development Platform.\n");
	memset(systemDateTime,0,sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);
	fprintf(outFp,"// Generated Time:: %s\n\n",systemDateTime);
	return;
}

/*
����	
	���̶��Ĳ��Դ���д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	���������
*/
int UnionAddFixedCodesToFile(FILE *fp)
{
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	FILE		*outFp = stdout;
	
	if ((UnionFileNameOfFixedCodesConfFile(fileName)) <= 0)
		return(0);

	if (fp != NULL)
		outFp = fp;
		
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionAddFixedCodesToFile:: no specical include files defined in [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"\n\n");
	fprintf(outFp,"// this is fixed codes testing users defined!\n");
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddFixedCodesToFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		if (lineLen <= 0)
			continue;
		fprintf(outFp,"%s\n",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}

/*
����	
	��ͷ�ļ�д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	ͷ�ļ�������
*/
int UnionAddIncludeFileToTestProgram(FILE *fp)
{
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	
	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"#include <stdlib.h>\n");
	fprintf(fp,"\n");
	includeNum = 3;
	
	UnionGetIncludeFileDefFileName(fileName);
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionAddIncludeFileToTestProgram:: no specical include files defined!\n");
		return(includeNum);
	}
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddIncludeFileToTestProgram:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		if (lineLen <= 0)
			continue;
		fprintf(fp,"#include \"%s\"\n",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�Ĵ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	return(0);
}

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�ĺ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionMallocForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	/*
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	*/
	UnionGenerateMallocCodesForPointerFunVarToFp(pdef,fp);
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
����	
	����һ��Ϊָ���ͺ��������ͷſռ�Ĵ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	return(0);
}

/*
����	
	����һ��Ϊָ���ͺ��������ͷſռ�ĺ���
�������
	pdef		��������ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionFreeForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	/*
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarToTestProgram:: UnionPrintValueTagListToFile!\n");
			return(ret);
		}
	}
	*/
	UnionGenerateFreeCodesForPointerFunVarToFp(pdef,fp);
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
����	
	����һ��Ϊ�������ɵ��ú���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCallFunOfFunToTestProgram(char *funName,FILE *fp)
{
	return(UnionGenerateCallFunOfFunGrpToTestProgram(-1,funName,fp));
	/*
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionCallFunOfFun()\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        ret = %s(",pdef->funName);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",pdef->varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	return(0);
	*/
}

/*
����	
	����һ����ȡ����ֵ����������ļ�
�������
	pdef		��������ָ��
	fileName	�����ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarValueTagListIndexFile(PUnionFunDef pdef)
{
	int	index;
	FILE	*inputFp = stdout,*outputFp = stdout,*returnFp = stdout;
	char	tmpBuf[1024+1];
	int	ret;
	FILE	*fp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	
	UnionGetFunVarInputValueListIndexFileName(pdef->funName,tmpBuf);
	if ((inputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarOutputValueListIndexFileName(pdef->funName,tmpBuf);
	if ((outputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarReturnValueListIndexFileName(pdef->funName,tmpBuf);
	if ((returnFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFile:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		fclose(outputFp);
		return(errCodeUseOSErrCode);
	}
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFilePrintToFile);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (pdef->varGrp[index].isOutput)	// �������
			fp = outputFp;
		else
			fp = inputFp;
		if ((ret = UnionPrintValueTagListToFile("",&(pdef->varGrp[index].varDef),fp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFile:: UnionPrintValueTagListToFile!\n");
			goto errExit;
		}
	}
	if ((ret = UnionPrintValueTagListToFile("ret",&(pdef->returnType),returnFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFile:: UnionPrintValueTagListToFile!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	fclose(inputFp);
	fclose(outputFp);
	fclose(returnFp);
	return(ret);
}

/*
����	
	���������弰Ϊ������������ı�������д�뵽���Գ�����
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddFunDefAndFunVarListToTestProgram(char *funName,FILE *fp)
{
	int		ret;
	TUnionFunDef	funDef;
	
	// ����������
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}
	// ����������д�뵽������
	if ((ret = UnionPrintFunDefToFp(&funDef,1,1,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionPrintFunDefToFp [%s]!\n",funName);
		return(ret);
	}
	// ���������ȱʡ��С
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// ���庯������
	if ((ret = UnionPrintFunVarDefToFpOfCPragram(&funDef,"ret",conDefaultArraySize,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionPrintFunVarDefToFpOfCPragram [%s]!\n",funName);
		return(ret);
	}
	// дһ��Ϊָ���ͺ���������ʼ���ռ�ĺ���
	if ((ret = UnionGenerateMallocFunForPointerFunVarToTestProgram(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateMallocFunForPointerFunVarToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// дһ���ͷ�ָ���ͺ��������ռ�ĺ���
	if ((ret = UnionGenerateFreeFunForPointerFunVarToTestProgram(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateFreeFunForPointerFunVarToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// дһ���������ɲ���������������ļ�
	if ((ret = UnionGenerateFunVarValueTagListIndexFile(&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListToTestProgram:: UnionGenerateFunVarValueTagListIndexFile [%s]!\n",funName);
		return(ret);
	}
	return(0);
}

/*
����	
	����һ�����ַ������ͽ��и�ֵ�����
�������
	localVarPrefix	����ǰ׺
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	varValueLenTag	����ֵ�ĳ���
	isBinary	����ֵ�Ƿ��Ƕ�����,1,��,0,��
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateStrAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp)
{
	FILE	*outFp = stdout;
	char	varName[128+1];
	
	if (fp != NULL)
		outFp = fp;
	
	sprintf(varName,"%s%s",localVarPrefix,varValueTag);
	fprintf(outFp,"        printf(\"%s = [%s]\\n\");\n",varName,value);
	
	if (isBinary)	// �Ƕ�����,�Զ����Ʒ�ʽ��ֵ
		goto copyAsBinary;
		
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))	// û��Ϊ�������ó���
	{
		if (valueIsVarName)
			fprintf(outFp,"        strcpy(%s,%s);\n",varName,value);
		else
			fprintf(outFp,"        strcpy(%s,\"%s\");\n",varName,value);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        memcpy(%s,%s,%s);\n",varName,value,varValueLenTag);
		else
			fprintf(outFp,"        memcpy(%s,\"%s\",%s);\n",varName,value,varValueLenTag);
	}
	return(0);

copyAsBinary:
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,strlen(%s),%s);",value,value,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",strlen(\"%s\"),%s);",value,value,varName);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,%s,%s);",value,varValueLenTag,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",%s,%s);",value,varValueLenTag,varName);
	}
	
	return(0);
}

/*
����	
	����һ����ָ�����ͽ��и�ֵ�����
�������
	localVarPrefix	����ǰ׺
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	varValueLenTag	����ֵ�ĳ���
	isBinary	����ֵ�Ƿ��Ƕ�����,1,��,0,��
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGeneratePointerAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp)
{
	FILE	*outFp = stdout;
	char	varName[128+1];
	char	lenFlag[128+1];
	
	if (fp != NULL)
		outFp = fp;
	
	sprintf(varName,"%s%s",localVarPrefix,varValueTag);
	fprintf(outFp,"        printf(\"%s = [%s]\\n\");\n",varName,value);
	
	if (strcmp(value,"NULL") == 0)	// ��ָ��
	{
		fprintf(outFp,"        %s = NULL;\n",varName);
		return(0);
	}
			
	if (isBinary)	// �Ƕ�����,�Զ����Ʒ�ʽ��ֵ
		goto copyAsBinary;
		
	if (valueIsVarName)
	{
		if (strlen(varValueLenTag) == 0)
		{
			if (value[0] != '&')
				sprintf(lenFlag,"sizeof(*%s)",value);
			else
				sprintf(lenFlag,"sizeof(%s)",value+1);
		}
		else
			strcpy(lenFlag,varValueLenTag);
		fprintf(outFp,"        memcpy(%s,%s,%s);\n",varName,value,lenFlag);
	}
	else
		fprintf(outFp,"        memcpy(%s,\"%s\",strlen(\"%s\"));\n",varName,value,value);
	return(0);
copyAsBinary:
	if ((varValueLenTag == NULL) || (strlen(varValueLenTag) == 0))
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,strlen(%s),(char *)%s);",value,value,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",strlen(\"%s\"),(char *)%s);",value,value,varName);
	}
	else
	{
		if (valueIsVarName)
			fprintf(outFp,"        aschex_to_bcdhex(%s,%s,(char *)%s);",value,varValueLenTag,varName);
		else
			fprintf(outFp,"        aschex_to_bcdhex(\"%s\",%s,(char *)%s);",value,varValueLenTag,varName);
	}
	return(0);
}

/*
����	
	����һ���Ի����������ͽ��и�ֵ�����
�������
	varPrefix	����ǰ׺
	baseType	��������
	varValueTag	��������
	valueIsVarName	ֵ�ǲ��Ǳ�������
	value		����ֵ
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAssignmentCSentenceToFile(char *varPrefix,int baseType,char *varValueTag,int valueIsVarName,char *value,FILE *fp)
{
	FILE *outFp = stdout;
	char	localVarPrefix[128+1];
	
	if ((varValueTag == NULL) || (value == NULL))
	{
		UnionUserErrLog("in UnionGenerateFunInputVarAssignmentFun:: null pointer!\n");
		return(errCodeParameter);
	}
	memset(localVarPrefix,0,sizeof(localVarPrefix));
	if (varPrefix != NULL)	
		strcpy(localVarPrefix,varPrefix);
	if (fp != NULL)
		outFp = fp;
	switch (baseType)
	{
		case	conVarTypeTagChar:	// �ַ���
		case	conVarTypeTagUnsignedChar:	// �ַ���
			if (varValueTag[0] == '*')	// ���ַ���
			{
				UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag+1,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			}
			else
			{
				if (strstr(varValueTag,"[") == NULL)	// ���ַ�
				{
					if (valueIsVarName)
						fprintf(outFp,"        %s%s = %s;\n",localVarPrefix,varValueTag,value);
					else
						fprintf(outFp,"        %s%s = '%c';\n",localVarPrefix,varValueTag,value[0]);
					fprintf(outFp,"        printf(\"%s%s = [%c]\\n\");\n",localVarPrefix,varValueTag,value[0]);
				}
				else
				{
					UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
				}
			}
			break;
		case	conVarTypeTagString:	// �ַ���
		case	conVarTypeTagUnsignedString:	// �ַ���
			UnionGenerateStrAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			break;
		case	conVarTypeTagPointer:	// ָ��
			UnionGeneratePointerAssignmentCSentenceToFile(localVarPrefix,varValueTag,valueIsVarName,value,gunionCurrentVarValueLen,gunionVarIsBinary,outFp);
			/*
			if (valueIsVarName)
			{
				if (strlen(gunionCurrentVarValueLen) == 0)
				{
					if (value[0] != '&')
						sprintf(lenFlag,"sizeof(*%s)",value);
					else
						sprintf(lenFlag,"sizeof(%s)",value+1);
				}
				else
					strcpy(lenFlag,gunionCurrentVarValueLen);
				fprintf(outFp,"        memcpy(%s%s,%s,%s);\n",localVarPrefix,varValueTag,value,lenFlag);
			}
			else
				fprintf(outFp,"        memcpy(%s%s,\"%s\",strlen(\"%s\"));\n",localVarPrefix,varValueTag,value,value);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			*/
			break;		
		case	conVarTypeTagArray:	// ����
			fprintf(outFp,"        memcpy(%s%s,%s,sizeof(%s%s));\n",localVarPrefix,varValueTag,value,localVarPrefix,varValueTag);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			break;		
		default:
			if (UnionIsBaseTypeTag(baseType))
				fprintf(outFp,"        %s%s = %s;\n",localVarPrefix,varValueTag,value);
			else
				fprintf(outFp,"        memcpy(&(%s%s),&(%s),sizeof(%s%s));\n",localVarPrefix,varValueTag,value,localVarPrefix,varValueTag);
			fprintf(outFp,"        printf(\"%s%s = [%s]\\n\");\n",localVarPrefix,varValueTag,value);
			break;
	}
	return(0);
}

/*
����	
	����һ���Ժ���������������и�ֵ�ĺ���
�������
	funName		��������
	testDataFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunInputVarAssignmentFun(char *funName,char *testDataFileName,FILE *fp)
{
	return(UnionGenerateFunInputVarOfFunGrpAssignmentFun(-1,funName,testDataFileName,fp));
}

/*
����	
	����һ���Ի����������ͽ��д�ӡ�����
�������
	varPrefix	����ǰ׺
	baseType	��������
	varValueTag	��������
	value		����ֵ
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintOutputVarCSentenceToFp(char *varPrefix,int baseTypeTag,char *varValueTag,FILE *fp)
{
	char	typeTag[48+1];
	int	valueTagOffset = 0;
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
		
	switch (baseTypeTag)
	{
		case	conVarTypeTagUnsignedInt:
		case	conVarTypeTagInt:
		case	conVarTypeTagEnum:
			strcpy(typeTag,"%d");
			break;		
		case	conVarTypeTagLong:
		case	conVarTypeTagUnsignedLong:
			strcpy(typeTag,"%ld");
			break;		
		case	conVarTypeTagDouble:
			strcpy(typeTag,"%lf");
			break;		
		case	conVarTypeTagFloat:
			strcpy(typeTag,"%f");
			break;		
		case	conVarTypeTagString:
			strcpy(typeTag,"%s");
			break;		
		case	conVarTypeTagUnsignedString:
			strcpy(typeTag,"%s");
			break;		
		case	conVarTypeTagChar:
		case	conVarTypeTagUnsignedChar:
			if (varValueTag[0] == '*')	// ���ַ���
			{
				strcpy(typeTag,"%s");
				valueTagOffset = 1;
			}
			else
			{
				if (strstr(varValueTag,"[") == NULL)	// ���ַ�
					strcpy(typeTag,"%c");
				else
					strcpy(typeTag,"%s");
			}
			break;
		default:
			UnionUserErrLog("in UnionPrintOutputVarCSentenceToFp:: invalid baseTypeTag = [%d] for [%s]\n",baseTypeTag,varValueTag);
			return(errCodeCDPMDL_InvalidVarType);
	}
	fprintf(outFp,"        printf(\"%s%s=%s\\n\",%s%s);\n",varPrefix,varValueTag,typeTag,varPrefix,varValueTag+valueTagOffset);
	return(0);
}

/*
����	
	����һ���Ժ����������������չʾ�ĺ���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunOutputVarDisplayFun(char *funName,FILE *fp)
{
	return(UnionGenerateFunOutputVarOfFunGrpDisplayFun(-1,funName,fp));
}
		
/*
����	
	����һ���Ժ����ķ��ز�������չʾ�ĺ���
�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunReturnVarDisplayFun(char *funName,FILE *fp)
{
	return(UnionGenerateFunReturnVarOfFunGrpDisplayFun(-1,funName,fp));
}

/*
����	
	Ϊһ��������
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMainFunToTestProgram(FILE *fp)
{
	return(UnionGenerateMainFunOfFunGrpToTestProgram(NULL,0,fp));
}

/*
����	
	Ϊһ����������һ�����Գ���
�������
	funName		��������
	fileName	�ļ�����
	testDataFileName	���������ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateTestProgramForFun(char *funName,char *testDataFileName,char *fileName)
{
	int		ret;
	FILE		*fp = stdout;

	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionGenerateTestProgramForFun:: fopen [%s] for writting error!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	
	// д�������������
	UnionAddAutoGenerateInfoToTestProgram(fp);
	// д������ͷ�ļ���Ϣ
	if ((ret = UnionAddIncludeFileToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionAddIncludeFileToTestProgram!\n");
		goto errExit;
	}
	// д������Ϣ
	if ((ret = UnionAddFunDefAndFunVarListToTestProgram(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionAddFunDefAndFunVarListToTestProgram!\n");
		goto errExit;
	}
	// д�Ժ����������������ʼ���ĺ���
	if ((ret = UnionGenerateFunInputVarAssignmentFun(funName,testDataFileName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunInputVarAssignmentFun of [%s] from [%s]!\n",funName,testDataFileName);
		goto errExit;
	}
	// д�Ժ��������������չʾ�ĺ���
	if ((ret = UnionGenerateFunOutputVarDisplayFun(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunOutputVarDisplayFun!\n");
		goto errExit;
	}
	// д�Ժ������з��ز���չʾ�ĺ���
	if ((ret = UnionGenerateFunReturnVarDisplayFun(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateFunReturnVarDisplayFun!\n");
		goto errExit;
	}
	// дһ�����ú����ĺ���
	if ((ret = UnionGenerateCallFunOfFunToTestProgram(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateCallFunOfFunToTestProgram [%s]!\n",funName);
		return(ret);
	}
	// д������
	if ((ret = UnionGenerateMainFunToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTestProgramForFun:: UnionGenerateMainFunToTestProgram!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}


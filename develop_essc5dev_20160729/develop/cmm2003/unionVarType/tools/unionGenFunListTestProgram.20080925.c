//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionFunDef.h"
#include "unionVarValue.h"
#include "unionErrCode.h"
#include "unionGenFunTestProgram.h"
#include "unionGenFunListTestProgram.h"
#include "unionVarAssignment.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionTestConf.h"
#include "unionFunDefFile.h"

#ifndef conDefaultArraySize
#define	conDefaultArraySize	3
#endif

/*
����
	����һ�����ú����ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateCallFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex < 0)
		fprintf(outFp,"UnionCallFunOfFun()");
	else
		fprintf(outFp,"UnionCallFunOfFun%02d_%s()",funIndex,funName);
}

/*
����
	����һ�������������ֵ�ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateInputVarSetFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionSetFunInputVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionSetFunInputVar()");
}

/*
����
	����һ����ʾ��������ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateOutputVarDisplayFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionDisplayOutputFunVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionDisplayOutputFunVar()");
}

/*
����
	����һ����ʾ����ֵ�ĺ�����
�������
	funIndex	�������
	funName		��������
	outFp		�ļ�����
�������
	��
����ֵ
	��
*/
void UnionGenerateReturnVarDisplayFunName(int funIndex,char *funName,FILE *outFp)
{
	if (funIndex >= 0)
		fprintf(outFp,"UnionDisplayReturnFunVarOfFun%02d_%s()",funIndex,funName);
	else
		fprintf(outFp,"UnionDisplayReturnFunVar()");
}


/*
����	
	Ϊһ��������������ǰ׺
�������
	funIndex	�������
�������
	funVarPrefix	����ǰ׺
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarNamePrefixOfFunGrp(int funIndex,char *funVarPrefix)
{
	if (funIndex < 0)
		strcpy(funVarPrefix,"");
	else
		sprintf(funVarPrefix,"fun%02d_",funIndex);
	return(0);
}

/*
����	
	����һ��Ϊһ�麯����ָ���ͺ��������ͷſռ�ĺ���
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		index;
	FILE		*outFp = stdout;
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex;
	char		funVarPrefix[10];
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionFreeForPointerFunVar()\n");
	fprintf(outFp,"{\n");

	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile);
	for (funIndex = 0; funIndex < numOfFun; funIndex++)
	{
		memset(&funDef,0,sizeof(funDef));
		if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
			return(ret);
		}
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
		for (index = 0; index < funDef.varNum; index++)
		{
			if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),outFp)) < 0)
			{
				UnionUserErrLog("in UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram:: UnionPrintValueTagListToFile!\n");
				return(ret);
			}
		}
	}
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
����	
	����һ��Ϊָ���ͺ�����������ռ�ĺ���
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		index;
	FILE		*outFp = stdout;
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex;
	char		funVarPrefix[10];

	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void UnionMallocForPointerFunVar()\n");
	fprintf(outFp,"{\n");
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile);
	for (funIndex = 0; funIndex < numOfFun; funIndex++)
	{
		memset(&funDef,0,sizeof(funDef));
		if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
			return(ret);
		}
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
		for (index = 0; index < funDef.varNum; index++)
		{
			if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),outFp)) < 0)
			{
				UnionUserErrLog("in UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram:: UnionPrintValueTagListToFile!\n");
				return(ret);
			}
		}
	}
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
����	
	����һ����ȡ����ֵ����������ļ�
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunVarValueTagListIndexFileOfFunGrp(char funGrp[][128+1],int numOfFun)
{
	int		index;
	FILE		*inputFp = stdout,*outputFp = stdout,*returnFp = stdout;
	char		tmpBuf[1024+1];
	int		ret;
	FILE		*fp;
	TUnionFunDef	funDef;
	int		funIndex = 0;
	char		funVarPrefix[40];

loop:	
	if (funIndex >= numOfFun)
		return(0);

	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);	
	UnionGetFunVarInputValueListIndexFileName(funDef.funName,tmpBuf);
	if ((inputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarOutputValueListIndexFileName(funDef.funName,tmpBuf);
	if ((outputFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		return(errCodeUseOSErrCode);
	}
	UnionGetFunVarReturnValueListIndexFileName(funDef.funName,tmpBuf);
	if ((returnFp = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: fopen [%s]\n",tmpBuf);
		fclose(inputFp);
		fclose(outputFp);
		return(errCodeUseOSErrCode);
	}
	UnionSetTagOfPrintValueTagListToFile(conTagOfPrintValueTagListToFilePrintToFile);
	for (index = 0; index < funDef.varNum; index++)
	{
		if (funDef.varGrp[index].isOutput)	// �������
			fp = outputFp;
		else
			fp = inputFp;
		if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.varGrp[index].varDef),fp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionPrintValueTagListToFile!\n");
			goto errExit;
		}
	}
	sprintf(funVarPrefix+strlen(funVarPrefix),"ret");
	if ((ret = UnionPrintValueTagListToFile(funVarPrefix,&(funDef.returnType),returnFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFunVarValueTagListIndexFileOfFunGrp:: UnionPrintValueTagListToFile!\n");
		goto errExit;
	}
	ret = 0;
errExit:
	fclose(inputFp);
	fclose(outputFp);
	fclose(returnFp);
	if (ret < 0)
		return(ret);
	funIndex++;
	goto loop;
}

/*
����	
	���������弰Ϊ������������ı�������д�뵽���Գ�����
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	int		ret;
	TUnionFunDef	funDef;
	int		funIndex = 0;
	char		funVarPrefix[10+1];
	
loop:
	if (funIndex >= numOfFun)
		return(0);
	// ����������
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funGrp[funIndex],&funDef)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	// ����������д�뵽������
	if ((ret = UnionPrintFunDefToFp(&funDef,1,1,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionPrintFunDefToFp [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	// ���������ȱʡ��С
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// ���庯������
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,funVarPrefix);
	if ((ret = UnionPrintFunVarDefWithPrefixToFpOfCPragram(funVarPrefix,&funDef,"ret",conDefaultArraySize,fp)) < 0)
	{
		UnionUserErrLog("in UnionAddFunDefAndFunVarListOfFunGrpToTestProgram:: UnionPrintFunVarDefWithPrefixToFpOfCPragram [%s]!\n",funGrp[funIndex]);
		return(ret);
	}
	funIndex++;
	goto loop;
}


/*
����	
	����һ���Ժ���������������и�ֵ�ĺ���
�������
	funIndex	�����ں���ϵ���е�������
	funName		��������
	testDataFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunInputVarOfFunGrpAssignmentFun(int funIndex,char *funName,char *testDataFileName,FILE *fp)
{
	int			ret;
	FILE			*outFp=stdout,*valueFp;
	char			lineStr[2048+1];
	int			lineLen;
	//TUnionVarValueTagDef	def;
	char			fullTestDataFileName[256+1];
	int			beginSetValueOfThisFun = 0;
	char			flag[128+1];
	int			varTypeTag;
	char			varPrefix[128+1];
	char			*ptr;
	int			lineNum = 0;
	TUnionVarAssignmentDef	def;	
	int			thisFunIndex = 0;
	
	if (fp != NULL)
		outFp = fp;
	
	// �򿪲��������ļ�
	UnionGetFullFileNameOfTestData(testDataFileName,fullTestDataFileName);	
	if ((valueFp = fopen(fullTestDataFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: fopen [%s]!\n",fullTestDataFileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateInputVarSetFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"�������::\\n\");\n");
	sprintf(flag,"valueOf::%s",funName);
	while (!feof(valueFp))
	{
		// ��һ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(valueFp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen != errCodeFileEnd)
			{
				UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadOneDataLineFromTxtFile!\n");
				break;
			}
			continue;
		}
		lineNum++;
		if (lineLen == 0)
			continue;
		if (funIndex >= 0)
		{
			if (!beginSetValueOfThisFun)	// ��û�е��ú��������������ֵ���
			{
				if ((ptr = strstr(lineStr,flag)) != NULL)	// �������������������������ֵ���
				{
					if (strcmp(ptr+strlen("valueOf::"),funName) == 0)	// ����ͬ�������ĸ�ֵ���
					{
						if (thisFunIndex == funIndex)	// �ҵ���Ҫ��ֵ�ĺ���
						{
							beginSetValueOfThisFun = 1;
							continue;
						}
					}
				}
				// û���ҵ�����
				if (strstr(lineStr,"valueOf::") != NULL)	// ����һ������
					thisFunIndex++;
				continue;					
			}
			if (strstr(lineStr,"valueOf::") != NULL)	// ������һ�����������������ֵ���
			{
				if (beginSetValueOfThisFun)
					break;
				thisFunIndex++;
				continue;
			}		
		}
		// �Ǳ������Ĳ����������::
		// ��������������ֵ����
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarAssignmentDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionAuditLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadVarAssignmentDefFromStr [%s]!\n",lineStr);
			//return(ret);
			continue;
		}
		// ��ñ�����������
		if ((varTypeTag = UnionReadTypeTagOfSpecVarFldOfSpecFun(funName,def.varName)) < 0)
		{
			UnionAuditLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionReadTypeTagOfSpecVarFldOfSpecFun var [%s] of fun [%s] in [%s]!\n",def.varName,funName,lineStr);
			//return(varTypeTag);
			continue;
		}
		UnionSetCurrentVarValueLenName(def.lenTag);
		UnionSetCurrentVarValueAsBinary(def.isBinary);
		// д�Ա�����ֵ�����
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
		//printf("xxxxxxx [%s][%d][%s][%d][%s] in lineIndex[%04d][%s]\n",varPrefix,varTypeTag,def.varName,def.isVarName,def.value,lineNum,lineStr);
		if ((ret = UnionGenerateAssignmentCSentenceToFile(varPrefix,varTypeTag,def.varName,def.isVarName,def.value,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: UnionGenerateAssignmentCSentenceToFile var [%s] of fun [%s] in [%s]\n",
				def.varName,funName,lineStr);
			return(ret);
		}
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(valueFp);
	if (!beginSetValueOfThisFun)
	{
		if (funIndex < 0)
			return(0);
		UnionUserErrLog("in UnionGenerateFunInputVarOfFunGrpAssignmentFun:: fun [%s] of index [%d] not found!\n",funName,funIndex);
		return(errCodeParameter);
	}
	return(0);
}

/*
����	
	����һ���Ժ����������������չʾ�ĺ���
�������
	funIndex	���������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunOutputVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp)
{
	int			ret;
	char			fileName[256+1];
	FILE			*outFp=stdout,*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	int			valueTagOffset=0;
	char			*ptr;
	//char			varPrefix[40+1];
		
	if (fp != NULL)
		outFp = fp;
	
	UnionGetFunVarOutputValueListIndexFileName(funName,fileName);
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateOutputVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n�������::\\n\");\n");
	//UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	while (!feof(indexFp))
	{
		// ��һ��
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// ���ļ��ж�ȡȡֵ��ʶ
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		if (funIndex >= 0)
		{
			if ((ptr = strstr(def.varValueTag,"fun")) != NULL)
				valueTagOffset = abs(ptr - def.varValueTag);
			else
				valueTagOffset = 0;
			def.varValueTag[valueTagOffset+3] = funIndex % 100 / 10 + '0';
			def.varValueTag[valueTagOffset+4] = funIndex % 10 + '0';
		}
		//UnionLog("in UnionGenerateFunOutputVarOfFunGrpDisplayFun:: typeTag = [%d] valueTag = [%s]\n",def.baseTypeTag,def.varValueTag);
		//UnionPrintOutputVarCSentenceToFp(varPrefix,def.baseTypeTag,def.varValueTag,outFp);
		UnionPrintOutputVarCSentenceToFp("",def.baseTypeTag,def.varValueTag,outFp);
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(indexFp);
	return(0);
}

/*
����	
	����һ���Ժ����ķ��ز�������չʾ�ĺ���
�������
	funIndex	���������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateFunReturnVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp)
{
	int			ret;
	char			fileName[256+1];
	FILE			*outFp=stdout,*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	int			valueTagOffset=0;
	char			*ptr;
	//char			varPrefix[40+1];
	
	if (fp != NULL)
		outFp = fp;
	
	UnionGetFunVarReturnValueListIndexFileName(funName,fileName);
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(outFp,"void ");
	UnionGenerateReturnVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n���ز���::\\n\");\n");
	//UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	while (!feof(indexFp))
	{
		// ��һ��
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// ���ļ��ж�ȡȡֵ��ʶ
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFunReturnVarOfFunGrpDisplayFun:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		//UnionPrintOutputVarCSentenceToFp(varPrefix,def.baseTypeTag,def.varValueTag,outFp);
		if (funIndex >= 0)
		{
			if ((ptr = strstr(def.varValueTag,"fun")) != NULL)
				valueTagOffset = abs(ptr - def.varValueTag);
			else
				valueTagOffset = 0;
			def.varValueTag[valueTagOffset+3] = funIndex % 100 / 10 + '0';
			def.varValueTag[valueTagOffset+4] = funIndex % 10 + '0';
		}
		UnionPrintOutputVarCSentenceToFp("",def.baseTypeTag,def.varValueTag,outFp);
	}
	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	fclose(indexFp);
	return(0);
}

/*
����	
	����һ��Ϊ�������ɵ��ú����ĺ���
�������
	funIndex	�������
	funName		��������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCallFunOfFunGrpToTestProgram(int funIndex,char *funName,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	int	ret;
	TUnionFunDef	funDef;
	char	varPrefix[10+1];
	
	// ����������
	memset(&funDef,0,sizeof(funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCallFunOfFunGrpToTestProgram:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}
	
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"void ");
	UnionGenerateCallFunName(funIndex,funName,outFp);
	fprintf(outFp,"\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"        printf(\"\\n\\n***** ��ʼִ�е�%02d������%s *****\\n\");\n",funIndex,funName);
	fprintf(outFp,"        ");
	UnionGenerateInputVarSetFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
	fprintf(outFp,"        printf(\"\\n����ִ�й���...\\n\");\n");
	fprintf(outFp,"        %sret = %s(",varPrefix,funDef.funName);
	for (index = 0; index < funDef.varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s%s",varPrefix,funDef.varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	fprintf(outFp,"        ");
	UnionGenerateOutputVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	fprintf(outFp,"        ");
	UnionGenerateReturnVarDisplayFunName(funIndex,funName,outFp);
	fprintf(outFp,";\n");
	fprintf(outFp,"        printf(\"***** ����ִ�е�%02d������%s *****\\n\");\n",funIndex,funName);

	fprintf(outFp,"        return;\n");
	fprintf(outFp,"}\n\n");
	return(0);
}

/*
����	
	���ļ��ж�ȡһ��Ҫ���Եĺ�������
�������
	testFileName	���������ļ�����
	numOfFun	�����嵥�п��Էŵĺ����������Ŀ
�������
	funGrp		���������嵥
����ֵ
	>=0		�����ĺ�������Ŀ
	<0		�������
*/
int UnionGetFunGrpListFromFile(char *testDataFileName,char funGrp[][128+1],int numOfFun)
{
	FILE			*valueFp;
	char			lineStr[2048+1];
	int			lineLen;
	char			fullTestDataFileName[256+1];
	int			realFunNum = 0;
	
	// �򿪲��������ļ�
	UnionGetFullFileNameOfTestData(testDataFileName,fullTestDataFileName);	
	if ((valueFp = fopen(fullTestDataFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGetFunGrpListFromFile:: fopen [%s]!\n",fullTestDataFileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(valueFp))
	{
		// ��һ��
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(valueFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionGetFunGrpListFromFile:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		if (strstr(lineStr,"valueOf::") == NULL)
			continue;	// ���Ǻ����Ķ���
		if (realFunNum >= numOfFun)
		{
			UnionAuditLog("in UnionGetFunGrpListFromFile:: sizeOfFunGrp [%d] is too small, and the left fun not read!\n",numOfFun);
			fclose(valueFp);
			return(realFunNum);
		}
		strcpy(funGrp[realFunNum],lineStr+strlen("valueOf::"));
		realFunNum++;
	}
	fclose(valueFp);
	return(realFunNum);
}

/*
����	
	����һ��������
�������
	funGrp		���������嵥
	numOfFun	�����嵥�еĺ����ĸ���
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateMainFunOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
		
	fprintf(outFp,"void main()\n");
	fprintf(outFp,"{\n");
	fprintf(outFp,"         // ���ó�ʼ�������������\n");
	//fprintf(outFp,"         #ifdef _initGlobal_\n");
	//fprintf(outFp,"         UnionInitGlobalVar();\n");
	//fprintf(outFp,"         #endif _initGlobal_\n");	
	fprintf(outFp,"         UnionMallocForPointerFunVar();\n");
	fprintf(outFp,"         // ���ú���\n");
	if (numOfFun < 1)
	{
		fprintf(outFp,"         ");
		UnionGenerateCallFunName(-1,"",outFp);
		fprintf(outFp,";\n");
	}
	else
	{
		for (index = 0; index < numOfFun; index++)
		{
			fprintf(outFp,"         ");
			UnionGenerateCallFunName(index,funGrp[index],outFp);
			fprintf(outFp,";\n");
		}
	}
	fprintf(outFp,"         // �����ͷ������������\n");
	fprintf(outFp,"         UnionFreeForPointerFunVar();\n");

	fprintf(outFp,"         return;\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	Ϊһ�麯���Զ�����һ�鴦����
�������
	testFileName	���������ļ�����
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAutoGenerateFunForFunGrp(char *testFileName,FILE *fp)
{
	int		ret;
	int		index;
	char 		funGrp[128][128+1];
	int 		numOfFun;
	
	// д�������������
	UnionAddAutoGenerateInfoToTestProgram(fp);

	// д������ͷ�ļ���Ϣ
	if ((ret = UnionAddIncludeFileToTestProgram(fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddIncludeFileToTestProgram!\n");
		return(ret);
	}
	// д�̶������Ĵ���
	if ((ret = UnionAddFixedCodesToFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddFixedCodesToFile!\n");
		return(ret);
	}
	// ���������嵥
	if ((numOfFun = UnionGetFunGrpListFromFile(testFileName,funGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGetFunGrpListFromFile from file [%s]\n",testFileName);
		return(numOfFun);
	}
	if (numOfFun == 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: no fun set in file [%s]\n",testFileName);
		return(errCodeTestFileContentError);
	}
	
	// ���������ȱʡ��С
	UnionSetDefaultArraySizeWhenNoSizeSet(conDefaultArraySize);
	// ���庯������
	if ((ret = UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionAddFunDefAndFunVarListOfFunGrpToTestProgram\n");
		return(ret);
	}
	// дһ��Ϊָ���ͺ���������ʼ���ռ�ĺ���
	if ((ret = UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram\n");
		return(ret);
	}
	// дһ���ͷ�ָ���ͺ��������ռ�ĺ���
	if ((ret = UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram\n");
		return(ret);
	}
	// дһ���������ɲ���������������ļ�
	if ((ret = UnionGenerateFunVarValueTagListIndexFileOfFunGrp(funGrp,numOfFun)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunVarValueTagListIndexFileOfFunGrp\n");
		return(ret);
	}
	for (index = 0; index < numOfFun; index++)
	{
		// ����һ���Ժ����������������չʾ�ĺ���
		if ((ret = UnionGenerateFunInputVarOfFunGrpAssignmentFun(index,funGrp[index],testFileName,fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunInputVarOfFunGrpAssignmentFun\n");
			return(ret);
		}
		// ����һ���Ժ����������������չʾ�ĺ���
		if ((ret = UnionGenerateFunOutputVarOfFunGrpDisplayFun(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunOutputVarOfFunGrpDisplayFun\n");
			return(ret);
		}
		// ����һ���Ժ����ķ��ز�������չʾ�ĺ���
		if ((ret = UnionGenerateFunReturnVarOfFunGrpDisplayFun(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateFunReturnVarOfFunGrpDisplayFun\n");
			return(ret);
		}
		// ����һ��Ϊ�������ɵ��ú����ĺ���
		if ((ret = UnionGenerateCallFunOfFunGrpToTestProgram(index,funGrp[index],fp)) < 0)
		{
			UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateCallFunOfFunGrpToTestProgram\n");
			return(ret);
		}
	}
	// дһ��������
	if ((ret = UnionGenerateMainFunOfFunGrpToTestProgram(funGrp,numOfFun,fp)) < 0)
	{
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrp:: UnionGenerateMainFunOfFunGrpToTestProgram\n");
		return(ret);
	}
	return(0);
}

/*
����	
	Ϊһ���������в���һ�����Գ���
�������
	cFileName	�ļ�����
	testDataFileName	���������ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAutoGenerateFunForFunGrpToSpecCFile(char *testDataFileName,char *cFileName)
{
	int		ret;
	FILE		*fp = stdout;
	
	if ((cFileName != NULL) && (strlen(cFileName) != 0) && (strcmp(cFileName,"null") != 0))
	{
		if ((fp = fopen(cFileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionAutoGenerateFunForFunGrpToSpecCFile:: fopen [%s] for writting error!\n",cFileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionAutoGenerateFunForFunGrp(testDataFileName,fp)) < 0)
		UnionUserErrLog("in UnionAutoGenerateFunForFunGrpToSpecCFile:: UnionAutoGenerateFunForFunGrp from [%s] to [%s]\n",testDataFileName,cFileName);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
	

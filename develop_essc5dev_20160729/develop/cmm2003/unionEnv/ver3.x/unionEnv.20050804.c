// 2003/09/19, Wolfgang Wang
// Version3.0

// 2003/09/19,Wolfgang Wang��2.0����Ϊ1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "UnionEnv.h"
#include "UnionStr.h"
#define _UnionLogMDL_3_x_
#include "UnionLog.h"
#include "unionErrCode.h"

PUnionEnviVariable	punionEnviVarTBL[MAXUNIONENVIVARIABLES];
int			punionEnviVarNum = 0;

int UnionInitEnvi(char *fileName)
{
	int 	i;
	char	tmpBuf[512];
	FILE	*fp;
	
	punionEnviVarNum = 0;	
	for (i=0;i<MAXUNIONENVIVARIABLES;i++)
	{
		punionEnviVarTBL[i] = NULL;
	}
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	for (;;)
	{
		memset(tmpBuf,0,512);
		if (feof(fp))
			break;
		if (UnionReadOneFileLine(fp,tmpBuf) < 0)
			break;
		punionEnviVarTBL[punionEnviVarNum] = ReadUnionEnviVarFromStr(tmpBuf);
		if (punionEnviVarTBL[punionEnviVarNum] == NULL)
			continue;
		
		punionEnviVarNum++;
	}
		
	fclose(fp);
	
	//UnionPrintEnvi();
	
	return(0);
}

int UnionPrintEnvi()
{
	int	i,j;
	
	for (i = 0; i < punionEnviVarNum; i++)
	{
		if (punionEnviVarTBL[i] == NULL)
			continue;
		for (j = 0; j < conMaxNumOfValuesForAnEnvVar; j++)
		{
			if (punionEnviVarTBL[i]->pVariableValue[j] == NULL)
				continue;
			UnionNullLog("i = [%04d] j = [%04d] var = [%s]\n",i,j,punionEnviVarTBL[i]->pVariableValue[j]);
		}
	}
	UnionNullLog("in UnionInitEnv:: punionEnviVarNum = [%d]\n",punionEnviVarNum);
	return(0);
}

int UnionClearEnvi()
{
	int i;
	
	for (i=0;i<punionEnviVarNum;i++)
		UnionFreeEnviVar(punionEnviVarTBL[i]);
	return(0);
}

int UnionFreeEnviVar(PUnionEnviVariable pvar)
{
	int	i;
	
	if (pvar == NULL)
		return(0);
	for (i = 0; i < conMaxNumOfValuesForAnEnvVar; i++)
		if (pvar->pVariableValue[i])
		{
			free(pvar->pVariableValue[i]);
			pvar->pVariableValue[i] = NULL;
		}
	free(pvar);
	pvar = NULL;
	return(0);
}

int UnionGetEnviVarNum()
{
	return(punionEnviVarNum);
}

char *UnionGetEnviVarNameByIndex(int varIndex)
{
	if ((varIndex >= punionEnviVarNum) || (varIndex >= MAXUNIONENVIVARIABLES) || (varIndex < 0))
		return(NULL);
	
	if (punionEnviVarTBL[varIndex] == NULL)
		return(NULL);
		
	//return(punionEnviVarTBL[varIndex]->pVariableName);
	return(UnionGetEnviVarOfTheIndexByIndex(varIndex,0));
}
	
// ��ȡָ�����������ģ���N��ֵ��N=varValueIndex
char *UnionGetEnviVarOfTheIndexByIndex(int varIndex,int varValueIndex)
{
#ifndef _LargeEnvVar_
	if ((varIndex >= punionEnviVarNum) || (varIndex >= MAXUNIONENVIVARIABLES) || (varValueIndex >= 10) || (varIndex < 0) || (varValueIndex < 0))
	{
		UnionNullLog("in UnionGetEnviVarOfTheIndexByIndex, not define _LargeEnvVar_, max index is: 10.\n");
		return NULL;
	}
#else
	if ((varIndex >= punionEnviVarNum) || (varIndex >= MAXUNIONENVIVARIABLES) || (varValueIndex >= conMaxNumOfValuesForAnEnvVar) || (varIndex < 0) || (varValueIndex < 0))
	{
		UnionNullLog("in UnionGetEnviVarOfTheIndexByIndex, define _LargeEnvVar_, max index is: 100.\n");
		return NULL;
	}
#endif
	/*
	if ((varIndex >= punionEnviVarNum) || (varIndex >= MAXUNIONENVIVARIABLES) || (varValueIndex >= conMaxNumOfValuesForAnEnvVar) || (varIndex < 0) || (varValueIndex < 0))
		return(NULL);
	*/
		
	if (punionEnviVarTBL[varIndex] == NULL)
		return(NULL);
		
	return(punionEnviVarTBL[varIndex]->pVariableValue[varValueIndex]);
}
	
char *UnionGetEnviVarByIndex(int varIndex)
{
	return(UnionGetEnviVarOfTheIndexByIndex(varIndex,1));
}
	
// ��ȡָ�����Ʊ����ģ���N��ֵ��N=varIndex
char *UnionGetEnviVarOfTheIndexByName(char *varName,int varValueIndex)
{
	int i;
	
	if ((varName == NULL) || (varValueIndex >= conMaxNumOfValuesForAnEnvVar) || (varValueIndex < 0))
		return(NULL);
			
	//UnionToUpperCase(varName);
		
	for (i=0;i<punionEnviVarNum;i++)
	{
		if (punionEnviVarTBL[i] == NULL)
			continue;
		
		//if (punionEnviVarTBL[i]->pVariableName == NULL)
		if (punionEnviVarTBL[i]->pVariableValue[0] == NULL)
			continue;
	
		//if (strcmp(punionEnviVarTBL[i]->pVariableName,varName) != 0)
		if (strcmp(punionEnviVarTBL[i]->pVariableValue[0],varName) != 0)
			continue;

		return(punionEnviVarTBL[i]->pVariableValue[varValueIndex]);
	}
	
	return(NULL);
}
		
char *UnionGetEnviVarByName(char *varName)
{
	return(UnionGetEnviVarOfTheIndexByName(varName,1));
}
		
// ��ȡָ�����Ʊ�����Ӧ��������
int UnionGetVarIndexOfTheVarName(char *varName)
{
	int i;
	
	if (varName == NULL)
		return(errCodeParameter);
			
	for (i=0;i<punionEnviVarNum;i++)
	{
		if (punionEnviVarTBL[i] == NULL)
			continue;
		
		//if (punionEnviVarTBL[i]->pVariableName == NULL)
		if (punionEnviVarTBL[i]->pVariableValue[0] == NULL)
			continue;
	
		//if (strcmp(punionEnviVarTBL[i]->pVariableName,varName) != 0)
		if (strcmp(punionEnviVarTBL[i]->pVariableValue[0],varName) != 0)
			continue;

		return(i);
	}
	
	return(errCodeEnviMDL_VarNotExists);
}

// Modified by Wolfgang Wang, 2003/09/12
PUnionEnviVariable ReadUnionEnviVarFromStr(char *str)
{
	int 	i;
	int	j;
	int	varValueNum;
	char	varName		[512];
	char	varValue	[512];
	PUnionEnviVariable pvar;
	int	len;
	
	if (str == NULL)
		return(NULL);
		
	len = strlen(str);
	
	// 1.��ȡ��������
	// 1.1ȡ�����ĵ�һ���ַ�
	for (i=0;i<len;i++)
	{
		if (str[i] == '[')
			break;
		if (str[i] == '\0')
			return(NULL);
		if (str[i] == '#')
			return(NULL);
	}
	if (i == len)
		return(NULL);		
	// 1.2��ʼ��ȡ������	
	for (++i,j=0;i < len;i++,j++)
	{
		if (str[i] == '\0')
			return(NULL);
			
		if (str[i] == ']')
			break;
		
		varName[j] = str[i];
	}
	// 1.3��������ȡ�ɹ�
	if (i == len)
		return(NULL);
	varName[j] = '\0';

	if ((pvar = (PUnionEnviVariable)malloc(sizeof(TUnionEnviVariable))) == NULL)
	{
		UnionSystemErrLog("in ReadUnionEnviVarFromStr:: malloc PUnionEnviVariable!\n");
		return(NULL);
	}
	for (varValueNum = 0; varValueNum < conMaxNumOfValuesForAnEnvVar; varValueNum++)
		pvar->pVariableValue[varValueNum] = NULL;

	//if ((pvar->pVariableName = (char *)malloc(strlen(varName)+1)) == NULL)
	if ((pvar->pVariableValue[0] = (char *)malloc(strlen(varName)+1)) == NULL)
	{
		UnionSystemErrLog("in ReadUnionEnviVarFromStr:: malloc pvar->pVariableName!\n");
		free(pvar);
		return(NULL);
	}
	//UnionToUpperCase(varName);
	//strcpy(pvar->pVariableName,varName);
	strcpy(pvar->pVariableValue[0],varName);
	
	// 2.��ʼ��ȡ����ֵ
	for (varValueNum = 1; varValueNum < conMaxNumOfValuesForAnEnvVar; varValueNum++)
	{
		// 2.1 ��ȡ����ֵ�ĵ�һ����ĸ��λ��
		for (++i;i<len;i++)
		{
			if (str[i] == '[')
				break;	
			if (str[i] == '\0')
				return(pvar);
		}
		if (i == len)
			return(pvar);
	
		// 2.2 ��ȡ����ֵ
		for (++i,j=0;i<len;i++,j++)
		{
			if (str[i] == '\0')
				return(pvar);
			if (str[i] == ']')
				break;
			varValue[j] = str[i];
		}
		if (i == len)
			return(pvar);
		varValue[j] = '\0';
	
		if ((pvar->pVariableValue[varValueNum] = (char *)malloc(strlen(varValue)+1)) == NULL)
			return(pvar);
	
		strcpy(pvar->pVariableValue[varValueNum],varValue);
	}
	
	return(pvar);
}

int UnionReadOneFileLine(FILE *fp,char *tmpBuf)
{
	char *p;
	char c;
	int  num;
	
	p = tmpBuf;
	num = 0;
	while (!feof(fp))
	{
		fscanf(fp,"%c",&c);
		if (c == '\n')
		{
			*p = 0;
			return(num);
		}
		*p = c;
		p++;
		num++;
	}
	
	*p = 0;
	num--;
	if (num >= 0)
		tmpBuf[num] = 0;
	if (num == 0)
		return(errCodeEnviMDL_NullLine);
	else
		return(num);
}

int UnionIsThisVarDefinitionLine(char *line,char *varName)
{
	PUnionEnviVariable 	pvar;
	int			yes;
	
	if ((pvar = ReadUnionEnviVarFromStr(line)) == NULL)
		return(0);
	
	if (pvar->pVariableValue[0] == NULL)
		yes = 0;
	else
	{	
		if (strcmp(pvar->pVariableValue[0],varName) == 0)
			yes = 1;
		else
			yes = 0;
	}
	UnionFreeEnviVar(pvar);
	
	return(yes);
}

int UnionReadVarNameFromLine(char *line,char *varName)
{
	PUnionEnviVariable 	pvar;
	
	if ((pvar = ReadUnionEnviVarFromStr(line)) == NULL)
		return(errCodeEnviMDL_NullLine);
	
	if (pvar->pVariableValue[0] == NULL)
		varName[0] = 0;
	else
		strcpy(varName,pvar->pVariableValue[0]);
		
	UnionFreeEnviVar(pvar);
	
	return(0);
}

int UnionReadEnviVarValueOfName(char *fileName,char *varName,int index,char *value)
{
	FILE	*fp1 = NULL;
	char	tmpBuf[1024+1];
	char	varNameStr[128+1];
	char	*ptr;
	PUnionEnviVariable 	pvar;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)) || (index < 0) || (index >= 10))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInsertEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp1,tmpBuf) <= 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))	// ע����
			continue;
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// ���Ǳ���������
			continue;
		if (ptr != tmpBuf)			// ���Ǳ���������
			continue;
		if ((pvar = ReadUnionEnviVarFromStr(tmpBuf)) == NULL)
		{
			fclose(fp1);
			return(errCodeEnviMDL_NullLine);
		}
		if (pvar->pVariableValue[index] == NULL)
			return(errCodeEnviMDL_NullLine);
		else
			strcpy(value,pvar->pVariableValue[index]);
		UnionFreeEnviVar(pvar);
		fclose(fp1);
		return(strlen(value));
	}
	fclose(fp1);
	return(errCodeEnviMDL_VarNotExists);
}

char *UnionReadValueOfTheIndexOfEnviVar(PUnionEnviVariable pVar,int index)
{
	if ((pVar == NULL) || (index < 0) || (index > conMaxNumOfValuesForAnEnvVar))
		return(NULL);
	return(pVar->pVariableValue[index]);
}
// 2006/8/21 ���Ӻ���
int UnionIsRemarkFileLine(char *str)
{
	if ((str == NULL) || (strlen(str) == 0))
		return(1);
	UnionFilterLeftBlank(str);
	if ((str[0] == '#') || (strncmp(str,"//",2) == 0))
		return(1);
	else
		return(0);
}

int UnionUpdateEnviVar(char *fileName,char *varName,char *fmt,...)
{
	FILE	*fp1 = NULL;
	FILE	*fp2 = NULL;
	int	ret;
	char	tmpBuf[1024+1];
	int	varUpdated = 0;
	char	varNameStr[128+1];
	char	*ptr;
	va_list 	args;
	TUnionEnviRecFldLength	recFldLen;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionUpdateEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	sprintf(tmpBuf,"%s/%d.tmp",getenv("UNIONTEMP"),getpid());
	if ((fp2 = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionUpdateEnviVar:: fopen [%s]\n",tmpBuf);
		fclose(fp1);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	UnionInitRecFldLengthDef(recFldLen);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneFileLine(fp1,tmpBuf)) < 0)
			continue;
		if (ret == 0)
		{
			if (!feof(fp1))
				goto writeLine;
			else
				break;
		}
		if (varUpdated)				// �����Ѹ���
			goto writeLine;
		if (UnionIsRemarkFileLine(tmpBuf))	// ע����
			goto writeLine;
		if (UnionIsRecFormatDefLine(tmpBuf))
			UnionReadEnviRecFldLengthFromStr(tmpBuf,recFldLen);
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// ���Ǳ���������
			goto writeLine;
		if (ptr != tmpBuf)			// ���Ǳ���������
			goto writeLine;
		// �Ǳ���������
		varUpdated = 1;
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",varNameStr);
		va_start(args,fmt);
		vsprintf(tmpBuf+strlen(varNameStr),fmt,args);
		va_end(args);
		if (strlen(tmpBuf+strlen(varNameStr)) == 0)	// δ�������ֵ��ɾ���������
			continue;
		UnionConvertEnviRecStrIntoFormat(recFldLen,tmpBuf,sizeof(tmpBuf));
		fprintf(fp2,"%s\n",tmpBuf);
		continue;
writeLine:	// ���Ǳ��������е�д��
		fprintf(fp2,"%s\n",tmpBuf);
		continue;
	}
	fclose(fp1);
	fclose(fp2);
	if (varUpdated)	// ������������
	{		// �����ļ�
		sprintf(tmpBuf,"cp %s/%d.tmp %s",getenv("UNIONTEMP"),getpid(),fileName);
		system(tmpBuf);
	}
	// ɾ����ʱ�ļ�
	sprintf(tmpBuf,"rm %s/%d.tmp",getenv("UNIONTEMP"),getpid());
	system(tmpBuf);
	if (varUpdated)
		return(0);
	else
		return(errCodeEnviMDL_VarNotExists);
}

int UnionInsertEnviVarWithRemark(char *fileName,char *remark,char *varName,char *fmt,...)
{
	FILE			*fp1 = NULL;
	char			tmpBuf[1024+1];
	va_list 		args;
	TUnionEnviRecFldLength	recFldLen;
	int			len;
	
	if (UnionExistEnviVarInFile(fileName,varName) >= 0)
	{
		UnionUserErrLog("in UnionInsertEnviVar:: varName [%s] already exists in [%s]\n",varName,fileName);
		return(errCodeEnviMDL_VarAlreadyExists);
	}

	UnionReadEnviRecFldLengthDefFromFile(fileName,recFldLen);
		
	if ((fileName == NULL) || (varName == NULL))
		return(errCodeParameter);
	
	if ((remark != NULL) && (strlen(remark) > 0))
		fprintf(fp1,"#%s\n",remark);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"[%s]",varName);
	len = strlen(tmpBuf);
	va_start(args,fmt);
	vsprintf(tmpBuf+len,fmt,args);
	va_end(args);
	if (strlen(tmpBuf+len) == 0)
		return(errCodeEnviMDL_NoValueDefinedForVar);
	if ((fp1 = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	UnionConvertEnviRecStrIntoFormat(recFldLen,tmpBuf,sizeof(tmpBuf));
	fprintf(fp1,"%s\n",tmpBuf);
	fclose(fp1);
	return(0);
}

int UnionInsertEnviVar(char *fileName,char *varName,char *fmt,...)
{
	FILE			*fp1 = NULL;
	char			tmpBuf[1024+1];
	va_list 		args;
	TUnionEnviRecFldLength	recFldLen;
	int			len;
	
	if (UnionExistEnviVarInFile(fileName,varName) >= 0)
	{
		UnionUserErrLog("in UnionInsertEnviVar:: varName [%s] already exists in [%s]\n",varName,fileName);
		return(errCodeEnviMDL_VarAlreadyExists);
	}

	UnionReadEnviRecFldLengthDefFromFile(fileName,recFldLen);
		
	if ((fileName == NULL) || (varName == NULL))
		return(errCodeParameter);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"[%s]",varName);
	len = strlen(tmpBuf);
	va_start(args,fmt);
	vsprintf(tmpBuf+len,fmt,args);
	va_end(args);
	if (strlen(tmpBuf+len) == 0)
		return(errCodeEnviMDL_NoValueDefinedForVar);
	if ((fp1 = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	UnionConvertEnviRecStrIntoFormat(recFldLen,tmpBuf,sizeof(tmpBuf));
	fprintf(fp1,"%s\n",tmpBuf);
	fclose(fp1);
	return(0);
}

// 2008/5/22,���ļ��������һ��	
int UnionAddStrToFile(char *fileName,char *str)
{
	FILE			*fp1 = NULL;
	
	if ((fileName == NULL) || (str == NULL))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(fp1,"%s\n",str);
	fclose(fp1);
	return(0);
}

int UnionDeleteEnviVar(char *fileName,char *varName)
{
	int	ret;
	
	if ((ret = UnionUpdateEnviVar(fileName,varName,"")) < 0)
	{
		UnionUserErrLog("in UnionDeleteEnviVar:: UnionUpdateEnviVar [%s] [%s]\n",fileName,varName);
		return(ret);
	}
	return(ret);
}

int UnionExistEnviVarInFile(char *fileName,char *varName)
{
	FILE	*fp1 = NULL;
	char	tmpBuf[1024+1];
	char	varNameStr[128+1];
	char	*ptr;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInsertEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp1,tmpBuf) <= 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))	// ע����
			continue;
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// ���Ǳ���������
			continue;
		if (ptr != tmpBuf)			// ���Ǳ���������
			continue;
		fclose(fp1);
		return(1);
	}
	fclose(fp1);
	return(errCodeEnviMDL_VarNotExists);
}

int UnionIsRecFormatDefLine(char *str)
{
	char	*ptr;
	
	if (UnionIsRemarkFileLine(str))
		return(0);
	
	if ((ptr = strstr(str,"RECFORMAT=")) == NULL)
		return(0);
	else
		return(1);
}

int UnionReadEnviRecFldLengthFromStr(char *str,TUnionEnviRecFldLength fldLenDef)
{
	int	index;
	char	*varStart;
	char	*varEnd;
	int	strLen;

	UnionInitRecFldLengthDef(fldLenDef);
		
	if (!UnionIsRecFormatDefLine(str))
		return(errCodeEnviMDL_NotRecFormatDefStr);

	if ((varStart = strstr(str,"RECFORMAT=")) == NULL)
		return(errCodeEnviMDL_NotRecFormatDefStr);
	varStart += 10;
	strLen = strlen(str);
	for (index = 0;index < conMaxFldNumOfEnviRec;index++)
	{
		if ((varEnd = strstr(varStart,";")) == NULL)
			break;
		*varEnd = 0;		
		fldLenDef[index] = atoi(varStart);
		*varEnd = ';';
		if (strLen + str <= varEnd)
			break;
		varStart = varEnd + 1;
	}
	return(0);
}

int UnionConvertEnviRecStrIntoFormat(TUnionEnviRecFldLength fldLenDef,char *recStr,int sizeOfRecStr)
{
	int	index;
	char	tmpBuf[512+1];
	int	strLen;
	char	*varStart;
	char	*varEnd;
	int	fldLen;
	int	offset = 0;
	char	tmpChar;
	
	if ((strLen = strlen(recStr)) >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionConvertEnviRecStrIntoFormat:: recStr [%s] too long!\n",recStr);
		return(errCodeEnviMDL_NotRecStrTooLong);
	}
	strcpy(tmpBuf,recStr);
	varEnd = tmpBuf;
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
	{
		if ((varStart = strstr(varEnd,"[")) == NULL)
		{
			strcpy(recStr+offset,varEnd);
			offset += strlen(varEnd);
		}
		if ((varEnd = strstr(varStart,"]")) == NULL)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			break;
		}
		varEnd = varEnd+1;
		if (tmpBuf+strLen <= varEnd)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			break;
		}
		tmpChar = *varEnd;
		*varEnd = 0;
		fldLen = strlen(varStart);
		if (offset + fldLen >= sizeOfRecStr)
		{
			UnionUserErrLog("in UnionConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLen);
			return(errCodeSmallBuffer);
		}
		strcpy(recStr+offset,varStart);
		if (fldLen < fldLenDef[index])
		{
			if (offset + fldLenDef[index] >= sizeOfRecStr)
			{
				UnionUserErrLog("in UnionConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLenDef[index]);
				return(errCodeSmallBuffer);
			}
			memset(recStr+offset+fldLen,' ',fldLenDef[index]-fldLen);
			fldLen = fldLenDef[index];
		}
		offset += fldLen;
		*varEnd = tmpChar;
	}
	return(offset);
}
	
int UnionInitRecFldLengthDef(TUnionEnviRecFldLength fldLenDef)
{
	int	index;
	
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
		fldLenDef[index] = -1;
	return(0);
}

int UnionReadEnviRecFldLengthDefFromFile(char *fileName,TUnionEnviRecFldLength recFldLen)
{
	FILE			*fp1 = NULL;
	int			ret;
	char			tmpBuf[1024+1];
		
	UnionInitRecFldLengthDef(recFldLen);

	if (fileName == NULL)
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionUpdateEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneFileLine(fp1,tmpBuf)) < 0)
			continue;
		if (UnionIsRecFormatDefLine(tmpBuf))
		{
			UnionReadEnviRecFldLengthFromStr(tmpBuf,recFldLen);
			break;
		}
	}
	fclose(fp1);
	return(0);
}
int UnionReadEnviVarDefLineInFile(char *fileName,char *varName,char *varDefLine)
{
	FILE	*fp1 = NULL;
	char	tmpBuf[1024+1];
	char	varNameStr[128+1];
	char	*ptr;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)) || (varDefLine == NULL))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInsertEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp1,tmpBuf) <= 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))	// ע����
			continue;
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// ���Ǳ���������
			continue;
		fclose(fp1);
		strcpy(varDefLine,tmpBuf);
		return(0);
	}
	fclose(fp1);
	return(errCodeEnviMDL_VarNotExists);
}

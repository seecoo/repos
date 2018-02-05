//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDef.h"
#include "UnionStr.h"

/* 从指定文件读一行c语言的有效程序行
输入参数
	fp		文件句柄
	maxNumOfLine	可读出的最大行数
输出参数
	cprogramLine	程序行
	lineNum		读出的是文件的第几行
返回值：
	>=0 	读出的行数目，0表示文件结束了
	<0	出错代码	
	
*/
int UnionReadCProgramLineFromFile(FILE *fp,char cprogramLine[][1024+1],int maxNumOfLine,int *lineNum)
{
	char	lineStr[1024+1];
	char	tmpStr[1024+1];
	int	ret;
	int	lineLen;
	int	isRemarkLines = 0;
	char	*ptr;
	char	*cStr;
	int	lenOfCStr;
	int	cStrNum = 0;
	
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// 空行
				continue;
			UnionSystemErrLog("in UnionReadCProgramLineFromFile:: UnionReadOneLineFromTxtStr error! lineIndex = [%d]\n",*lineNum);
			return(lineLen);
		}
		*lineNum += 1;
		if (lineLen == 0)	// 空行
			continue;
		cStr = lineStr;
		lenOfCStr = lineLen;
loop:
		// 去掉多余空格和tab
		if ((lenOfCStr = UnionFilterRubbisBlankAndTab(cStr,lenOfCStr,tmpStr,sizeof(tmpStr))) <= 0)	// 空行
			continue;
		memcpy(cStr,tmpStr,lenOfCStr);
		cStr[lenOfCStr] = 0;
		if (strncmp(cStr,"//",2) == 0) // 注释行
			continue;
		if (isRemarkLines)	// 读到是注释标识以内的行
		{
			if ((ptr = strstr(cStr,"*/")) == NULL)	// 未遇到注释结束标识，还是注释内容
				continue;
			// 该行包括了注释结束标识
			isRemarkLines = 0;
			cStr = ptr + 2;
			lenOfCStr -= 2;
			goto loop;	// 判断注释结束标志后是否还有程序行
		}
		// 假设是程序行
		if ((ptr = strstr(cStr,"/*")) != NULL)	// 该行包括了注释开始标志
		{
			isRemarkLines = 1;
			*ptr = 0;
			if ((lenOfCStr = strlen(cStr)) <= 0) // 注释开始标志在行开始位置
				continue;
		}
		if ((ptr = strstr(cStr,"//")) != NULL)	// 该行包括了行注释
		{
			*ptr = 0;
			if ((lenOfCStr = strlen(cStr)) <= 0) // 该行为空
				continue;
		}		
		// 是程序行
		if (cStrNum >= maxNumOfLine)	// 已读出了足够多的行数
		{
			UnionUserErrLog("in UnionReadCProgramLineFromFile:: too many cPhase defined in line [%d]\n",*lineNum); 
			return(errCodeSmallBuffer);
		}
		strcpy(cprogramLine[cStrNum],cStr);
		cStrNum++;
		// 该程序行中不包括注释信息
		if (!isRemarkLines)
			return(cStrNum);
		// 该程序行中包括注释信息
		cStr = cStr + lenOfCStr + 2;
		lenOfCStr = strlen(cStr);
		goto loop;
	}
	return(0);
}

/* 去掉一个程序中的注释行
输入参数
	oriFileName	源文件名
	desFileName	目标文件名
输出参数
	无
返回值：
	>=0 	处理后的总行数
	<0	出错代码	
	
*/
int UnionFilterRemarkFromCProgramFile(char *oriFileName,char *desFileName)
{
	FILE	*inFp;
	int	oriLineNum = 0,desLineNum = 0;
	char	tmpBuf[10][1024+1];
	int	ret;
	int	i;
	FILE	*outFp = stdout;
	
	if ((desFileName != NULL) && (strlen(desFileName) != 0) && (strcmp(desFileName,"null") != 0))
	{
		if ((outFp = fopen(desFileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionFilterRemarkFromCProgramFile:: fopen [%s]\n",desFileName);
			return(errCodeUseOSErrCode);
		}
	}
		
	if ((inFp = fopen(oriFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionFilterRemarkFromCProgramFile:: fopen [%s]!\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		if ((ret = UnionReadCProgramLineFromFile(inFp,tmpBuf,10,&oriLineNum)) <= 0)
			break;
		desLineNum += ret;
		for (i = 0; i < ret; i++)
		{
			fprintf(outFp,"%s\n",tmpBuf[i]);
		}
	}
	fclose(inFp);
	fclose(outFp);
	return(ret);
}

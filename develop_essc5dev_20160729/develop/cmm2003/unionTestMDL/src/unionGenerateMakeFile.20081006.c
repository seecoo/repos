//	Wolfgang Wang
//	2008/10/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionTestConf.h"
#include "unionGenerateMakeFile.h"
#include "UnionLog.h"

/*
功能	
	将对头文件目录的定义增加到makefile中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddIncludeDirDefToMakeFile(FILE *fp)
{
	int		ret;
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	FILE		*outFp = stdout;

	UnionGetIncludeFileDirDefFileName(fileName);
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionAddIncludeFileToTestProgram:: no specical include dir defined!\n");
		return(includeNum);
	}
	if (fp != NULL)
		outFp = fp;
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
		fprintf(outFp,"-I %s ",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}

/*
功能	
	将库文件引用增加到makefile中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddLibDefToMakeFile(FILE *fp)
{
	int		ret;
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	FILE		*outFp = stdout;

	UnionGetLibFileDefFileName(fileName);
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionAddLibDefToMakeFile:: no specical include dir defined!\n");
		return(includeNum);
	}
	if (fp != NULL)
		outFp = fp;
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddLibDefToMakeFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		fprintf(outFp,"%s ",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}


/*
功能	
	为一个测试程序产生一个编译文件
输入参数
	cFileName		测试程序文件名称
	exeFileName		生成的可执行程序名称
	makeFileName		编译文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMakeFileForTestProgramForFun(char *cFileName,char *exeFileName,char *makeFileName)
{
	int	ret;
	FILE	*fp = stdout;
	char	tmpBuf[256+1];
	
	if ((makeFileName != NULL) && (strlen(makeFileName) != 0) && (strcmp(makeFileName,"null") != 0))
	{
		if ((fp = fopen(makeFileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionGenerateMakeFileForTestProgramForFun:: fopen [%s]\n",makeFileName);
			return(errCodeUseOSErrCode);
		}
	}
	// 特殊的编译开关
	fprintf(fp,"cc %s ",getenv("OSSPEC"));
	// 头文件路径
	if ((ret = UnionAddIncludeDirDefToMakeFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMakeFileForTestProgramForFun:: UnionAddIncludeDirDefToMakeFile!\n");
		return(ret);
	}
	// 可执行程序名称
	if ((exeFileName != NULL) && (strlen(exeFileName) != 0))
		fprintf(fp,"-o %s ",exeFileName);
	// 源程序名称
	if ((exeFileName != NULL) && (strlen(exeFileName) != 0))
		fprintf(fp,"%s ",cFileName);
	// 库文件
	if ((ret = UnionAddLibDefToMakeFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMakeFileForTestProgramForFun:: UnionAddLibDefToMakeFile!\n");
		return(ret);
	}
	// 特殊的库
	fprintf(fp,"%s\n",getenv("OSSPECLIBS"));
	if (fp != stdout)
	{
		fclose(fp);
		// 将makefile的权限改为可执行
		sprintf(tmpBuf,"chmod a+x %s",makeFileName);
		system(tmpBuf);
	}	
	return(0);
}

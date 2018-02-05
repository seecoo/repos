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
����	
	����ͷ�ļ�Ŀ¼�Ķ������ӵ�makefile��
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
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
����	
	�����ļ��������ӵ�makefile��
�������
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
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
����	
	Ϊһ�����Գ������һ�������ļ�
�������
	cFileName		���Գ����ļ�����
	exeFileName		���ɵĿ�ִ�г�������
	makeFileName		�����ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
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
	// ����ı��뿪��
	fprintf(fp,"cc %s ",getenv("OSSPEC"));
	// ͷ�ļ�·��
	if ((ret = UnionAddIncludeDirDefToMakeFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMakeFileForTestProgramForFun:: UnionAddIncludeDirDefToMakeFile!\n");
		return(ret);
	}
	// ��ִ�г�������
	if ((exeFileName != NULL) && (strlen(exeFileName) != 0))
		fprintf(fp,"-o %s ",exeFileName);
	// Դ��������
	if ((exeFileName != NULL) && (strlen(exeFileName) != 0))
		fprintf(fp,"%s ",cFileName);
	// ���ļ�
	if ((ret = UnionAddLibDefToMakeFile(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMakeFileForTestProgramForFun:: UnionAddLibDefToMakeFile!\n");
		return(ret);
	}
	// ����Ŀ�
	fprintf(fp,"%s\n",getenv("OSSPECLIBS"));
	if (fp != stdout)
	{
		fclose(fp);
		// ��makefile��Ȩ�޸�Ϊ��ִ��
		sprintf(tmpBuf,"chmod a+x %s",makeFileName);
		system(tmpBuf);
	}	
	return(0);
}

//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#include "unionFldGrp.h"

//---------------------------------------------------------------------------
/*
功能	从文件中读取查询域定义
输入参数
	fileName	文件名
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadQueryFldGrpDef(char *fileName,PUnionQueryFldGrp prec)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	FILE	*fp;

	if ((fileName == NULL) || (prec == NULL))
		return(errCodeParameter);

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadQueryFldGrpDef:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(prec,0,sizeof(prec));
	prec->fldNum = 0;
	while (!feof(fp))
	{
		if (prec->fldNum >= conMaxNumOfQueryFld)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"queryFldName",prec->fldName[prec->fldNum],sizeof(prec->fldName[prec->fldNum]))) <= 0)
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"fldAlais",prec->fldAlais[prec->fldNum],sizeof(prec->fldAlais[prec->fldNum]))) <= 0)
			strcpy(prec->fldAlais[prec->fldNum],prec->fldName[prec->fldNum]);
		prec->fldNum += 1;
	}
	fclose(fp);
	return(prec->fldNum);
}

/*
功能	向文件中写入查询域定义
输入参数
	prec		查询域定义
	fp		文件句柄
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintQueryFldGrpDefToFp(PUnionQueryFldGrp prec,FILE *fp)
{
	int	ret,len = 0;
	char	tmpBuf[2048+1];
	FILE	*outFp = stdout;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
	if (prec == NULL)
		return(errCodeParameter);

	fprintf(outFp,"\n");
	for (index = 0; index < prec->fldNum; index++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		len = 0;
		if ((ret = UnionPutRecFldIntoRecStr("queryFldName",prec->fldName[index],strlen(prec->fldName[index]),tmpBuf+len,sizeof(tmpBuf)-len)) <= 0)
		{
			UnionUserErrLog("in UnionPrintQueryFldGrpDefToFp:: UnionPutRecFldIntoRecStr queryFldName\n");
			return(ret);
		}
		len += ret;
		if ((ret = UnionPutRecFldIntoRecStr("fldAlais",prec->fldAlais[index],strlen(prec->fldAlais[index]),tmpBuf+len,sizeof(tmpBuf)-len)) <= 0)
		{
			UnionUserErrLog("in UnionPrintQueryFldGrpDefToFp:: UnionPutRecFldIntoRecStr fldAlais\n");
			return(ret);
		}
		len += ret;
		fprintf(outFp,"%s\n",tmpBuf);
	}
	return(prec->fldNum);
}

//---------------------------------------------------------------------------
/*
功能	从文件中读取域定义
输入参数
	fileName	文件名
	fldFlag		域标识
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadFldGrpDef(char *fileName,char *fldFlag,PUnionFldGrp prec)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	FILE	*fp;
	char	fldFlagName[100];
	
	if ((fileName == NULL) || (prec == NULL))
		return(errCodeParameter);
	if (fldFlag == NULL)
		strcpy(fldFlagName,"fldName");
	else
		strcpy(fldFlagName,fldFlag);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFldGrpDef:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(prec,0,sizeof(prec));
	prec->fldNum = 0;
	while (!feof(fp))
	{
		if (prec->fldNum >= conMaxNumOfPrimaryKeyFld)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,fldFlagName,prec->fldName[prec->fldNum],sizeof(prec->fldName[prec->fldNum]))) <= 0)
			continue;
		prec->fldNum += 1;
	}
	fclose(fp);
	return(prec->fldNum);
}

//---------------------------------------------------------------------------
/*
功能	向文件中写入域定义
输入参数
	prec		查询域定义
	fldFlag		域标识
	fp		文件句柄
输出参数
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintFldGrpDefToFp(char *fldFlag,PUnionFldGrp prec,FILE *fp)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	FILE	*outFp = stdout;
	char	fldFlagName[100];
	int	index;
	
	if (fp != NULL)
		outFp = fp;
		
	if (prec == NULL)
		return(errCodeParameter);
		
	if (fldFlag == NULL)
		strcpy(fldFlagName,"fldName");
	else
		strcpy(fldFlagName,fldFlag);
	
	fprintf(outFp,"\n");
	for (index = 0; index < prec->fldNum; index++)		
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		len = 0;
		if ((ret = UnionPutRecFldIntoRecStr(fldFlagName,prec->fldName[index],strlen(prec->fldName[index]),tmpBuf,sizeof(tmpBuf)-len)) <= 0)
		{
			UnionUserErrLog("in UnionPrintFldGrpDefToFp:: UnionPutRecFldIntoRecStr [%s]\n",fldFlagName);
			return(ret);
		}
		len += ret;
		fprintf(outFp,"%s\n",tmpBuf);
	}
	return(prec->fldNum);
}

/*
功能	从文件中读取关键字域定义
输入参数
	fileName	文件名
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadPrimaryKeyFldGrpDef(char *fileName,PUnionFldGrp prec)
{
	return(UnionReadFldGrpDef(fileName,"primaryKeyFldName",prec));
}

//---------------------------------------------------------------------------
/*
功能	向文件中写入域定义
输入参数
	prec		查询域定义
	fp		文件句柄
输出参数
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintPrimaryKeyFldGrpDefToFp(PUnionFldGrp prec,FILE *fp)
{
	return(UnionPrintFldGrpDefToFp("primaryKeyFldName",prec,fp));
}

//#pragma package(smart_init)

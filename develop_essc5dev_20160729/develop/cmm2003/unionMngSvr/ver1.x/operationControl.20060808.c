// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operationControl.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"

char    gunionResOperationRemark[128+1]="";

// 读取资源操作的说明
char *UnionGetOperationRemark(int resID,int operationID)
{
        TUnionOperationControlRec       rec;
        int                             ret;

        if ((ret = UnionReadSpecOperationFromDefFile(resID,operationID,&rec)) < 0)
        {
                UnionAuditLog("in UnionGetOperationRemark:: operation not defined for resID [%d] operationID [%d]\n",resID,operationID);
                sprintf(gunionResOperationRemark,"对资源%d进行%d操作",resID,operationID);
        }
        else
                strcpy(gunionResOperationRemark,rec.remark);
        return(gunionResOperationRemark);
}

#ifndef _WIN32
int UnionGetDefFileNameOfOperationController(char *fileName)
{
	sprintf(fileName,"%s/operation/operationControl.Def",getenv("UNIONETC"));
	return(0);
}
#endif

// 从字符串中读出操作定义
int UnionReadOperationFromDefStr(char *str,PUnionOperationControlRec prec)
{
	char	*ptr;
	char	tmpChar;
	char	varName[40+1];
	int	remarkSet = 0;
	int	index;
	
	if ((str == NULL) || (prec == NULL))
		return(errCodeParameter);

	if (UnionIsUnixShellRemarkLine(str))
		return(errCodeEsscMDL_NotOperationDefLine);

	if ((ptr = strstr(str,"operation=")) == NULL)
		return(errCodeEsscMDL_NotOperationDefLine);

	if (ptr != str)	// 说明在开始
	{
		tmpChar = *ptr;
		*ptr = 0;
		if (strlen(str) >= sizeof(prec->remark))
		{
			memcpy(prec->remark,str,sizeof(prec->remark)-1);
			prec->remark[sizeof(prec->remark)-1] = 0;
		}
		else
			strcpy(prec->remark,str);
		*ptr = tmpChar;
		remarkSet = 1;
	}
	
	//ptr += strlen("operation=");
	// 读资源号
	ptr += 10;
	if (ptr[3] != '.')
		return(errCodeEsscMDL_NotOperationDefLine);
	tmpChar = ptr[3];
	ptr[3] = 0;
	prec->resID = atoi(ptr);
	ptr[3] = tmpChar;
	// 读资源命令
	ptr += 4;
	if (ptr[3] != '=')
		return(errCodeEsscMDL_NotOperationDefLine);
	tmpChar = ptr[3];
	ptr[3] = 0;
	prec->resCmd = atoi(ptr);
	ptr[3] = tmpChar;
	// 读角色
	ptr += 4;
	for (index = 0; index < strlen(ptr); index++)
	{
#ifdef _WIN32
                if ((ptr[index] < '0') || (ptr[index] > '9'))
#else
		if (!isdigit(ptr[index]))
#endif
		{
			tmpChar = ptr[index];
			ptr[index] = 0;
			prec->roles = atol(ptr);
			ptr[index] = tmpChar;
			break;
		}
	}
	if (index == strlen(ptr))
	{
		prec->roles = atol(ptr);
		return(0);
	}
	// 读说明
	if (remarkSet)
		return(0);
	ptr += index;
	UnionFilterLeftBlank(ptr);
	if (strlen(ptr) >= sizeof(prec->remark))
	{
		memcpy(prec->remark,ptr,sizeof(prec->remark)-1);
		prec->remark[sizeof(prec->remark)-1] = 0;
	}
	else
		strcpy(prec->remark,ptr);
	return(0);
}

// 从文件中读取操作
int UnionReadSpecOperationFromSpecDefFile(char *fileName,int resID,int resCmd,PUnionOperationControlRec prec)
{
	int		ret;
	char		tmpBuf[1024+1];
	FILE		*fp;
	int		len;
	char		varName[40];
	char		*ptr;

	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionGetDefFileNameOfOperationController(tmpBuf);
		if ((fp = fopen(tmpBuf,"r")) == NULL)
		{
			UnionSystemErrLog("in UnionReadSpecOperationFromSpecDefFile:: fopen [%s]!\n",tmpBuf);
			return(errCodeUseOSErrCode);
		}
	}
	else
	{
		if ((fp = fopen(fileName,"r")) == NULL)
		{
			UnionSystemErrLog("in UnionReadSpecOperationFromSpecDefFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}

	sprintf(varName,"operation=%03d.%03d=",resID,resCmd);
	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ptr = strstr(tmpBuf,varName)) == NULL)
			continue;
		fclose(fp);
		return(UnionReadOperationFromDefStr(tmpBuf,prec));
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadSpecOperationFromSpecDefFile:: resID = [%03d] resCmd = [%03d] not defined!\n",resID,resCmd);
	return(errCodeEsscMDL_NoSuchOperationDefined);
}

// 从文件中读取操作
int UnionReadSpecOperationFromDefFile(int resID,int resCmd,PUnionOperationControlRec prec)
{
	return(UnionReadSpecOperationFromSpecDefFile(NULL,resID,resCmd,prec));
}

// 判断角色级别是不是授权了
int UnionIsAuthorizedRoleLevel(long roles,char roleLevel)
{
	if (roles < 0)
		return(errCodeEsscMDL_NoOperationAuthority);
	if (roles == 0)		// 0表示不限制级别
		return(0);
	for (;roles > 0;)
	{
		if (roles % 10 == roleLevel - '0')
			return(0);
		roles = roles / 10;
	}
	return(errCodeEsscMDL_NoOperationAuthority);
}

// 验证角色级别能否进行这个操作
int UnionVerifyOperation(int resID,int resCmd,char roleLevel)
{
	return(UnionVerifyOperationInSpecFile(NULL,resID,resCmd,roleLevel));
}

// 验证角色级别能否进行这个操作
int UnionVerifyOperationInSpecFile(char *fileName,int resID,int resCmd,char roleLevel)
{
	TUnionOperationControlRec	rec;
	int				ret;
			
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadSpecOperationFromSpecDefFile(fileName,resID,resCmd,&rec)) < 0)
	{
		UnionUserErrLog("in UnionVerifyOperationInSpecFile:: UnionReadSpecOperationFromSpecDefFile [%03d] [%03d]\n",resID,resCmd);
		return(ret);
	}
	UnionDebugNullLog("in UnionVerifyOperation:: %c %03d %03d %10ld %s\n",roleLevel,rec.resID,rec.resCmd,rec.roles,rec.remark);
	return(UnionIsAuthorizedRoleLevel(rec.roles,roleLevel));
}

int UnionPrintOperationToFile(PUnionOperationControlRec prec,FILE *fp)
{
	if (prec == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		fp = stdout;
	fprintf(fp,"operation=%03d.%03d=%010ld %s\n",prec->resID,prec->resCmd,prec->roles,prec->remark);
	return(0);
}

int UnionPrintAllSpecOperationToFile(FILE *outputFP,int resID,int resCmd,char roleLevel)
{
	int				ret;
	char				tmpBuf[1024+1];
	FILE				*fp;
	TUnionOperationControlRec	rec;
	long				totalNum = 0;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetDefFileNameOfOperationController(tmpBuf);
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAllSpecOperationToFile:: fopen [%s]!\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}

	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		memset(&rec,0,sizeof(rec));
		if ((ret = UnionReadOperationFromDefStr(tmpBuf,&rec)) < 0)
			continue;
		if (resID >= 0)
		{
			if (rec.resID != resID)
				continue;
		}
		if (resCmd >= 0)
		{
			if (rec.resCmd != resCmd)
				continue;
		}
#ifdef _WIN32
                if ((roleLevel >= '0') && (roleLevel <= '9'))
#else
		if (isdigit(roleLevel))
#endif
		{
			if ((ret = UnionIsAuthorizedRoleLevel(rec.roles,roleLevel)) < 0)
				continue;
		}
		if ((ret = UnionPrintOperationToFile(&rec,outputFP)) < 0)
			continue;
		totalNum++;
	}
	fclose(fp);
	fprintf(outputFP,"totalNum = %ld\n",totalNum);
	return(0);
}

int UnionPrintAllSpecOperationToSpecFile(char *fileName,int resID,int resCmd,char roleLevel)
{
	FILE	*outputFP;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0) || 
		(strcmp(fileName,"stdout") == 0))
		outputFP = stdout;
	else if (strcmp(fileName,"stderr") == 0)
		outputFP = stderr;
	else
	{
		if ((outputFP = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAllSpecOperationToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	
	ret = UnionPrintAllSpecOperationToFile(outputFP,resID,resCmd,roleLevel);
	
	if ((outputFP != stderr) && (outputFP != stdout))
		fclose(outputFP);
		
	return(ret);
}


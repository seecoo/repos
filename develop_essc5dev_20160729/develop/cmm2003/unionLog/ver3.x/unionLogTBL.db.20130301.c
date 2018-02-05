//	Author:		wangk
//	Date:		2009-9-22
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	
#define _realBaseDB_2_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "unionCommand.h"
#include "unionREC.h"
#include "UnionStr.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

extern PUnionSharedMemoryModule	pgunionLogFileMDL;
extern PUnionLogFileTBL		pgunionLogFileTBL;
extern PUnionLogFile		pgunionLogFile;

int UnionGetCategoryNameOfZlog(char *logFileName,char *categoryName)
{
	int	i,j;
	
	if (logFileName == NULL)
		return(0);
		
	for (i = 0,j = 0; i < strlen(logFileName); i++)
	{
		if (UnionIsDigitChar(logFileName[i]) || 	// 是数字
			UnionIsAlphaChar(logFileName[i]) ||	// 是字母
			(logFileName[i] == '_'))		// 是下划线
		{
			categoryName[j] = logFileName[i];
			j++;
		}
	}
	return(j);
}

/*
功能	写zlog配置文件RULES
输入参数
	无
输出参数
	无
返回值
	>=0	正确
	<0	错误
*/
int UnionZlogWriteRules(int logFileNum[])
{
	int	i,j;
	int	offset = 0;
	int	len;
	int	old_num = 0;
	int	rule_num = 0;
	int	appflag = 0;
	char	tmpBuf[1024+1];
	char	fileRecord[512][256+1];
	char	ruleRecord[512][256+1];
	char	fileName[512+1];
	char	ruleName[3][256+1];
	char	logName[256+1];
	char	mdc[16+1];
	char	noMdcLogName[128+1];
	char	placeLogName[256+1];
	char	logFileName[256+1];
	char	sizeName[256+1];
	char	logDir[128+1];
	char	categoryName[128+1];
	char	*ptr = NULL;
	char	*ptr2 = NULL;
	FILE	*fp = NULL;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/%s",getenv("UNIONETC"),"zlog_default.conf");
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		for (i = 0; i < 512; i++)
			memset(fileRecord[i],0,sizeof(fileRecord[i]));
		
		strcpy(fileRecord[0],"[global]");
		strcpy(fileRecord[1],"reload conf period = 0");
		strcpy(fileRecord[2],"default format = \"[%-8p]%d %us[%-7V][%F:%L] %m\"");
		strcpy(fileRecord[3],"file perms = 666");

		strcpy(fileRecord[10],"[levels]");
		strcpy(fileRecord[11],"NONELOG = 1, LOG_DEBUG");
		strcpy(fileRecord[12],"PGM = 10, LOG_DEBUG");
		strcpy(fileRecord[13],"USERERR = 110, LOG_ERR");
		strcpy(fileRecord[14],"OSERR = 130, LOG_ERR");
		

		strcpy(fileRecord[20],"[formats]");
		strcpy(fileRecord[21],"simple1 = \"[%-8p] %m\"");
		strcpy(fileRecord[22],"simple2 = \"[%-8p] %m%n\"");
		strcpy(fileRecord[23],"simple3 = \"[%-8p] %n%m\"");

		strcpy(fileRecord[30],"[rules]");
		sprintf(fileRecord[31],"%-40s\"%%E(UNIONLOG)/unknown.log\",10MB * 1","!.*");
		old_num = 32;
		offset = old_num - 1;
	}
	else
	{
		while (!feof(fp))
		{
			// 读取一行
			memset(fileRecord[old_num],0,sizeof(fileRecord[old_num]));
			if ((len = UnionReadOneLineFromTxtFile(fp,fileRecord[old_num],sizeof(fileRecord[old_num]))) < 0)
				continue;
			
			if (appflag == 0)
			{
				sprintf(tmpBuf,"[%s]","rules");
				if (memcmp(fileRecord[old_num],tmpBuf,strlen(tmpBuf)) == 0)
					appflag = 1;
			}
			else
			{
				// 匹配那些没有找到规则的分类
				if (memcmp(fileRecord[old_num],"!.*",3) != 0)
					old_num --;
				else
					offset = old_num;
	
				if ((fileRecord[old_num][0] == '[') && (fileRecord[old_num][len-1] == ']'))
					appflag = 0;
			}
			old_num ++;
		}
		fclose(fp);
		fp = NULL;
	}
	
	for(i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strlen((pgunionLogFile+i)->name) == 0)
			continue;
		
		memset(logFileName,0,sizeof(logFileName));
		strcpy(logFileName,(pgunionLogFile+i)->name);
		
		memset(categoryName,0,sizeof(categoryName));
		UnionGetCategoryNameOfZlog(logFileName,categoryName);
		
		// 级别
		memset(ruleName[0],0,sizeof(ruleName[0]));
		memset(ruleName[1],0,sizeof(ruleName[1]));
		memset(ruleName[2],0,sizeof(ruleName[2]));
		if ((pgunionLogFile+i)->level == conLogNoneLogLevel)
		{
			sprintf(ruleName[0],"%s.=NONELOG",categoryName);
			sprintf(ruleName[1],"%s_N.=NONELOG",categoryName);
			sprintf(ruleName[2],"%s_H.=NONELOG",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogSystemErrLevel)
		{
			sprintf(ruleName[0],"%s.OSERR",categoryName);
			sprintf(ruleName[1],"%s_N.OSERR",categoryName);
			sprintf(ruleName[2],"%s_H.OSERR",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogUserErrLevel)
		{
			sprintf(ruleName[0],"%s.USERERR",categoryName);
			sprintf(ruleName[1],"%s_N.USERERR",categoryName);
			sprintf(ruleName[2],"%s_H.USERERR",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogAuditLevel)
		{
			sprintf(ruleName[0],"%s.INFO",categoryName);
			sprintf(ruleName[1],"%s_N.INFO",categoryName);
			sprintf(ruleName[2],"%s_H.INFO",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogDebugLevel)
		{
			sprintf(ruleName[0],"%s.DEBUG",categoryName);
			sprintf(ruleName[1],"%s_N.DEBUG",categoryName);
			sprintf(ruleName[2],"%s_H.DEBUG",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogProgramerLevel)
		{
			sprintf(ruleName[0],"%s.PGM",categoryName);
			sprintf(ruleName[1],"%s_N.PGM",categoryName);
			sprintf(ruleName[2],"%s_H.PGM",categoryName);
		}
		else if ((pgunionLogFile+i)->level == conLogAlwaysLevel)
		{
			sprintf(ruleName[0],"%s.PGM",categoryName);
			sprintf(ruleName[1],"%s_N.PGM",categoryName);
			sprintf(ruleName[2],"%s_H.PGM",categoryName);
		}

		// 重写标志
		//conLogFullRewriteAtonce,	// 日志满立即重写，		缺省值
		//conLogFullRewriteAfterBackup,	// 再重写日志前备份日志
		//conLogNewEveryDay = 11,	// 每天一个新日志，日志名称加上日期，日志满后备份
		//conLogNewEveryMonth = 21,	// 每月一个新日志，日志名称加上月份，日志满后备份
		//conLogNewEverySeason = 31,	// 每季一个新日志，日志名称加上季度，日志满后备份
		//conLogNewHalfYear = 41,	// 每季一个新日志，日志名称加上半年，日志满后备份
		//conLogNewEveryYear = 51	// 每年一个新日志，日志名称加上半年，日志满后备份
		
		// mdc在括号内
		memset(mdc,0,sizeof(mdc));
		memset(noMdcLogName,0,sizeof(noMdcLogName));
		strcpy(noMdcLogName,logFileName);
		if ((ptr = strchr(noMdcLogName,'(')) && (ptr2 = strchr(ptr + 1,')')))
		{
			UnionInsertString(noMdcLogName,"%M",ptr - noMdcLogName);
		}
		
		memset(logDir,0,sizeof(logDir));
		//sprintf(logDir,"\"%%E(UNIONLOG)/%s",mdc);
		sprintf(logDir,"\"%%E(UNIONLOG)/");
		
		// 日志满立即重写
		memset(logName,0,sizeof(logName));
		memset(placeLogName,0,sizeof(placeLogName));
		if ((pgunionLogFile+i)->rewriteAttr == conLogFullRewriteAtonce)
		{
			sprintf(logName,"%s%s.log\"",logDir,noMdcLogName);
			if ((pgunionLogFile+i)->maxSize > 0)
			{
				logFileNum[i] = 1;
				sprintf(placeLogName," ~ %s%s.#1s.log\"",logDir,noMdcLogName);
			}
		}
		// 再重写日志前备份日志
		else if ((pgunionLogFile+i)->rewriteAttr == conLogFullRewriteAfterBackup)
		{
			sprintf(logName,"%s%s.log\"",logDir,noMdcLogName);
			if ((pgunionLogFile+i)->maxSize > 0)
			{
				sprintf(placeLogName," ~ %s%s.#4s.log\"",logDir,noMdcLogName);
			}				
		}
		// 每天一个新日志，日志名称加上日期，日志满后备份
		else if ((pgunionLogFile+i)->rewriteAttr == conLogNewEveryDay)
		{
			sprintf(logName,"%s%s.%%d(%%Y%%m%%d).log\"",logDir,noMdcLogName);
			if ((pgunionLogFile+i)->maxSize > 0)
			{
				sprintf(placeLogName," ~ %s%s.%%d(%%Y%%m%%d).#4s.log\"",logDir,noMdcLogName);
			}				
		}
		// 每月一个新日志，日志名称加上月份，日志满后备份
		// 每季一个新日志，日志名称加上季度，日志满后备份
		// 每半年一个新日志，日志名称加上半年，日志满后备份
		else if (((pgunionLogFile+i)->rewriteAttr == conLogNewEveryMonth) || 
			((pgunionLogFile+i)->rewriteAttr == conLogNewEverySeason) ||
			((pgunionLogFile+i)->rewriteAttr == conLogNewHalfYear))
		{
			sprintf(logName,"%s%s.%%d(%%Y%%m).log\"",logDir,noMdcLogName);
			if ((pgunionLogFile+i)->maxSize > 0)
			{
				sprintf(placeLogName," ~ %s%s.%%d(%%Y%%m).#4s.log\"",logDir,noMdcLogName);
			}				
		}
		// 每年一个新日志，日志名称加上半年，日志满后备份
		else if ((pgunionLogFile+i)->rewriteAttr == conLogNewEveryYear)
		{
			sprintf(logName,"%s%s.%%d(%%Y).log\"",logDir,noMdcLogName);
			if ((pgunionLogFile+i)->maxSize > 0)
			{
				sprintf(placeLogName," ~ %s%s.%%d(%%Y).#4s.log\"",logDir,noMdcLogName);
			}				
		}
		
		memset(sizeName,0,sizeof(sizeName));
		if ((pgunionLogFile+i)->maxSize > 0)
			sprintf(sizeName,",%ldMB * %d",(pgunionLogFile+i)->maxSize/1000000,logFileNum[i]);
			
		memset(ruleRecord[rule_num],0,sizeof(ruleRecord[rule_num]));
		sprintf(ruleRecord[rule_num],"%-40s %s %s %s" ,
			ruleName[0],
			logName,
			sizeName,
			placeLogName);
		rule_num ++;
	
		memset(ruleRecord[rule_num],0,sizeof(ruleRecord[rule_num]));
		sprintf(ruleRecord[rule_num],"%-40s %s %s %s; simple1" ,
			ruleName[1],
			logName,
			sizeName,
			placeLogName);
		rule_num ++;

		memset(ruleRecord[rule_num],0,sizeof(ruleRecord[rule_num]));
		sprintf(ruleRecord[rule_num],"%-40s %s %s %s; simple2" ,
			ruleName[2],
			logName,
			sizeName,
			placeLogName);

		if (++rule_num > 512)
			break;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		//printf("in UnionZlogWriteRules:: fopen w[%s]!\n",fileName);
		return(errCodeUseOSErrCode);			
	}
	
	for (i = 0; i < old_num; i++)
	{
		fprintf(fp,"%s\n",fileRecord[i]);
		if (i == offset)
		{
			for (j = 0; j < rule_num; j++)
			{
				fprintf(fp,"%s\n",ruleRecord[j]);
			}
		}
	}
	fflush(fp);
	fclose(fp);

	return(0);
}

int UnionGetNameOfLogFileTBL(char *fileName)
{
	sprintf(fileName,"unionLog");
	return(0);
}

int UnionReloadLogFileTBL()
{
	int	ret = 0;
	int	i = 0;
	int	j = 0;
	char	tmpBuf[128+1];
	char	sql[128+1];
	int	realNum = 0;
	int	iCnt = 0;
	char	logName[40+1];
	char	logLevel[16+1];
	char	maxSize[16+1];
	char	rewriteAttr[16+1];
	int	logFileNum[1024];
	PUnionLogFile	plogFile;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionConnectLogFileTBL!\n");
		return(ret);
	}

	if( (pgunionLogFileTBL->maxLogFileNum = UnionGetMaxNumOfLogFile()) < 0 )
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetMaxNumOfLogFile\n");
		return (errCodeLogMDL_ConfFile);
	}

	if( pgunionLogFileTBL->maxLogFileNum > 1024 )
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: MaxNumOfLogFile is too large\n");
		return (errCodeDefaultErrCode);
	}

	memset(sql,0,sizeof(sql));
	strcpy(sql,"select logName,logLevel,maxSize,rewriteAttr,logFileNum from unionLog");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);

	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{

		if (strlen((pgunionLogFile+i)->name) == 0)
			continue;

		// 查找是否有定义文件名为(pgunionLogFile+i)->name的日志
		for( j = 0; j < iCnt; j++ )
		{
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionReloadLogFileTBL:: UnionLocateXMLPackage\n");
				continue;
			}
			
			memset(logName,0,sizeof(logName));
			UnionReadXMLPackageValue("logName", logName, sizeof(logName));
			
			if( strcmp((pgunionLogFile+i)->name, logName) == 0 )
				break;
		}
		if( j >= iCnt )
		{
			if ((pgunionLogFile+i)->users > 0)
				continue;
			//UnionNullLog("[%s] not used and defined,delete it\n",(pgunionLogFile+i)->name);
			memset(pgunionLogFile+i,0,sizeof(*plogFile));
		}
	}
	
	realNum = 0;
	for (i = 0; (i < iCnt) && (realNum < pgunionLogFileTBL->maxLogFileNum); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionLocate\n");
			continue;
		}
			
		memset(logName,0,sizeof(logName));
		UnionReadXMLPackageValue("logName", logName, sizeof(logName));
		memset(logLevel,0,sizeof(logLevel));
		UnionReadXMLPackageValue("logLevel", logLevel, sizeof(logLevel));
		memset(maxSize,0,sizeof(maxSize));
		UnionReadXMLPackageValue("maxSize", maxSize, sizeof(maxSize));
		memset(rewriteAttr,0,sizeof(rewriteAttr));
		UnionReadXMLPackageValue("rewriteAttr", rewriteAttr, sizeof(rewriteAttr));

		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadXMLPackageValue("logFileNum", tmpBuf, sizeof(tmpBuf));
		logFileNum[realNum] = atoi(tmpBuf);
		
		// 读取名称
		if (strlen(logName) > sizeof(plogFile->name) - 1)
			continue;
		if ((plogFile = UnionConnectLogFile(logName)) == NULL)
		{
			//UnionNullLog("in UnionReloadLogFileTBL:: [%s] not in logfiletable, load it!\n",p);
			for (;realNum < pgunionLogFileTBL->maxLogFileNum;realNum++)
			{
				if (strlen((plogFile = pgunionLogFile + realNum)->name) == 0)
					break;
				else
					continue;
			}
			if (realNum >=  pgunionLogFileTBL->maxLogFileNum)
			{
				UnionUserErrLog("in UnionReloadLogFileTBL:: logfiletbl is full!\n");
				break;
			}
			memset(plogFile,0,sizeof(*plogFile));
			plogFile->users = 0;
			strcpy(plogFile->name, logName);
		}
		else
			--plogFile->users;
		
	
		// 读取Level
		plogFile->level = atoi(logLevel);
		//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
		if (plogFile->level > conLogAlwaysLevel)
			plogFile->level = conLogAuditLevel;
			
		// 读取日志的最大尺寸
		plogFile->maxSize = atoi(maxSize) * 1000000;	// 日志文件中的尺寸以M计
		if (plogFile->maxSize < 0)
			plogFile->maxSize = 1000000;
		
		// 读取重写标志
		switch (plogFile->rewriteAttr = atoi(rewriteAttr))
		{	
			case	conLogFullRewriteAtonce:
			case	conLogFullRewriteAfterBackup:
			case	conLogNewEveryDay:
			case	conLogNewEveryMonth:
			case	conLogNewEverySeason:
			case	conLogNewHalfYear:
			case	conLogNewEveryYear:
				break;
			default:
				plogFile->rewriteAttr = conLogFullRewriteAtonce;
				break;
		}
		
		UnionCalSizeOfLogFile(plogFile);
		
		++realNum;
	}

	UnionZlogWriteRules(logFileNum);
	return(0);
}

int UnionGetMaxNumOfLogFile()
{
	int	num = 0;

	if( (num=UnionReadIntTypeRECVar("maxNumOfLogFile")) < 0 )
	{
		UnionUserErrLog("in UnionGetMaxNumOfLogFile:: UnionReadIntTypeRECVar for [%s]\n!","MaxNumOfLogFile");
		num = -1;
	}

	return(num);	
}

int UnionReadLogFileDef(char *logFileName,PUnionLogFile plogFile)
{
	int	ret;
	char	sql[128+1];
	//char	tmpBuf[128+1];
	char	logLevel[16+1];
	char	maxSize[16+1];
	char	rewriteAttr[16+1];
	
	if ((logFileName == NULL) || (plogFile == NULL))
	{
		UnionUserErrLog("in UnionReadLogFileDef:: logFileName or plogFile is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(logFileName) > sizeof(plogFile->name) - 1)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: logFileName [%s] longer than expected [%d]!\n",logFileName,(int)sizeof(plogFile->name)-1);
		return(errCodeParameter);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select taskName,logLevel,maxSize,rewriteAttr logFileNum from unionLog where taskName = '%s'",logFileName);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	/*
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);
	*/
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionLocate\n");
		return (ret);
	}
			
	memset(logLevel,0,sizeof(logLevel));
	UnionReadXMLPackageValue("logLevel", logLevel, sizeof(logLevel));
	memset(maxSize,0,sizeof(maxSize));
	UnionReadXMLPackageValue("maxSize", maxSize, sizeof(maxSize));
	memset(rewriteAttr,0,sizeof(rewriteAttr));
	UnionReadXMLPackageValue("rewriteAttr", rewriteAttr, sizeof(rewriteAttr));

	// 找到了该日志的定义
	// 读取名称
	memset(plogFile,0,sizeof(*plogFile));
	strcpy(plogFile->name,logFileName);

	// 读取Level
	plogFile->level = atoi(logLevel);
	//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
	if (plogFile->level > conLogAlwaysLevel)
		plogFile->level = conLogAuditLevel;
		
	// 读取日志的最大尺寸
	plogFile->maxSize = atoi(maxSize) * 1000000;	// 日志文件中的尺寸以M计
	if (plogFile->maxSize < 0)
		plogFile->maxSize = 1000000;
	
	// 读取重写标志
	switch (plogFile->rewriteAttr = atoi(rewriteAttr))
	{
		case	conLogFullRewriteAtonce:
		case	conLogFullRewriteAfterBackup:
		case	conLogNewEveryDay:
		case	conLogNewEveryMonth:
		case	conLogNewEverySeason:
		case	conLogNewHalfYear:
		case	conLogNewEveryYear:
			break;
		default:
			plogFile->rewriteAttr = conLogFullRewriteAtonce;
			break;
	}
	
	UnionCalSizeOfLogFile(plogFile);

	plogFile->users = 0;
	
	UnionLog("in UnionReadLogFileDef:: [%s] [%d] [%ld] [%d]\n",plogFile->name,plogFile->level,plogFile->maxSize,plogFile->rewriteAttr);
	return(0);
}

// Wolfgang Wang
// 2002/08/26

// ***** 2007/11/15 Wolfgang Wang
// unionStr ver1.x
// 在UnionStr1.2中，修改了函数：	UnionGetPKOutOfRacalHsmCmdReturnStr

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionREC.h"
#include "UnionStr.h"
#include "unionErrCode.h"

int gunionIsDBFldNameUseUpperCase = 0;
int gunionTBLFldWithoutPrefix = 0;

struct timeval  		gunionLastUserSetTime;
struct timeval  		gunionLastCaculateTime;
int				gunionLastUserSetTimeInited = 0;

static int giCurrntExponent = 65537;  //add by hzh 2012.11.8

// 2011-1-25,王纯军增加
/* 从目录中读取一个文件名、版本、后缀
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	nullFileName	读出的文件名
	version		版本
	suffix		后缀
返回值：
	>=0 	成功
	<0	出错代码

*/
int UnionReadSrcFileInfoFromFullDir(char *dir,int lenOfDir,char *nullFileName,char *version,char *suffix)
{
	int	ret;
	char	fileName[512];
	char	*ptr;
	int	offset;
	int	fileLen;
	int	verLen;
	
	if ((fileLen = UnionReadFullFileNameFromFullDir(dir,lenOfDir,fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadSrcFileInfoFromFullDir:: UnionReadFullFileNameFromFullDir [%04d][%s]!\n",lenOfDir,dir);
		return(fileLen);
	}
	fileName[fileLen] = 0;
	
	// 读取文件名
	if ((ptr = strstr(fileName,".")) == NULL)
	{
		memcpy(nullFileName,fileName,fileLen);
		nullFileName[fileLen] = 0;
		return(0);
	}
	*ptr = 0;
	memcpy(nullFileName,fileName,strlen(fileName));
	offset = strlen(nullFileName)+1;
	if (fileLen == offset)
		return(0);
	// 读取后缀
	if ((ret = UnionReadSuffixOfFileName(fileName+offset,strlen(fileName+offset),0,suffix)) < 0)
	{
		UnionUserErrLog("in UnionReadSrcFileInfoFromFullDir:: UnionReadSuffixOfFileName [%04zu][%s]!\n",strlen(fileName+offset),fileName+offset);
		return(ret);
	}
	// 读取版本
	verLen = strlen(fileName+offset)-1-strlen(suffix);
	if (verLen < 0)
		return(0);
	fileName[offset+verLen] = 0;
	memcpy(version,fileName+offset,verLen);
	version[verLen] = 0;
	return(0);
}

// 2011/1/1,王纯军增加
/* 将一个文件全名，折分成目录和文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	onlyDir		拆分出的目录
	fileName	读出的文件名
返回值：
	>=0 	成功
	<0	出错代码

*/
int UnionSeperateDir(char *dir,int lenOfDir,char *onlyDir,char *fileName)
{
	int	index;
	char	ch;
	
	if ((dir == NULL) || (lenOfDir < 0))
	{
		UnionUserErrLog("in UnionSeperateDir:: wrong parameter! lenOfDir = [%d]\n",lenOfDir);
		return(errCodeParameter);
	}
	for (index = lenOfDir - 1;index >= 0; index--)
	{
		if ((dir[index] != '\\') && (dir[index] != '/'))
			continue;
		ch = dir[index];
		dir[index] = 0;
		if (onlyDir != NULL)
			strcpy(onlyDir,dir);
		dir[index] = ch;
		index++;
		if ((index < lenOfDir) && (fileName != NULL))
			strcpy(fileName,dir+index);
		return(0);
	}
	if (onlyDir != NULL)
		strcpy(onlyDir,dir);
	return(0);
}

// 2010/12/10,王纯军
/*
功能	判断是否同一文件
输入参数
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
输出参数
	无
返回值
	1		同一文件
	0		不同文件
*/
int UnionIsSameFileName(char *firstFileName,char *secondFileName)
{
	char	tmpBuf1[256];
	char	tmpBuf2[256];
	
	UnionReadFullFileNameFromFullDir(firstFileName,strlen(firstFileName),tmpBuf1);
	UnionReadFullFileNameFromFullDir(secondFileName,strlen(secondFileName),tmpBuf2);
	if ((strlen(tmpBuf1) == 0) || (strlen(tmpBuf2) == 0))
		return(0);
	if (strcmp(tmpBuf1,tmpBuf2) != 0)
		return(0);
	else
		return(1);
}

	
// 2010/12/10,王纯军
/*
功能	将一个unix文件的属性改为可执行
输入参数
	dir		文件目录
	fileName	文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionChangeUnixFileToExecutable(char *dir,char *fileName)
{
	char	tmpBuf[1024];
	
	sprintf(tmpBuf,"chmod a+x %s/%s",dir,fileName);
	UnionLog("in UnionChangeUnixFileToExecutable:: [%s]\n",tmpBuf);
	system(tmpBuf);
	return(0);
}

// 2010/12/10,王纯军
/*
功能	写一个执行文件的脚本文件
输入参数
	dir		文件目录
	excuteFileName	可执行文件名称
	scriptFileName	脚本文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateScriptFileForExecuteSpecFile(char *dir,char *excuteFileName,char *scriptFileName)
{
	FILE	*fp;
	char	fullFileName[1024];
	char	nullFileName[256];
	char	nullDir[512];
	
	if ((fp = fopen(scriptFileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateScriptFileForExecuteSpecFile:: fopen [%s]!\n",scriptFileName);
		return(errCodeUseOSErrCode);
	}
	// 写更改文件属性的语句
	fprintf(fp,"chmod a+x %s/%s\n",dir,excuteFileName);
	// 获得文件命名
	snprintf(fullFileName,sizeof(fullFileName),"%s/%s",dir,excuteFileName);
	memset(nullFileName,0,sizeof(nullFileName));
	memset(nullDir,0,sizeof(nullDir));
	UnionGetFileDirAndFileNameFromFullFileName(fullFileName,strlen(fullFileName),nullDir,nullFileName);
	// 写进入指定目录的语句
	fprintf(fp,"cd %s\n",nullDir);
	// 写执行指定文件的语句
	fprintf(fp,"%s\n",nullFileName);
	fclose(fp);
	return(0);
}

// 2010/12/10,王纯军
/*
功能	执行一个文件，并将结果输出到指定文件中
输入参数
	dir		文件目录
	excuteFileName	可执行文件名称
	outputFileName	输出文件名称
	overwrite	如果输出文件已存在，是否覆盖它，1，覆盖，0，不覆盖
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionExecuteSpecFile(char *dir,char *excuteFileName,char *outputFileName,int overwrite)
{
	char	tmpBuf[2048];
	int	offset = 0;
	int	ret;
	char	scriptFileName[512];
	
	// 生成一个执行脚本文件
	sprintf(scriptFileName,"%s/%d.x",getenv("UNIONTEMP"),getpid());
	if ((ret = UnionGenerateScriptFileForExecuteSpecFile(dir,excuteFileName,scriptFileName)) < 0)
	{
		UnionUserErrLog("in UnionExecuteSpecFile:: UnionGenerateScriptFileForExecuteSpecFile!\n");
		return(ret);
	}
	// 将执行脚本改为可执行
	UnionChangeUnixFileToExecutable("",scriptFileName);
	// 拼装命令
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",scriptFileName);
	offset = strlen(tmpBuf);
	if (outputFileName == NULL)
		goto executeNow;
	if (strlen(outputFileName) == 0)
		goto executeNow;
	if (overwrite)
	{
		// 创建一个新文件
		//UnionCreateFile(outputFileName);
	}
	sprintf(tmpBuf+offset," >> %s 2>> %s",outputFileName,outputFileName);
executeNow:
	UnionLog("in UnionExecuteSpecFile:: [%s]\n",tmpBuf);
	system(tmpBuf);
	return(0);
}

// 2010/12/9,王纯军
/*
功能	将一个字符串写到文件中
输入参数
	str		字符串
	lenOfStr	字符串长度
	seperator	分隔符
	fileName	文件名
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputStrToSpecFile(char *str,int lenOfStr,char seperator,char *fileName)
{
	FILE	*fp = stdout;
	
	if (fileName != NULL)
	{
		if (strlen(fileName) != 0)
		{
			if ((fp = fopen(fileName,"w")) == NULL)
			{
				UnionUserErrLog("in UnionOutputStrToSpecFile:: fopen [%s]!\n",fileName);
				return(errCodeUseOSErrCode);
			}
		}
	}
	if (seperator != 0)
	{
		UnionConvertOneFldSeperatorInRecStrIntoAnother(str,lenOfStr,seperator,'\n',str,lenOfStr+1);
	}
	fwrite(str,lenOfStr,1,fp);
	if (fp != stdout)
		fclose(fp);
	if (seperator != 0)
	{
		UnionConvertOneFldSeperatorInRecStrIntoAnother(str,lenOfStr,'\n',seperator,str,lenOfStr+1);
	}
	return(0);
}

// 2010/11/1,王纯军
/*
功能	打印指定数量的字符到文件中
输入参数
	num		空格数量
	specChar	指定的字符
	fp		文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintNumOfSpecCharToFp(int num,char specChar,FILE *fp)
{
	int	index;

	if (fp == NULL)
		return(0);
	for (index = 0; index < num; index++)
		fprintf(fp,"%c",specChar);
	return(num);
}

/*
功能	以指定的格式打印字符串到文件中
输入参数
	str		字符串
	lenOfStr	字符串长度
	fp		文件指针
	formatLen	字符串在文件中所占的长度，不足补空格
	DJFS		字符串对齐方式
			0	左对齐
			1	中间对齐
			2	右对齐
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintStrToFpInFormat(char *str,int lenOfStr,FILE *fp,int formatLen,int DJFS)
{
	int	index;
	int	leftBlankNum = 0;	// 要打印的左空格数
	int	rightBlankNum = 0;	// 要打印的右空格数
	int	printStrLen = 0;	// 要打印的字符串长度
	int	totalBlankNum = 0;	// 要打印的空格数
	
	UnionLog("*** [%04d][%s][%d][%d]\n",lenOfStr,str,formatLen,DJFS);
	if (formatLen <= 0)
	{
		fprintf(fp,"%s",str);
		return(0);
	}
	if (lenOfStr < 0)
		return(errCodeParameter);
	printStrLen = lenOfStr;
	if (formatLen < printStrLen)	// 所占长度比实际长度短
		printStrLen = formatLen;
	totalBlankNum = formatLen - printStrLen;
	switch (DJFS)
	{
		case	conStrCellAlignLeft:	// 左对齐
			rightBlankNum = totalBlankNum;
			break;
		case	conStrCellAlignMiddle:	// 位于中间
			leftBlankNum = totalBlankNum / 2;
			rightBlankNum = totalBlankNum - leftBlankNum;
			//UnionLog("left = [%02d] right = [%02d] total = [%02d] for [%04d][%s]\n",
			//		leftBlankNum,rightBlankNum,totalBlankNum,printStrLen,str);
			break;
		case	conStrCellAlignRight:	// 右对齐
			leftBlankNum = totalBlankNum;
			break;
		default:
			rightBlankNum = totalBlankNum;
			break;
	}
	// 打印左空格		
	UnionPrintNumOfSpecCharToFp(leftBlankNum,' ',fp);
	// 打印字符串
	for (index = 0; index < printStrLen; index++)
		fprintf(fp,"%c",str[index]);
	// 打印右空格		
	UnionPrintNumOfSpecCharToFp(rightBlankNum,' ',fp);
	return(0);
}

// 2010/10/30,王纯军增加
/*
功能
	打开一个文件
输入参数
	fileName	文件名称
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionOpenFile(char *fileName)
{
	FILE	*fp;
	
	if (fileName == NULL)
		return(stdout);
	if (strlen(fileName) == 0)
		return(stdout);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionOpenFile:: fopen [%s]!\n",fileName);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	return(fp);
}

// 2010/10/30,王纯军增加
/*
功能
	打开一个文件
输入参数
	fileName	文件名称
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionOpenFileForAppend(char *fileName)
{
	FILE	*fp;
	
	if (fileName == NULL)
		return(stdout);
	if (strlen(fileName) == 0)
		return(stdout);
	if ((fp = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionOpenFile:: fopen [%s]!\n",fileName);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	return(fp);
}


// 2010/10/30,王纯军增加
/*
功能
	返回一个合法的文件句柄，如果fp不为空，则返回fp，否则返回stdout
输入参数
	fp		文件句柄
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionGetValidFp(FILE *fp)
{
	if (fp != NULL)
		return(fp);
	else
		return(stdout);
}

// 2010/10/30,王纯军增加
/*
功能
	关闭一个文件句柄
输入参数
	fp		文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseFp(FILE *fp)
{
	if (fp == NULL)
		return;
	if ((fp != stdout) && (fp != stderr))
		fclose(fp);
	return;
}


// 2010/8/19,王纯军增加
/*
功能
	根据一个域选择定义，将相应的域写入到指定串中
输入参数
	fldDefStr		域定义串
	lenOfFldDefStr		域定义串长度
	oriDataStr		原始数据串
	lenOfOriDataStr		原始数据串长度
	sizeOfDesDataStr	目标数据串大小
输出参数
	desDataStr		目标数据串
返回值
	>=0			目标数据串的长度
	<0			出错代码
*/
int UnionFilterSpecRecFldUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr)
{
	int	fldNum;
	char	fldGrp[16][128];
	char	fldValue[2048];
	int	index;
	int	ret;
	int	offset = 0;
	
	if (lenOfOriDataStr <= 0)	// 原始数据长度是0,不拼装任何数据
	{
		UnionAuditLog("in UnionFilterSpecRecFldUnderSpecDef:: lenOfOriDataStr = [%d]!\n",lenOfOriDataStr);
		return(0);
	}
		
	// 拼分域定义串
	if ((fldNum = UnionSeprateVarStrIntoArray(fldDefStr,lenOfFldDefStr,'^',(char *)&fldGrp,16,128)) < 0)
	{
		UnionUserErrLog("in UnionFilterSpecRecFldUnderSpecDef:: UnionSeprateVarStrIntoArray [%04d][%s]!\n",lenOfFldDefStr,fldDefStr);
		return(fldNum);
	}
	
	// 根据每个域来进行赋值
	//UnionLog("in UnionFilterSpecRecFldUnderSpecDef:: oriDataStr=[%04d][%s]\n",lenOfOriDataStr,oriDataStr);
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionReadRecFldFromRecStr(oriDataStr,lenOfOriDataStr,fldGrp[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionFilterSpecRecFldUnderSpecDef:: UnionReadRecFldFromRecStr [%s] from [%d][%s]!\n",fldGrp[index],lenOfOriDataStr,oriDataStr);
			return(ret);
		}
		fldValue[ret] = 0;
		if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],fldValue,ret,desDataStr+offset,sizeOfDesDataStr-offset)) < 0)
		{
			UnionUserErrLog("in UnionFilterSpecRecFldUnderSpecDef:: UnionPutRecFldIntoRecStr [%s]!\n",fldGrp[index]);
			return(ret);
		}
		offset += ret;
	}
	desDataStr[offset] = 0;
	//UnionLog("in UnionFilterSpecRecFldUnderSpecDef:: desDataStr=[%04d][%s]\n",resOffset,desDataStr);
	return(offset);
}	

/*
功能
	获取一个月份的天数
输入参数
	month			月份
	year			年
输出参数
	无
返回值
	>=0			指定月份的天数
	<0			失败
*/
int UnionGetDaysOfSpecMonth(int month,int year)
{
	switch (month)
	{
		case	1:
		case	3:
		case	5:
		case	7:
		case	8:
		case	10:
		case	12:
			return(31);
		case	4:
		case	6:
		case	9:
		case	11:
			return(30);
		case	2:
			if ((year % 4 == 0) && (year % 100 != 0))
				return(29);
			else	
				return(28);
			break;
		default:
			return(errCodeParameter);
	}
}

/*
功能
	计算一个日期
输入参数
	firstYear		起如年份
	daysSinceFirstYear	从起始时间之后已过去的日
输出参数
	year			年
	month			月
	day			日
返回值
	>=0			成功
	<0			失败
*/
int UnionAnalysisAfterDate(int firstYear,long daysSinceFirstYear,long *year,long *month,long *day)
{
	long	daysOfAllPastYears = 0;
	long	daysOfThisYear,maxDaysOfThisYear,daysOfAllPastMonths;
	long	currentMonth,currentYear;
	long	maxDaysOfThisMonth;
	
	if ((firstYear < 0) || (daysSinceFirstYear < 0))
		return(errCodeParameter);
		
	for (daysOfAllPastYears = 0,currentYear = firstYear;; currentYear++)
	{
		if ((currentYear % 4 == 0) && (currentYear % 100 != 0))
			maxDaysOfThisYear = 366;
		else
			maxDaysOfThisYear = 365;
		if (daysOfAllPastYears + maxDaysOfThisYear > daysSinceFirstYear)
		{
			daysOfThisYear = daysSinceFirstYear - daysOfAllPastYears;
			*year = currentYear;
			break;
		}
		daysOfAllPastYears += maxDaysOfThisYear;
		continue;
	}
	for (currentMonth = 1,daysOfAllPastMonths = 0;;currentMonth++)
	{
		maxDaysOfThisMonth = UnionGetDaysOfSpecMonth(currentMonth,currentYear);
		if (maxDaysOfThisMonth + daysOfAllPastMonths == daysOfThisYear)
		{
			*month = currentMonth;
			*day = maxDaysOfThisMonth;
			break;
		}
		if (maxDaysOfThisMonth + daysOfAllPastMonths > daysOfThisYear)
		{
			*month = currentMonth;
			*day = daysOfThisYear - daysOfAllPastMonths;
			if (*day == 0)
				*day = 1;
			break;
		}
		daysOfAllPastMonths += maxDaysOfThisMonth;
	}
	return(0);
}

/*
功能
	分析特定时间
输入参数
	totalSeconds		从起始年过后的时间，以秒表
	firstYear		起始年
	timeZoneOffset		时区偏移，以格林威治时间为准
输出参数
	timeStr			日期时间串，格式"YYYY-MM-DD HH:MM:SS"
返回值
	>=0			成功
	<0			失败
*/
int UnionAnalysisSpecDateTime(long totalSeconds,int firstYear,int timeZoneOffset,char *timeStr)
{
	long			hour,minute,second;
	long			totalMinute,totalHour,totalDays;
	long			currentYear;
	long			currentMonth;
	long			currentDay;
	
	second = totalSeconds % 60;
	minute = (totalMinute = totalSeconds / 60) % 60;
	hour = (totalHour = totalMinute / 60 + timeZoneOffset) % 24;
	totalDays = totalHour / 24;

	UnionAnalysisAfterDate(firstYear,totalDays,&currentYear,&currentMonth,&currentDay);
	sprintf(timeStr,"%04ld-%02ld-%02ld %02ld:%02ld:%02ld",
			currentYear,currentMonth,currentDay,
			hour,minute,second);	
	return(strlen(timeStr));
}
#ifndef _USE_HOST_TIME_
/*
功能
	获取当前日期时间
输入参数
	无
输出参数
	timeStr			日期时间串，格式"YYYY-MM-DD HH:MM:SS MMM:MMM"
返回值
	>=0			成功
	<0			失败
*/
void UnionGetCurrentDateTimeInMacroFormat(char *timeStr)
{
	struct timeval  	now;
	int			offset;
	
	if (gettimeofday(&now,NULL) != 0)
		return;

	if ((offset = UnionAnalysisSpecDateTime(now.tv_sec,1970,8,timeStr)) < 0)
		return;
	
	sprintf(timeStr+offset," %03d:%03d",(int)now.tv_usec/1000,(int)now.tv_usec%1000);

	return;
}
#else
/*
功能
	获取当前日期时间
输入参数
	无
输出参数
	timeStr			日期时间串，格式"YYYY-MM-DD HH:MM:SS MMM:MMM"
返回值
	>=0			成功
	<0			失败
*/
void UnionGetCurrentDateTimeInMacroFormat(char *timeStr)
{
	struct timeval		now;
	int			offset;
	struct tm		*ptm;

	if (gettimeofday(&now, NULL) != 0)
		return;

	ptm = localtime(&now.tv_sec);
	offset = 8 + 2 + 1 + 6 + 2;
	strftime(timeStr, offset + 1, "%Y-%m-%d %H:%M:%S", ptm);

	sprintf(timeStr + offset," %03d:%03d", now.tv_usec / 1000, now.tv_usec % 1000);

	return;
}
#endif
/*
功能
	把秒值的时间转换为字符串格式
输入参数
	secondTime
	format		如：格式"YYYY-MM-DD HH:MM:SS"
	sizeOfTimeStr
输出参数
	timeStr			日期时间串
返回值
	无
*/
int UnionGetTimeStrFromSecondTime(int secondTime, char *format, int sizeOfTimeStr, char *timeStr)
{
	struct tm		*ptm;
	time_t			stime;

	stime = secondTime;

	ptm = localtime(&stime);
	if(format == NULL || strlen(format) == 0)
	{
		strftime(timeStr, sizeOfTimeStr, "%Y-%m-%d %H:%M:%S", ptm);
	}
	else
	{
		strftime(timeStr, sizeOfTimeStr, format, ptm);
	}

	return(0);
}

// 读取当前设置时间
void UnionGetUserSetTime(struct timeval *userTime)
{
	memcpy(userTime,&gunionLastUserSetTime,sizeof(gunionLastUserSetTime));
	return;
}

// 设置当前时间
void UnionSetUserSetTime(struct timeval *userTime)
{
	if (userTime == NULL)
		return;
		
	memcpy(&gunionLastUserSetTime,userTime,sizeof(*userTime));
	memcpy(&gunionLastCaculateTime,&gunionLastUserSetTime,sizeof(gunionLastUserSetTime));
	gunionLastUserSetTimeInited = 1;
}

// 重置当前时间
void UnionReInitUserSetTime()
{
	struct timeval	now;
	
	if (gettimeofday(&now,NULL) != 0)
		return;
	
	UnionSetUserSetTime(&now);

	return;
}

// 获取当前运行时间,以微秒表示
long UnionGetRunningTimeInMicroSeconds()
{
	struct timeval	now;
	long		snap;
	
	if (gettimeofday(&now,NULL) != 0)
		return(errCodeUseOSErrCode);
	if (!gunionLastUserSetTimeInited)
		UnionSetUserSetTime(&now);
	snap = (now.tv_sec - gunionLastCaculateTime.tv_sec) * 1000000 + (now.tv_usec - gunionLastCaculateTime.tv_usec);
	memcpy(&gunionLastCaculateTime,&now,sizeof(gunionLastUserSetTime));
	return(snap);
}

// 获取运行时间,以毫秒表示
long UnionGetRunningTimeInMilliSeconds()
{
	return(UnionGetRunningTimeInMicroSeconds() / 1000);
}

// 获取运行时间,以秒表示
long UnionGetRunningTimeInSeconds()
{
	return(UnionGetRunningTimeInMicroSeconds() / 1000000);
}

// 获取运行时间,以秒表示
long UnionGetRunningTime()
{
	return(UnionGetRunningTimeInSeconds());
}

// 获取总运行时间,以微秒表示
long UnionGetTotalRunningTimeInMacroSeconds()
{
	struct timeval	now;
	long		snap;
	
	if (gettimeofday(&now,NULL) != 0)
		return(errCodeUseOSErrCode);
	if (!gunionLastUserSetTimeInited)
		UnionSetUserSetTime(&now);
	snap = (now.tv_sec - gunionLastUserSetTime.tv_sec) * 1000000 + (now.tv_usec - gunionLastUserSetTime.tv_usec);
	UnionSetUserSetTime(&now);
	return(snap);
}

// 获取运行时间偏移量,以微秒表示
long UnionGetRunningTimeOffsetInMacroSeconds()
{
	struct timeval	now;
	long		snap;
	
	if (gettimeofday(&now,NULL) != 0)
		return(errCodeUseOSErrCode);
	if (!gunionLastUserSetTimeInited)
		UnionSetUserSetTime(&now);
	snap = (now.tv_sec - gunionLastUserSetTime.tv_sec) * 1000000 + (now.tv_usec - gunionLastUserSetTime.tv_usec);
	return(snap);
}

// 获取总运行时间,以毫秒表示
long UnionGetTotalRunningTimeInMilliSeconds()
{
	return(UnionGetTotalRunningTimeInMacroSeconds() / 1000);
}

// 获取总运行时间,以秒表示
long UnionGetTotalRunningTimeInSeconds()
{
	return(UnionGetTotalRunningTimeInMacroSeconds() / 1000000);
}

// 获取总运行时间,以秒表示
long UnionGetTotalRunningTime()
{
	return(UnionGetTotalRunningTimeInSeconds());
}

// 获取毫秒表示的当前时间
// 返回值>0,当前时间.<0,出错了
long UnionGetCurrentTimeInMilliSeconds()
{
	struct timeval  		now;

	if (gettimeofday(&now,NULL) != 0)
		return(errCodeUseOSErrCode);
	return(now.tv_sec * 1000 + now.tv_usec / 1000);
}


// 获取微秒表示的当前时间
// 返回值>0,当前时间.<0,出错了
long UnionGetCurrentTimeInMicroSeconds()
{
	struct timeval  		now;

	if (gettimeofday(&now,NULL) != 0)
		return(errCodeUseOSErrCode);
	return(now.tv_sec * 1000000 + now.tv_usec);
}

// 2010/8/19,王纯军增加
/*
功能
	拆分一个域定义
	域定义格式为
	fldName=fldOffset[,fldLen]
输入参数
	fldDefStr		域定义串
	sizeOfFldName		域名缓冲大小
	sizeOfFldAlais		域别名缓冲大小
输出参数
	fldName			域名
	fldOffset		域偏移
	fldLen			域长度
	fldAlais		域别名
返回值
	>=0			成功
	<0			出错代码
*/
int UnionSeperateFldAssignmentDef(char *fldDefStr,char *fldName,int sizeOfFldName,int *fldOffset,int *fldLen,char *fldAlais,int sizeOfFldAlais)
{
	char	*ptr,*ptr2;
	int	len;
	int	fldNum;
	char	fldGrp[4][128];
	
	if (strlen(fldDefStr) == 0)
		return(0);

	*fldLen = -1;
	*fldOffset = -1;
	// 折变量名
	if ((ptr = strstr(fldDefStr,"=")) != NULL)
	{
		*ptr = 0;
		if ((len = strlen(fldDefStr)) >= sizeOfFldName)
		{
			UnionUserErrLog("in UnionSeperateFldAssignmentDef:: too small buffer!\n");
			return(errCodeSmallBuffer);
		}
		memcpy(fldName,fldDefStr,len);
		fldName[len] = 0;
		*ptr = '=';
		ptr++;
	}
	else
	{
		if ((fldNum = UnionSeprateVarStrIntoArray(fldDefStr,strlen(fldDefStr),',',(char *)&fldGrp,4,128)) <= 0)
			return(fldNum);
		strcpy(fldName,fldGrp[0]);
		if (fldNum >= 2)
			*fldOffset = atoi(fldGrp[1]);
		if (fldNum >= 3)
			*fldLen = atoi(fldGrp[2]);
		if ((fldNum >= 4) && (fldAlais != NULL))
			strcpy(fldAlais,fldGrp[3]);
		return(0);
	}
	// 拆偏移
	*fldOffset = 0;
	if ((ptr2 = strstr(ptr,",")) == NULL)
	{
		*fldOffset = atoi(ptr);
		*fldLen = -1;
		return(0);
	}
	else
	{
		*ptr2 = 0;
		*fldOffset = atoi(ptr);
		*ptr2 = ',';
	}
	// 拆长度
	ptr2++;
	if ((ptr = strstr(ptr2,",")) == NULL)
	{
		*fldLen = atoi(ptr2);
		return(0);
	}
	else
	{
		*ptr = 0;
		*fldLen = atoi(ptr2);
		*ptr = ',';
	}
	ptr++;
	// 拆域别名
	if (fldAlais != NULL)
	{
		if ((len = strlen(ptr)) >= sizeOfFldAlais)
		{
			UnionUserErrLog("in UnionSeperateFldAssignmentDef:: too small buffer!\n");
			return(errCodeSmallBuffer);
		}
		memcpy(fldAlais,ptr,len);
		fldAlais[len] = 0;
	}	
	return(0);
}

// 2010/8/19,王纯军增加
/*
功能
	根据一个响应定义，将相应的响应信息写入到指定响应串中
输入参数
	fldDefStr		域定义串
	lenOfFldDefStr		域定义串长度
	oriDataStr		原始数据串
	lenOfOriDataStr		原始数据串长度
	sizeOfDesDataStr	目标数据串大小
输出参数
	desDataStr		目标数据串
返回值
	>=0			目标数据串的长度
	<0			出错代码
*/
int UnionFormRecStrUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr)
{
	int	fldNum;
	char	fldGrp[16][128];
	char	fldName[128],fldAlais[128];
	char	fldValue[2048];
	int	fldOffset;
	int	fldLen;
	int	resOffset = 0;
	int	index;
	int	ret;
	int	fldValueLen;
	
	if (lenOfOriDataStr <= 0)	// 原始数据长度是0,不拼装任何数据
	{
		UnionAuditLog("in UnionFormRecStrUnderSpecDef:: lenOfOriDataStr = [%d]!\n",lenOfOriDataStr);
		return(0);
	}
		
	// 拼分域定义串
	if ((fldNum = UnionSeprateVarStrIntoArray(fldDefStr,lenOfFldDefStr,'^',(char *)&fldGrp,16,128)) < 0)
	{
		UnionUserErrLog("in UnionFormRecStrUnderSpecDef:: UnionSeprateVarStrIntoArray [%04d][%s]!\n",lenOfFldDefStr,fldDefStr);
		return(fldNum);
	}
	
	// 根据每个域来进行赋值
	//UnionLog("in UnionFormRecStrUnderSpecDef:: oriDataStr=[%04d][%s]\n",lenOfOriDataStr,oriDataStr);
	for (index = 0; index < fldNum; index++)
	{
		// 拆分域的赋值方法
		memset(fldName,0,sizeof(fldName));
		memset(fldAlais,0,sizeof(fldAlais));
		if ((ret = UnionSeperateFldAssignmentDef(fldGrp[index],fldName,sizeof(fldName),&fldOffset,&fldLen,fldAlais,sizeof(fldAlais))) < 0)
		{
			UnionUserErrLog("in UnionFormRecStrUnderSpecDef:: UnionSeperateFldAssignmentDef [%s]!\n",fldGrp[index]);
			return(ret);
		}
		UnionLog("*** [%s][%s][%04d][%04d]of[%04zu][%s]\n",fldName,fldAlais,fldOffset,fldLen,strlen(fldGrp[index]),fldGrp[index]);
		if (strlen(fldName) == 0)
			continue;
		// 读取指定域
		memset(fldValue,0,sizeof(fldValue));
		if ((fldValueLen = UnionReadRecFldFromRecStr(oriDataStr,lenOfOriDataStr,fldName,fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionFormRecStrUnderSpecDef:: UnionReadRecFldFromRecStr [%s]!\n",fldName);
			return(fldValueLen);
		}
		if (fldOffset >= fldValueLen)
		{
			UnionUserErrLog("in UnionFormRecStrUnderSpecDef:: fldOffset = [%d] > fldValue = [%s] ' len [%d]\n",fldOffset,fldValue,fldValueLen);
			return(errCodeParameter);
		}
		if (fldOffset < 0)
			fldOffset = 0;
		if ((fldLen < 0) || (fldLen+fldOffset > fldValueLen))
			fldLen = fldValueLen - fldOffset;
		//UnionLog("*** [%04d][%04d][%04d]\n",resOffset,fldOffset,fldLen);
		if (strlen(fldAlais) == 0)
			strcpy(fldAlais,fldName);
		if ((ret = UnionPutRecFldIntoRecStr(fldAlais,fldValue+fldOffset,fldLen,desDataStr+resOffset,sizeOfDesDataStr-resOffset)) < 0)
		{
			UnionUserErrLog("in UnionFormRecStrUnderSpecDef:: UnionPutRecFldIntoRecStr [%s]!\n",fldAlais);
			return(ret);
		}
		resOffset += ret;
	}
	desDataStr[resOffset] = 0;
	//UnionLog("in UnionFormRecStrUnderSpecDef:: desDataStr=[%04d][%s]\n",resOffset,desDataStr);
	return(resOffset);
}	

// 2010/7/6,王纯军增加
/*
功能
	将一个整型值写入到字符串中
输入参数：
	intValue	整型值
	format		写入格式
	sizeOfBuf	串大小
输出参数
	value		字符串
返回值
	>=0	成功,串长度
	<0	错误码
*/
int UnionPutIntTypeValueIntoStr(int intValue,int format,char *value,int sizeOfBuf)
{
	if (sizeOfBuf <= 0)
		return(0);

	switch (format)
	{
		case	conIntValueFormatShiJinZhi:	// 十进制
			sprintf(value,"%d",intValue);
			break;
		case	conIntValueFormatXiaoXieShiLiuZhi:	// 小写十六进制
			sprintf(value,"%0x",intValue);
			break;
		case	conIntValueFormatDaXieShiLiuZhi:	// 大写十六进制
			sprintf(value,"%0X",intValue);
			break;
		default:
			sprintf(value,"%d",intValue);
			break;
	}
	return(strlen(value));
}

// 2010/5/12,王纯军增加
/*
功能
	复制一个串，如果源串比想复制的长度短，则左补位
输入参数：
	oriStr		源串
	expectedFldLen	期待复制的长度,如果该值<=0，则全部复制
	patchChar	补位字符
	sizeOfBuf	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>=0	成功,域的总数目
	<0	错误码
*/
int UnionCopyStrWithLeftPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf)
{
	int	oriLen;
	int	offset;
	
	if (oriStr == NULL)	// 参数串不能为空
	{
		UnionUserErrLog("in UnionCopyStrWithLeftPatch:: oriStr cannot be null!\n");
		return(errCodeParameter);
	}
	oriLen = strlen(oriStr);
	if (expectedFldLen <= 0)
		offset = 0;
	else
	{
		if ((offset = expectedFldLen - oriLen) < 0)
		{
			offset = 0;
			oriLen = expectedFldLen;
		}
	}
	if (offset + oriLen >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionCopyStrWithLeftPatch:: sizeOfBuf [%d] < expected [%d]!\n",sizeOfBuf,offset+oriLen);
		return(errCodeSmallBuffer);
	}
	memset(desStr,patchChar,offset);
	memcpy(desStr+offset,oriStr,oriLen);
	return(offset+oriLen);
}

// 2010/5/12,王纯军增加
/*
功能
	复制一个串，如果源串比想复制的长度短，则右补位
输入参数：
	oriStr		源串
	expectedFldLen	期待复制的长度,如果该值<=0，则全部复制
	patchChar	补位字符
	sizeOfBuf	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>=0	成功,域的总数目
	<0	错误码
*/
int UnionCopyStrWithRightPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf)
{
	int	oriLen;
	int	offset;
	
	if (oriStr == NULL)	// 参数串不能为空
	{
		UnionUserErrLog("in UnionCopyStr:: oriStr cannot be null!\n");
		return(errCodeParameter);
	}
	oriLen = strlen(oriStr);
	if (expectedFldLen > 0)
	{
		if (oriLen > expectedFldLen)
			oriLen = expectedFldLen;
		offset = expectedFldLen - oriLen;
	}
	else
	{
		offset = 0;
		expectedFldLen = oriLen;
	}
	if (offset + oriLen >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionCopyStr:: sizeOfBuf [%d] < expected [%d]!\n",sizeOfBuf,offset+oriLen);
		return(errCodeSmallBuffer);
	}
	memcpy(desStr,oriStr,oriLen);
	memset(desStr+offset,patchChar,expectedFldLen - oriLen);
	return(offset+oriLen);
}

// 2010/3/6
/* 读出文件名的后缀
输入参数
	fileName	文件全名
	lenOfFileName	文件全名的长度
	withDot		是否输出点
			1输出
			0不输出
输出参数
	suffix		读出的后缀
返回值：
	>=0 		后缀的长度
	<0		出错代码

*/
int UnionReadSuffixOfFileName(char *fileName,int lenOfFileName,int withDot,char *suffix)
{
	char	tmpBuf[512];
	int	len = 0;
	int	index;
	
	//UnionLog("fileName = [%s] lenOfFileName = [%04d]\n",fileName,lenOfFileName);
	for (index = lenOfFileName - 1; index >= 0; index--,len++)
	{
		if (fileName[index] != '.')
			tmpBuf[len] = fileName[index];
		else
		{
			if (withDot)
			{
				tmpBuf[len] = fileName[index];
				len++;
			}
			break;
		}
	}
	for (index = 0; index < len; index++)
	{
		suffix[index] = tmpBuf[len-index-1];
	}
	suffix[len] = 0;
	return(len);
}
/*
将一个SQL的where语句拆分成一个一个单词
*/
int UnionReadSQLWhereWordFromStr(char *str,int lenOfStr,char wordGrp[][128],int maxNum)
{
	int	index;
	int	wordNum = 0;
	int	wordCharIndex = 0;
	int	isWordType = 0;
	
	//UnionProgramerLog("in UnionReadSQLWhereWordFromStr:: === inStr = [%s] lenOfStr = %d maxNum = %d !\n", str, lenOfStr, maxNum);
	for (index = 0; (index < lenOfStr) && (wordNum < maxNum); index++)
	{
		if (isWordType == conWordAsStr)	// 当前的word是字符串
		{
			wordGrp[wordNum][wordCharIndex] = str[index];
			wordCharIndex++;
			if (str[index] == 0x27)	// == ',字符串结束
			{
				wordGrp[wordNum][wordCharIndex] = 0;
				wordNum++;
				wordCharIndex = 0;
				isWordType = conWordAsName;
			}
			continue;
		}
		if (isWordType == conWordAsSeperator)	// 当前的word是分隔符
		{
			switch (str[index])
			{
				case	' ':	
				case	0x09:	// 是tab键
					// 仍然是分隔符
					wordGrp[wordNum][wordCharIndex] = str[index];
					wordCharIndex++;
					continue;
				default:
					break;
			}
			// 本单词结束了			
			wordGrp[wordNum][wordCharIndex] = 0;
			// 是下一个单词
			wordNum++;
			wordCharIndex = 0;
		}
		switch (str[index])
		{
			case	' ':	
			case	0x09:	// 是tab键
			case	'=':
			case	'|':
			// added 2012-03-13
			case	'(':
			case	')':
			// end of addition 2012-03-13
				// 当前单词结束
				if (wordCharIndex != 0)
				{
					wordGrp[wordNum][wordCharIndex] = 0;
					wordNum++;
					wordCharIndex = 0;
				}
				break;
		}
		if (wordNum >= maxNum)
			break;
		wordGrp[wordNum][wordCharIndex] = str[index];
		wordCharIndex++;
		//UnionProgramerLog("in UnionReadSQLWhereWordFromStr:: ====wordGrp[%d] = [%s] !\n", wordNum , wordGrp[wordNum]);
		switch (str[index])
		{
			case	' ':	
			case	0x09:	// 是tab键
				isWordType = conWordAsSeperator;
				continue;
			case	'=':
			case	'|':
			// added 2012-03-13
			case	'(':
			case	')':
			// end of addition 2012-03-13
				// 开始下一个单词
				wordGrp[wordNum][wordCharIndex] = 0;
				wordNum++;
				wordCharIndex = 0;
				isWordType = conWordAsCalculateChar;
				continue;
			case	0x27:	// 是字符串开始
				isWordType = conWordAsStr;
				continue;
			default:	// 是单词
				isWordType = conWordAsName;
				continue;
		}
	}
	if (index != lenOfStr)
	{
		UnionUserErrLog("in UnionReadSQLWhereWordFromStr:: lenOfStr = [%04d][%s] too long!\n",lenOfStr,str);
		return(errCodeParameter);
	}
	wordGrp[wordNum][wordCharIndex] = 0;
	if (wordCharIndex > 0)
	{
		wordNum++;
	}
	return(wordNum);
}

int UnionSetTBLFldPrefixAttr(char *tblName)
{
	char	varName[128];

	sprintf(varName,"%sNoFldPrefix",tblName);
	if (UnionReadIntTypeRECVar(varName) > 0)
		gunionTBLFldWithoutPrefix = 1;
	else
		gunionTBLFldWithoutPrefix = 0;
	return(0);
}

int UnionIsTBLFldWithoutPrefix(char *tblName)
{
	UnionSetTBLFldPrefixAttr(tblName);
	return(gunionTBLFldWithoutPrefix);
}

// 2008/10/13
/* 从目录中读取一个文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	fileName	读出的文件名
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionReadFullFileNameFromFullDir(char *dir,int lenOfDir,char *fileName)
{
	char	*ptr = NULL;

	if ((dir == NULL) || (lenOfDir < 0) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionReadFullFileNameFromFullDir:: wrong parameter! lenOfDir = [%d]\n",lenOfDir);
		return(errCodeParameter);
	}
	if ((ptr = strrchr(dir,'/')) == NULL)
		strcpy(fileName,dir);
	else
		strcpy(fileName,ptr + 1);
	return(strlen(fileName));
}


// 判断一个目录是否是另一个目录的子目录
/*
输入参数
	childDir		子目录
	fullDir			父目录
输出参数
	无
返回值
	1			是
	0			不是
	<0			错误码
*/
int UnionThisDirIsFirstChildDirOfSpecDir(char *childDir,char *fullDir)
{
	char	dir1[512],dir2[512];
	int	dir1Len,dir2Len;
	
	// 读目录
	if ((dir1Len = UnionReadDirFromStr(childDir,-1,dir1)) < 0)
	{
		UnionUserErrLog("in UnionThisDirIsFirstChildDirOfSpecDir:: UnionReadDirFromStr [%s]!\n",childDir);
		return(dir1Len);
	}
	// 读目录
	if ((dir2Len = UnionReadDirFromStr(fullDir,-1,dir2)) < 0)
	{
		UnionUserErrLog("in UnionThisDirIsFirstChildDirOfSpecDir:: UnionReadDirFromStr [%s]!\n",fullDir);
		return(dir2Len);
	}
	if (strncmp(dir1,dir2,dir1Len) != 0)
		return(0);
	if ((dir2[dir1Len] == 0) || (dir2[dir1Len] == '/'))
		return(1);
	else
		return(0);
}

// 从一个文件全名中折分出目录和文件名
/*
输入参数
	fullFileName		包含了路径的文件全名
	lenOfFullFileName	文件全名的长度
输出参数
	dir			文件目录，去掉了最后的/
	fileName		文件名
返回值
	>=0			成功
	<0			失败
*/
int UnionGetFileDirAndFileNameFromFullFileName(char *fullFileName,int lenOfFullFileName,char *dir,char *fileName)
{
	int	dirLen;
	int	fileLen;
	
	// 读目录
	if ((dirLen = UnionReadDirFromStr(fullFileName,-1,dir)) < 0)
	{
		UnionUserErrLog("in UnionGetFileDirAndFileNameFromFullFileName:: UnionReadDirFromStr [%s]!\n",fullFileName);
		return(dirLen);
	}
	//UnionLog("dir = [%s]\n",dir);
	// 读文件名
	if ((fileLen = UnionReadFullFileNameFromFullDir(fullFileName,lenOfFullFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetFileDirAndFileNameFromFullFileName:: UnionReadFullFileNameFromFullDir [%s] ret = [%d]!\n",fullFileName,fileLen);
		return(fileLen);
	}
	dir[dirLen-fileLen] = 0;
	dirLen = strlen(dir);
	if ((dirLen > 1) && (dir[dirLen-1] == '/'))
		dir[dirLen-1] = 0;
	return(0);
}

// 2009/6/23，张永定增加
/****************************
功能
	把一个域名转换成带一个带前缀的域名。
输入参数
	fldName		转换前域名
输出参数
	prefixFldName	转换后增加前缀的域名
返回值
	>=0	成功
	<0	错误码
*****************************/
int UnionChargeFieldNameToDBFieldName(char *fldName,char *prefixFldName)
{
	char	*ptr = NULL;

	if (gunionTBLFldWithoutPrefix)
		strcpy(prefixFldName,fldName);
	else
	{	
		if ((ptr = UnionGetPrefixOfDBField()) == NULL)
			strcpy(prefixFldName,fldName);
		else
			sprintf(prefixFldName,"%s%s",ptr,fldName);
	}
	//UnionProgramerLog("in UnionChargeFieldNameToDBFieldName:: oriFldName = [%s] destFldname = [%s]!\n", fldName, prefixFldName);
	
	return(0);
}

// 2009/5/3，王纯军增加
/*
功能
	判断一个串，在不在字串中
输入参数
	str		串
	lenOfStr	串长度
	speratorTag	域分隔符
	specFld		要查的串
输出参数
	无
返回值
	>0	在串中的序号
	==0	不在串在
	<0	错误码
*/
int UnionIsFldStrInUnionFldListStr(char *str,int lenOfStr,char speratorTag,char *specFld)
{
	int	len;
	int	index = 0;
	char	*ptr = NULL;

	if (lenOfStr == 0)
		return(0);

	if ((specFld == NULL) || (len = strlen(specFld)) == 0)
		return(errCodeParameter);
	
	ptr = str;
	do
	{
		index ++;
		if ((memcmp(ptr,specFld,len) == 0) && ((ptr[len] == speratorTag) || (ptr[len] == 0)))
			return(index);
	}while(((ptr = strchr(ptr,speratorTag)) != NULL) && (++ptr != NULL));
	
	return(0);
}

// 2009/4/30,王纯军
/*
功能	根据一个缺省值定义，获取缺省值
输入参数
	无
输出参数
	无
返回值
	1	是
	0	否
*/
int UnionConvertDefaultValue(char *defaultValueDef,char *defaultValue)
{
	if ((defaultValueDef == NULL) || (defaultValue == NULL))
		return(errCodeNullPointer);
	if (strcmp(defaultValueDef,"currentDate") == 0)
		UnionGetFullSystemDate(defaultValue);
	else if (strcmp(defaultValueDef,"currentTime") == 0)
		UnionGetSystemTime(defaultValue);
	else if (strcmp(defaultValueDef,"currentDateTime") == 0)
		UnionGetFullSystemDateTime(defaultValue);
	else
		strcpy(defaultValue,defaultValueDef);
	return(strlen(defaultValue));
}

// 2008/12/17,王纯军
/*
功能	判断域名是否采用大写
输入参数
	无
输出参数
	无
返回值
	1	是
	0	否
*/
int UnionIsFldNameUseUpperCase()
{
	return(gunionIsDBFldNameUseUpperCase);
}

// 2008/12/17,王纯军
/*
功能	设置一个域名采用大写字母
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionSetFldNameUseUpperCase()
{
	gunionIsDBFldNameUseUpperCase = 1;
}

// 2008/12/17,王纯军
/*
功能	设置一个域名不采用大写字母
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionSetFldNameUseNonUpperCase()
{
	gunionIsDBFldNameUseUpperCase = 0;
}

// 2010/10/30,王纯军
/*
功能	拼成一个空格串
输入参数
	num		空格数量
	sizeOfStr	串大小
输出参数
	str		串指针
返回值
	>=0		成功，串长度
	<0		出错代码
*/
int UnionGenerateStrOfBlankOfNum(int num,char *str,int sizeOfStr)
{
	int	realNum;
	
	if ((str == NULL) || (sizeOfStr <= 0))
		return(errCodeParameter);

	if (num < sizeOfStr)
		realNum = num;
	else
		realNum = sizeOfStr - 1;
	if (realNum <= 0)
	{
		str[0] = 0;
		return(0);
	}
	memset(str,' ',realNum);
	if (realNum + 1 < sizeOfStr)
		str[realNum] = 0;
	else
	{
		str[realNum-1] = 0;
		realNum--;
	}
	return(realNum);	
}

// 2008/11/24,王纯军
/*
功能	打印指定数量的空格
输入参数
	num		空格数量
	fp		文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintBlankOfNumToFp(int num,FILE *fp)
{
	int	index;

	if (fp == NULL)
		return(0);
	for (index = 0; index < num; index++)
		fprintf(fp," ");
	return(num);
}

// 2008/11/11,王纯军
/*
功能	将一个串拼装成科友的记录域串
输入参数
	oriStr	 	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateRecFldStrIntoStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
	return(UnionRestoreSpecCharInStr('.',oriStr,lenOfOriStr,desStr,sizeOfDesStr));
	/*
	int	index;
	int	desLen = 0;
	char	tmpBuf[4096];

	if (lenOfOriStr >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionTranslateRecFldStrIntoStr:: oriLen [%d] too long!\n",lenOfOriStr);
		return(errCodeSmallBuffer);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if ((oriStr[index] == '^') && (index + 1 < lenOfOriStr))
		{
			switch (oriStr[index+1])
			{
				case	'^':
					tmpBuf[desLen] = '.';
					desLen++;
					index++;
					continue;
				case	'*':
					tmpBuf[desLen] = '^';
					desLen++;
					index++;
					continue;
				default:
					break;
			}
		}
		tmpBuf[desLen] = oriStr[index];
		desLen++;
	}
	if (desLen >= sizeOfDesStr)
	{
		UnionUserErrLog("in UnionTranslateRecFldStrIntoStr:: sizeOfDesStr [%d] too small!\n",sizeOfDesStr);
		return(errCodeSmallBuffer);
	}
	memcpy(desStr,tmpBuf,desLen);
	desStr[desLen] = 0;
	return(desLen);
	*/
}

// 2008/11/11,王纯军
/*
功能	将一个串拼装成科友的记录域串
输入参数
	oriStr	 	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateStrIntoRecFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
	return(UnionTranslateSpecCharInStr('.',oriStr,lenOfOriStr,desStr,sizeOfDesStr));
	/*
	int	index;
	int	desLen = 0;
	char	tmpBuf[4096];

	if (lenOfOriStr >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionTranslateStrIntoRecFldStr:: oriLen [%d] too long!\n",lenOfOriStr);
		return(errCodeSmallBuffer);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (desLen+1 >= sizeof(tmpBuf))
		{
			UnionUserErrLog("in UnionTranslateStrIntoRecFldStr:: oriLen [%d] too long!\n",lenOfOriStr);
			return(errCodeSmallBuffer);
		}
		switch (oriStr[index])
		{
			case	'.':
				tmpBuf[desLen] = '^';
				desLen++;
				tmpBuf[desLen] = '^';
				desLen++;
				continue;
			case	'^':
				tmpBuf[desLen] = '^';
				desLen++;
				tmpBuf[desLen] = '*';
				desLen++;
				continue;
			default:
				tmpBuf[desLen] = oriStr[index];
				desLen++;
				continue;
		}
	}
	if (desLen >= sizeOfDesStr)
	{
		UnionUserErrLog("in UnionTranslateStrIntoRecFldStr:: sizeOfDesStr [%d] too small!\n",sizeOfDesStr);
		return(errCodeSmallBuffer);
	}
	memcpy(desStr,tmpBuf,desLen);
	desStr[desLen] = 0;
	return(desLen);
	*/
}

// 2009/5/23,王纯军
/*
功能	恢复一个串中的指定字符
输入参数
	specChar	指定字符
	oriStr	 	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionRestoreSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
	int	index;
	int	desLen = 0;
	char	tmpBuf[8192*2];

	if (lenOfOriStr >= (int)sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionRestoreSpecCharInStr:: oriLen [%d] too long!\n",lenOfOriStr);
		return(errCodeSmallBuffer);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if ((oriStr[index] == '^') && (index + 1 < lenOfOriStr))
		{
			switch (oriStr[index+1])
			{
				case	'^':
					tmpBuf[desLen] = specChar;
					desLen++;
					index++;
					continue;
				case	'*':
					tmpBuf[desLen] = '^';
					desLen++;
					index++;
					continue;
				default:
					break;
			}
		}
		tmpBuf[desLen] = oriStr[index];
		desLen++;
	}
	if (desLen >= sizeOfDesStr)
	{
		UnionUserErrLog("in UnionRestoreSpecCharInStr:: sizeOfDesStr [%d] too small!\n",sizeOfDesStr);
		return(errCodeSmallBuffer);
	}
	memcpy(desStr,tmpBuf,desLen);
	desStr[desLen] = 0;
	return(desLen);
}

// 2009/5/23,王纯军
/*
功能	将一个串中的指定字符，作为特殊字符处理后，产生一个新串
输入参数
	specChar	指定字符
	oriStr	 	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
	int	index;
	size_t	desLen = 0;
	char	tmpBuf[8192*2];

	if (lenOfOriStr >= (int)sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionTranslateSpecCharInStr:: oriLen [%d] too long!\n",lenOfOriStr);
		return(errCodeSmallBuffer);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (desLen+1 >= sizeof(tmpBuf))
		{
			UnionUserErrLog("in UnionTranslateSpecCharInStr:: oriLen [%d] too long!\n",lenOfOriStr);
			return(errCodeSmallBuffer);
		}
		if (oriStr[index] == specChar)
		{
			tmpBuf[desLen] = '^';
			desLen++;
			tmpBuf[desLen] = '^';
			desLen++;
			continue;
		}
		if (oriStr[index] == '^')
		{
			tmpBuf[desLen] = '^';
			desLen++;
			tmpBuf[desLen] = '*';
			desLen++;
			continue;
		}
		tmpBuf[desLen] = oriStr[index];
		desLen++;
		continue;
	}
	if (desLen >= (unsigned int)sizeOfDesStr)
	{
		UnionUserErrLog("in UnionTranslateSpecCharInStr:: sizeOfDesStr [%d] too small!\n",sizeOfDesStr);
		return(errCodeSmallBuffer);
	}
	memcpy(desStr,tmpBuf,desLen);
	desStr[desLen] = 0;
	return(desLen);
}

// 2008/11/11,王纯军
/*
功能	拼装一个c语言的变量名，要将源变量名中的*移到变量最前面
输入参数
	prefix	 	变量名的前缀
	oriVarName  	源变量名
输出参数
	desVarName	拼好的变量名
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionFormCSentenceVarName(char *prefix,char *oriVarName,char *desVarName)
{
	int	index;
	int	oriLen,desLen;

	oriLen = strlen(oriVarName);
	for (index = 0; index < oriLen; index++)
	{
		if (oriVarName[index] == '*')
			desVarName[index] = '*';
		else
			break;
	}
	desLen = index;
	if (prefix != NULL)
	{
		strcpy(desVarName+desLen,prefix);
		desLen = strlen(desVarName);
	}
	if (index < oriLen)
		strcpy(desVarName+desLen,oriVarName+index);
	return(strlen(desVarName));
}

// 2008/10/23,王纯军
/*
功能：
	拼装一个空值串
输入参数：
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
	sizeOfRecStr	域串缓冲的大小
输出参数：
	recStr		域串
返回值：
	>=0：成功，返回域串的长度
	<0：失败，错误码
*/
int UnionGenerateNullValueRecStr(char *fldGrp,int lenOfFldGrp,char *recStr,int sizeOfRecStr)
{
	char	fldNameGrp[64][128];
	int	fldNum;
	char	tmpBuf[1024];
	int	index;
	int	offset = 0;
	int	ret;

	if ((fldGrp == NULL) || (lenOfFldGrp <= 0))
	{
		return(0);
	}
	if ((fldNum = UnionSeprateVarStrIntoArray(fldGrp,lenOfFldGrp,',',(char *)&fldNameGrp,64,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateNullValueRecStr:: UnionSeprateVarStrIntoArray [%s]\n",fldGrp);
		return(fldNum);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionPutRecFldIntoRecStr(fldNameGrp[index],tmpBuf,0,recStr+offset,sizeOfRecStr-offset)) < 0)
			continue;
		offset += ret;
	}
	recStr[offset] = 0;
	UnionDebugLog("in UnionGenerateNullValueRecStr:: recStr = [%04d][%s]\n",offset,recStr);
	return(offset);
}

// 2008/10/23,王纯军
/*
功能：
	从记录串中读出指定域
输入参数：
	recStr		记录串
	lenOfRecStr	记录串长度
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
	sizeOfFldStr	域串缓冲的大小
输出参数：
	fldStr		域串
返回值：
	>=0：成功，返回域串的长度
	<0：失败，错误码
*/
int UnionReadFldGrpFromRecStr(char *recStr,int lenOfRecStr,char *fldGrp,int lenOfFldGrp,char *fldStr,int sizeOfFldStr)
{
	char	fldNameGrp[64][128];
	int	fldNum;
	char	tmpBuf[1024];
	int	index;
	int	len;
	int	offset = 0;
	int	ret;

	if ((recStr == NULL) || (lenOfRecStr < 0) || (fldStr == NULL))
		return(errCodeParameter);

	if ((fldGrp == NULL) || (lenOfFldGrp <= 0))
	{
		if (sizeOfFldStr > lenOfRecStr)
		{
			memcpy(fldStr,recStr,lenOfRecStr);
			return(lenOfRecStr);
		}
		UnionUserErrLog("in UnionReadFldGrpFromRecStr:: sizeOfFldStr [%d] is too small!\n",sizeOfFldStr);
		return(errCodeSmallBuffer);
	}
	//UnionDebugLog("in UnionReadFldGrpFromRecStr:: fldGrp = [%04d][%s]\n",lenOfFldGrp,fldGrp);
	if ((fldNum = UnionSeprateVarStrIntoArray(fldGrp,lenOfFldGrp,',',(char *)&fldNameGrp,64,128)) < 0)
	{
		UnionUserErrLog("in UnionReadFldGrpFromRecStr:: UnionSeprateVarStrIntoArray [%s]\n",fldGrp);
		return(fldNum);
	}
	for (index = 0; index < fldNum; index++)
	{
		//memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldNameGrp[index],tmpBuf,sizeof(tmpBuf))) < 0)
		{
			len = 0;
			//continue;
		}
		tmpBuf[len] = 0;
		if ((ret = UnionPutRecFldIntoRecStr(fldNameGrp[index],tmpBuf,len,fldStr+offset,sizeOfFldStr-offset)) < 0)
			continue;
		offset += ret;
	}
	fldStr[offset] = 0;
	UnionDebugLog("in UnionReadFldGrpFromRecStr:: fldStr = [%04d][%s]\n",offset,fldStr);
	return(offset);
}


// 2008/10/20,王纯军
// 尽量少用，可以使用UnionSeprateVarStrIntoArray替代
/*
函数功能：按分隔符拆串
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	maxNumOfVar	折分的字符串可有的最大数目
输出参数
	varGrp		折分的字符串变量
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoVarGrp(char *varStr, int lenOfVarStr,char oper,char varGrp[][128],int maxNumOfVar)
{
	int	varNum = 0;
	int	varLen = 0;
	int	len;
	char	*ptr = NULL;
	char	*tmp = NULL;

	if ((varStr == NULL) || (varGrp == NULL) || (lenOfVarStr < 0))
		return(errCodeParameter);

	if (lenOfVarStr == 0)
		return(0);
	
	ptr = varStr;
	len = lenOfVarStr;
	while (ptr && len > 0)
	{
		if ((tmp = memchr(ptr,oper,len)) != NULL)
			varLen = tmp - ptr;
		else
			varLen = len;
			
		if ((len -= varLen) < 0)
			varLen += len;
		
		if (varLen >= (int)sizeof(varGrp[varNum]))
		{
			UnionUserErrLog("in UnionSeprateVarStrIntoVarGrp:: varLen = [%d] of [%zu] in [%s] too long!\n",varLen,sizeof(varGrp[varNum]),varStr);
			return(errCodeParameter);
		}

		memcpy(varGrp[varNum],ptr,varLen);
		varGrp[varNum][varLen] = 0;

		if (++varNum >= maxNumOfVar)
			return(varNum);

		if (tmp)
		{
			len -= 1;
			ptr = tmp + 1;
		}
		else
			break;
	}
	return(varNum);
}

// 2015/1/21,张永定
/*
函数功能：按分隔符拆串，用于替代UnionSeprateVarStrIntoVarGrp
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	numOfArray	数组的一维
	sizeOfArray	数组的二维
输出参数
	arrayPtr	数组的首地址
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoArray(char *varStr, int lenOfVarStr,char oper,char *arrayPtr,int numOfArray,int sizeOfArray)
{
	int	varNum = 0;
	int	varLen = 0;
	int	len;
	char	*ptr = NULL;
	char	*tmp = NULL;

	if ((varStr == NULL) || (lenOfVarStr < 0) || (arrayPtr == NULL) || (numOfArray <= 0) || (numOfArray <= 0))
		return(errCodeParameter);

	if (lenOfVarStr == 0)
		return(0);

	ptr = varStr;
	len = lenOfVarStr;
	while (ptr && len > 0)
	{
		if ((tmp = memchr(ptr,oper,len)) != NULL)
			varLen = tmp - ptr;
		else
			varLen = len;
			
		if ((len -= varLen) < 0)
			varLen += len;
		
		if (varLen >= sizeOfArray)
		{
			UnionUserErrLog("in UnionSeprateVarStrIntoArray:: varLen = [%d] of [%d] in [%s] too long!\n",varLen,sizeOfArray,varStr);
			return(errCodeParameter);
		}

		memcpy(arrayPtr,ptr,varLen);		
		arrayPtr[varLen] = 0;
		arrayPtr += sizeOfArray;
	
		if (++varNum >= numOfArray)
			return(varNum);

		if (tmp)
		{
			len -= 1;
			ptr = tmp + 1;
		}
		else
			break;
	}
	return(varNum);
}

// 2015/1/21,张永定
/*
函数功能：按分隔符拆串，源串varStr会改变，
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	maxNumOfVar	折分的字符串可有的最大数目
输出参数
	varPtr		折分的字符串指针的指针
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoPtr(char *varStr, int lenOfVarStr,char oper,char **varPtr,int maxNumOfVar)
{
	int	varNum = 0;
	int	len = 0;
	char	*ptr = NULL;
	char	*tmp = NULL;

	if ((varStr == NULL) || (varPtr == NULL) || (lenOfVarStr < 0))
		return(errCodeParameter);

	if (lenOfVarStr == 0)
		return(0);

	ptr = varStr;
	len = lenOfVarStr;
	while (ptr && len > 0)
	{
		varPtr[varNum] = ptr;

		if (++varNum >= maxNumOfVar)
			return(varNum);

		if ((tmp = strchr(ptr,oper)) == NULL)
			break;
			
		len -= (tmp - ptr + 1);
		*tmp = 0;
		ptr = tmp + 1;
	}
		
	return(varNum);
}

// 2008/10/22
/* 过滤掉指定级别的目录
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
	dirNum		过滤掉的目录级数
输出参数
	desDir		目标目录
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionFilterDirFromDirStr(char *dir,int lenOfDir,int dirNum,char *desDir)
{
	char	*ptr;
	int	currentDirLevel = 0;

	if ((dir == NULL) || (lenOfDir < 0) || (desDir == NULL))
		return(errCodeParameter);

	ptr = dir;
loop:
	if (currentDirLevel >= dirNum)
	{
		strcpy(desDir,ptr);
		return(strlen(desDir));
	}
	if (ptr >= dir + lenOfDir)
		return(0);
	//strcpy(tmpBuf,ptr);
	if ((ptr = strstr(ptr,"/")) == NULL)	// 已不是目录
		return(0);
	currentDirLevel++;
	ptr++;
	goto loop;
}

// 2008/10/13
/* 从目录中读取一个文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	fileName	读出的文件名
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionReadFileNameFromFullDir(char *dir,int lenOfDir,char *fileName)
{
	char	tmpBuf[512];
	char	*ptr;
	int	len;

	if ((dir == NULL) || (lenOfDir < 0) || (fileName == NULL))
		return(errCodeParameter);
	//UnionLog("dir = [%s]\n",dir);
	ptr = dir;
	//UnionLog("dir = [%s]\n",ptr);
loop:
	if (ptr >= dir + lenOfDir)
		return(0);
	strcpy(tmpBuf,ptr);
	if ((ptr = strstr(ptr,"/")) == NULL)	// 已不是目录
	{
		for (len = strlen(tmpBuf); len > 0; len--)
		{
			if (tmpBuf[len-1] == '.')
			{
				tmpBuf[len-1] = 0;
				break;
			}
		}
		strcpy(fileName,tmpBuf);
		return(len);
	}
	ptr += 1;
	//UnionLog("*dir = [%s]\n",ptr);
	goto loop;
}

// 2008/10/13
// 判断一个字符是否是数字
int UnionIsDigitChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return(1);
	else
		return(0);
}

// 判断一个字符串是否全是数字
int UnionIsDigitString(char *str)
{
	char    *ptr;

	if (str == NULL)
		return 0;

	ptr = str;
	while(*ptr != '\0')
	{
		if (!UnionIsDigitChar(*ptr))
			return 0;
		ptr++;
	}
	return 1;
}


// 2008/10/13
// 判断一个字符是否是字母
int UnionIsAlphaChar(char ch)
{
	if ((ch >= 'a') && (ch <= 'z'))
		return(1);
	else if ((ch >= 'A') && (ch <= 'Z'))
		return(1);
	else
		return(0);
}

//2014/5/4
//add by tengyx判断一个字符串是否为大写十六进制
int UnionIsUppercaseHEX(char *str)
{
	int i;
	
	for(i = 0;str[i] != '\0';i++)
	{
		if(str[i]>='0' && str[i]<='9')
			continue;
		else if ((str[i] >= 'A') && (str[i] <= 'F'))
			continue;
		else
			return(-1);
	}
	return 1;
}

// 2008/10/13
/* 从一个字符串中读取一个C语言的变量名字
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	varName		读出的名字
返回值：
	>=0 	返回名字在字符串中占的长度
	<0	出错代码

*/
int UnionReadCProgramVarNameFromStr(char *str,int lenOfStr,char *varName)
{
	int	index;
	int	offset = 0;

	for (index = 0; index < lenOfStr; index++)
	{
		if ((str[index] == ' ') || (str[index] == 9))
		{
			if (offset == 0)
				continue;
			else
				break;
		}
		if (UnionIsAlphaChar(str[index]) || UnionIsDigitChar(str[index]) || (str[index] == '_'))
		{
			if ((index == 0) && (!UnionIsAlphaChar(str[index])))	// 第一个字符不是字母
				return(errCodeParameter);
			varName[offset] = str[index];
			offset++;
		}
		else
			break;
	}
	return(index);
}

// 2008/10/13
/* 从一个字符串中读取所有C语言的变量名字
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
	maxNumOfVar	最大的变量名字
输出参数
	varName		读出的名字
返回值：
	>=0 		读出的变量名字的数量
	<0		出错代码

*/
int UnionReadAllCProgramVarNameFromStr(char *str,int lenOfStr,char varName[][128],int maxNumOfVar)
{
	int	num = 0;
	int	offset = 0;
	int	len;

	while ((offset < lenOfStr) && (num < maxNumOfVar))
	{
		memset(varName[num],0,sizeof(varName[num]));
		if ((len = UnionReadCProgramVarNameFromStr(str+offset,lenOfStr-offset,varName[num])) < 0)
		{
			UnionUserErrLog("in UnionReadAllCProgramVarNameFromStr:: UnionReadCProgramVarNameFromStr!\n");
			return(len);
		}
		if (len == 0)
			break;
		num++;
		offset += len;
	}
	return(num);
}

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutCharTypeRecFldIntoRecStr(char *fldName,char value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[32];
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutCharTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	snprintf(buf,sizeof(buf),"%c",value);
	lenOfValue = strlen(buf);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutCharTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutDoubleTypeRecFldIntoRecStr(char *fldName,double value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[32];
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutDoubleTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	snprintf(buf,sizeof(buf),"%lf",value);
	lenOfValue = strlen(buf);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutDoubleTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutLongTypeRecFldIntoRecStr(char *fldName,long value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[32];
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutLongTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	snprintf(buf,sizeof(buf),"%ld",value);
	lenOfValue = strlen(buf);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutLongTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutLongLongTypeRecFldIntoRecStr(char *fldName,long long value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[32];
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutLongLongTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

#ifdef _WIN32	
	snprintf(buf,sizeof(buf),"%I64d",value);
#else
	snprintf(buf,sizeof(buf),"%lld",value);
#endif
	lenOfValue = strlen(buf);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutLongTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}


// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutStringTypeRecFldIntoRecStr(char *fldName,char *value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL) || (value == NULL))
	{
		UnionUserErrLog("in UnionPutIntTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	lenOfValue = strlen(value);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutIntTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,value);
	return(fldTagLen+lenOfValue);
}

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutIntTypeRecFldIntoRecStr(char *fldName,int value,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[32];
	int	lenOfValue;

	if ((recStr == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutIntTypeRecFldIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	snprintf(buf,sizeof(buf),"%d",value);
	lenOfValue = strlen(buf);

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutIntTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}

// 2008/10/5，增加
/*
功能	从文件中读取一个数据行，过滤掉注释行
输入参数
	fp		文件名柄
	sizeOfBuf	缓冲区大小
输出参数
	buf		读出的数据
返回值
	>= 0		读出的数据的长度
	<0		出错代码
*/
int UnionReadOneDataLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf)
{
	int	lineLen;

	while (!feof(fp))
	{
		if (((lineLen = UnionReadOneLineFromTxtFile(fp,buf,sizeOfBuf)) < 0) && (lineLen != errCodeEnviMDL_NullLine))
		{
			UnionUserErrLog("in UnionReadOneDataLineFromTxtFile:: UnionReadOneLineFromTxtFile!\n");
			return(lineLen);
		}
		buf[lineLen] = 0;
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(buf))	// 注释行
			continue;
		//UnionLog("in UnionReadOneDataLineFromTxtFile:: [%04d][%s]\n",lineLen,buf);
		return(lineLen);
	}
	return(errCodeFileEnd);
}

// 2008/10/5，增加
/*
功能	判断一个名字是否是合法的c语言变量名字
输入参数
	name		名字
	lenOfName	名字长度
输出参数
	无
返回值
	1		是
	0		不是
*/
int UnionIsValidCProgramName(char *name,int lenOfName)
{
	int	index;

	if ((name == NULL) || (lenOfName <= 0))
		return(0);
	for (index = 0; index < lenOfName; index++)
	{
		if (isdigit(name[index]))
			continue;
		else if (!isalpha(name[index]))
			continue;
		else if (name[index] == '_')
			continue;
		else
			return(0);
	}
	return(1);
}

// 2008/7/29，增加
/*
功能	使用通用的域分隔符替换指定的分隔符
输入参数
	oriStr		串
	lenOfOriStr	串长度
	oriFldSeperator	源分隔符
	desFldSeperator	目标分隔符
	sizeOfDesStr	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>= 0		目标串的长度
	<0		出错代码
*/
int UnionConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr)
{
	int	index;

	if ((oriStr == NULL) || (desStr == NULL) || (lenOfOriStr >= sizeOfDesStr) || (lenOfOriStr < 0))
	{
		UnionUserErrLog("in UnionConvertOneFldSeperatorInRecStrIntoAnother:: parameter error! oriStr=%p,desStr=%p,lenOfOriStr=%d,sizeOfDesStr=%d\n",
				oriStr,desStr,lenOfOriStr,sizeOfDesStr);
		return(errCodeParameter);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (oriStr[index] != oriFldSeperator)
			desStr[index] = oriStr[index];
		else
			desStr[index] = desFldSeperator;
	}
	desStr[lenOfOriStr] = 0;
	return(lenOfOriStr);
}

// 替换指定数量的符号(从左往右)
int UnionConvertOneFldSeperatorInRecStrIntoAnotherNum(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,int num,char *desStr,int sizeOfDesStr)
{
	int	index;
	int	realNum=0;

	if ((oriStr == NULL) || (desStr == NULL) || (lenOfOriStr >= sizeOfDesStr) || (lenOfOriStr < 0))
	{
		UnionUserErrLog("in UnionConvertOneFldSeperatorInRecStrIntoAnother:: parameter error!\n");
		return(errCodeParameter);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (oriStr[index] != oriFldSeperator)
			desStr[index] = oriStr[index];
		else
		{
			if (num>0 && realNum >= num)
				continue;
			desStr[index] = desFldSeperator;
			realNum++;
		}
	}
	desStr[lenOfOriStr] = 0;
	return(lenOfOriStr);
}

// 2008/7/24，增加
/*
功能	使用通用的域分隔符替换指定的分隔符
输入参数
	oriStr		串
	lenOfOriStr	串长度
	fldSeperator	分隔符
	sizeOfDesStr	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>= 0		目标串的长度
	<0		出错代码
*/
int UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(char *oriStr,int lenOfOriStr,char fldSeperator,char *desStr,int sizeOfDesStr)
{
	int	index;

	if ((oriStr == NULL) || (desStr == NULL) || (lenOfOriStr >= sizeOfDesStr) || (lenOfOriStr < 0))
	{
		UnionUserErrLog("in UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator:: parameter error!\n");
		return(errCodeParameter);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (oriStr[index] != fldSeperator)
			desStr[index] = oriStr[index];
		else
			desStr[index] = '|';
	}
	desStr[lenOfOriStr] = 0;
	return(lenOfOriStr);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value)
{
	char	tmpVar[32];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadCharTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}
	*value = tmpVar[0];
	return(0);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value)
{
	char	tmpVar[32];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadDoubleTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}
	tmpVar[ret] = 0;
	*value = atof(tmpVar);
	return(0);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value)
{
	char	tmpVar[32];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadLongTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}
	tmpVar[ret] = 0;
	*value = atol(tmpVar);
	return(0);
}
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadLongLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int64_t  *value)
{
	char	tmpVar[32];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadLongLongTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}
	tmpVar[ret] = 0;
#ifdef _WIN32
	sscanf(tmpVar,"%I64d",value);
#else
	*value = atoll(tmpVar);
#endif
	return(0);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value)
{
	char	tmpVar[32];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadIntTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(ret);
	}
	tmpVar[ret] = 0;
	*value = atoi(tmpVar);
	return(0);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionExistsRecFldInRecStr(char *recStr,int lenOfRecStr,char *fldName)
{
	char	tmpVar[2048];
	int     ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		//UnionUserErrLog("in UnionReadIntTypeRecFldFromRecStr:: UnionReadRecFldFromRecStr!\n");
		return(0);
	}
	return(1);
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf)
{	
	return(UnionReadRecFldFromRecStrForCommon(recStr,lenOfRecStr,fldName,value,sizeOfBuf,0));
}

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadBitsTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,unsigned char *value,int sizeOfBuf)
{
	return(UnionReadRecFldFromRecStrForCommon(recStr,lenOfRecStr,fldName,(char *)value,sizeOfBuf,1));
}

// 2011-12-27，王纯军修改了这个函数，以支持字符串中包括|
// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
/*
int UnionReadRecFldFromRecStrForCommon(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf,int isBitsData)
{
	char	*ptr;
	int	lenOfValue;
	char	fldTag[100];
	char    tmpFldName[256];
	int	realLen = 0;
	int	leftLen = 0;
	char	*str;
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionReadRecFldFromRecStrForCommon:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	if (lenOfRecStr == 0)
	{
		//UnionLog("in UnionReadRecFldFromRecStrForCommon:: fld [%s] not exists!\n",fldName);
		return(errCodeEsscMDL_FldNotExists);
	}

	if (UnionIsFldNameUseUpperCase())
	{
		snprintf(tmpFldName,sizeof(tmpFldName),"%s",fldName);
		UnionToUpperCase(tmpFldName);
		str = tmpFldName;
	}
	else
		str = fldName;
	
	recStr[lenOfRecStr] = 0;
	snprintf(fldTag,sizeof(fldTag),"%s=",str);
	if (strncmp(recStr,fldTag,strlen(fldTag)) == 0 )
	{
		ptr = recStr;
	}
	else
	{
		sprintf(fldTag,"|%s=",str);
		if ((ptr = strstr(recStr,fldTag)) == NULL)
		{
			//UnionLog("in UnionReadRecFldFromRecStrForCommon:: fld [%s] not exists!\n",fldName);
			return(errCodeEsscMDL_FldNotExists);
		}
	}
	if (value == NULL)	// 不需要读数据
		return(0);
	ptr = ptr + strlen(fldTag);
	//for (lenOfValue = 0; lenOfValue < lenOfRecStr; lenOfValue++)
	for (lenOfValue = 0,leftLen = strlen(ptr); lenOfValue < leftLen; lenOfValue++)
	{
		if (ptr[lenOfValue] == '|')
		{
			if (lenOfValue + 1 < leftLen)	// >= leftLen，则已到字符串结束
			{
				// 没有到字符串结束
				if (ptr[lenOfValue+1] == '|')	// 当前的|可能是转义字符，如果后续字符还是|，则是转义字符
				{
					lenOfValue++;	// 取后一个字符|，因为它才是真正的值
					goto assignValue;
				}
			}
			break;
		}
		// 读取当前字符
assignValue:
		if (realLen >= sizeOfBuf)
		{
			UnionUserErrLog("in UnionReadRecFldFromRecStrForCommon:: sizeOfBuf = [%d] <= expected [%d] for fld [%s] in [%04d][%s]!\n",
				sizeOfBuf,realLen,fldName,lenOfRecStr,recStr);
			return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
		}
		value[realLen] = ptr[lenOfValue];
		realLen++;
	}
	if (isBitsData)
		goto readBits;
	value[realLen] = 0;
	UnionFilterHeadAndTailBlank(value);
	//return(UnionTranslateRecFldStrIntoStr(value,strlen(value),value,sizeOfBuf));
	return(strlen(value));

readBits:
	if (sizeOfBuf * 2 < lenOfValue)
	{
		UnionUserErrLog("in UnionReadBitsTypeRecFldFromRecStr:: sizeOfBuf [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfBuf,lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}
	aschex_to_bcdhex(ptr,lenOfValue,value);
	value[lenOfValue/2] = 0;
	return(lenOfValue/2);
}
*/
int UnionReadRecFldFromRecStrForCommon(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf,int isBitsData)
{
	int	ret;
	int	offset = 0;
	int	len = 0,realLen;
	char	*ptr = NULL;
	char	*ptr1 = NULL;
	char	tmpFldName[128];

	if ((recStr == NULL) || (lenOfRecStr < 0) || (fldName == NULL) || ((len = strlen(fldName)) == 0))
	{
		UnionUserErrLog("in UnionReadRecFldFromRecStrForCommon:: null pointer or lenOfRecStr = [%d] len = [%d]!\n",lenOfRecStr,len);
		return(errCodeParameter);
	}

	if (lenOfRecStr == 0)
	{
		//UnionLog("in UnionReadRecFldFromRecStrForCommon:: fld [%s] not exists!\n",fldName);
		return(errCodeEsscMDL_FldNotExists);
	}

	ptr = recStr;
	do
	{
		if (UnionIsFldNameUseUpperCase())
		{
			snprintf(tmpFldName,sizeof(tmpFldName),"%s",fldName);
			UnionToUpperCase(tmpFldName);
			ret = memcmp(ptr,tmpFldName,len);
		}
		else
			ret = memcmp(ptr,fldName,len);
		
		if ((ret == 0) && (ptr[len] == '='))
		{
			ptr += (len + 1);
			while(1)
			{
				if ((ptr1 = strchr(ptr,'|')) == NULL)
				{
					realLen = strlen(ptr);
					break;
				}
				else
				{
					realLen = ptr1 - ptr;
					if (ptr1[1] != '|')	// 当前的|可能是转义字符，如果后续字符还是|，则是转义字符					
						break;
					else
					{
						realLen += 1;						
						if (isBitsData)
						{
							if (realLen >= (sizeOfBuf - offset) * 2)
							{
								UnionUserErrLog("in UnionReadRecFldFromRecStr:: sizeOfBuf = [%d] <= expected [%d] for fld [%s] in [%04d][%s]!\n",
									(sizeOfBuf - offset) * 2,realLen,fldName,lenOfRecStr,recStr);
								return(errCodeSmallBuffer);
							}
							aschex_to_bcdhex(ptr,realLen,value + offset);
							realLen = realLen / 2;
							offset += realLen;
						}
						else
						{
							if (realLen >= sizeOfBuf - offset)
							{
								UnionUserErrLog("in UnionReadRecFldFromRecStr:: sizeOfBuf = [%d] <= expected [%d] for fld [%s] in [%04d][%s]!\n",
									sizeOfBuf - offset,realLen,fldName,lenOfRecStr,recStr);
								return(errCodeSmallBuffer);
							}
							memcpy(value + offset,ptr,realLen);
							offset += realLen;
						}
						ptr += (realLen + 1);
					}
				}
			}
			
			if (isBitsData)
			{
				if (realLen >= (sizeOfBuf - offset) * 2)
				{
					UnionUserErrLog("in UnionReadRecFldFromRecStr:: sizeOfBuf = [%d] <= expected [%d] for fld [%s] in [%04d][%s]!\n",
						(sizeOfBuf - offset) * 2,realLen,fldName,lenOfRecStr,recStr);
					return(errCodeSmallBuffer);
				}
				aschex_to_bcdhex(ptr,realLen,value + offset);
				realLen = realLen / 2;
				realLen += offset;
			}
			else
			{
				if (realLen >= sizeOfBuf - offset)
				{
					UnionUserErrLog("in UnionReadRecFldFromRecStr:: sizeOfBuf = [%d] <= expected [%d] for fld [%s] in [%04d][%s]!\n",
						sizeOfBuf - offset,realLen,fldName,lenOfRecStr,recStr);
					return(errCodeSmallBuffer);
				}
				memcpy(value + offset,ptr,realLen);
				realLen += offset;
			}
			value[realLen] = 0;
			return(realLen);
		}
	}while(((ptr = strchr(ptr,'|')) != NULL) && (++ptr != NULL));

	//UnionLog("in UnionReadRecFldFromRecStrForCommon:: fld [%s] not exists!\n",fldName);
	return(errCodeEsscMDL_FldNotExists);
}

// 2008/7/18,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[8092*2];

	if ((recStr == NULL) || (lenOfValue < 0) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutRecFldIntoRecStr:: null pointer or lenOfValue = [%d]!\n",lenOfValue);
		return(errCodeParameter);
	}

	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}

	//modi by hzh 2007.12.11
	if (lenOfValue >= (int )sizeof(buf))
	{
		UnionUserErrLog("in UnionPutRecFldIntoRecStr:: lenOfValue = [%d] too long!\n",lenOfValue);
		return(errCodeParameter);
	}

	memcpy(buf,value,lenOfValue);
	buf[lenOfValue] = 0;
	/*if ((lenOfValue = UnionTranslateStrIntoRecFldStr(value,lenOfValue,buf,sizeof(buf))) < 0)
	{
		UnionUserErrLog("in UnionPutRecFldIntoRecStr:: UnionTranslateStrIntoRecFldStr ret = [%d]\n",lenOfValue);
		return(lenOfValue);
	}*/
	snprintf(recStr,sizeOfRecStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}

// 2008/7/18,增加
// 将一个二进制类型的记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutBitsTypeRecFldIntoRecStr(char *fldName,unsigned char *value,int lenOfValue,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	int 	offset = 0;
	int	realLen;

	if ((recStr == NULL) || (lenOfValue < 0) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionPutBitsTypeRecFldIntoRecStr:: null pointer or lenOfValue = [%d]!\n",lenOfValue);
		return(errCodeParameter);
	}

	for (realLen = lenOfValue; realLen > 0; realLen--)
	{
		if (value[realLen] != 0)
			break;
	}
	if ((fldTagLen = strlen(fldName) + 2) + realLen * 2 >= sizeOfRecStr)
	{
		UnionUserErrLog("in UnionPutBitsTypeRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen + realLen * 2,fldName);
		return(errCodeSmallBuffer);
	}
	sprintf(recStr,"%s=",fldName);
	offset = fldTagLen - 1;
	bcdhex_to_aschex((char *)value,realLen,recStr+offset);
	offset += (realLen * 2);
	recStr[offset] = '|';
	offset++;
	return(offset);
}
// add by chenliang, 2009-02-09
/*
功能:
	将unionRecStr中域名为fldName的值改为fldValue
参数:
	输入参数 [in]
		sizeOfRecStr	:需要修改的 unionRec 容量大小
		fldName		:需要修改的域名
		fldValue	:修改后的域值
	输出参数 [out]
		unionRecStr	:需要修改的 unionRec
返回值:
	>=0	修改后 unionRecStr 的长度
	< 0	出错代码
*/
int UnionUpdateSpecFldValueOnUnionRec(char *unionRecStr, int sizeOfRecStr, char *fldName, char *fldValue)
{
	char		tmpBuf[8192*2 + 1];
	char		fullFldName[256];
	char		*pos, *pos2;
	int		offset;

	if ((NULL == unionRecStr) || (NULL == fldName) || (NULL == fldValue))
	{
		UnionUserErrLog("in UnionUpdateSpecFldValueOnUnionRec:: bad parameter.\n");
		return(errCodeParameter);
	}

	snprintf(fullFldName,sizeof(fullFldName), "%s=", fldName);
	if ((pos = strstr(unionRecStr, fullFldName)) == NULL)
	{
		//UnionLog("in UnionUpdateSpecFldValueOnUnionRec, fld [%s] not exists in [%s].\n", fldName, unionRecStr);
		return(strlen(unionRecStr));
	}

	offset	= pos - unionRecStr;
	memcpy(tmpBuf, unionRecStr, offset);
	offset += snprintf(tmpBuf + offset,sizeof(tmpBuf) - offset, "%s=%s|", fldName, fldValue);

	if ((pos2 = strchr(pos, '|')) == NULL)
	{
		UnionUserErrLog("in UnionUpdateSpecFldValueOnUnionRec, bad form of [%s].\n", unionRecStr);
		return(errCodeParameter);
	}

	if ((0 != (pos2 + 1)) || ('\0' != (pos2 + 1)))
		offset	+= snprintf(tmpBuf + offset,sizeof(tmpBuf) - offset, "%s", pos2 + 1);

	if (offset > sizeOfRecStr)
	{
		UnionUserErrLog("in UnionUpdateSpecFldValueOnUnionRec, sizeOfRecStr [%d] smaller than excepte [%d].\n", sizeOfRecStr, offset);
		return(errCodeParameter);
	}

	memcpy(unionRecStr, tmpBuf, offset);
	unionRecStr[offset] = 0;

	return(offset);
}
// add end, 2009-02-09

// UnionDeleteSpecFldOnUnionRec; add by LiangJie, 2009-05-13
/*
功能:
	删除记录串中指定的域
参数:
	输入参数 [in]
		unionRecStr	:修改前的记录串
		lenOfRecStr	:记录串的长度
		fldName		:需要删除的域名
	输出参数 [out]
		unionRecStr	:删除指定域后的记录串
返回值:
	>=0	修改后 unionRecStr 的长度
	< 0	出错代码
*/
int UnionDeleteSpecFldOnUnionRec(char *unionRecStr, int lenOfRecStr, char *fldName)
{
	char		tmpBuf[4096];
	char		fullFldName[256];
	char		*pos;
	int		offset;

	if ((NULL == unionRecStr) || (NULL == fldName))
	{
		UnionUserErrLog("in UnionDeleteSpecFldOnUnionRec:: errCodeParameter.\n");
		return errCodeParameter;
	}

	sprintf(fullFldName, "%s=", fldName);
	if ((pos = strstr(unionRecStr, fullFldName)) == NULL)
	{
		//UnionLog("in UnionDeleteSpecFldOnUnionRec, fld [%s] not exists in [%s].\n", fullFldName, unionRecStr);
		return lenOfRecStr;
	}

	offset = pos - unionRecStr;
	memcpy(tmpBuf, unionRecStr, offset);

	if ((pos = strchr(pos, '|')) == NULL)
	{
		UnionUserErrLog("in UnionDeleteSpecFldOnUnionRec, bad form of [%s].\n", unionRecStr);
		return errCodeParameter;
	}

	if ((0 != (pos + 1)) || ('\0' != (pos + 1)))
	{
		memcpy(tmpBuf + offset, pos + 1, lenOfRecStr - (pos - unionRecStr) - 1);
		offset += lenOfRecStr - (pos - unionRecStr) - 1;
	}

	memcpy(unionRecStr, tmpBuf, offset);
	unionRecStr[offset] = 0;

	return(offset);
}

// 2008/5/22，增加
/* 保留指定的IP地址级别
输入参数
	ipAddr	原始ip地址
	level	要保留的ip地址的级别
		4，保留4部分
		3，保留前3部分
		2，保留前2部分
		1，保留前1部分
		0，全部不保留
输出参数
	outIPAddr	加工后的ip地址
			不保留的部分以XXX替代
返回值
	0	成功
	负值	出错代码
*/
int UnionDealWithIPAddr(char *ipAddr,int level,char *outIPAddr)
{
	int	index = 0,currentLevel = 0;
	int	inlen,outLen;

	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionDealWithIPAddr:: not validIPAddr [%s]\n",ipAddr);
		return(errCodeParameter);
	}
	if ((level < 0) || (level > 4))
	{
		strcpy(outIPAddr,ipAddr);
		return(0);
	}
	memset(outIPAddr,0,15+1);
	outLen = 0;
	for (inlen = strlen(ipAddr); index < inlen; index++)
	{
		if (level == currentLevel)
		{
			for (;currentLevel < 4; currentLevel++)
			{
				memcpy(outIPAddr+outLen,"XXX",3);
				outLen += 3;
				if (currentLevel < 3)
				{
					outIPAddr[outLen] = '.';
					outLen++;
				}
			}
			return(0);
		}
		outIPAddr[outLen] = ipAddr[index];
		outLen++;
		if (ipAddr[index] == '.')
			currentLevel++;
	}
	return(0);
}

// 2008/5/15增加
// 检查一个字符是否符合奇校验
int UnionIsOddParityChar(char chCheckChar)
{
	int		nNumber,i;
	unsigned char	ch,ch1;

	nNumber=0;
	ch=(unsigned char)0x01;
	for (i=0;i<8;i++)
	{
		ch1=(unsigned char)(chCheckChar & ch);
		if (ch1!=(unsigned char)0x00) nNumber++;
		ch=(unsigned char)(ch << 1);
	}

	if (nNumber%2 == 0)
		return(0);
	else
		return(1);
}

// 2008/5/15增加
// 对一个字符串增加奇校验
int UnionMakeStrIntoOddParityStr(char *pData,int nLenOfData)
{
	int     i,nLength;
	char    aCompData[2048];

	aschex_to_bcdhex(pData,nLenOfData,aCompData);
	for (i = 0,nLength = nLenOfData / 2; i < nLength; i++)
	{
		if (UnionIsOddParityChar(aCompData[i]))
			continue;
		else
			aCompData[i] ^= 0x01;
	}
	bcdhex_to_aschex(aCompData,nLength,pData);
	return(0);
}

// 2008/5/15增加
// 检查一个字符串是否符合奇校验
// 符合奇校验返回值为1，否则返回值为0
int UnionCheckStrIsOddStr(char *pData,int nLenOfData)
{
	int     i,nLength;
	char    aCompData[2048];

	aschex_to_bcdhex(pData,nLenOfData,aCompData);
	for (i = 0,nLength = nLenOfData / 2; i < nLength; i++)
	{
		if (UnionIsOddParityChar(aCompData[i]))
			continue;
		else
			return(0);
	}
	return(1);
}

/* 2008/3/31增加
功能：从一个字符串得到指定第几个域的值
输入参数:
	buf		输入字符串
	spaceMark	分隔符
	index		第几个值
输出参数：
	fld		第index的值

返回值：
	返回值的长度
*/
int UnionReadFldFromBuf(char *buf,char spaceMark,int index,char *fld)
{
	int	i;
	int	varNum = 0;
	int	varLen = 0;

	if (index < 1)
		return(errCodeParameter);

	for (i = 0,varNum = 0,varLen = 0; i < strlen(buf); i++)
	{
		if (buf[i] == spaceMark)	// 遇到分隔符
		{
			if (++varNum >= index)
				break;
			else
			{
				varLen = 0;
				continue;
			}
		}

		if (varNum + 1 == index)
		{
			fld[varLen] = buf[i];
			varLen++;
		}
	}
	fld[varLen] = 0;
	return(varLen);
}

// 去掉字符串中所有空格，2008-3-19
int UnionFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	// int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if (sourStr[i] == ' ')
			continue;
		/*
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		*/
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//UnionUserErrLog("in UnionFilterAllBlankChars:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if (destStr[j-1] == ' ')
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

// 去掉字符串中所有空格，2008-3-19
int UnionFilterAllSpecChars(char *sourStr,int lenOfSourStr,char specChar,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	// int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if (sourStr[i] == specChar)
			continue;
		/*
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		*/
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//UnionUserErrLog("in UnionFilterAllSpecChars:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if (destStr[j-1] == specChar)
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

// 判断是否合法的变量名
int UnionIsValidAnsiCVarName(char *varName)
{
	int	len;
	int	index;

	if (varName == NULL)
		return(0);
	for (index = 0,len = strlen(varName); index < len;index++)
	{
		if ((varName[index] >= 'A') && (varName[index] <= 'Z'))
			continue;
		else if ((varName[index] >= 'a') && (varName[index] <= 'z'))
			continue;
		else if ((varName[index] >= '0') && (varName[index] <= '9'))
		{
			if (index == 0)
				return(0);
			else
				continue;
		}
		else if (varName[index] == '_')
		{
			if ((index == 0) || (index == len - 1))
				return(0);
			else
				continue;
		}
		else
			return(0);
	}
	return(1);
}

// 从一个字符串中读取参数
/* 输入参数：
	str	字符串
   输出参数
   	par	放读出的参数
   	maxNum	要读出的最大参数数目
   返回值
   	返回读取的参数数目
*/
int UnionReadParFromStr(char *str,char par[][128],int maxNum)
{
	int	num = 0;
	int	yinhaoExists = 0;
	int	len;
	int	index;
	int	nextParExists = 0;
	int	parCharIndex = 0;

	len = strlen(str);

	for (index = 0; index < len; index++)
	{
		if (num >= maxNum)
			break;
		if (nextParExists && (parCharIndex >= (int)sizeof(par[num])))	// 参数值太长
		{
			//printf("参数值太长 num = [%d] parCharIndex = [%d]\n",num,parCharIndex);
			return(errCodeParameter);
		}
		switch (str[index])
		{
			case	' ':	// 空格
				if (!nextParExists)
					continue;
				if (yinhaoExists)	// 是引号内的空格，是参数的一部分
				{
					par[num][parCharIndex] = str[index];
					parCharIndex++;
					continue;
				}
				// 读到了一个参数
				par[num][parCharIndex] = 0;
				num++;
				// 初始化读下一个参数
				nextParExists = 0;
				parCharIndex = 0;
				continue;
			case	'"':	// 是双引号
				if (!yinhaoExists) // 是参数的起始引号
				{	// 起始引号的前一个字符必须是空格，或者起始引号是第一个字符
					if (index > 0)
					{
						if (str[index-1] != ' ')
						{
							/*
							printf("引号前一个字符不是空格! num = [%d]\n",num);
							par[num][parCharIndex] = 0;
							for (index = 0; index <= num; index++)
								printf("[%02d] [%s]\n",index,par[index]);
							*/
							return(errCodeParameter);	// 参数字符串出错了
						}
					}
					yinhaoExists = 1;
					nextParExists = 1;
					parCharIndex = 0;
					continue;
				}
				// 是参数的结束引号
				par[num][parCharIndex] = 0;
				num++;
				yinhaoExists = 0;
				// 初始化读下一个参数
				nextParExists = 0;
				parCharIndex = 0;
				continue;
			default:	//是参数中的一个字符
				if (!nextParExists)
					nextParExists = 1;
				par[num][parCharIndex] = str[index];
				parCharIndex++;
				continue;
		}
	}
	if (yinhaoExists)
	{
		//printf("引号没有配匹的引号 num = [%d]\n",num);
		return(errCodeParameter);
	}
	if (parCharIndex > 0)
	{
		par[num][parCharIndex] = 0;
		num++;
	}
	return(num);
}

// 将人民币小写金额转换为大写金额
int UnionSmallAmtToBigAmt(char *smallAmt, char *bigAmt)
{
	size_t	i,j;
	char	in[128];
	char	out[128];
	char	number[128];
	char	units[128];
	char	*p;
	char	ch;

	strcpy(number,"零壹贰叁肆伍陆柒捌玖");
	strcpy(units,"分角元拾佰仟万拾佰仟亿");

	// 去掉开头的0
	for (i = 0; i < strlen(smallAmt); i++)
	{
		if (smallAmt[i] == '0')
			continue;
		else
		{
			strcpy(in,smallAmt + i);
			break;
		}
	}
	// 去掉小数点
	for (i = 0; i < strlen(in); i++)
	{
		if (in[i] == '.')
		{
			in[i] = in[i+1];
			in[i+1] = in[i+2];
			in[i+2] = 0;
			break;
		}
	}
	memset(out,0,sizeof(out));
	p = out;
	i = (strlen(in) -1 ) * 2;
	for (j = 0; j < strlen(in); j++)
	{
		ch = in[j];
		memcpy(p,&number[atoi(&ch) * 2],2);

		p = p + 2;
		memcpy(p,&units[i],2);
		i = i - 2;
		p = p + 2;
	}
	strcpy(bigAmt,out);
	return(0);
}

// 从参数组中读取参数
// 参数组中
/* 输入参数：
	parGrp,参数组
	parNum，参数数目
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UnionReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue)
{
	char	parIDDef[128];
	int	lenOfPar;
	int	index;
	int	times = 0;
	char	par[128];

	if ((parGrp == NULL) || (parID == NULL) || (strlen(parID) >= sizeof(parIDDef)-1) || (parNum < 0))
	{
		UnionUserErrLog("in UnionReadParFromParGrp:: parGrp or parID is NULL! or parID too long\n");
		return(errCodeParameter);
	}

	if (parID[0] != '-')
		sprintf(parIDDef,"-%s",parID);
	else
		sprintf(parIDDef,"%s",parID);

retry:
	if (times > 0)
		UnionToUpperCase(parIDDef);
	for (index = 0; index < parNum; index++)
	{
		strcpy(par,parGrp[index]);
		UnionToUpperCase(par);
		if (strcmp(par,parIDDef) != 0)
			continue;
		if (++index >= parNum)
		{
			UnionLog("in UnionReadParFromParGrp:: parID [%s] not defined in this parGrp\n",parID);
			return(errCodeEnviMDL_VarNotExists);
		}
		if ((lenOfPar = strlen(parGrp[index])) >= sizeOfParValue)
		{
			UnionUserErrLog("in UnionReadParFromParGrp:: lenOfPar [%04d] of [%s] too long!\n",lenOfPar,parIDDef);
			return(errCodeSmallBuffer);
		}
		memcpy(parValue,parGrp[index],lenOfPar);
		return(lenOfPar);
	}
	if (times == 0)
	{
		times++;
		goto retry;
	}
	UnionLog("in UnionReadParFromParGrp:: parID [%s] not defined in this parGrp\n",parID);
	return(errCodeEnviMDL_VarNotExists);
}

// 从命令串中读取参数
// 命令串的定义格式为:
//	-parID parValue
/* 输入参数：
	cmdStr,命令串
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UnionReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue)
{
	char	parIDDef[256];
	char	*start = NULL;
	char	tmpBuf[2048];
	int	lenOfPar;

	if ((cmdStr == NULL) || (parID == NULL) || (strlen(parID) >= sizeof(parIDDef)-1))
	{
		UnionUserErrLog("in UnionReadParFromCmdStr:: cmdStr or parID is NULL! or parID too long\n");
		return(errCodeParameter);
	}

	if (parID[0] != '-')
		sprintf(parIDDef,"-%s",parID);
	else
		sprintf(parIDDef,"%s",parID);
	if ((start = strstr(cmdStr,parIDDef)) == NULL)
	{
		UnionLog("in UnionReadParFromCmdStr:: parID [%s] not defined in this cmdStr [%s]\n",parID,cmdStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	start += strlen(parIDDef);
	sscanf(start,"%s",tmpBuf);
	if (((lenOfPar = strlen(tmpBuf)) == 0) || ((strlen(tmpBuf) == 1) && (tmpBuf[0] == '-')))
	{
		UnionUserErrLog("in UnionReadParFromCmdStr:: cmdStr [%s] error!\n",cmdStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	if (lenOfPar >= sizeOfParValue)
	{
		UnionUserErrLog("in UnionReadParFromCmdStr:: lenOfPar [%04d] of [%s] too long!\n",lenOfPar,parIDDef);
		return(errCodeSmallBuffer);
	}
	memcpy(parValue,tmpBuf,lenOfPar);
	return(lenOfPar);
}

// 从记录定义串中读取域值
// 记录串的定义格式为:
//	fldName=fldValue;fldName=fldValue;...fldName=fldValue;
/* 输入参数：
	recStr,记录串
	fldName,域名
	fldTypeName,类型名
		合法值	char/short/int/long/double/string
	sizeOfFldValue,域值缓冲的大小
   输出参数
   	fldValue,接受域值的缓冲
   返回值：
   	<0,错误码
   	>=0,域值长度
*/
int UnionReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue)
{
	char	fldNameDef[256];
	char	*start = NULL;
	char	*end = NULL;
	int	tmpInt;
	long	tmpLong;
	double	tmpDouble;
	char	tmpChar;
	short	tmpShort;
	char	*pvalue = NULL;
	int	minSizeOfValue;

	if ((recStr == NULL) || (fldName == NULL) || (strlen(fldName) >= sizeof(fldNameDef)-1))
	{
		//UnionUserErrLog("in UnionReadFldFromRecStr:: recStr or fldName is NULL! or fldName too long\n");
		return(errCodeParameter);
	}
	sprintf(fldNameDef,"%s=",fldName);
	if ((start = strstr(recStr,fldNameDef)) == NULL)
	{
		//UnionLog("in UnionReadFldFromRecStr:: fldName [%s] not defined in this recStr [%s]\n",fldName,recStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	start += strlen(fldNameDef);
	if ((end = strstr(start,";")) == NULL)
	{
		//UnionUserErrLog("in UnionReadFldFromRecStr:: fldName [%s] defined in this recStr [%s] without end char ';'\n",fldName,recStr);
		return(errCodeEnviMDL_NotRecFormatDefStr);
	}
	*end = 0;
	if (strcmp(fldTypeName,"int") == 0)
	{
		tmpInt = atoi(start);
		pvalue = (char *)(&tmpInt);
		minSizeOfValue = sizeof(int);
	}
	else if (strcmp(fldTypeName,"long") == 0)
	{
		tmpLong = atol(start);
		pvalue = (char *)&tmpLong;
		minSizeOfValue = sizeof(long);
	}
	else if (strcmp(fldTypeName,"double") == 0)
	{
		tmpDouble = atof(start);
		pvalue = (char *)&tmpDouble;
		minSizeOfValue = sizeof(double);
	}
	else if (strcmp(fldTypeName,"char") == 0)
	{
		tmpChar = start[0];
		pvalue = &tmpChar;
		minSizeOfValue = sizeof(char);
	}
	else if (strcmp(fldTypeName,"short") == 0)
	{
		tmpShort = atoi(start);
		pvalue = (char *)&tmpShort;
		minSizeOfValue = sizeof(short);
	}
	else
	{
		pvalue = start;
		minSizeOfValue = strlen(start);
	}
	if (minSizeOfValue > sizeOfFldValue)
	{
		//UnionUserErrLog("in UnionReadFldFromRecStr:: sizeOfFldValue [%d] < expected = [%d]\n",sizeOfFldValue,minSizeOfValue);
		*end = ';';
		return(errCodeSmallBuffer);
	}
	memcpy(fldValue,pvalue,minSizeOfValue);
	fldValue[minSizeOfValue] = 0;
	*end = ';';
	return(minSizeOfValue);
}

// 返回PK串的长度
int UnionGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf)
{
	int	offset;
	int	i;
	int	lenOfNextPart;
	int	bitsOfLenFlag;
	int	lenOfStr;

	if ((racalPKStr == NULL) || (lenOfRacalPKStr <= 0))
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: racalPKStr is null or lenOfRacalPKStr = [%d] error!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}

	// 起始标志位
	offset = 0;
	if (racalPKStr[offset] != 0x30)
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: invalid start flag [%c]\n",racalPKStr[offset]);
		return(errCodeParameter);
	}
	offset++;

	// 判断下一部分的长度
	if (racalPKStr[offset] <= 0x80)
	{
		lenOfNextPart = racalPKStr[offset];
		offset++;
	}
	else
	{
		// 长度指示位
		bitsOfLenFlag = racalPKStr[offset] - 0x80;
		offset++;
		for (i = 0,lenOfNextPart = 0; i < bitsOfLenFlag; i++,offset++)
			//lenOfNextPart += racalPKStr[offset];
			lenOfNextPart = lenOfNextPart * 256 + racalPKStr[offset];	// wolfgang wang,2008/11/24
	}
	lenOfStr = lenOfNextPart + offset;
	if (lenOfNextPart + offset > lenOfRacalPKStr)
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: lenOfRacalPKStr [%d] too short!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}

	// 判断是不是分隔位
	if (racalPKStr[offset] != 0x02)
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: invalid sperator flag [%c]\n",racalPKStr[offset]);
		return(errCodeParameter);
	}
	offset++;
	lenOfNextPart--;

	// 判断PK的长度
	if (racalPKStr[offset] <= 0x80)
	{
		*lenOfPK = racalPKStr[offset];
		offset++;
	}
	else
	{
		// 长度指示位
		bitsOfLenFlag = racalPKStr[offset] - 0x80;
		offset++;
		for (i = 0,*lenOfPK = 0; i < bitsOfLenFlag; i++,offset++)
			//*lenOfPK += racalPKStr[offset];		// 2007/11/15 修改这一句!! **2008/11/24 wolfgang delete this
			*lenOfPK = *lenOfPK * 256 + racalPKStr[offset];	// 2008/11/24 wolfgang add
	}

	// 取出PK的值
	if (*lenOfPK + offset > lenOfRacalPKStr)
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: lenOfRacalPKStr [%d] too short!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}
	// 2007/11/15 增加
	while (*lenOfPK % 8 != 0)
	{
		if (racalPKStr[offset] != 0x00)
		{
			// return(errCodeParameter);
			offset++;
			--(*lenOfPK);
			break;
		}
		offset++;
		--(*lenOfPK);
	}
	// 2007/11/15 增加结束
	if (*lenOfPK * 2 > sizeOfBuf)
	{
		UnionUserErrLog("in UnionGetPKOutOfRacalHsmCmdReturnStr:: sizeOfBuf [%d] < expected [%d]!\n",sizeOfBuf,*lenOfPK*2);
		return(errCodeParameter);
	}
	bcdhex_to_aschex((char *)racalPKStr+offset,*lenOfPK,pk);
	*lenOfPK = *lenOfPK * 2;
	pk[*lenOfPK] = 0;

	return(lenOfStr);
}

int UnionFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate)
{
	char	tmpBuf[32];
	int	larger;

	UnionGetFullSystemDate(tmpBuf);
	tmpBuf[8] = 0;
	memcpy(fullDate,tmpBuf,4);
	memcpy(fullDate+4,mmDDDate,4);
	if ((larger = strncmp(fullDate,tmpBuf,6)) == 0)	// 是同年同月
		return(0);
	if ((strncmp(fullDate+4,"12",2) == 0) && (strncmp(tmpBuf+4,"01",2) == 0))	// 拼装的日期是最后一个月，而当前日期是第一个月
	{
		tmpBuf[4] = 0;
		sprintf(fullDate,"%04d",atoi(tmpBuf) - 1);
		memcpy(fullDate+4,mmDDDate,4);
		return(0);
	}
	if ((strncmp(fullDate+4,"01",2) == 0) && (strncmp(tmpBuf+4,"12",2) == 0))	// 拼装的日期是第一个月，而当前日期是最后一个月
	{
		tmpBuf[4] = 0;
		sprintf(fullDate,"%04d",atoi(tmpBuf) + 1);
		memcpy(fullDate+4,mmDDDate,4);
		return(0);
	}
	return(0);
}

// 判断串中是否含有.
int UnionIsStringHasDotChar(char *str)
{
	int	i,len;

	for (i = 0, len = strlen(str); i < len; i++)
	{
		if (str[i] == '.')
			return(1);
	}
	return(0);
}

// 去掉字符串中的多余空格，即将多个空格压缩为一个空格。去掉头和尾的空格
int UnionFilterRubbisBlank(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0,FirstBlank=1; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if (sourStr[i] == ' ')
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//UnionUserErrLog("in UnionFilterRubbisBlank:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if (destStr[j-1] == ' ')
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

// 2008/3/17增加
// 去掉字符串中的多余空格和tab，即将多个空格压缩为一个空格。去掉头和尾的空格和tab
int UnionFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0,FirstBlank=1; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if ((sourStr[i] == ' ') || (sourStr[i] == '\t'))
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				//destStr[j] = sourStr[i];
				destStr[j] = ' ';
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//UnionUserErrLog("in UnionFilterRubbisBlankAndTab:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if ((destStr[j-1] == ' ') || (sourStr[j-1] == '\t'))
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

int UnionConvertDoubleOf2TailStrIntoLongStr(char *str)
{
	int	len = strlen(str);
	int	offset;
	char 	*ptr = NULL;
	
	if ((ptr = strchr(str,'.')) == NULL)
		return(len);
	
	offset = ptr - str;
	memmove(str + offset,ptr + 1,len - offset);
	return(len - 1);
}

int UnionConvertLongStrIntoDoubleOf2TailStr(char *str)
{
	char	tmpBuf[128];
	char	tmpBuf2[128];
	int	len;
	int	offset = 0;
	int	retOffset = 0;
	int	i;

	//sprintf(tmpBuf,"%ld",atol(str));
	strcpy(tmpBuf2,str);
	if ((len = strlen(tmpBuf2)) > 0)
	{
		switch (tmpBuf2[0])
		{
			case	'-':
				offset = 1;
				retOffset = 1;
				str[0] = '-';
				break;
			case	'+':
				offset = 1;
				break;
			default:
				break;
		}
	}
	for (i = offset; i < len; i++)
	{
		if (tmpBuf2[i] == '0')
			continue;
		else
			break;
	}
	memcpy(tmpBuf,tmpBuf2+i,len-i);
	tmpBuf[len-i] = 0;
	switch (len = strlen(tmpBuf))
	{
		case	0:
			strcpy(str+retOffset,"0.00");
			break;
		case	1:
			strcpy(str+retOffset,"0.0");
			retOffset += 3;
			strcpy(str+retOffset,tmpBuf);
			break;
		case	2:
			strcpy(str+retOffset,"0.");
			retOffset += 2;
			strcpy(str+retOffset,tmpBuf);
			break;
		default:
			memcpy(str+retOffset,tmpBuf,len-2);
			retOffset += len - 2;
			memcpy(str+retOffset,".",1);
			retOffset += 1;
			memcpy(str+retOffset,tmpBuf+len-2,2);
			retOffset += 2;
			str[retOffset] = 0;
			break;
	}
	return(strlen(str));
}

int UnionIsDigitStr(char *str)
{
	int	i;
	int	len;

	for (i = 0,len = strlen(str); i < len; i++)
		if (!isdigit(str[i]))
			return(0);
	return(1);
}

// 2009/4/29,王纯军增加，判断一个字符串是否是数学串
int UnionIsDecimalStr(char *str)
{
	int	i;
	int	len;
	int	dotExists = 0;

	for (i = 0,len = strlen(str); i < len; i++)
	{
		if (isdigit(str[i]))
			continue;
		if (dotExists)
			return(0);
		if (str[i] != '.')
			return(0);
		dotExists = 1;
	}
	return(1);
}

int UnionFilterRSASignNullChar(char *str,int len)
{
	int	offset;
	int     validLen = 0;

	for (offset = len - 1; offset >= 0; offset--)
	{
		if (str[offset] != 0x00)
			validLen++;
		else
		{
			offset++;
			if (offset == len)
				return(0);
			memcpy(str,str+offset,validLen);
			return(validLen);
		}
	}
	return(len);
}

int UnionGetPKOutOfANSIDERRSAPK(char *derPK,int lenOfDERPK,char *pk,int sizeOfPK)
{
	int	len;
	int	realPKLen;
	char	tmpBuf[100];

	if (lenOfDERPK < 64)
		return(errCodeParameter);

	len = 0;
	if (derPK[len] != 0x30)
		return(errCodeParameter);
	len++;
	bcdhex_to_aschex(derPK+len,20,tmpBuf);
	if (strncmp(tmpBuf,"470240",6) == 0)
	{
		len += 3;
		realPKLen = 64;
	}
	else if (strncmp(tmpBuf,"8188028180",10) == 0)
	{
		len += 5;
		realPKLen = 128;
	}
	else if (strncmp(tmpBuf,"82010902820100",14) == 0)
	{
		len += 7;
		realPKLen = 256;
	}
	else
		return(errCodeParameter);

	if ((len+realPKLen > lenOfDERPK) || (realPKLen * 2 > sizeOfPK))
		return(errCodeParameter);

	bcdhex_to_aschex(derPK+len,realPKLen,pk);
	return(realPKLen * 2);
}

int UnionConvertLongIntoStr(long data,int len,char *str)
{
	switch (len)
	{
		case	1:
			sprintf(str,"%ld",data%10);
			break;
		case	2:
			sprintf(str,"%02ld",data%100);
			break;
		case	3:
			sprintf(str,"%03ld",data%1000);
			break;
		case	4:
			sprintf(str,"%04ld",data%10000);
			break;
		case	5:
			sprintf(str,"%05ld",data%100000);
			break;
		case	6:
			sprintf(str,"%06ld",data%1000000);
			break;
		case	7:
			sprintf(str,"%07ld",data%10000000);
			break;
		case	8:
			sprintf(str,"%08ld",data%100000000);
			break;
		case	9:
			sprintf(str,"%09ld",data%1000000000);
			break;
		case	10:
			sprintf(str,"%010ld",data);
			break;
		case	11:
			sprintf(str,"%011ld",data);
			break;
		case	12:
			sprintf(str,"%012ld",data);
			break;
		default:
			return(errCodeParameter);
	}
	return(len);
}

int UnionConvertIntIntoStr(int data,int len,char *str)
{
	switch (len)
	{
		case	1:
			sprintf(str,"%d",data%10);
			break;
		case	2:
			sprintf(str,"%02d",data%100);
			break;
		case	3:
			sprintf(str,"%03d",data%1000);
			break;
		case	4:
			sprintf(str,"%04d",data%10000);
			break;
		case	5:
			sprintf(str,"%05d",data%100000);
			break;
		default:
			return(errCodeParameter);
	}
	return(len);
}

long UnionConvertLongStringToLong(char *str,int lenOfStr)
{
	int	i;
	long	tmpInt = 0;
	int	isMinus = 1;
	int	maxLen = 12;

	for (i = 0; (i < lenOfStr) && (i < maxLen); i++)
	{
		if (!isdigit(str[i]))
		{
			if (i != 0)
				return(tmpInt*isMinus);
			switch (str[i])
			{
				case '-':
					isMinus = -1;
					maxLen = 13;
					break;
				case '+':
					isMinus = 1;
					maxLen = 13;
					break;
				default:
					return(tmpInt*isMinus);
			}
			continue;
		}
		tmpInt = tmpInt * 10 + str[i] - '0';
	}
	return(tmpInt * isMinus);
}

int UnionConvertIntStringToInt(char *str,int lenOfStr)
{
	int	i;
	int	tmpInt = 0;
	int	isMinus = 1;

	for (i = 0; i < lenOfStr; i++)
	{
		if (!isdigit(str[i]))
		{
			if (i != 0)
				return(tmpInt*isMinus);
			switch (str[i])
			{
				case '-':
					isMinus = -1;
					break;
				case '+':
					isMinus = 1;
					break;
				default:
					return(tmpInt*isMinus);
			}
			continue;
		}
		if (tmpInt >= 100000000)
			return(tmpInt * isMinus);
		tmpInt = tmpInt * 10 + str[i] - '0';
	}
	return(tmpInt * isMinus);
}

int UnionIsStringContainingOnlyZero(char *str,int lenOfStr)
{
	int	i;
	for (i = 0; i < lenOfStr; i++)
	{
		if (str[i] != '0')
			return(0);
	}
	return(1);
}

/*
// Mary modify this function, 2004-3-18, for changing Exponent from 3 to 65537
int UnionFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK)
{
	int	len;
	int	ret;
	int	realPKLen;
	//char	tmpBuf[4096];
	//int	pkLenFlagLen;

	realPKLen = lenOfPK / 2;

	// Mary modify, 2004-3-18
	//if (sizeOfDERPK < 9 + realPKLen)
	if (sizeOfDERPK < 11 + realPKLen)
	{
		//UnionUserErrLog("in UnionFormANSIDERRSAPK:: too small sizeOfDERPK [%d]\n",sizeOfDERPK);
		return(errCodeParameter);
	}

	len = 0;
	derPK[len] = 0x30;
	len++;

	switch (realPKLen)
	{
		case	64:
			derPK[len] = 0x47;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x40;
			len++;
			break;
		case	128:
			derPK[len] = 0x81;
			len++;
			derPK[len] = 0x88;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x81;
			len++;
			derPK[len] = 0x80;
			len++;
			break;
		case	256:
			derPK[len] = 0x82;
			len++;
			derPK[len] = 0x01;
			len++;
			derPK[len] = 0x09;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x82;
			len++;
			derPK[len] = 0x01;
			len++;
			derPK[len] = 0x00;
			len++;
			break;
		default:
			return(errCodeParameter);
	}

	aschex_to_bcdhex(pk,lenOfPK,derPK+len);
	len += realPKLen;

	
	// Mary add begin, 2004-3-18
	derPK[len] = 0x02;
	len++;
	derPK[len] = 0x03;
	len++;
	derPK[len] = 0x01;
	len++;
	derPK[len] = 0x00;
	len++;
	derPK[len] = 0x01;
	len++;
	// Mary add end, 2004-3-18

	//memset(tmpBuf,0,sizeof(tmpBuf));
	//bcdhex_to_aschex(derPK,len,tmpBuf);
	//UnionLog("in UnionFormANSIDERRSAPK:: pk = [%s]\n",tmpBuf);

	return(len);
}
*/

/*
功能：将长度转为asn1格式的长度
输入：
	len:长度
输出：
	asn1Len:在asn1中表示的长度
返回:
	asn1Len二进制字符串的输出长度
*/
static int UnionAsn1LenExt(int len,unsigned char *asn1Len)
{
	unsigned char buff[32];
	int ret = 0;

	if(len > 65535)
	{
		return -1;
	}
	if (len > 255)
	{
		buff[0] = 0x82;
		buff[1] = ((len & 0xFF00) >> 8);
		buff[2] = (len & 0x00FF);
		ret  =3;
	}
	else {
		if ((len & 0x80) != 0)
		{
			buff[0] = 0x81;
			buff[1] = len;
			ret = 2;
		}
		else {
			buff[0] = len;
			ret = 1;
		}
	}
	memcpy(asn1Len,buff,ret);
	asn1Len[ret] = 0;
	return ret;
}

/*
功能:将公钥模与指数拼装为DER编码格式的公钥
输入:
	lenOfPkModule:公钥模的二进制字符长度
	pkModule:公钥模的二进制字符
	lenOfExp:公钥指数的二进制字符长度
	exp:公钥指数的二进制字符
输出:
	derPK:DER编码格式的公钥二进制字符串
返回:
	DER编码格式的公钥二进制字符串长度
*/
int UnionFormDerPKExt(int lenOfPkModule,unsigned char *pkModule,int lenOfExp,unsigned char *exp,unsigned char *derPK)
{
	char buff[4096];
	char tbuff[4096];

	int offset = 0;
	int len = 0;
	int 	ret=0;

	if (pkModule == NULL || exp == NULL || derPK == NULL)
	{
		return -1;
	}

	offset = 0;
	tbuff[offset] = 0x02;
	offset += 1;
	ret = UnionAsn1LenExt(lenOfPkModule+1,(unsigned char *)tbuff+offset);
	offset += ret;
	tbuff[offset] = 0x00;
	offset += 1;
	memcpy(tbuff+offset,pkModule,lenOfPkModule);
	offset += lenOfPkModule;
	tbuff[offset] = 0x02;
	offset += 1;
	ret = UnionAsn1LenExt(lenOfExp,(unsigned char *)tbuff+offset);
	offset += ret;
	memcpy(tbuff+offset,exp,lenOfExp);
	offset += lenOfExp;
	tbuff[offset] = 0;
	len = offset;

	offset = 0;
	buff[offset] = 0x30; 
	offset += 1;
	ret = UnionAsn1LenExt(len,(unsigned char *)buff+offset);
	offset += ret;
	memcpy(buff+offset,tbuff,len);
	offset += len;
	buff[offset] = 0;
	
	memcpy(derPK,buff,offset);
	derPK[offset] = 0;
	return offset;
}

// 将裸公钥，编码成DER格式
int UnionPKCSASN1DEREncodeExt(char *pkModule,char *exp, char *derPK)
{
	int ret = 0;
	int len = 0;
	unsigned char pkModule_bcdhex[8192];
	unsigned char exp_bcdhex[50];
	unsigned char derPK_bcdhex[8192];
	int lenOfPkModule = 0;
	int lenOfExp = 0;
	
	if (pkModule == NULL || exp == NULL || derPK == NULL)
		return -1;

	aschex_to_bcdhex(pkModule,strlen(pkModule),(char *)pkModule_bcdhex);
	lenOfPkModule = strlen(pkModule)/2;

	aschex_to_bcdhex(exp,strlen(exp),(char *)exp_bcdhex);
	lenOfExp = strlen(exp)/2;

	ret = UnionFormDerPKExt(lenOfPkModule,pkModule_bcdhex,lenOfExp,exp_bcdhex,derPK_bcdhex);
	bcdhex_to_aschex((char *)derPK_bcdhex,ret,(char *)derPK);
	len = ret*2;
	derPK[len] = 0;
	return len;
}



void UnionSetCurrentRsaExponent(int exponent)
{
	giCurrntExponent = exponent;
}

int UnionGetCurrentRsaExponent()
{
	return giCurrntExponent;
}


//2012.11.8 hzh 重写了UnionFormANSIDERRSAPK，为支持1408,1152等RSA及指数为3的支持
int UnionFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK)
{
	unsigned char pk_bcdhex[4096];
	int lenOfExp = 0;
	unsigned char exp_bcdhex[32];
	unsigned char derPK_bcdhex[4096];
	int ret = 0;
	
	aschex_to_bcdhex(pk,lenOfPK,(char *)pk_bcdhex);  //模
	if (giCurrntExponent == 3)      //指数为3
	{
		memcpy(exp_bcdhex,"\x03",1);
		lenOfExp = 1;
	}
	else {                    //指数为65537
		memcpy(exp_bcdhex,"\x01\x00\x01",3);
		lenOfExp = 3;
	}
	
	ret = UnionFormDerPKExt(lenOfPK/2,pk_bcdhex,lenOfExp,exp_bcdhex,derPK_bcdhex);
	if (ret > sizeOfDERPK)
	{
		UnionUserErrLog("in UnionFormANSIDERRSAPK new:: too small sizeOfDERPK [%d]\n",sizeOfDERPK);
		return errCodeParameter;
	}
	memcpy(derPK,derPK_bcdhex,ret);
	derPK[ret] = 0;
	return ret;
	
}


int UnionFormANSIX80LenString(int len,char *str,int sizeOfStr)
{
	/*
	int	i;
	int	bal;

	if (len < 0)
	{
		//UnionUserErrLog("in UnionFormANSIX80LenString:: len [%d] erorr!\n",len);
		return(errCodeParameter);
	}

	if ((len < 128 * 128) && (len >= 128))
	{
		if (sizeOfStr < 2)
		{
			//UnionUserErrLog("in UnionFormANSIX80LenString:: sizeOfStr [%d] too small!\n",sizeOfStr);
			return(errCodeParameter);
		}
		str[0] = 0x80 + len / 128;
		str[1] = 0x80 + len % 128;
		return(2);
	}

	if (len < 128)
	{
		if (sizeOfStr < 1)
		{
			//UnionUserErrLog("in UnionFormANSIX80LenString:: sizeOfStr [%d] too small!\n",sizeOfStr);
			return(errCodeParameter);
		}
		str[0] = 0x80 + len % 128;
		return(1);
	}
	*/
	int i;
	int bal;

	if (len < 0)
	{
		//UnionUserErrLog("in UnionFormANSIX80LenString:: len [%d] erorr!\n",len);
		return(errCodeParameter);
	}
	for (i = 0,bal = len % 128;(len / 128 > 0) && (i < sizeOfStr);i++,len = len - 128)
		str[i] = 0x80 + len / 128;
	if (i > 0)
		str[i] = bal + 0x80;
	else
		str[i] = bal;

	return(i+1);

	//UnionUserErrLog("in UnionFormANSIX80LenString:: len [%d] erorr!\n",len);
	//return(errCodeParameter);
}

int UnionConvertIntoLen(char *str,int intLen)
{
	int	len,i;

	if ((intLen <= 0) || (intLen > 5))
		return(errCodeParameter);
	len = 0;
	for (i = 0; i < intLen; i++)
	{
		if (!isdigit(str[i]))
			return(errCodeParameter);
		len = len * 10 + (str[i] - '0');
	}
	return(len);
}

int UnionIsBCDStr(char *str1)
{
	int	i;
	int	len;

	for (i = 0,len = strlen(str1); i < len; i++)
	{
		if ((str1[i] >= '0') && (str1[i] <= '9'))
			continue;
		if ((str1[i] >= 'a') && (str1[i] <= 'f'))
			continue;
		if ((str1[i] >= 'A') && (str1[i] <= 'F'))
			continue;
		return(0);
	}
	return(1);
}

int UnionToUpper(char *lowerstr,char *upperstr)
{
	int	i;
	int	Len;

	for (i = 0, Len = strlen(lowerstr); i < Len; i++)
		upperstr[i] = toupper(lowerstr[i]);

	return(0);
}

int UnionUpperMySelf(char *str,int lenOfStr)
{
	int     i = 0;

	for (i = 0; i < lenOfStr; i++)
		str[i] = toupper(str[i]);
	return(0);
}

int UnionToUpperCase(char *str)
{
	int	i = 0;
	int	len;

	for (i = 0,len=strlen(str); i < len; i++)
	{
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			//str[i] = toupper(str[i]);
			str[i] = str[i] + 'A' - 'a';
	}
	return(0);
}

int UnionToLowerCase(char *str)
{
	int	i = 0;
	int	len;

	while (i < (len = strlen(str)))
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] = tolower(str[i]);
		i++;
	}
	return(0);
}

int UnionFilterRightBlank(char *Str)
{
	int	i = strlen(Str)-1;;

	while((Str[i] == ' ' || Str[i] == '\t') && i >= 0)
		i --;
	Str[i+1] = '\0';
	return(i + 1);
}

int UnionFilterLeftBlank(char *Str)
{
	int	i;
	int	Len;

	for (i = 0,Len = strlen(Str); i < Len; i++)
	{
		if ((Str[i] != ' ') && (Str[i] != 0x09))
			break;
	}
	if (i == 0)
		return(Len);

	memmove(Str,Str+i,Len - i);
	Str[Len - i] = 0;

	return(Len - i);
}

int UnionVerifyDateStr(char *DateStr)
{
	int	Year;
	int	Month;
	int	Day;

	char	TmpBuf[32];

	if (DateStr == NULL)
		return(0);

	if (strlen(DateStr) != 8)
		return(0);

	memcpy(TmpBuf,DateStr,4);
	TmpBuf[4] = 0;
	Year = atoi(TmpBuf);
	memcpy(TmpBuf,DateStr+4,2);
	TmpBuf[2] = 0;
	Month = atoi(TmpBuf);
	memcpy(TmpBuf,DateStr+6,2);
	TmpBuf[2] = 0;
	Day = atoi(TmpBuf);

	switch (Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if ((Day >= 1) && (Day <= 31))
				return(1);
			else
				return(0);
		case 4:
		case 6:
		case 9:
		case 11:
			if ((Day >= 1) && (Day <= 30))
				return(1);
			else
				return(0);
		case 2:
			if ((Year % 4 == 0) && (Year % 100 != 0))
			{
				if ((Day >= 1) && (Day <= 29))
					return(1);
				else
					return(0);
			}
			else
			{
				if ((Day >= 1) && (Day <= 28))
					return(1);
				else
					return(0);
			}
		default:
			return(0);
	}
}

int UnionCopyFilterHeadAndTailBlank(char *str1,char *str2)
{
	strcpy(str2,str1);
	UnionFilterLeftBlank(str2);
	return(UnionFilterRightBlank(str2));
}

int UnionFilterHeadAndTailBlank(char *str)
{
	UnionFilterLeftBlank(str);
	return(UnionFilterRightBlank(str));
}

int UnionIsValidIPAddrStr(char *ipAddr)
{
	unsigned int 	i,j,k;
	char		tmpBuf[32];

	if (strlen(ipAddr) > 15)
		return(0);

	for (i = -1,k = 0; k < 4; k++)
	{
		for (++i,j = 0; (j < 3) && (i < strlen(ipAddr)); i++,j++)
		{
			if (ipAddr[i] == '.') // 遇到一个.,一节结束
				break;
			else
			{
				if (!isdigit(ipAddr[i])) // 包含非数字
					return(0);
				else
					tmpBuf[j] = ipAddr[i];
			}
		}
		tmpBuf[j] = 0;
		if (j == 0) // 连续2个.间无数字
			return(0);
		if ((atoi(tmpBuf) > 255) || (atoi(tmpBuf) < 0))
			return(0);	// 大于255或小于0
		if (k == 3) // 最后一节结束
			break;
		if (ipAddr[i] != '.') // 一节的结束符不是.
			return(0);
	}
	if (strlen(ipAddr) != i)
		return(0);	// 四节结束后，还有字符
	else
		return(1);
}

char hextoasc(int xxc)
{
    xxc&=0x0f;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char hexlowtoasc(int xxc)
{
    xxc&=0x0f;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char hexhightoasc(int xxc)
{
    xxc&=0xf0;
    xxc = xxc>>4;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char asctohex(char ch1,char ch2)
{
    char ch;
    if (ch1>='A') ch=(char )((ch1-0x37)<<4);
    else ch=(char)((ch1-'0')<<4);
    if (ch2>='A') ch|=ch2-0x37;
    else ch|=ch2-'0';
    return ch;
}

int aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
    int i,j;

	if (len % 2 == 0)
		j = len / 2;
	else
		j = len / 2 + 1;

    for (i = 0; i < j; i++)
	bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);

    return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
    int i;

    for (i=0;i<len;i++)
    {
	aschex[2*i]   = hexhightoasc(bcdhex[i]);
	aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
    }

    return(len*2);
}

int byte2int_to_bcd(int aa,char xx[])
{
	if (aa >= 256 * 256)
		return(errCodeParameter);

	xx[0] = aa/256;
	xx[1] = aa%256;

	return(0);
}

int IntToBcd(int aa,unsigned char xx[])
{
	if (aa > 256 * 256)
		return(errCodeParameter);
	xx[0] = (unsigned char)(aa/256);
	xx[1] = (unsigned char)(aa%256);
	return(0);
}

int BcdToInt(char xx[])
{
	return(xx[0]*256+xx[1]);
}

int UnionReadDirFromStr(char *str,int dirLevel,char *dir)
{
	int	strIndex=0,dirIndex=0,tmpIndex=0;
	char	tmpBuf[512];
	char	envName[128];
	int	curDirLevel = 0;
	int	i;

	while (str[strIndex])
	{
		if (str[strIndex] != '$')
		{
			tmpBuf[tmpIndex] = str[strIndex];
			++tmpIndex;
			++strIndex;
			continue;
		}
		i = 0;
		++strIndex;
		while ((str[strIndex] != '/') && (str[strIndex]))
		{
			envName[i] = str[strIndex];
			i++;
			strIndex++;
		}
		envName[i] = 0;
		if (getenv(envName) == NULL)
		{
			UnionUserErrLog("in UnionReadDirFromStr:: envName [%s] not defined in .profile!\n",envName);
			return(errCodeParameter);
		}
		sprintf(tmpBuf + tmpIndex,"%s",getenv(envName));
		tmpIndex = strlen(tmpBuf);
		if (!str[strIndex])
			break;
		tmpBuf[tmpIndex] = str[strIndex];
		tmpIndex++;
		strIndex++;
	}
	tmpBuf[tmpIndex] = 0;
	//UnionLog("in UnionReadDirFromStr:: fullDir = [%s]\n",tmpBuf);
	// 2008/3/20增加，王纯军
	if (dirLevel <= 0)
	{
		memcpy(dir,tmpBuf,tmpIndex);
		dir[tmpIndex] = 0;
		return(tmpIndex);
	}
	// 2008/3/20增加结束

	dirIndex = 0;
	while (tmpBuf[dirIndex])
	{
		if (tmpBuf[dirIndex] != '/')
			continue;
		curDirLevel++;
		if (curDirLevel == dirLevel)
		{
			memcpy(dir,tmpBuf,dirIndex);
			dir[dirIndex] = 0;
			return(dirIndex);
		}
	}
	curDirLevel++;
	if (curDirLevel == dirLevel)
	{
		memcpy(dir,tmpBuf,dirIndex);
		dir[dirIndex] = 0;
		return(dirIndex);
	}

	return(errCodeParameter);
}

int UnionIsUnixShellRemarkLine(char *line)
{
	int	i=0;

	if (line == NULL)
		return(0);

	while (line[i])
	{
		switch (line[i])
		{
			case	'#' :
				return(1);
			case	' ':
				++i;
				continue;
			default:
				return(0);
		}
	}
	return(1);
}

int UnionReadUnixEnvVarFromStr(char *envVarStr,int lenOfEnvVarStr,char *envVar,int *envVarNameLen)
{
	int	i = 0, j = 0;
	char	varName[256];

	if (envVarStr[0] != '$')
		return(0);
	i = 1;
	while (i < lenOfEnvVarStr)
	{
		if (isdigit(envVarStr[i]) || isalpha(envVarStr[i]) || (envVarStr[i] == '_'))
		{
			if (j >= (int)sizeof(varName) - 1)
			{
				//UnionUserErrLog("in UnionReadUnixEnvVarFromStr:: sizeOfEnvVarName [%d] too small!\n",sizeof(varName)-1);
				return(errCodeParameter);
			}
			varName[j] = envVarStr[i];
			i++;
			j++;
		}
		else
			break;
	}
	varName[j] = 0;
	sprintf(envVar,"%s",getenv(varName));
	*envVarNameLen = i;
	return(strlen(envVar));
}

int UnionConvertUnixShellStr(char *oldStr,int lenOfOldStr,char *newStr,int sizeOfNewStr)
{
	int	i = 0,j = 0;
	int	ret;
	int	varNameLen;

	while (i < lenOfOldStr)
	{
		if (j >= sizeOfNewStr)
		{
			//UnionUserErrLog("in UnionConvertUnixShellStr:: too small sizeOfNewStr [%d]\n",sizeOfNewStr);
			return(errCodeParameter);
		}

		if (oldStr[i] == '$')
		{
			if ((ret = UnionReadUnixEnvVarFromStr(oldStr+i,lenOfOldStr-i,newStr+j,&varNameLen)) < 0)
			{
				//UnionUserErrLog("in UnionConvertUnixShellStr:: UnionReadUnixEnvVarFromStr!\n");
				return(ret);
			}
			i += varNameLen;
			j += ret;
		}
		else
		{
			newStr[j] = oldStr[i];
			++i;
			++j;
		}
	}
	return(j);
}

int UnionDeleteSuffixVersionFromStr(char *oldStr,char *newStr)
{
	int	len;

	strcpy(newStr,oldStr);
	len = strlen(oldStr);

	while (len > 0)
	{
		if (isdigit(newStr[len-1]) || (newStr[len-1] == '.'))
			len--;
		else
			break;
	}
	newStr[len] = 0;
	return(len);
}

int UnionStrContainCharStar(char *str)
{
	int	i=0;

	while (str[i])
	{
		if (str[i] == '*')
			return(1);
		i++;
	}
	return(0);
}

int UnionIsValidFullDateStr(char *date)
{
	int	tmp,month,day,year;
	char	str[32];

	if (strlen(date) != 8)
		return(0);

	for(tmp = 0; tmp < 8; tmp++)
		if (!isdigit(date[tmp]))
			return(0);
	memcpy(str,date+4,2);
	str[2] = 0;
	if (((month = atoi(str)) < 1) || (month > 12))
		return(0);
	memcpy(str,date+6,2);
	str[2] = 0;
	if (((day = atoi(str)) < 0) || (day > 31))
		return(0);
	switch (month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return(1);
		case 4:
		case 6:
		case 9:
		case 11:
			if (day <= 30)
				return(1);
			else
				return(0);
		case 2:
			memcpy(str,date,4);
			str[4] = 0;
			year = atoi(str);
			if ((year % 4 == 0) && (year % 100 != 0))
			{
				if (day <= 29)
					return(1);
				else
					return(0);
			}
			else
			{
				if (day <= 28)
					return(1);
				else
					return(0);
			}
		default:
			return(0);
	}
}

// 从文本文件读取一行
int UnionReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf)
{
	char *p;
	char c,ch;
	int  num;

	if ((fp == NULL) || (buf == NULL) || (sizeOfBuf <= 0))
		return(errCodeParameter);

	p = buf;
	num = 0;
	while (!feof(fp))
	{
		if (num >= sizeOfBuf - 1)
		{
			*p = 0;
			//UnionUserErrLog("in UnionReadOneFileLine:: line [%s] too large!\n",buf);
			return(-1);
		}
		fscanf(fp,"%c",&c);
		if (c == '\n')
		{
			*p = 0;
			//UnionLog("*** [%s] [%d] [%d]\n",buf,num,sizeOfBuf);
			return(num);
		}
		// 2008/10/22 增加
		if (c == 0x0d)
		{
			fscanf(fp,"%c",&ch);
			if (ch == 0x0a)
			{
				*p = 0;
				return(num);
			}
			*p = c;
			p++;
			*p = ch;
			p++;
			num += 2;
			continue;
		}
		else if (c == 0x0a)
		{
			fscanf(fp,"%c",&ch);
			if (ch == 0x0a)
			{
				*p = 0;
				return(num);
			}
			*p = c;
			p++;
			*p = ch;
			p++;
			num += 2;
			continue;
		}
		// 2008/10/22 增加结束
		*p = c;
		p++;
		num++;
	}

	*p = 0;
	num--;
	if (num >= 0)
		buf[num] = 0;
	//UnionLog("*** [%s] [%d] [%d]\n",buf,num,sizeOfBuf);
	if (num == 0)
		return(errCodeEnviMDL_NullLine);
	else
		return(num);
}

// 2008/10/3,增加
// 从文本文件读取一行
int UnionReadOneLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf)
{
	return(UnionReadOneLineFromTxtStr(fp,buf,sizeOfBuf));
}

// Mary add begin, 20080925
// 以下3个函数由昌子祥编写测试，陈家梅将源码拷贝到本程序中
int	UnionIsDigit(char c)
{
	int 	isdigit;
	isdigit = c - '0';

	if((isdigit >=0) &&(isdigit <=9))
		return isdigit;
	else return -1;
}

int	UnionIsHChar(char c)
{
	int 	ischar;
	ischar = c - 'A';

	if((ischar >=0) &&(ischar <=26))
		return ischar;
	else return -1;
}

int	UnionStrToInt(char	*str,int Len)
{
	return(UnionConvertIntStringToInt(str,Len));
}
// Mary add end, 20080925

// Mary add begin, 20081007
// 以下2个函数由昌子祥编写测试，陈家梅将源码拷贝到本程序中
/*
2008/10/06，昌子翔增加
功能：返回以分隔符分割的子串
输入参数
	srcStr：串
	Seperator：源分隔符
输出参数
	desStr：分割字符串
	CntOfDesStr：分割后子串个数
返回值
	>=0：串位移 offset
	<0：出错代码
*/
int UnionStrChr(char *srcStr,char Seperator,char desStr[][1024],int *CntOfDesStr)
{
	int	fieldIndex , offset, v_offset;
	char	*p;
	char	*pIndex = NULL;

	fieldIndex = 0;
	offset = 0 ;
	v_offset = 0;
	p = srcStr;
	while((pIndex = strchr(p, Seperator))!=NULL)
	{
		v_offset = pIndex - p;
		memcpy(desStr[fieldIndex], p, v_offset);
		desStr[fieldIndex][v_offset] = 0;
		offset += v_offset;
		p = pIndex + 1;
		fieldIndex++;
	}
	*CntOfDesStr = fieldIndex;
	return offset+ fieldIndex;
}

/*
2008/10/06，昌子翔增加
功能
	返回以分隔符分割的子串
输入参数
	srcStr：串
	Seperator：分隔串
输出参数
	desStr：分割字符串
返回值
	>=0：分割后子串个数
	<0：出错代码
*/
int UnionStrToK(char *srcStr,char *Seperator,char desStr[][1024])
{
	char	*ptr = NULL;
	char	*var = NULL;
	int	Cnt = 0;
	char	tmpBuf[40960];

	if (strlen(srcStr) < sizeof(tmpBuf))
	{
		strcpy(tmpBuf,srcStr);
		ptr = strtok(tmpBuf,Seperator);
	}
	else
	{
		var = strdup(srcStr);
		ptr = strtok(var,Seperator);
	}
	while(ptr != NULL)
	{
		strcpy(desStr[Cnt],ptr);
		Cnt += 1;
		ptr = strtok(NULL,Seperator);
	}

	if (var)
		free(var);
	return Cnt;
}
// Mary add end, 20081007

// Mary add begin, 20081124
/*
功能：根据转换表将十六进制字符串转换为十进制字符串
输入参数：
	hexStr：十六进制字符串
	hexStrLen：十六进制字符串的长度
	transTable：十六进制字符对应的数字表，例如："0123456789012345"，表示'A-F'转换为'0-5'
	digitStrLen：需要返回的数字串的长度
输出参数：
	digitStr：十进制字符串
	sizeOfDigitStr：digitStr的存储空间大小
返回值：
	>=0：成功，返回digitStr的长度
	<0：失败
算法过程：
	1、十六进制字符串中从左到右取'0'－'9'之间的数字，顺序组成一个串；
	2、十六进制字符串中从左到右取'A'－'F'之间的字母，根据十六进制字母转换表，
	   将字母转换成数字，按顺序组成一个串；
	3、将第1步和第2步的两个串相连，得到一个全数字串，按照长度取值返回
*/
int UnionTranslateHEXStringToDigitString(char *hexStr,int hexStrLen,char *transTable,int digitStrLen,char *digitStr,int sizeOfDigitStr)
{
	int	i,j,iHexNum=0,iDigitNum=0;
	char	caHexTable[7]="ABCDEF",caTransTable[17]="0123456789012345";
	char	caDigitBuf[1024],caAbcBuf[1024];

	if (hexStr == NULL || hexStrLen <= 0 || digitStrLen < 0 || digitStr == NULL || sizeOfDigitStr <= 0)
	{
		UnionUserErrLog("in UnionTranslateHEXStringToDigitString:: Parameters error!\n");
		return(errCodeParameter);
	}
	if (digitStrLen == 0)
		return(0);

	if (hexStrLen < digitStrLen)
	{
		UnionUserErrLog("in UnionTranslateHEXStringToDigitString:: hexStrLen [%d] < required digitStrLen [%d]!\n",hexStrLen,digitStrLen);
		return(errCodeTooShortLength);
	}

	if (transTable != NULL && strlen(transTable) >= 16)
		memcpy(caTransTable,transTable,16);

	for ( i = 0; i < hexStrLen; i++)
	{
		if ((hexStr[i] >= 'A' && hexStr[i] <= 'F') || (hexStr[i]>= 'a' && hexStr[i]<='f'))
		{
			caAbcBuf[iHexNum] = toupper(hexStr[i]);
			for (j=0;j<6;j++)
			{
				if (caAbcBuf[iHexNum] == caHexTable[j])
					caAbcBuf[iHexNum] = caTransTable[10+j];
			}
			iHexNum++;
		}
		else if (hexStr[i] >= '0' && hexStr[i] <= '9')
		{
			caDigitBuf[iDigitNum] = hexStr[i];
			iDigitNum++;
		}
		else
		{
			UnionUserErrLog("in UnionTranslateHEXStringToDigitString:: string [%s] is not HEX string!\n",hexStr);
			return(errCodeParameter);
		}
	}
	caAbcBuf[iHexNum] = 0;
	caDigitBuf[iDigitNum] = 0;

	if (sizeOfDigitStr < digitStrLen)
	{
		UnionUserErrLog("in UnionTranslateHEXStringToDigitString:: sizeOfDigitStr [%d] < digitStrLen [%d]!\n",sizeOfDigitStr,digitStrLen);
		return(errCodeSmallBuffer);
	}

	if (iDigitNum >= digitStrLen)
		memcpy(digitStr,caDigitBuf,digitStrLen);
	else
	{
		memcpy(digitStr,caDigitBuf,iDigitNum);
		memcpy(&digitStr[iDigitNum],caAbcBuf,digitStrLen-iDigitNum);
	}

	return(digitStrLen);
}
// Mary add end, 20081124

// 陈家梅增加张永定写的函数, 20081124
// 将字符串类型的的时间值转换为time_t型，datetime必须为"yyyymmddhhmmss"
time_t UnionTranslateStringTimeToTime(char *datetime)
{
	char	tmpBuf[32];
	struct tm	t;

	if (strlen(datetime) != 14)
		return(0);

	memcpy(tmpBuf,datetime,4);
	tmpBuf[4] = 0;
	t.tm_year = atoi(tmpBuf) - 1900;

	memcpy(tmpBuf,datetime+4,2);
	tmpBuf[2] = 0;
	t.tm_mon = atoi(tmpBuf) - 1;

	memcpy(tmpBuf,datetime+6,2);
	tmpBuf[2] = 0;
	t.tm_mday = atoi(tmpBuf);

	memcpy(tmpBuf,datetime+8,2);
	tmpBuf[2] = 0;
	t.tm_hour = atoi(tmpBuf);

	memcpy(tmpBuf,datetime+10,2);
	tmpBuf[2] = 0;
	t.tm_min = atoi(tmpBuf);

	memcpy(tmpBuf,datetime+12,2);
	tmpBuf[2] = 0;
	t.tm_sec = atoi(tmpBuf);

	// Add By Huangbaoxin, 20090104
	t.tm_isdst = -1;

	return(mktime(&t));
}
// 陈家梅增加张永定写的函数, 20081124

int UnionPower(int base, int n)
{
	int i;
	int result=1;
	for (i=0; i<n; i++)
		result*=base;
	return result;
}

int UnionOxToD(char *hexStr)
{
    int i,value=0,length;
    length=strlen(hexStr);

    for(i=0;i<length;i++)
    {
	if(*(hexStr+i)=='\0')
	    break;

	if(('0' <= *(hexStr+i)) && (*(hexStr+i)<='9') )
	{
	    value=value+(*(hexStr+i)-'0')*UnionPower(16,(length-i-1));

	}
       else if(*(hexStr+i)>='a')
	{

	    value=value+(*(hexStr+i)-'a'+10)*UnionPower(16,(length-i-1));

	}
	else
	{
	    value=value+(*(hexStr+i)-'A'+10)*UnionPower(16,(length-i-1));

	}
    }
    return value;
}

// Mary add begin, 20090327, 函数由张永定编写
/*
功能：		对数字进行四舍五入

输入参数：	dNumber	输入的数字
		digit保留小数位的位数

返回值：	四舍五入后的数字
*/

double UnionChangeDigitOfNumber(double dNumber,int digit)
{
	double	fTmp;
	int	iTmp;
	int	i;
	double	fMul=1;
	for (i = 1; i <= digit; i++)
		fMul = fMul * 10;

	fTmp = dNumber * fMul;
	iTmp = (int)fTmp;

	if(fTmp-iTmp >= 0.5)
	{
		return (iTmp + 1) / fMul;
	}
	else
	{
		return iTmp/fMul;
	}
}
// Mary add end, 20090327

// 去除字符串的右空格
int UnionRTrim(char *str)
{
	int 	len;
	int	num;
	
	if ( (str == NULL) || ((len=strlen(str))==0) )
		return 0;
	//for (num = len-1; len >= 0;num--)
	for (num = len-1; num >= 0;num--)
	{
		if (str[num] != ' ')
			break;
		str[num] = 0;
	}
	return 0;
}

// 从一个mngSvr格式的请求串中删除某域
/*
int UnionDeleteRecFldFromRecStr(char *str, char *fldName)
{
	int     ret;
	char    *p,*q;
	// modify by wangk 2010-01-14
	//char    tmpStr[1024];
	char	tmpStr[4096*4];
	// modify end by wangk

	memset(tmpStr, 0, sizeof tmpStr);

	if ((str == NULL) || (fldName == NULL))
		return 0;

	p = q = strstr(str, fldName);
	if (p == NULL)
		return 0;

	p = strchr(q,'|');
	if (p == NULL)
		return 0;
	p++;

	if (q == str)
	{
		ret = sprintf(tmpStr,"%s", p);
	}
	else
	{
		*q = 0;
		ret = sprintf(tmpStr,"%s%s", str, p);
	}
	strcpy(str, tmpStr);
	return (ret);
}
*/
/*hzh in 2011.8.9重写了该函数,
避免对UnionDeleteRecFldFromRecStr("keyModelID=mdk-mac|mac=D252343A4EB75801|","mac")出现误删除的错误
*/
//删除某个域
int UnionDeleteRecFldFromRecStr(char *unionRecStr,  char *fldName)
{
	char		tmpBuf[8192];
	char		fullFldName[256];
	char		*pos = NULL;
	int		offset = 0;
	int lenOfRecStr = 0;

	if ((NULL == unionRecStr) || (NULL == fldName))
	{
		return 0;
	}
	
	lenOfRecStr = strlen(unionRecStr);

	sprintf(fullFldName,"%s=",fldName);
	if (strncmp(unionRecStr,fullFldName,strlen(fullFldName)) == 0 )
	{
		pos = unionRecStr;
	}
	else
	{
		sprintf(fullFldName,"|%s=",fldName);
		if ((pos = strstr(unionRecStr,fullFldName)) == NULL)
		{
			return 0;
		}
		pos++;
	}
	
	if (pos == NULL)
	{
		return 0;
	}

	//memset(tmpBuf, 0, sizeof(tmpBuf));
	offset = pos - unionRecStr;
	memcpy(tmpBuf, unionRecStr, offset);
	tmpBuf[offset] = '\0';
	
	if ((pos = strchr(pos, '|')) == NULL)
	{
		printf("in UnionDeleteSpecFldOnUnionRec, bad form of [%s].\n", unionRecStr);
		return 0;
	}

	if ((0 != (pos + 1)) || ('\0' != (pos + 1)))
		memcpy(tmpBuf + offset, pos + 1, lenOfRecStr - (pos - unionRecStr) - 1);
	tmpBuf[offset+lenOfRecStr - (pos - unionRecStr)-1] = '\0';

	//memcpy(unionRecStr, tmpBuf, strlen(tmpBuf) + 1);
	UnionDebugLog("in UnionDeleteSpecFldOnUnionRec:: tmpBuf [%s] unionRecStr [%s] \n", tmpBuf, unionRecStr);
	strcpy(unionRecStr, tmpBuf);


	return strlen(tmpBuf);
}

// 判断两个校验值是否一致
int UnionIsCheckValueTheSame(char *checkValue1, char *checkValue2)
{
	if ( checkValue1 == NULL || checkValue2 == NULL)
		return 0;

	while( (*checkValue1!='\0') && (*checkValue2!='\0') )
	{
		if ( *checkValue1 != *checkValue2 )
			return 0;

		checkValue1++;
		checkValue2++;
	}
	return 1;
}

char *UnionGetPrefixOfDBField()
{
	return getenv("prefixOfDBField");
}

/*
功能	将数据库的字符串中的单引号替换为2个单引号，插入和更新时字符串中有单引号的时候会出错
输入参数
	oriStr	 	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateSpecCharInDBStr(char *oriStr, int lenOfOriStr, char *desStr, int sizeOfDesStr)
{
	int	index;
	int	desLen = 0;
	char	tmpBuf[8192*2];

	if( lenOfOriStr >= (int)sizeof(tmpBuf) )
	{
		UnionUserErrLog("in UnionTranslateSpecCharInDBStr:: oriLen [%d] too long!\n", lenOfOriStr);
		return(errCodeSmallBuffer);
	}

	for( index = 0; index < lenOfOriStr; index++ )
	{
		if( desLen + 1 >= (int)sizeof(tmpBuf) )
		{
			UnionUserErrLog("in UnionTranslateSpecCharInDBStr:: oriLen [%d] too long!\n", lenOfOriStr);
			return(errCodeSmallBuffer);
		}

		if( oriStr[index] == '\'' )
		{
			// 前后都没有单引号
			if( index == 0 )
			{
				if( index + 1 < lenOfOriStr )
				{
					if( oriStr[index+1] != '\'' )
					{
						tmpBuf[desLen] = '\'';
						desLen++;
					}
				}
				if( index + 1 == lenOfOriStr )
				{
					tmpBuf[desLen] = '\'';
					desLen++;
				}
			}
			else
			{
				if( index + 1 < lenOfOriStr )
				{
					if( oriStr[index-1] != '\'' && oriStr[index+1] != '\'' )
					{
						tmpBuf[desLen] = '\'';
						desLen++;
					}
				}
				if( index + 1 == lenOfOriStr )
				{
					if( oriStr[index-1] != '\'' )
					{
						tmpBuf[desLen] = '\'';
						desLen++;
					}
				}
			}
		}
		tmpBuf[desLen] = oriStr[index];
		desLen++;
	}

	if (desLen >= sizeOfDesStr)
	{
		UnionUserErrLog("in UnionTranslateSpecCharInDBStr:: sizeOfDesStr [%d] too small!\n",sizeOfDesStr);
		return(errCodeSmallBuffer);
	}

	memcpy(desStr,tmpBuf,desLen);
	desStr[desLen] = 0;

	return(desLen);
}

// 如果输入串(inStr)的长度小于len，则在串前补fixedChar，补足长度，如果strLen>len，则只取串最前面的len个字符
int UnionLeftFixedSpecCharToStr(char *inStr, int lenOfInStr, int len, char fixedChar, char *outStr, int sizeOfBuf)
{
	if (len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionLeftFixdSpecCharToStr:: buf is too small [%d]!\n", sizeOfBuf);
		return(errCodeSmallBuffer);
	}
		
	if (len <= lenOfInStr)
	{
		memcpy(outStr, inStr, len);
		outStr[len] = 0;
	}
	else
	{
		memset(outStr, fixedChar, len);
		memcpy(outStr+len-lenOfInStr, inStr, lenOfInStr);
	}

	return(len);		
}

int UnionLeftFixedZeroToStr(char *inStr, int lenOfInStr, int len, char *outStr, int sizeOfBuf)
{
	return(UnionLeftFixedSpecCharToStr(inStr, lenOfInStr, len, '0', outStr, sizeOfBuf));
}


// 将格式${para}转化为para
int UnionConvertParaToRuleStr(char *para, char *ruleStr, int sizeOfBuf)
{
	char	    *p,*q;

	p = strchr(para,'{');
	if (p == NULL)
		return(errCodeParameter);

	p++;
	q = p;

	p = para;
	p = strchr(para,'}');
	if (p == NULL)
		return(errCodeParameter);

	while(*p++ != '\0')
		continue;
	p -= 2;

	memcpy(ruleStr, q, (p-q));
	return(UnionConvertOneFldSeperatorInRecStrIntoAnotherNum(ruleStr,strlen(ruleStr), '-',',', 2, ruleStr, sizeOfBuf));
}

/*
int UnionXOR(char *pData1, char *pData2, int nLenOfData, char *pData3)
{
	int     i,nLength1,nLength2;
	char    ch1,ch2;

	if (nLenOfData<=0)
	{
		//strcpy(aErrorCode,S_XOR_DATA_LEN_ERROR);
		return(errCodeParameter);
	}

	for (i=0;i<nLenOfData;i++)
	{
		ch1=pData1[i];
		if (ch1>='A' && ch1 <='F')
			ch1=ch1-0x40+0x09;
		else if (ch1>='a' && ch1 <='f')
			ch1=ch1-0x60+0x09;
		else ch1 -= 0x30;

		ch2=pData2[i];
		if (ch2>='A' && ch2 <='F')
			ch2=ch2-0x40+0x09;
		else if (ch2>='a' && ch2 <='f')
			ch2=ch2-0x60+0x09;
		else ch2 -= 0x30;

		ch1=ch1 ^ ch2;

		if (ch1 > 0x09) ch1 = 0x40+(ch1-0x09);
		else ch1 += 0x30;
		pData3[i]=ch1;
	}

	return(nLenOfData);
}
*/

// 检查日期是不是MMYY的格式
int UnionIsValidMMYYDateStr(char *mmyyDate)
{
	int	tmp,month;
	char	str[32];

	if (strlen(mmyyDate) != 4)
		return(0);

	for(tmp = 0; tmp < 4; tmp++)
		if (!isdigit(mmyyDate[tmp]))
			return(0);
	memcpy(str,mmyyDate,2);
	str[2] = 0;
	if (((month = atoi(str)) < 1) || (month > 12))
		return(0);
	
	return(1);
}


// 按位取反
int UnionCPL(unsigned char *pData1, int nLenOfData, unsigned char *pData2)
{
	int		i;
	unsigned char	ch1;

	if (nLenOfData <= 0)
	{
		UnionUserErrLog("in UnionCPL:: nLenOfData [%d] Error!\n",nLenOfData);
		return(errCodeParameter);
	}

	for (i = 0; i < nLenOfData; i++)
	{
		ch1 = pData1[i];
		if (ch1>='A' && ch1 <='F')
			ch1 = ch1 - 0x40 + 0x09;
		else if (ch1>='a' && ch1 <='f')
			ch1 = ch1 - 0x60 + 0x09;
		else
			ch1 -= 0x30;
		
		ch1 = ~ch1;
		ch1 &= 0x0f;

		if (ch1 > 0x09)
			ch1 = 0x40 + (ch1 - 0x09);
		else
			ch1 += 0x30;
		pData2[i] = ch1;
	}

	return (nLenOfData);
}

/*
功能	写ini文件中的一个域
输入参数
	appName		字段名
	keyName		键名
	keyValue	键值
	fileName	ini文件名
输出参数
	无
返回值
	>=0	正确
	<0	错误
*/
int UnionINIWriteString(char *appName,char *keyName,char *keyValue,char *fileName)
{
	int	len;
	int	appflag = 0;
	int	keyflag = 0;
	int	fistBlank = 0;
	char	tmpBuf[1024];
	char	tmpFileName[128];
	char	tmpBuf2[1024];
	FILE	*fp = NULL;
	FILE	*tmpfp = NULL;
	
	// 检查ini文件是否存在
	if (access(fileName,0) != 0)	// 文件不存在
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionINIWriteString:: fopen w [%s]!\n",fileName);
			return(errCodeUseOSErrCode);			
		}
		
		fprintf(fp,"[%s]\n",appName);
		fprintf(fp,"%s=%s\n",keyName,keyValue);
		fprintf(fp,"\n");
		fclose(fp);
		return(0);
	}

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionINIWriteString:: fopen r[%s]!\n",fileName);
		return(errCodeUseOSErrCode);			
	}
		
	len = sprintf(tmpFileName,"%s.tmp",fileName);
	tmpFileName[len] = 0;
	if ((tmpfp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionINIWriteString:: fopen w[%s]!\n",tmpFileName);
		fclose(fp);
		return(errCodeUseOSErrCode);			
	}
	
	while (!feof(fp))
	{
		// 读取一行
		if ((len = UnionReadOneLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		
		tmpBuf[len] = 0;
		// 空行
		if (len == 0)
		{
			if (fistBlank == 0)	// 第一个空行
			{
				fprintf(tmpfp,"\n");
				fistBlank = 1;
			}
			continue;
		}
		else
			fistBlank = 0;
		
		if (keyflag == 1)	// 已经找到了
		{
			fprintf(tmpfp,"%s\n",tmpBuf);
			continue;
		}
		
		if (appflag == 0)	// 查找字段
		{	
			len = sprintf(tmpBuf2,"[%s]",appName);
			tmpBuf2[len] = 0;
			if (memcmp(tmpBuf,tmpBuf2,strlen(tmpBuf2)) == 0)
				appflag = 1;
			
			fprintf(tmpfp,"%s\n",tmpBuf);
		}
		else
		{
			// 新的字段开始，参数不存在，增加新的参数
			if ((tmpBuf[0] == '[') && (tmpBuf[len-1] == ']'))
			{
				fprintf(tmpfp,"%s=%s\n",keyName,keyValue);
				fprintf(tmpfp,"%s\n",tmpBuf);
				keyflag = 1;
				continue;
			}
			
			len = sprintf(tmpBuf2,"%s=",keyName);	// 查找建值
			tmpBuf2[len] = 0;
			if (memcmp(tmpBuf,tmpBuf2,strlen(tmpBuf2)) == 0)
			{
				fprintf(tmpfp,"%s=%s\n",keyName,keyValue);					
				keyflag = 1;
				continue;
			}
			
			fprintf(tmpfp,"%s\n",tmpBuf);
		}
	}

	if (appflag == 0)	// 文件中不存在这个字段
	{
		fprintf(tmpfp,"\n");
		fprintf(tmpfp,"[%s]\n",appName);
	}
	
	if (keyflag == 0)	// 文件中不存在这个参数
		fprintf(tmpfp,"%s=%s\n",keyName,keyValue);

	fclose(fp);
	fflush(tmpfp);
	fclose(tmpfp);
	
	// 复制文件
	len = sprintf(tmpBuf,"mv %s %s > /dev/null 2>&1",tmpFileName,fileName);
	tmpBuf[len] = 0;
	system(tmpBuf);
		
	return(0);
}

/*
功能	读ini文件中的一个字符串域
输入参数
	appName		字段名
	keyName		键名
	defaultValue	默认值，如果没找到，返回这个值
	sizeOfBuf	键值缓冲区的大小
	fileName	ini文件名
输出参数
	keyValue	键值	
返回值
	>0	键值的长度
	=0	未找
	<0	错误
*/
int UnionINIReadString(char *appName,char *keyName,char *defaultValue,char *keyValue,int sizeOfBuf,char *fileName)
{
	int	len;
	int	appflag = 0;
	char	tmpBuf[1024];
	char	tmpBuf2[1024];
	FILE	*fp = NULL;
	char	*ptr = NULL;
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionINIReadString:: file[%s] is not exist!\n",fileName);
		if (defaultValue != NULL)
		{
			if ((len = strlen(defaultValue)) < sizeOfBuf)
			{
				memcpy(keyValue,defaultValue,len);
				keyValue[len] = 0;
				return(len);
			}
		}
		return(0);
	}
	
	while (!feof(fp))
	{
		// 读取一行
		if ((len = UnionReadOneLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		
		tmpBuf[len] = 0;
		// 空行
		if (len == 0)
			continue;
		
		if (appflag == 0)	// 查找字段
		{	
			len = sprintf(tmpBuf2,"[%s]",appName);
			tmpBuf2[len] = 0;
			if (memcmp(tmpBuf,tmpBuf2,strlen(tmpBuf2)) == 0)
				appflag = 1;
				
		}
		else
		{
			// 新的字段开始，参数不存在
			if ((tmpBuf[0] == '[') && (tmpBuf[len-1] == ']'))
				break;
			
			len = sprintf(tmpBuf2,"%s=",keyName);	// 查找建值
			tmpBuf2[len] = 0;
			if (strstr(tmpBuf,tmpBuf2) != NULL)
			{
				ptr = tmpBuf+strlen(keyName)+1;
				if ((len = strlen(ptr)) < sizeOfBuf)
				{
					memcpy(keyValue,ptr,len);
					keyValue[len] = 0;
					fclose(fp);
					return(len);
				}
				else
				{
					fclose(fp);
					return(0);
				}
			}
		}
	}		

	fclose(fp);
	return(0);
}

/*
功能	读ini文件中的一个整型域
输入参数
	appName		字段名
	keyName		键名
	defaultValue	默认值，如果没找到，返回这个值
	fileName	ini文件名
输出参数
返回值
	键值

*/
int UnionINIReadInt(char *appName,char *keyName,int defaultValue,char *fileName)
{
	int	ret;
	char	value[32];

	ret = UnionINIReadString(appName,keyName,NULL,value,sizeof(value),fileName);
	if(ret <= 0)
		return(defaultValue);
	else
		return(atoi(value));
}

// 获得指定秒数之前的一个时间值agoTime，格式为"yyyymmddHHMMSS"
int UnionGetFullDateTimeSecondsBeforeNow(int secondsBefore, char *agoTime)
{
	time_t			lCurTime;
	time_t			lAgoTime;
	struct tm		*ptm;

	lCurTime = time(NULL);
	lAgoTime = lCurTime - secondsBefore;
	ptm = localtime(&lAgoTime);
	sprintf(agoTime, "%04d%02d%02d%02d%02d%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return(0);
}
// 返回星期值:1~7
int UnionGetDays()
{
	struct tm		*ptm = NULL;
	time_t			curTime;
	char			timeStr[16];

	time(&curTime);
	ptm = localtime(&curTime);

	strftime(timeStr, sizeof(timeStr), "%u", ptm);

	return(atoi(timeStr));
}

// added 2012-09-12
// 是否使用动态RSA批次号 
// 批次号:1NNNNYYDDD, NNNN为批次号,YY为年份,DDD为一年中的第几天
int UnionIsUseDynamicRsaBatchNo()
{       
	int			ret = 0;
	if(UnionReadIntTypeRECVar("useDynamicRsaBatchNo") <= 0)
	{       
		ret = 0;
	}	       
	else	    
	{       
		ret = 1;
	}       
	
	return(ret);
}

int UnionFormRsaBatchNoCondition(int inBatchNo, char *condition)
{
	if(!UnionIsUseDynamicRsaBatchNo())
	{
		sprintf(condition, "unbatchNo=%d", inBatchNo);
		return(0);
	}
	else
	{
		if(inBatchNo > 9999)
		{
			sprintf(condition, "unbatchNo=%d", inBatchNo);
			return(0);
		}
		sprintf(condition, "unbatchNo > 1%04d10000 and unbatchNo < 1%04d99365", inBatchNo, inBatchNo);
		return(1);
	}
}

/*
 * UnionTransRsaBatchNo 返回转换后的批次号
 * 使用时间为前缀的批次号，以防止流程号重复使用以导致记录重复
 * 转换后的批次号格式为:1NNNNYYDDD,
 *   其中YY为年份，DDD为一年中的天数，NNNN为传进来的批次号
 */
int UnionTransRsaBatchNo(int inBatchNo)
{
	int			ret;
	struct tm		*ptm;
	time_t			curTime;
	char			timeStr[16];

	if(!UnionIsUseDynamicRsaBatchNo())
	{
		return(inBatchNo);
	}
	else
	{
		if(inBatchNo > 9999)
		{
			return(inBatchNo);
		}
		time(&curTime);
		ptm = localtime(&curTime);
		ret = snprintf(timeStr,sizeof(timeStr), "1%04d", inBatchNo);
		strftime(timeStr + ret, sizeof(timeStr) - ret, "%y%j", ptm);

		return(atoi(timeStr));
	}
}

int UnionTime(int *ctime)
{
	time_t		curTime;
	int		itime;
	time(&curTime);
	itime = curTime;
	if(ctime != NULL)
	{
		*ctime = itime;
	}
	return(itime);
}
// end of addition 2012-09-12

char *UnionStrCaseStr(const char *haystack, const char *needle)
{
	unsigned int nlen, hlen;
	unsigned int ix;
 
	if (needle == NULL || haystack == NULL) 
		return (NULL);
	nlen = strlen(needle);
	hlen = strlen(haystack);
  
	for (ix = 0; ix + nlen <= hlen; ix++) {
		if (strncasecmp(needle, haystack + ix, nlen) == 0) {
			return ((char *)haystack + ix);
		}
	}
	return (NULL);
}

char *UnionStrRCaseStr(const char *haystack, const char *needle)
{
	unsigned int nlen, hlen;
	unsigned int ix;
 
	if (needle == NULL || haystack == NULL) 
		return (NULL);
	nlen = strlen(needle);
	hlen = strlen(haystack);
  
	for (ix = hlen - nlen; ix > 0; ix--) {
		if (strncasecmp(needle, haystack + ix, nlen) == 0) {
			return ((char *)haystack + ix);
		}
	}
	return (NULL);
}

int UnionGetTableNameAndFieldNameFromSQL(char *sql,char *tableName,char *fieldName)
{
	char	*ptr1 = NULL;
	char	*ptr2 = NULL;
	char	*ptr3 = NULL;
	char	sqlID1[128];
	char	sqlID2[128];
	
	if (tableName != NULL)
		tableName[0] = 0;

	if (fieldName != NULL)
		fieldName[0] = 0;
	
	sscanf(sql,"%s",sqlID1);
	
	// 去掉多于的空格
	if (strcasecmp(sqlID1,"select") == 0)
	{
		ptr1 = strstr(sql,sqlID1);
		
		if ((ptr2 = UnionStrCaseStr(ptr1," from ")) == NULL)
			return(errCodeSQLInvalidSQLStr);
	
		// 表名的起始位置
		if (tableName != NULL)
			sscanf(ptr2 + 6,"%s",tableName);

		if (fieldName != NULL)
		{
			sprintf(fieldName,"%.*s",(int)(ptr2 - ptr1 - 7),ptr1 + 7);
			UnionFilterHeadAndTailBlank(fieldName);
		}	
	}
	else if (strcasecmp(sqlID1,"update") == 0)
	{
		ptr1 = strstr(sql,sqlID1);
		// 表名的起始位置
		if (tableName != NULL)
			sscanf(ptr1 + 7,"%s",tableName);
	}
	else if (strcasecmp(sqlID1,"insert") == 0)
	{
		ptr1 = strstr(sql,sqlID1);
		sscanf(ptr1 + 7,"%s",sqlID2);
		if (strcasecmp(sqlID2,"into") != 0)
			return(errCodeSQLInvalidSQLStr);
		ptr2 = UnionStrCaseStr(ptr1 + 7,"into ");
		
		if (tableName != NULL)
			sscanf(ptr2 + 5,"%[^(^ ]",tableName);
	}
	else if (strcasecmp(sqlID1,"delete") == 0)
	{
		ptr1 = strstr(sql,sqlID1);
		sscanf(ptr1 + 7,"%s",sqlID2);
		if (strcasecmp(sqlID2,"from") != 0)
			return(errCodeSQLInvalidSQLStr);
		ptr2 = UnionStrCaseStr(ptr1 + 7,"from ");
		if (tableName != NULL)
			sscanf(ptr2 + 5,"%s",tableName);
	}
	else if (strcasecmp(sqlID1,"with") == 0)
	{
		ptr1 = strstr(sql,sqlID1);

		if ((ptr2 = UnionStrCaseStr(ptr1," from ")) == NULL)
			return(errCodeSQLInvalidSQLStr);

		// 表名的起始位置
		if (tableName != NULL)
			sscanf(ptr2 + 6,"%s",tableName);

		if ((ptr3 = UnionStrRCaseStr(ptr2,"select ")) == NULL)
			return(errCodeSQLInvalidSQLStr);

		if ((ptr1 = UnionStrCaseStr(ptr3," from ")) == NULL)
			return(errCodeSQLInvalidSQLStr);
	
		if (fieldName != NULL)
		{
			sprintf(fieldName,"%.*s",(int)(ptr1 - ptr3 - 7),ptr3 + 7);
			UnionFilterHeadAndTailBlank(fieldName);
		}	
	}
	else
		return(errCodeSQLInvalidSQLStr);

	return(1);
}

int UnionGetCurrentSecondTime()
{
	return((int)time(NULL));
}

long UnionGetFileSizeByFilePointer(FILE *fp)
{
        if(fp == NULL)
        {
                UnionUserErrLog("in UnionGetFileSizeByFilePointer:: NULL pointer!\n");
                return(errCodeUseOSErrCode);
        }

        fseek(fp, 0L, SEEK_END);
        return(ftell(fp));
}
long UnionGetFileSizeByFileName(char *fileName)
{
        long            fSize = 0;
        FILE            *fp = NULL;

        if((fp = fopen(fileName,"r")) == NULL)
        {
                UnionSystemErrLog("in UnionGetFileSizeByFileName:: fopen [%s]\n", fileName);
                return(errCodeUseOSErrCode);
        }
        fSize = UnionGetFileSizeByFilePointer(fp);
        fclose(fp);
        return(fSize);
}

// 把str2插入str1的指定pos位置,pos的下标为1
int UnionInsertString(char *str1,char *str2,int pos)
{
	int len1,len2;
	int i,j;

        len1 = strlen(str1);
        len2 = strlen(str2);

	memmove(str1+pos+len2,str1+pos,len1 - pos);
        for(i = pos,j = 0; j < len2; i++,j++)
                str1[i] = str2[j];
        return(len1+len2);
}

// 读取客户化标识
char *UnionGetIDOfCustomization()
{
	return(UnionReadStringTypeRECVar("abOfMyself"));
}

// 判断是否是界面服务
int UnionIsUITrans(char *sysID)
{
	if ((memcmp(sysID,"BUI",4) == 0) || (memcmp(sysID,"CUI",4) == 0) || (memcmp(sysID,"CONFUI",7) == 0))
		return(1);
	else
		return(0);
}

// 字符串填充
// bcdStr: 输入的BCD扩展字符串
// algGrpLen: 分组长度, 如des算法分组为8, sm4分组为16
// fillMode: 填充方式, 1:填充00, 2: 填充8000, 3: 强制填充00, 4: 强制填充8000
int UnionFillStr(char *bcdStr, int algGrpLen, int fillMode)
{
	int                     fillLen = 0;
	int                     len = 0;
	char                    *fillStr00 = "00000000000000000000000000000000";
	char                    *fillStr80 = "80000000000000000000000000000000";

	algGrpLen *= 2;

	len = strlen(bcdStr);

	if(len % 2 != 0) 
	{
		len--;
	}

	fillLen = algGrpLen  - len % algGrpLen;

	switch(fillMode)
	{
		// 填充00
		case 1:
			if(fillLen != algGrpLen)
			{
				sprintf(bcdStr + len, "%.*s", fillLen, fillStr00);
				len += fillLen;
			}
			break;
		// 填充8000
		case 2:
			if(fillLen != algGrpLen)
			{
				sprintf(bcdStr + len, "%.*s", fillLen, fillStr80);
				len += fillLen;
			}
			break;
		// 强制填充00
		case 3:
			if(fillLen == algGrpLen)
			{
				
				sprintf(bcdStr + len, "%.*s", algGrpLen, fillStr00);
				return(len + algGrpLen);
			}
			else
			{
				sprintf(bcdStr + len, "%.*s", fillLen, fillStr00);
				len += fillLen;
			}
			break;
		// 强制填充8000
		case 4:
			if(fillLen == algGrpLen)
			{
				sprintf(bcdStr + len, "%.*s", algGrpLen, fillStr80);
				return(len + algGrpLen);
			}
			else
			{
				sprintf(bcdStr + len, "%.*s", fillLen, fillStr80);
				len += fillLen;
			}
			break;
	}

	return(len);
}

// 2014-6-10 张永定增加
// 判断密码是否过期
// 输入：passwdUpdateTime - 密码最近修改日期
// 返回：>=0 - 还剩下的天数；<0 - 已过期
int UnionIsOverduePasswd(char *passwdUpdateTime,char *remark)
{
	char	passwdUpdateDate[32];
	int     maxEffictiveDays;	// 密码的最大有效期天数
	int     usedDays;		// 密码的使用天数

	strncpy(passwdUpdateDate,passwdUpdateTime,8);
	passwdUpdateDate[8] = 0;

	if ((maxEffictiveDays = UnionReadIntTypeRECVar("effictiveDaysOfPwd")) <=0 )
		maxEffictiveDays = 36500;  // 默认36500天

	if (strlen(passwdUpdateDate) == 0)
	{
		if (remark != NULL)
			strcpy(remark, "使用的是初始密码，请修改密码");
		return(0);
	}

	// 取密码已使用的天数
	usedDays = UnionDecideDaysBeforeToday(passwdUpdateDate);
	if (usedDays < 0)
	{
		UnionUserErrLog("in UnionIsOverduePasswd:: UnionDecideDaysBeforeToday [%s] \n", passwdUpdateDate);
		return(usedDays);
	}

	if ((maxEffictiveDays-usedDays) <= 7 && (maxEffictiveDays-usedDays) >= 0)
	{
		if (remark != NULL)
			sprintf(remark, "%s%d%s", "密码还差", (maxEffictiveDays-usedDays), "天过期，请修改密码");
	}
	return(maxEffictiveDays-usedDays);
}

unsigned long	gunionCryptTable[0x500]; 
int		gunionIsInitCryptTable = 0;
 
// 以下的函数生成一个长度为0x500（合10进制数：1280）的cryptTable[0x500]  
void UnionHashTablePrepareCryptTable()
{
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;  
  
	for( index1 = 0; index1 < 0x100; index1++ )  
	{   
		for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 )  
		{   
			unsigned long temp1, temp2;  
  
			seed = (seed * 125 + 3) % 0x2AAAAB;  
			temp1 = (seed & 0xFFFF) << 0x10;  
  
			seed = (seed * 125 + 3) % 0x2AAAAB;  
			temp2 = (seed & 0xFFFF);  
  
			gunionCryptTable[index2] = ( temp1 | temp2 );   
		}   
	}
	gunionIsInitCryptTable = 1;
	return;
}

//以下函数计算hashData 字符串的hash值，其中dwHashType 为hash的类型，  
//在下面GetHashTablePos函数里面调用本函数，其可以取的值为0、1、2；该函数  
//返回hashData 字符串的hash值；  
unsigned long UnionHashTableHashString( char *hashData, unsigned long dwHashType )
{
	unsigned char *key  = (unsigned char *)hashData;  
	unsigned long seed1 = 0x7FED7FED;  
	unsigned long seed2 = 0xEEEEEEEE;  
	int ch;  
	
	if (!gunionIsInitCryptTable)
		UnionHashTablePrepareCryptTable();
	
	while( *key != 0 )  
	{   
		//modify by zhangyd 20151111
		//ch = toupper(*key++);  
		ch = *key++;  
		//modify end 20151111

		seed1 = gunionCryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);  
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;   
	}  
	return seed1;   
}

// 检查报文格式，并返回报文长度
int UnionIsCheckKMSPackage(char flag,char *package,int lenOfPackage,char *operator,char *appID,char *resID,char *serviceCode)
{
	int	ret = 0;
	char	*ptr = NULL;
	int	offset = 0;
	int	i = 0;
	char	keyName[128];
	char	tmpBuf[8], tmpBuf1[8];;

	if ((package != NULL) && (package[0] != flag))
	{
		//UnionLog("in UnionIsCheckKMSPackage:: this is a 2.x or 3.x request package[%s]!\n",package);
		return(errCodeEsscMDL_NotEsscRequestPackage);
	}

	//add by linxj 20151102	增加判断为3.x报文
	if (lenOfPackage >= 8)
	{
		memcpy(tmpBuf, package+2, 3);	//三位服务码
		tmpBuf[3] = 0;
		memcpy(tmpBuf1, package+6, 3);	//三位域个数
		tmpBuf1[3] = 0;
		if ((UnionIsDigitString(tmpBuf) == 1) && (package[5] == '1') && (UnionIsDigitString(tmpBuf1) == 1))
		{
			//UnionLog("in UnionIsCheckKMSPackage:: this is a 2.x or 3.x request package[%s]!\n",package);
			return(errCodeEsscMDL_NotEsscRequestPackage);
		}
	}
	//add end 20151102
	

	// 后面补空格
	if (package[1] != ' ')
	{
		if ((ptr = strchr(package,' ')) != NULL)
		{
			offset = ptr - package;
			// 报文头最大不超过128
			for (i = offset; i < 128-6; i++) 
			{
				if (package[i] != ' ')  
					break;
			}
			if (i == 128-6)
			{
				//UnionLog("in UnionIsCheckKMSPackage:: this is a 2.x or 3.x request package[%s]!\n",package);
				return(errCodeEsscMDL_NotEsscRequestPackage);
			}
			offset = i;
		}
		else    // 如果没有空格代表用户名刚好长度为8
			offset = 9;
	}
	else
	{
		if (((ptr = strstr(package,"88888888")) == NULL))
		{
			//UnionLog("in UnionIsCheckKMSPackage:: this is a 2.x or 3.x request package[%s]!\n",package);
			return(errCodeEsscMDL_NotEsscRequestPackage);
		}
		else
			offset = ptr - package + 8;
	}

	// 操作者
	memcpy(operator,package+1,offset-1);
	operator[offset-1] = 0;
	
	// 资源号
	memcpy(resID,package+offset,3);
	resID[3] = 0;
	offset += 3;

	// 旧服务码
	memcpy(serviceCode,package+offset,3);
	serviceCode[3] = 0;
	offset += 3;

	if (flag == '1')
	{
		memcpy(tmpBuf,package+offset,4);
		tmpBuf[4] = 0;

		if ((ret = UnionReadRecFldFromRecStr(package+offset+4,lenOfPackage-offset-4,"fullName",keyName,sizeof(keyName))) < 0)
			sprintf(appID,"%s",resID);
		else
		{
			if ((ptr = strchr(keyName,'.')) == NULL)	
			{
				UnionUserErrLog("in UnionIsCheckKMSPackage:: UnionReadRecFldFromRecStr keyName[%s]!\n",keyName);
				return(errCodeKeyCacheMDL_WrongKeyName);	
			}
			ret = ptr - keyName;
			memcpy(appID,keyName,ret);
			appID[ret] = 0;
		}
	}
	else if (flag == '0')
	{
		memcpy(appID,package+offset,6);
		appID[6] = 0;
		offset += 6;
	}

	return(offset);
}
 
// 是否显示敏感信息
int UnionIsDisplaySensitiveInfo()
{
	// 缺省为不打印敏感信息
	if (UnionReadIntTypeRECVar("isDisplaySensitiveInfo") > 0)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

/*
功能：	
	比较字符串，含通配符
输入：	
	s,不含有通配符
	t，含有通配符
返回:
	0表示不匹配，
	1表示匹配
*/
// 20150626 zhangyd add
int UnionStrComp(char *s, char *t)
{
        if (*t == '\0')
        {
                if (*s == '\0')
                        return 1;
                else
                        return 0;
        }
        if (*s == '\0')
        {
                if (*t == '*')
                        return UnionStrComp(s, t + 1);
                else
                        return 0;
        }
        if (*s == *t)
                return UnionStrComp(s + 1, t + 1);
        if (*t == '*')
                return UnionStrComp(s, t + 1) || UnionStrComp(s + 1, t);
        else
                return 0;
}

/*
	20160129 add by leipp
	功能:	判断字符串是否为控制字符或空格
	输入参数:
		str	输入字符串
		len	字符串长度

	返回参数:
		0:	否
		1:	是
*/
int UnionIsCntrlOrSpace(char *str,int len)
{
	int     i = 0;

	for (i = 0; i < len; i++)
	{
		if (iscntrl(str[i]) || isspace(str[i]))
		{
			return(1);
		}
	}
	return(0);
}

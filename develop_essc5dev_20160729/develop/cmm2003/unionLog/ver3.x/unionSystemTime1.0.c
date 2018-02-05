//	Author:		Wolfgang Wang
//	Date:		2001/08/29

// 2003/09/19,Wolfgang Wang
// 在将UnionLog2.0升级为UnionLog3.0时，从UnionLog3.0中将原系统日期和时间操作函数从该程序分离形成本程序

#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "unionErrCode.h"

#include "UnionStr.h"

char	gunionFullSystemDateTime[32] = "";
char	gunionFullSystemDate[32] = "";
char	gunionSystemTime[32] = "";

#ifdef _WIN32
#	include <time.h>
#	include <process.h>
#else
#	include <sys/time.h>
#endif //_WIN32

#include "UnionLog.h"

char *UnionGetCurrentFullSystemDateTime()
{
	UnionGetFullSystemDateTime(gunionFullSystemDateTime);
	gunionFullSystemDateTime[14] = 0;
	return(gunionFullSystemDateTime);
}

char *UnionGetCurrentFullSystemDate()
{
	UnionGetFullSystemDate(gunionFullSystemDate);
	gunionFullSystemDate[8] = 0;
	return(gunionFullSystemDate);
}

char *UnionGetCurrentSystemTime()
{
	UnionGetSystemTime(gunionSystemTime);
	gunionSystemTime[6] = 0;
	return(gunionSystemTime);
}

int UnionGetSystemTime(char *strtime)
{
	time_t tmptime;
	struct tm *tmptm;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	
	sprintf(strtime,"%02d%02d%02d",tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);
}

int UnionGetSystemDate(char *strdate)
{
	time_t tmptime;
	struct tm *tmptm;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	
	sprintf(strdate,"%02d%02d",tmptm->tm_mon+1,tmptm->tm_mday);
	
	return(0);
	
}

int UnionGetSystemYear(char *stryear)
{
	time_t tmptime;
	struct tm *tmptm;
	int  tmpyear;

	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;
		
	sprintf(stryear,"%04d",tmpyear);
	
	return(0);
}

int UnionGetFullSystemDate(char *strdate)
{
	time_t tmptime;
	struct tm *tmptm;
	int  tmpyear;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;
		
	sprintf(strdate,"%04d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday);
	
	return(0);
}

int UnionGetSystemDateTime(char *datetime)
{
	time_t tmptime;
	struct tm *tmptm;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	
	sprintf(datetime,"%02d%02d%02d%02d%02d",tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);
	
}

int UnionGetFullSystemDateTime(char *datetime)
{
	time_t tmptime;
	struct tm *tmptm;
	int  tmpyear;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;
	
	sprintf(datetime,"%04d%02d%02d%02d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);

}

long UnionDecideDaysBeforeToday(char *date)
{
	int	month1,day1,year1,month2,day2,year2,year;
	long	days1,days2,days;
	char	today[32],str[32];
	int	ret;
	
	if (!UnionIsValidFullDateStr(date))
		return(errCodeParameter);
	UnionGetFullSystemDate(today);
	today[8] = 0;
	if ((ret = strncmp(today,date,8)) == 0)
		return(0);
	if (ret < 0)
		return(errCodeParameter);
	
	memcpy(str,date+4,2);
	str[2] = 0;
	month1 = atoi(str);
	memcpy(str,date+6,2);
	str[2] = 0;
	day1 = atoi(str);
	memcpy(str,date,4);
	str[4] = 0;
	year1 = atoi(str);
	switch (month1)
	{
		case	1:
			days1 = day1;
			break;
		case	2:
			days1 = 31 + day1;
			break;
		case	3:
			days1 = 31 + day1;
			break;
		case	4:
			days1 = 31 * 2 + day1;
			break;
		case	5:
			days1 = 31 * 2 + 30 + day1;
			break;
		case	6:
			days1 = 31 * 3 + 30 + day1;
			break;
		case	7:
			days1 = 31 * 3 + 30 * 2 + day1;
			break;
		case	8:
			days1 = 31 * 4 + 30 * 2 + day1;
			break;
		case	9:
			days1 = 31 * 5 + 30 * 2 + day1;
			break;
		case	10:
			days1 = 31 * 5 + 30 * 3 + day1;
			break;
		case	11:
			days1 = 31 * 6 + 30 * 3 + day1;
			break;
		case	12:
			days1 = 31 * 6 + 30 * 4 + day1;
			break;
		default:
			return(errCodeParameter);
	}
	if (month1 > 2)
	{
		if ((year1 % 4 == 0) && (year1 % 100 != 0))
			days1 += 29;
		else
			days1 += 28;
	}
		

	memcpy(str,today+4,2);
	str[2] = 0;
	month2 = atoi(str);
	memcpy(str,today+6,2);
	str[2] = 0;
	day2 = atoi(str);
	memcpy(str,today,4);
	str[4] = 0;
	year2 = atoi(str);
	switch (month2)
	{
		case	1:
			days2 = day2;
			break;
		case	2:
			days2 = 31 + day2;
			break;
		case	3:
			days2 = 31 + day2;
			break;
		case	4:
			days2 = 31 * 2 + day2;
			break;
		case	5:
			days2 = 31 * 2 + 30 + day2;
			break;
		case	6:
			days2 = 31 * 3 + 30 + day2;
			break;
		case	7:
			days2 = 31 * 3 + 30 * 2 + day2;
			break;
		case	8:
			days2 = 31 * 4 + 30 * 2 + day2;
			break;
		case	9:
			days2 = 31 * 5 + 30 * 2 + day2;
			break;
		case	10:
			days2 = 31 * 5 + 30 * 3 + day2;
			break;
		case	11:
			days2 = 31 * 6 + 30 * 3 + day2;
			break;
		case	12:
			days2 = 31 * 6 + 30 * 4 + day2;
			break;
		default:
			return(errCodeParameter);
	}
	if (month2 > 2)
	{
		if ((year2 % 4 == 0) && (year2 % 100 != 0))
			days2 += 29;
		else
			days2 += 28;
	}
	//printf("date = [%s] today = [%s]\n",date,today);
	//printf("days1 = [%ld] days2 = [%ld]\n",days1,days2);
	days = days2 - days1;
	for (year = year1; year < year2; year++)
	{
		if ((year % 4 == 0) && (year % 100 != 0))
			days += 366;
		else
			days += 365;
	}
	return(days);
}

long UnionDecideDaysAfterToday(char *date)
{
	int	month1,day1,year1,month2,day2,year2,year;
	long	days1,days2,days;
	char	today[32],str[32];
	int	ret;
	
	if (!UnionIsValidFullDateStr(date))
		return(errCodeParameter);
	UnionGetFullSystemDate(today);
	today[8] = 0;
	if ((ret = strncmp(today,date,8)) == 0)
		return(0);
	if (ret > 0)
		return(errCodeParameter);
	
	memcpy(str,date+4,2);
	str[2] = 0;
	month1 = atoi(str);
	memcpy(str,date+6,2);
	str[2] = 0;
	day1 = atoi(str);
	memcpy(str,date,4);
	str[4] = 0;
	year1 = atoi(str);
	switch (month1)
	{
		case	1:
			days1 = day1;
			break;
		case	2:
			days1 = 31 + day1;
			break;
		case	3:
			days1 = 31 + day1;
			break;
		case	4:
			days1 = 31 * 2 + day1;
			break;
		case	5:
			days1 = 31 * 2 + 30 + day1;
			break;
		case	6:
			days1 = 31 * 3 + 30 + day1;
			break;
		case	7:
			days1 = 31 * 3 + 30 * 2 + day1;
			break;
		case	8:
			days1 = 31 * 4 + 30 * 2 + day1;
			break;
		case	9:
			days1 = 31 * 5 + 30 * 2 + day1;
			break;
		case	10:
			days1 = 31 * 5 + 30 * 3 + day1;
			break;
		case	11:
			days1 = 31 * 6 + 30 * 3 + day1;
			break;
		case	12:
			days1 = 31 * 6 + 30 * 4 + day1;
			break;
		default:
			return(errCodeParameter);
	}
	if (month1 > 2)
	{
		if ((year1 % 4 == 0) && (year1 % 100 != 0))
			days1 += 29;
		else
			days1 += 28;
	}
		

	memcpy(str,today+4,2);
	str[2] = 0;
	month2 = atoi(str);
	memcpy(str,today+6,2);
	str[2] = 0;
	day2 = atoi(str);
	memcpy(str,today,4);
	str[4] = 0;
	year2 = atoi(str);
	switch (month2)
	{
		case	1:
			days2 = day2;
			break;
		case	2:
			days2 = 31 + day2;
			break;
		case	3:
			days2 = 31 + day2;
			break;
		case	4:
			days2 = 31 * 2 + day2;
			break;
		case	5:
			days2 = 31 * 2 + 30 + day2;
			break;
		case	6:
			days2 = 31 * 3 + 30 + day2;
			break;
		case	7:
			days2 = 31 * 3 + 30 * 2 + day2;
			break;
		case	8:
			days2 = 31 * 4 + 30 * 2 + day2;
			break;
		case	9:
			days2 = 31 * 5 + 30 * 2 + day2;
			break;
		case	10:
			days2 = 31 * 5 + 30 * 3 + day2;
			break;
		case	11:
			days2 = 31 * 6 + 30 * 3 + day2;
			break;
		case	12:
			days2 = 31 * 6 + 30 * 4 + day2;
			break;
		default:
			return(errCodeParameter);
	}
	if (month2 > 2)
	{
		if ((year2 % 4 == 0) && (year2 % 100 != 0))
			days2 += 29;
		else
			days2 += 28;
	}
	//printf("date = [%s] today = [%s]\n",date,today);
	//printf("days1 = [%ld] days2 = [%ld]\n",days1,days2);
	days = days1 - days2;
	for (year = year2; year < year1; year++)
	{
		if ((year % 4 == 0) && (year % 100 != 0))
			days += 366;
		else
			days += 365;
	}
	return(days);
}

//计算当前日期前N天的日期
int UnionGetDateBeforSpecDays(int days, char *date)
{
        time_t  time1;
        time_t  time2;
        struct tm *tmptm;
        int     tmpyear;

        time(&time1);
        time2=time1-days*24*3600;
        tmptm = (struct tm *)localtime(&time2);
        if(tmptm->tm_year > 90)
                tmpyear = 1900 + tmptm->tm_year%1900;
        else
                tmpyear = 2000 + tmptm->tm_year;

        sprintf(date,"%04d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday);

	return 0;
}

int UnionGetDateAfterSpecifiedMonths(int months,char *date)
{
	char	tmpBuf[64];
	int	year,month,day;
	
	if (months <= 0)
		return(errCodeParameter);
	
	UnionGetFullSystemDate(tmpBuf);
	tmpBuf[8] = 0;
	day = atoi(tmpBuf+6);
	tmpBuf[6] = 0;
	month = atoi(tmpBuf+4);
	tmpBuf[4] = 0;
	year = atoi(tmpBuf);
	
	month += months;
	year = year + (month-1) / 12;
	month = month % 12;
	if (month == 0)
		month = 12;
	if (day == 31)
	{
		switch (month)
		{
			case	2:
				if ((year % 4 == 0) && (year % 100 != 0))
					day = 29;
				else
					day = 28;
				break;
			case	4:
			case	6:
			case	9:
			case	11:
				day = 30;
				break;
			default:
				break;
		}
	}
	else if (day == 30)
	{
		switch (month)
		{
			case	2:
				if ((year % 4 == 0) && (year % 100 != 0))
					day = 29;
				else
					day = 28;
				break;
			default:
				break;
		}
	}
	sprintf(tmpBuf,"%04d%02d%02d",year,month,day);
	memcpy(date,tmpBuf,8);
	return(8);
}

// 计算指定日期过后几日是哪一天。
// specDate:YYYYMMDD，输入参数
// snapDays是过后的天数，输入参数
// afterDate::YYYYMMDD,输出参数
int UnionDecideDateAfterSpecDate(char *specDate,int snapDays,char *afterDate)
{
	int	month,day,year;
	char	tmpBuf[32];
	int	leftDays;
	int	maxDaysOfMonth;
	
	if ((!UnionIsValidFullDateStr(specDate)) || (snapDays < 0))
		return(errCodeParameter);
	
	strcpy(tmpBuf,specDate);
	day = atoi(tmpBuf+6);
	tmpBuf[6] = 0;
	month = atoi(tmpBuf+4);
	tmpBuf[4] = 0;
	year = atoi(tmpBuf);

	leftDays = snapDays;
loop:
	if (leftDays <= 0)
	{
		sprintf(afterDate,"%04d%02d%02d",year,month,day);
		return(0);
	}
	switch (month)
	{
		case	1:
		case	3:
		case	5:
		case	7:
		case	8:
		case	10:
		case	12:
			maxDaysOfMonth = 31;
			break;
		case	4:
		case	6:
		case	9:
		case	11:
			maxDaysOfMonth = 30;
			break;
		case	2:
			if ((year % 4 == 0) && (year % 100 != 0))
				maxDaysOfMonth = 29;
			else
				maxDaysOfMonth = 28;
			break;
		default:
			//UnionUserErrLog("in UnionDecideDateAfterSpecDate:: wrong parameter [%s]!\n",specDate);
			return(errCodeParameter);
	}
	if (leftDays + day > maxDaysOfMonth)
	{
		++month;
		// modify by leipp 20150706
		//day = 1;
		leftDays = leftDays - (maxDaysOfMonth - day) - 1;
		day = 1;
		// modify end
	}
	else
	{
		day = day + leftDays;
		leftDays = 0;
	}
	if (month == 13)
	{
		year++;
		month = 1;
	}
	goto loop;
}

// 计算一个指定时间过后多少秒后的时间
// specTime:输入参数：YYYYMMDDHHMMSS
// snap，输入参数，以秒计
// afterTime，输出参数：YYYYMMDDHHMMSS
int UnionGetTimeAfterSpecTime(char *specTime,long maxExistSeconds,char *afterTime)
{
	int	second,minute,hour,days=0;
	int	ret;
	char	tmpBuf[32];
		
	if ((specTime == NULL) || (afterTime == NULL) || (maxExistSeconds < 0) || (strlen(specTime) != 14))
	{
		//UnionUserErrLog("in UnionGetTimeAfterSpecTime:: wrong parameters!\n");
		return(errCodeParameter);
	}
	strcpy(tmpBuf,specTime);
	second = atoi(tmpBuf+12);
	tmpBuf[12] = 0;
	minute = atoi(tmpBuf+10);
	tmpBuf[10] = 0;
	hour = atoi(tmpBuf+8);
	tmpBuf[8] = 0;

	if (!UnionIsValidFullDateStr(tmpBuf))
	{
		//UnionUserErrLog("in UnionGetTimeAfterSpecTime:: wrong parameters [%s]!\n",specTime);
		return(errCodeParameter);
	}
	if ((second += maxExistSeconds) >= 60)
	{
		minute += (second / 60);
		second = second % 60;
	}
	if (minute >= 60)
	{
		hour += (minute / 60);
		minute = minute % 60;
	}
	if (hour >= 24)
	{
		days = (hour / 24);
		hour = hour % 24;
	}
	//memcpy(tmpBuf,specTime);
	//tmpBuf[8] = 0;
	if ((ret = UnionDecideDateAfterSpecDate(tmpBuf,days,afterTime)) < 0)
	{
		//UnionUserErrLog("in UnionGetTimeAfterSpecTime:: UnionDecideDateAfterSpecDate!\n");
		return(ret);
	}
	sprintf(afterTime+8,"%02d%02d%02d",hour,minute,second);

	return(0);
}

// 判断一个时间串是否合法
/*
输入参数：
	timeStr		时间采用HHMMSS
输入参数
	无
返回值
	1		合法
	0		不合法
*/
int UnionIsValidTimeStr(char *timeStr)
{
	int	hour,minute,second;
	int	ret;
	
	if ((ret = UnionAnalysisTime(timeStr,&hour,&minute,&second)) < 0)
		return(0);
	else
		return(1);
}

// 将一个时间串拼分成小时，分、秒
/*
输入参数：
	timeStr		时间采用HHMMSS
输入参数
	hour		小时
	minute		分
	second		秒
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAnalysisTime(char *timeStr,int *hour,int *minute,int *second)
{
	char	tmpChar;
	
	if ((timeStr == NULL) || (strlen(timeStr) != 6) || (!UnionIsDigitStr(timeStr)))
		return(errCodeParameter);
	tmpChar = timeStr[2];
	timeStr[2] = 0;
	if (((*hour = atoi(timeStr)) < 0) || (*hour >= 24))
	{
		timeStr[2] = tmpChar;
		return(errCodeParameter);
	}
	timeStr[2] = tmpChar;
	tmpChar = timeStr[4];
	timeStr[4] = 0;
	if (((*minute = atoi(timeStr+2)) < 0) || (*minute >= 60))
	{
		timeStr[4] = tmpChar;
		return(errCodeParameter);
	}
	timeStr[4] = tmpChar;
	if (((*second = atoi(timeStr+4)) < 0) || (*second >= 60))
	{
		return(errCodeParameter);
	}
	return(0);
}
	
// 计算两个时间的时间间隔
/*
输入参数：
	firstTime	第一个时间
	secondTime	第二个时间
	时间采用HHMMSS
输入参数
	seconds		间隔，以秒计	firstTime - secondTime
返回值
	>=0		成功
	<0		出错代码
*/

// afterTime，输出参数：YYYYMMDDHHMMSS
int UnionCalcuSecondsBetweenTwoTime(char *firstTime,char *secondTime,long *seconds)
{
	int	ret;
	int	hour1,hour2,minute1,minute2,second1,second2;
	
	//UnionProgramerLog("in UnionCalcuSecondsBetweenTwoTime:: [%s] [%s]\n",firstTime,secondTime);
	if ((ret = UnionAnalysisTime(firstTime,&hour1,&minute1,&second1)) < 0)
	{
		UnionUserErrLog("in UnionCalcuSecondsBetweenTwoTime:: UnionAnalysisTime [%s]\n",firstTime);
		return(ret);
	}
	if ((ret = UnionAnalysisTime(secondTime,&hour2,&minute2,&second2)) < 0)
	{
		UnionUserErrLog("in UnionCalcuSecondsBetweenTwoTime:: UnionAnalysisTime [%s]\n",secondTime);
		return(ret);
	}

	*seconds = (hour2-hour1)*60*60+(minute2-minute1)*60+second2-second1;
	return(0);
}

// 计算两个时间的时间间隔
/*
输入参数：
	firstTime	第一个时间
	secondTime	第二个时间
	时间采用YYMMDDHHMMSS
输出参数
	seconds		间隔，以秒计	firstTime - secondTime
返回值
	>=0		成功
	<0		出错代码
*/
int UnionCalcuSecondsBetweenTwoFullSystemDateTime(char *firstTime,char *secondTime,long *seconds)
{
	int	ret;
	long	days1,days2;
	char	tmpChar1,tmpChar2;
	
	if ((firstTime == NULL) || (secondTime == NULL))
		return(errCodeParameter);
		
	tmpChar1 = firstTime[8];
	tmpChar2 = secondTime[8];
	firstTime[8] = 0;
	secondTime[8] = 0;
	if ((days1 = UnionDecideDaysBeforeToday(firstTime)) < 0)
	{
		if (days1 == errCodeParameter)
		{
			UnionUserErrLog("in UnionCalcuSecondsBetweenTwoFullSystemDateTime:: UnionDecideDaysBeforeToday [%s]!\n",firstTime);
			ret = days1;
			goto errExit;
		}
	}
	if ((days2 = UnionDecideDaysBeforeToday(secondTime)) < 0)
	{
		if (days2 == errCodeParameter)
		{
			UnionUserErrLog("in UnionCalcuSecondsBetweenTwoFullSystemDateTime:: UnionDecideDaysBeforeToday [%s]!\n",secondTime);
			ret = days2;
			goto errExit;
		}
	}
	firstTime[8] = tmpChar1;
	secondTime[8] = tmpChar2;
	//UnionProgramerLog("in UnionCalcuSecondsBetweenTwoTime:: [%s] [%s]\n",firstTime,secondTime);
	if ((ret = UnionCalcuSecondsBetweenTwoTime(firstTime+8,secondTime+8,seconds)) < 0)
	{
		UnionUserErrLog("in UnionCalcuSecondsBetweenTwoFullSystemDateTime:: UnionCalcuSecondsBetweenTwoTime!\n");
		goto errExit;
	}
	*seconds = *seconds + (days1 - days2) * 3600 * 24;
	return(0);
errExit:
	firstTime[8] = tmpChar1;
	secondTime[8] = tmpChar2;
	return(ret);
}

// 计算一个时间距离当前时间的间隔
/*
输入参数：
	firstTime	时间
输出参数:
	seconds		间隔，假设当前时间为currentTime,计算的间隔为currentTime-fisrtTime
返回值
	>=0		成功
	<0		出错代码
*/
int UnionCalcuSecondsPassedAfterSpecTime(char *firstTime,long *seconds)
{
	char	currentTime[32];
	int	ret;
	
	UnionGetFullSystemDateTime(currentTime);
	currentTime[14] = 0;
	//UnionDebugLog("in UnionCalcuSecondsPassedAfterSpecTime::[%s] [%s]\n", firstTime, currentTime);

	if ((ret = UnionCalcuSecondsBetweenTwoFullSystemDateTime(firstTime, currentTime, seconds)) < 0)
	{
		UnionUserErrLog("in UnionCalcuSecondsPassedAfterSpecTime:: UnionCalcuSecondsBetweenTwoFullSystemDateTime! [%s] [%s]\n",firstTime,currentTime);
		return(ret);
	}
	return(ret);
}

// Add By HuangBaoxin, 20090916
// 计算一个指定时间前多少秒的时间
// specTime:输入参数：YYYYMMDDHHMMSS
// seconds,输入参数,以秒计
// beforeTime,输出参数：YYYYMMDDHHMMSS
int UnionGetTimeBeforeSpecTime(char *specTime, long seconds,char *beforeTime)
{
	char	tmp[32], tmpDate[32], tmpTime[32];
	time_t	timer1, timer2;
	struct tm	stm, *ptm;
	int	tmpyear;


	strncpy(tmpDate, specTime, 8);
	strncpy(tmpTime, specTime+8, 6);
	
	if (!UnionIsValidTimeStr(tmpTime))
	{
		UnionUserErrLog("in UnionGetTimeBeforeSpecTime::UnionIsValidTimeStr [%s] is envalid\n", tmpTime);
		return(errCodeParameter);
	}

	if (!UnionIsValidFullDateStr(tmpDate))
	{
		UnionUserErrLog("in UnionGetTimeBeforeSpecTime::UnionIsValidFullDateStr [%s] is envalid\n", tmpDate);
		return(errCodeParameter);
	}
		
	strncpy(tmp, specTime, 4); tmp[4] = 0;
	stm.tm_year = atoi(tmp) - 1900;
	strncpy(tmp, specTime+4, 2); tmp[2] = 0;
	stm.tm_mon = atoi(tmp) - 1;
	strncpy(tmp, specTime+6, 2); tmp[2] = 0;
	stm.tm_mday = atoi(tmp);
	
	strncpy(tmp, specTime+8, 2); tmp[2] = 0;
	stm.tm_hour = atoi(tmp);
	
	strncpy(tmp, specTime+10, 2); tmp[2] = 0;
	stm.tm_min = atoi(tmp);
	
	strncpy(tmp, specTime+12, 2); tmp[2] = 0;
	stm.tm_sec = atoi(tmp);
	
	timer1 = mktime(&stm);
	
	timer2 = timer1 - seconds;

	ptm = (struct tm *)localtime(&timer2);
	if(ptm->tm_year > 90)
		tmpyear = 1900 + ptm->tm_year%1900;
	else
		tmpyear = 2000 + ptm->tm_year;
	
	sprintf(beforeTime, "%04d%02d%02d%02d%02d%02d", tmpyear, ptm->tm_mon+1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	
	return 0;
}


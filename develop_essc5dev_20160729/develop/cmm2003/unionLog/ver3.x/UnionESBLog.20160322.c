// Author:	lusj
// Date:	20160322
//功能:东莞农商ESB系统定制日志格式


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>



#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#define snprintf _snprintf 
#define vsnprintf _vsnprintf 
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if ( defined __linux__ )
#include <sys/syscall.h>
#endif

#include "UnionESBLog.h"
#include "UnionStr.h"
#include "UnionEnv.h"
#include "UnionLog.h"

#define LOG_FILE_NAME 		"esscClient.log"
#define TranscationLog_FILE_NAME 		"ESBTranscation.log"

#define c_filename 		"esscPackageConvert.Esb.20130301.c"
int 	c_fileline=111;
char	strLogPrintToFile[20480];
char	ESBTranceLog[204800];

int EndPointOfstrLog=0;
int EndPointOfESBTranceLog=0;



struct timeval	tmptimeval_start;
struct timeval	tmptimeval_end;


unsigned long get_file_size(const char *LogFileName)
{
	unsigned long   filesize =-1;
	struct stat     statbuff;

	DIR 			*dir;
	struct	dirent *ptr1;
	//char		*ptr2 = NULL;

	char	LogFileNameOfPath[256];
	char		*path = NULL;

	if((path = getenv("UNIONLOG")) == NULL)
	{
		UnionUserErrLog("in get_file_size::  get UNIONLOG failed!\n");
		return -1;
	}

	snprintf(LogFileNameOfPath,sizeof(LogFileNameOfPath),"%s/%s",path,LogFileName);

	if(stat(LogFileNameOfPath,&statbuff)<0)
	{
		return filesize;

	}
	else
	{
		filesize=statbuff.st_size;
	}

	return filesize;
}


int	BackupLogFile(const char *LogFileName)
{
	int				i;
	int 			MaxNumOfLogFile=-1;
	int				totalNumOfLogFile=0;
	DIR             *dir;
	struct  dirent *ptr1;
	char		*ptr2 = NULL;
	char		tmpBuf[1024];

	char		*path = NULL;

	if((path = getenv("UNIONLOG")) == NULL)
	{
		UnionUserErrLog("in get_file_size::  get UNIONLOG failed!\n");
		return -1;
	}

	if( (MaxNumOfLogFile=UnionReadIntTypeRECVar("MaxNumOfLogFile")) < 0 )
		MaxNumOfLogFile = 10;//默认10


	dir = opendir(path);
	while((ptr1=readdir(dir))!=NULL)
	{
		if((ptr2=strstr(ptr1->d_name,LogFileName))!=NULL)
		{
			totalNumOfLogFile++;
		}
	}	

	if(totalNumOfLogFile<MaxNumOfLogFile)
	{
		snprintf(tmpBuf,sizeof(tmpBuf),"mv %s/%s %s/%s-%04d",path,LogFileName,path,LogFileName,totalNumOfLogFile);
		system(tmpBuf);
	}
	else
	{
		for(i=1;i<MaxNumOfLogFile;i++)
		{
			snprintf(tmpBuf,sizeof(tmpBuf),"mv %s/%s-%04d %s/%s-%04d",path,LogFileName,i+1,path,LogFileName,i);
			system(tmpBuf);
		}
		snprintf(tmpBuf,sizeof(tmpBuf),"mv %s/%s %s/%s-%04d",path,LogFileName,path,LogFileName,i);
		system(tmpBuf);
	}


	return 1;
}




void UnionPrintLogToLogFile(char *tital)
{
	int		ret;
	char		*ptr = NULL;
	FILE		*fp = NULL;
	char		logFile[256];
	unsigned long	filesize =-1;

	int IntMAX_SIZE0fLOG_FILE_NAME;


	if( (IntMAX_SIZE0fLOG_FILE_NAME=UnionReadIntTypeRECVar("IntMAX_SIZE0fLOG_FILE_NAME")) < 0 )
		IntMAX_SIZE0fLOG_FILE_NAME = 30;//默认30M

	IntMAX_SIZE0fLOG_FILE_NAME=IntMAX_SIZE0fLOG_FILE_NAME*1024*1024;

	if((ptr = getenv("UNIONLOG")) != NULL)
	{

		//add by lusj 20160627 日志备份写
		if((filesize=get_file_size(LOG_FILE_NAME))<0)
			return;

		if(IntMAX_SIZE0fLOG_FILE_NAME<=filesize)
			BackupLogFile(LOG_FILE_NAME);
		//add end by lusj


		ret = sprintf(logFile, "%s/%s", ptr, LOG_FILE_NAME);	

		logFile[ret] = 0;

		if ((fp = fopen(logFile, "a")) == NULL)
		{
			fp = stderr;
			fprintf(fp, "in UnionAPIPrintLogToLogFile fopen[%s] ret = NULL!\n", logFile);
		}
		else
		{
			if(ftell(fp) >= IntMAX_SIZE0fLOG_FILE_NAME)
			{
				fclose(fp);
				if ((fp = fopen(logFile, "w")) == NULL)
				{
					fp = stderr;
				}
			}
		}
	}
	else
	{
		return;
	}

	if(tital != NULL)
	{
		fprintf(fp, "%s", tital);
	}

	if(fp != stderr)
	{
		fflush(fp);
		fclose(fp);	
	}

	return;
}



void UnionPrintTranscationLogToLogFile(char *tital)
{
	int		ret;
	char		*ptr = NULL;
	FILE		*fp = NULL;
	char		logFile[256];
	unsigned long	filesize =-1;
	int IntMAX_SIZE0fTranscationLog;


	if( (IntMAX_SIZE0fTranscationLog=UnionReadIntTypeRECVar("IntMAX_SIZE0fTranscationLog")) < 0 )
		IntMAX_SIZE0fTranscationLog = 50;//默认50M

	IntMAX_SIZE0fTranscationLog=IntMAX_SIZE0fTranscationLog*1024*1024;


	if((ptr = getenv("UNIONLOG")) != NULL)
	{

		//add by lusj 20160627 日志备份写
		if((filesize=get_file_size(TranscationLog_FILE_NAME))<0)
			return;

		if(IntMAX_SIZE0fTranscationLog<=filesize)
			BackupLogFile(TranscationLog_FILE_NAME);
		//add end by lusj

		ret = sprintf(logFile, "%s/%s", ptr, TranscationLog_FILE_NAME);	

		logFile[ret] = 0;

		if ((fp = fopen(logFile, "a")) == NULL)
		{
			fp = stderr;
			fprintf(fp, "in UnionAPIPrintLogToLogFile fopen[%s] ret = NULL!\n", logFile);
		}
		else
		{
			if(ftell(fp) >= IntMAX_SIZE0fTranscationLog)
			{
				fclose(fp);
				if ((fp = fopen(logFile, "w")) == NULL)
				{
					fp = stderr;
				}
			}
		}
	}
	else
	{
		return;
	}

	if(tital != NULL)
	{
		fprintf(fp, "%s", tital);
	}

	if(fp != stderr)
	{
		fflush(fp);
		fclose(fp);	
	}

	return;
}


void UnionRecESBTranscationLog(unsigned char *TranceLog,int lenOfTranceLog)
{
	int		len;
	double		used_time;
	char		buf[128+1];
	char		log[20480];
	char		tital[20480];
	char		time_buf[128];
	struct tm	*tmptm, tbuf;
	struct timeval	tmptimeval;

	if (UnionReadIntTypeRECVar("isPrintESBTranscationLog") <= 0)
		return;


	gettimeofday(&tmptimeval,NULL);
	tmptm = (struct tm *)localtime_r(&tmptimeval.tv_sec, &tbuf);

	memset(buf,0,sizeof(buf));
	len = strftime(buf,sizeof(buf), "%Y%m%d %T",tmptm);


	sprintf(buf+len," %03ld",tmptimeval.tv_usec);
	memcpy(time_buf,buf,21);
	time_buf[21]=0;
	used_time=11;

	memcpy(log,(char *)TranceLog,lenOfTranceLog);

	len= sprintf(tital, "\n******************* New of Transcation *******************\n\n");		
	len+= sprintf(tital+len, "[DT=%s][LFN=%s][LL=%s][PN=%-8u]*****ESB请求报文*****:\n %s\n",time_buf,"ESBTrance","INFO",(long)getpid(),log);
	tital[len] = 0;
	EndPointOfESBTranceLog+=sprintf(ESBTranceLog,"%s\n",tital);
	return;
}


void UnionSendESBTranscationLog(unsigned char *TranceLog,int lenOfTranceLog)
{
	int		len;
	double		used_time;
	char		buf[128+1];
	char		log[20480];
	char		tital[20480];
	char		time_buf[128];
	struct tm	*tmptm, tbuf;
	struct timeval	tmptimeval;

	if (UnionReadIntTypeRECVar("isPrintESBTranscationLog") <= 0)
		return;

	gettimeofday(&tmptimeval,NULL);
	tmptm = (struct tm *)localtime_r(&tmptimeval.tv_sec, &tbuf);

	memset(buf,0,sizeof(buf));
	len = strftime(buf,sizeof(buf), "%Y%m%d %T",tmptm);


	sprintf(buf+len," %03ld",tmptimeval.tv_usec);
	memcpy(time_buf,buf,21);
	time_buf[21]=0;
	used_time=11;

	memcpy(log,(char *)TranceLog,lenOfTranceLog);
	len= sprintf(tital, "[DT=%s][LFN=%s][LL=%s][PN=%-8u][lenOflog=%08d]*****ESB接受报文*****:\n%s\n",time_buf,"ESBTrance","INFO",(long)getpid(),lenOfTranceLog,log);
	len+= sprintf(tital+len, "\n\n******************* End of Transcation *******************\n");
	tital[len] = 0;

	EndPointOfESBTranceLog+=sprintf(ESBTranceLog+EndPointOfESBTranceLog,"%s",tital);
	ESBTranceLog[EndPointOfESBTranceLog]=0;

	UnionPrintTranscationLogToLogFile(ESBTranceLog);

	EndPointOfESBTranceLog=0;
	ESBTranceLog[EndPointOfESBTranceLog]=0;

	return;
}



void UnionESBStartLog(char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO)
{
	int		len;
	char		buf[128+1];
	char		tital[1024];
	char		start_time[128];
	struct tm	*tmptm, tbuf;


	gettimeofday(&tmptimeval_start,NULL);
	tmptm = (struct tm *)localtime_r(&tmptimeval_start.tv_sec, &tbuf);

	memset(buf,0,sizeof(buf));
	len = strftime(buf,sizeof(buf), "%Y%m%d %T",tmptm);


	sprintf(buf+len," %03ld",tmptimeval_start.tv_usec);
	memcpy(start_time,buf,21);
	start_time[21]=0;

	len = sprintf(tital, "[DT=%s][LFN=%s][LL=%s][PN=%-8u][CN=%s][CL=%s][LT=SYS][CONSUMER_ID=%s][SERVICE_CODE=%s][CONSUMER_SEQ_NO=%s]系统处理开始,SYS_N={密码服务平台},SYS_S={%s}\n",start_time,"esscClient","INFO",(long)getpid(),c_filename,"471",CONSUMER_ID,SERVICE_CODE,CONSUMER_SEQ_NO,start_time);


	tital[len] = 0;

	EndPointOfstrLog+=sprintf(strLogPrintToFile,"%s",tital);

	return;

}

void UnionESBEndLog(int result_flag,char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO)
{
	int		len;
	long		used_time;//微妙
	char	end_time[128];
	char		buf[128+1];
	char		tital[1024];
	long		second_start;//秒
	long		second_end;	//秒
	long		susecond_start;	//微秒
	long		susecond_end;		//微秒

	struct tm	*tmptm, tbuf;

	gettimeofday(&tmptimeval_end,NULL);
	tmptm = (struct tm *)localtime_r(&tmptimeval_end.tv_sec, &tbuf);
	memset(buf,0,sizeof(buf));
	len = strftime(buf,sizeof(buf), "%Y%m%d %T",tmptm);

	sprintf(buf+len," %03ld",tmptimeval_end.tv_usec);
	memcpy(end_time,buf,21);
	end_time[21]=0;

	second_start=tmptimeval_start.tv_sec;
	second_end=tmptimeval_end.tv_sec;

	susecond_start=tmptimeval_start.tv_usec;
	susecond_end=tmptimeval_end.tv_usec;


	if (susecond_end > susecond_start)
	{
		used_time=(second_end-second_start)*1000+(susecond_end/1000-susecond_start/1000);
	}
	else if (susecond_end == susecond_start)
	{
		used_time=(second_end-second_start)*1000;
	}
	else
	{
		used_time=(second_end-1-second_start)*1000+(susecond_end/1000+1*1000-susecond_start/1000);
	}
	len = sprintf(tital, "[DT=%s][LFN=%s][LL=%s][PN=%-8u][CN=%s][CL=%s][LT=SYS][CONSUMER_ID=%s][SERVICE_CODE=%s][CONSUMER_SEQ_NO=%s]系统处理结束,SYS_N={密码服务平台},SYS_E={%s},耗时SYS_USE_T={%ld}毫秒,SYS_RESULT_C={%d}\n",end_time,"esscClient","INFO",(long)getpid(),c_filename,"816",CONSUMER_ID,SERVICE_CODE,CONSUMER_SEQ_NO,end_time,used_time,result_flag);

	tital[len] = 0;

	EndPointOfstrLog+=sprintf(strLogPrintToFile+EndPointOfstrLog,"%s",tital);
	strLogPrintToFile[EndPointOfstrLog]=0;

	UnionPrintLogToLogFile(strLogPrintToFile);

	EndPointOfstrLog=0;
	strLogPrintToFile[EndPointOfstrLog]=0;
	return;
}




void UnionESBErrLog(char *err_code,char *err_MSG,char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO)
{
	int		len;
	double		used_time;
	char		buf[128+1];
	char		tital[1024];
	char		time_buf[128];
	struct tm	*tmptm, tbuf;
	struct timeval	tmptimeval;

	gettimeofday(&tmptimeval,NULL);
	tmptm = (struct tm *)localtime_r(&tmptimeval.tv_sec, &tbuf);

	memset(buf,0,sizeof(buf));
	len = strftime(buf,sizeof(buf), "%Y%m%d %T",tmptm);


	sprintf(buf+len," %03ld",tmptimeval.tv_usec);
	memcpy(time_buf,buf,21);
	time_buf[21]=0;
	used_time=11;

	len = sprintf(tital, "[DT=%s][LFN=%s][LL=%s][PN=%-8u][CN=%s][CL=%s][LT=SYS][CONSUMER_ID=%s][SERVICE_CODE=%s][CONSUMER_SEQ_NO=%s]密码服务平台系统处理异常,SYS_N={密码服务平台},SYS_ERROR_S={%s},SYS_ERROR_C={%s},SYS_ERROR_M={%s}\n",time_buf,"esscClient","INFO",(long)getpid(),c_filename,"734",CONSUMER_ID,SERVICE_CODE,CONSUMER_SEQ_NO,time_buf,err_code,err_MSG);


	tital[len] = 0;
	EndPointOfstrLog+=sprintf(strLogPrintToFile+EndPointOfstrLog,"%s",tital);

	return;
}



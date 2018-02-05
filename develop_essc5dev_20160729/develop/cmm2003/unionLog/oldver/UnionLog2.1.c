//	Author:		Wolfgang Wang
//	Date:		2001/08/29

// 2003/07/16,王纯军，将1.2升级为2.0
// 2.0与1.x的区别在于，当日志超过规定的大小时，会将原日志备份后再重写日志

// 2.1与2.0版本的区别在于：调用UnionUserErrLog、UnionSystemErrLog和
// UnionMemErrLog写日志时，会同时将该日志写在另一个单独指定的错误日志文件中。
// 该错误日志文件的名称为unionError.log，其存放路径在运行用户的.profile文件中指定。
// 定义错误日志文件存放路径的宏名称为UNIONERRLOGDIR。

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#ifdef _WIN32
#	include <time.h>
#	include <process.h>
#else
#ifdef _UNIX
#	include <sys/time.h>
/*
struct tm
{
        int     tm_sec;
        int     tm_min;
        int     tm_hour;
        int     tm_mday;
        int     tm_mon;
        int     tm_year;
        int     tm_wday;
        int     tm_yday;
        int     tm_isdst;
#define LTZNMAX 50
        long tm_tzadj;
        char tm_name[LTZNMAX];      
};
*/
#endif //_UNIX
#endif //_WIN32

#include "UnionLog.old.h"

extern int UnionIsDebug();
extern long UnionGetSizeOfLogFile();
extern int UnionBackupLogFileBeforeRewriteIt();

char	gunionFullSystemDateTime[14+1] = "";
char	gunionFullSystemDate[8+1] = "";
char	gunionSystemTime[6+1] = "";

char *UnionGetCurrentFullSystemDateTime()
{
	memset(gunionFullSystemDateTime,0,sizeof(gunionFullSystemDateTime));
	UnionGetFullSystemDateTime(gunionFullSystemDateTime);
	return(gunionFullSystemDateTime);
}

char *UnionGetCurrentFullSystemDate()
{
	memset(gunionFullSystemDate,0,sizeof(gunionFullSystemDate));
	UnionGetFullSystemDateTime(gunionFullSystemDate);
	return(gunionFullSystemDate);
}

char *UnionGetCurrentSystemTime()
{
	memset(gunionSystemTime,0,sizeof(gunionSystemTime));
	UnionGetSystemTime(gunionSystemTime);
	return(gunionSystemTime);
}

// 2006/08/11 增加
int UnionSetUserDefinedErrorCode(int errCode)
{
	return(errCode);
}

int UnionGetNameOfLogFile(char *nameOfLogFile);

int UnionConnectLogMDL()
{
	return(0);
}

int UnionDisconnectLogMDL()
{
	return(0);
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

FILE *UnionOpenLogFile()
{
	char filename[512];
	char cmdBuf[512];
	char time[20];	
	
	FILE *UnionLogfile;
	
	memset(filename,0,512);
	if (UnionGetNameOfLogFile(filename) < 0 )
		return(stderr);
	//printf("filename = [%s]\n",filename);
	
	if ((UnionLogfile = fopen(filename,"a")) == NULL)
		return(stderr);
		
	if (ftell(UnionLogfile) >= UnionGetSizeOfLogFile())
	{
		UnionCloseLogFile(UnionLogfile);
		if (UnionBackupLogFileBeforeRewriteIt())
		{
			memset(time,0,sizeof(time));
			UnionGetFullSystemDateTime(time);
			memset(cmdBuf,0,sizeof(cmdBuf));
			sprintf(cmdBuf,"mv %s %s.%s",filename,filename,time);
			system(cmdBuf);
		}
		if ((UnionLogfile = fopen(filename,"w")) == NULL)
			return(stderr);
	}

	return(UnionLogfile);
}

// Mary add begin, 2004-2-3
FILE *UnionOpenErrLogFile()
{
	char filename[512];
	char cmdBuf[512];
	char time[20];	
	FILE *UnionErrLogFile;
	
	memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/unionError.log",getenv("UNIONERRLOGDIR"));

	if ((UnionErrLogFile = fopen(filename,"a")) == NULL)
		return(stderr);
		
	if (ftell(UnionErrLogFile) >= UnionGetSizeOfLogFile())
	{
		UnionCloseLogFile(UnionErrLogFile);
		if (UnionBackupLogFileBeforeRewriteIt())
		{
			memset(time,0,sizeof(time));
			UnionGetFullSystemDateTime(time);
			memset(cmdBuf,0,sizeof(cmdBuf));
			sprintf(cmdBuf,"mv %s %s.%s",filename,filename,time);
			system(cmdBuf);
		}
		if ((UnionErrLogFile = fopen(filename,"w")) == NULL)
			return(stderr);
	}

	return(UnionErrLogFile);
}
// Mary add end, 2004-2-3

int UnionCloseLogFile(FILE *UnionLogfile)
{
	if ((UnionLogfile == stderr) || (UnionLogfile == stdout))
		return(0);
	return(fclose(UnionLogfile));
}

void UnionSystemErrLog(char *fmt,...)
{
	va_list args;
	char	buf[512];
	FILE	*UnionLogfile;
	FILE	*UnionErrLogFile;			// Mary add, 2004-2-3

	UnionLogfile = UnionOpenLogFile();
	UnionErrLogFile = UnionOpenErrLogFile();	// Mary add, 2004-2-3
	
	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);
	
	fprintf(UnionLogfile,"%10s System Defined Error:: ",buf);
	fprintf(UnionErrLogFile,"%10s System Defined Error:: ",buf);		// Mary add, 2004-2-3
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);
	fprintf(UnionErrLogFile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);	// Mary add, 2004-2-3
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);
	fprintf(UnionErrLogFile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);	// Mary add, 2004-2-3
#endif

	//memset(buf,0,sizeof(buf));
	//perror(buf);
	//fprintf(UnionLogfile," [%s]\n",buf);
	
	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	vfprintf(UnionErrLogFile,fmt,args);		// Mary add, 2004-2-3
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fprintf(UnionErrLogFile,"\n");			// Mary add, 2004-2-3
	fflush(UnionLogfile);
	fflush(UnionErrLogFile);			// Mary add, 2004-2-3
	
	UnionCloseLogFile(UnionLogfile);
	UnionCloseLogFile(UnionErrLogFile);		// Mary add, 2004-2-3
}

void UnionUserErrLog(char *fmt,...)
{
	va_list args;
	char	buf[40];
	FILE	*UnionLogfile;
	FILE	*UnionErrLogFile;			// Mary add, 2004-2-3

	UnionLogfile = UnionOpenLogFile();
	UnionErrLogFile = UnionOpenErrLogFile();	// Mary add, 2004-2-3

	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);

	fprintf(UnionLogfile,"%10s User Defined Error:: ",buf);
	fprintf(UnionErrLogFile,"%10s User Defined Error:: ",buf);	// Mary add, 2004-2-3
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
	fprintf(UnionErrLogFile,"PID = [%d]\n",getpid());		// Mary add, 2004-2-3
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
	fprintf(UnionErrLogFile,"PID = [%d]\n",getpid());		// Mary add, 2004-2-3
#endif

	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	vfprintf(UnionErrLogFile,fmt,args);		// Mary add, 2004-2-3
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fprintf(UnionErrLogFile,"\n");			// Mary add, 2004-2-3
	fflush(UnionLogfile);
	fflush(UnionErrLogFile);			// Mary add, 2004-2-3

	UnionCloseLogFile(UnionLogfile);
	UnionCloseLogFile(UnionErrLogFile);		// Mary add, 2004-2-3
}

void UnionSuccessLog(char *fmt,...)
{
	va_list args;
	char	buf[40];
	FILE	*UnionLogfile;

	UnionLogfile = UnionOpenLogFile();

	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);

	fprintf(UnionLogfile,"%10s Success:: ",buf);
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fflush(UnionLogfile);

	UnionCloseLogFile(UnionLogfile);
}

void UnionLog(char *fmt,...)
{
	va_list args;
	char	buf[40];
	FILE	*UnionLogfile;


	if (!UnionIsDebug())
		return;

	UnionLogfile = UnionOpenLogFile();

	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);

	fprintf(UnionLogfile,"%10s:: Debug::",buf);
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif


	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fflush(UnionLogfile);

	UnionCloseLogFile(UnionLogfile);
}


void UnionNoTimeLog(char *fmt,...)
{
	va_list args;
	FILE	*UnionLogfile;


	if (!UnionIsDebug())
		return;

	UnionLogfile = UnionOpenLogFile();

	fprintf(UnionLogfile,"Debug::");
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif


	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fflush(UnionLogfile);

	UnionCloseLogFile(UnionLogfile);
}

/* Mary add begin, 2001-8-28 */

/*
  Function:	Write memory of buffer to file as HEX and ASCII mode,
		it is not used directly when write log file
  Parameters:
		fp -- ID of file opened
		pBuffer -- buffer waiting to be written to file
		iBufLen -- length of buffer
  Return:	NULL
*/
void WriteMemory(FILE *fp,unsigned char *pBuffer,int iBufLen)
{
	int	i,j,iLine,iBytes,iLineBytes,iSpaceNum;
	char	caHint[]="Displacement -1--2--3--4--5--6--7--8-Hex-0--1--2--3--4--5--6  --ASCII Value--\n";

	if ((iBufLen % 16) != 0)
		iLine = iBufLen / 16 + 1;
	else
		iLine = iBufLen / 16;

	for (i=0;i<iLine;i++)
	{
		if (i % 20 == 0)
			fprintf(fp,caHint);

		// Write total bytes
		iBytes = i*16;
		fprintf(fp,"%05d(%05X) ",iBytes,iBytes);

		// Write 16 bytes as HEX mode
		if (i != (iLine-1))
			iLineBytes = 16;
		else
			iLineBytes = iBufLen-iBytes;

		for (j=0;j<iLineBytes;j++)
		{
			fprintf(fp,"%02X ",pBuffer[iBytes+j]);
		}

		// Write space
		if (i != (iLine-1))
			iSpaceNum = 1;
		else
			iSpaceNum = 1+(16-iLineBytes)*3;
		for (j=0;j<iSpaceNum;j++)
			fprintf(fp," ");

		// Write 16 bytes as ASCII mode
		for (j=0;j<iLineBytes;j++)
		{
			fprintf(fp,"%c",pBuffer[iBytes+j]);
		}
		fprintf(fp,"\n");
	}
}

/*
  Function:	Write memory of buffer to log file as HEX and ASCII mode,
		write date,time,process id and user message at the same time
  Parameters:
		pMessage -- user's message of the buffer describing
		pBuffer -- buffer waiting to be written to log file
		iBufLen -- length of buffer
  Return:	NULL
*/
void UnionMemErrLog(char *pMessage,unsigned char *pBuffer,int iBufLen)
{
	char	caBuf[40];
	FILE	*UnionLogfile;
	FILE	*UnionErrLogFile;			// Mary add, 2004-2-3

	if (pBuffer == NULL || iBufLen <= 0)
		return;

	UnionLogfile = UnionOpenLogFile();
	UnionErrLogFile = UnionOpenErrLogFile();	// Mary add, 2004-2-3
	
	memset(caBuf,0,40);
	UnionGetFullSystemDateTime(caBuf);

	fprintf(UnionLogfile,"DateAndTime = [%10s] ",caBuf);
	fprintf(UnionErrLogFile,"DateAndTime = [%10s] ",caBuf);		// Mary add, 2004-2-3
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
	fprintf(UnionErrLogFile,"PID = [%d]\n",getpid());		// Mary add, 2004-2-3
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
	fprintf(UnionErrLogFile,"PID = [%d]\n",getpid());		// Mary add, 2004-2-3
#endif

	fprintf(UnionLogfile,"%s\n",pMessage);
	fprintf(UnionErrLogFile,"%s\n",pMessage);			// Mary add, 2004-2-3

	// Write the memory to file
	if (iBufLen > 0)
	{
		WriteMemory(UnionLogfile,pBuffer,iBufLen);
		WriteMemory(UnionErrLogFile,pBuffer,iBufLen);		// Mary add, 2004-2-3
	}
	fprintf(UnionLogfile,"\n");
	fprintf(UnionErrLogFile,"\n");		// Mary add, 2004-2-3

	fflush(UnionLogfile);
	fflush(UnionErrLogFile);		// Mary add, 2004-2-3

	UnionCloseLogFile(UnionLogfile);
	UnionCloseLogFile(UnionErrLogFile);	// Mary add, 2004-2-3
}

/*
  Function:	Write memory of buffer to log file as HEX and ASCII mode,
		write date,time,process id and user message at the same time,
		if debug function is closed, the function doesn't do any actions
  Parameters:
		pMessage -- user's message of the buffer describing
		pBuffer -- buffer waiting to be written to log file
		iBufLen -- length of buffer
  Return:	NULL
*/
void UnionMemLog(char *pMessage,unsigned char *pBuffer,int iBufLen)
{
	char	caBuf[40];
	FILE	*UnionLogfile;

	if (pBuffer == NULL || iBufLen <= 0)
		return;

	if (!UnionIsDebug())
		return;

	UnionLogfile = UnionOpenLogFile();

	memset(caBuf,0,40);
	UnionGetFullSystemDateTime(caBuf);

	fprintf(UnionLogfile,"DateAndTime = [%10s] ",caBuf);
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d]\n",getpid());
#endif

	fprintf(UnionLogfile,"%s\n",pMessage);

	// Write the memory to file
	if (iBufLen > 0)
		WriteMemory(UnionLogfile,pBuffer,iBufLen);
	fprintf(UnionLogfile,"\n");

	fflush(UnionLogfile);	

	UnionCloseLogFile(UnionLogfile);
}
/* Mary add end, 2001-8-28 */

void UnionPrintf(char *fmt,...)
{
	va_list args;

#ifdef _UNIX
	printf("(%d)",getpid());
#endif

#ifdef _WIN32
	printf("(%d)",getpid());
#endif

	
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
}

void UnionNullLog(char *fmt,...)
{
	va_list args;
	FILE	*UnionLogfile;


	if (!UnionIsDebug())
		return;

	UnionLogfile = UnionOpenLogFile();

	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	va_end(args);
	
	fflush(UnionLogfile);
	
	UnionCloseLogFile(UnionLogfile);
}


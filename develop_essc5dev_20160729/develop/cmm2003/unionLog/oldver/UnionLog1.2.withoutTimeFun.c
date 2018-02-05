//	Author:		Wolfgang Wang
//	Date:		2001/08/29

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#ifdef _WIN32
#	include <time.h>
#	include <process.h>
#else
#ifdef _UNIX
#	include <sys/time.h>
#endif //_UNIX
#endif //_WIN32
/*
struct UnionTM
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

#include "UnionLog.old.h"

extern int UnionIsDebug();
extern long UnionGetSizeOfLogFile();
int UnionGetNameOfLogFile(char *nameOfLogFile);

// 2006/08/11 增加
int UnionSetUserDefinedErrorCode(int errCode)
{
	return(errCode);
}

int UnionConnectLogMDL()
{
	return(0);
}

int UnionDisconnectLogMDL()
{
	return(0);
}

/*
int UnionGetSystemTime(char *strtime)
{
	time_t tmptime;
	struct UnionTM *tmptm;
	
	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
	
	sprintf(strtime,"%02d%02d%02d",tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);
}

int UnionGetSystemDate(char *strdate)
{
	time_t tmptime;
	struct UnionTM *tmptm;
	
	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
	
	sprintf(strdate,"%02d%02d",tmptm->tm_mon+1,tmptm->tm_mday);
	
	return(0);
	
}

int UnionGetSystemYear(char *stryear)
{
	time_t tmptime;
	struct UnionTM *tmptm;
	int  tmpyear;

	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
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
	struct UnionTM *tmptm;
	int  tmpyear;
	
	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
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
	struct UnionTM *tmptm;
	
	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
	
	sprintf(datetime,"%02d%02d%02d%02d%02d",tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);
	
}

int UnionGetFullSystemDateTime(char *datetime)
{
	time_t tmptime;
	struct UnionTM *tmptm;
	int  tmpyear;
	
	time(&tmptime);
	tmptm = (struct UnionTM *)localtime(&tmptime);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;
	
	sprintf(datetime,"%04d%02d%02d%02d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);
	
	return(0);

}
*/

FILE *UnionOpenLogFile()
{
	char filename[512];
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
		if ((UnionLogfile = fopen(filename,"w")) == NULL)
			return(stderr);
	}

	return(UnionLogfile);
}

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

	UnionLogfile = UnionOpenLogFile();
	
	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);
	
	fprintf(UnionLogfile,"%10s System Defined Error:: ",buf);
#ifdef _UNIX
	fprintf(UnionLogfile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);
#endif

#ifdef _WIN32
	fprintf(UnionLogfile,"PID = [%d] ErrCode = [%d]\n",getpid(),errno);
#endif

	//memset(buf,0,sizeof(buf));
	//perror(buf);
	//fprintf(UnionLogfile," [%s]\n",buf);
	
	va_start(args,fmt);
	vfprintf(UnionLogfile,fmt,args);
	va_end(args);

	fprintf(UnionLogfile,"\n");
	fflush(UnionLogfile);
	
	UnionCloseLogFile(UnionLogfile);
}

void UnionUserErrLog(char *fmt,...)
{
	va_list args;
	char	buf[40];
	FILE	*UnionLogfile;

	UnionLogfile = UnionOpenLogFile();

	memset(buf,0,40);
	UnionGetFullSystemDateTime(buf);

	fprintf(UnionLogfile,"%10s User Defined Error:: ",buf);
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

void UnionAuditLog(char *fmt,...)
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

// 2007/12/14 增加
void UnionDebugLog(char *fmt,...)
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

	if (pBuffer == NULL || iBufLen <= 0)
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

void UnionAuditNullLog(char *fmt,...)
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

void UnionDebugNullLog(char *fmt,...)
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

// 2007/12/14 增加
void UnionProgramerLog(char *fmt,...)
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


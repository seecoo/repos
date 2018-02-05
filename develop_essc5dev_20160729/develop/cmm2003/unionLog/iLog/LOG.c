/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.11
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOG.h"

/* 日志等级描述对照表 */ /* log level describe */
static char		sg_aszLogLevelDesc[][5+1] = { "NOLOG" , "OSERR" , "ERROR" , "INFO" , "DEBUG" , "PGM" } ;
static int              g_log_error_file = 0;		//add by linxj 20151111

/* 版本标识 */ /* version */
_WINDLL_FUNC int	_LOG_VERSION_1_0_11 = 0 ;

/* 临界区 */ /* critical region */
#if ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
pthread_mutex_t		g_pthread_mutex = PTHREAD_MUTEX_INITIALIZER ;
#endif

#if ( defined _WIN32 )
__declspec( thread ) char	mdc_g[128+1] = {0};
#elif ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
//static __thread char		mdc_g[128+1] = {0};
static  __thread char		mdc_g[128] = {0};
#endif

void mdc_put(char *mdc)
{
	strcpy(mdc_g,mdc);
}

char *mdc_get()
{
	return(mdc_g);
}

/* 设置错误日志单独一个文件 */
void SetErrorLogAloneFile()
{
        g_log_error_file = 1;
        return;
}

void SetErrorLogNoAloneFile()
{
        g_log_error_file = 0;
        return;
}


static int CreateMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	char		lock_pathfilename[ MAXLEN_FILENAME ] ;
	strcpy( lock_pathfilename , "Global\\iLOG3_ROTATELOCK" );
	g->rotate_lock = CreateMutex( NULL , FALSE , lock_pathfilename ) ;
	if( g->rotate_lock == NULL )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	char		lock_pathfilename[ MAXLEN_FILENAME ] ;
	mode_t		m ;
	SNPRINTF( lock_pathfilename , sizeof(lock_pathfilename) , "/tmp/iLOG3.lock" );
	m=umask(0);
	g->rotate_lock = open( lock_pathfilename , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
	umask(m);
	if( g->rotate_lock == FD_NULL )
		return LOG_RETURN_ERROR_INTERNAL;
#endif
	return 0;
}

static int DestroyMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	if( g->rotate_lock )
	{
		CloseHandle( g->rotate_lock );
	}
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->rotate_lock != -1 )
	{
		close( g->rotate_lock );
	}
#endif
	return 0;
}

static int EnterMutexSection(int cmd,LOG *g )
{
#if ( defined _WIN32 )
	DWORD	dw ;
	dw = WaitForSingleObject( g->rotate_lock , INFINITE ) ;
	if( dw != WAIT_OBJECT_0 )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	int	nret ;
	memset( & (g->lock) , 0x00 , sizeof(g->lock) );
	g->lock.l_type = F_WRLCK ;
	g->lock.l_whence = SEEK_SET ;
	g->lock.l_start = 0 ;
	g->lock.l_len = 0 ;
	nret = fcntl( g->rotate_lock , cmd , & (g->lock) ) ;
	if( nret == -1 )
		return LOG_RETURN_ERROR_INTERNAL;
	
	pthread_mutex_lock( & g_pthread_mutex );
#endif
	return 0;
}

static int LeaveMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	BOOL	bret ;
	bret = ReleaseMutex( g->rotate_lock ) ;
	if( bret != TRUE )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	int	nret ;
	
	pthread_mutex_unlock( & g_pthread_mutex );
	
	memset( & (g->lock) , 0x00 , sizeof(g->lock) );
	g->lock.l_type = F_UNLCK ;
	g->lock.l_whence = SEEK_SET ;
	g->lock.l_start = 0 ;
	g->lock.l_len = 0 ;
	nret = fcntl( g->rotate_lock , F_SETLK , & (g->lock) ) ;
	if( nret == -1 )
		return LOG_RETURN_ERROR_INTERNAL;
#endif
	return 0;
}

/* 调整缓冲区大小 */ /* adjust buffer size */
static int SetBufferSize( LOG *g , LOGBUF *logbuf , long buf_size , long max_buf_size )
{
	int		nret = 0 ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( logbuf == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( max_buf_size != -1 )
	{
		logbuf->max_buf_size = max_buf_size ;
	}
	
	if( buf_size != -1 )
	{
		if( logbuf->bufbase == NULL )
		{
			nret = EnterMutexSection(F_SETLK, g ) ;
			if( nret )
				return nret;
			logbuf->bufbase = (char*)malloc( buf_size ) ;
			LeaveMutexSection( g );
			if( logbuf->bufbase == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			
			logbuf->buf_size = buf_size ;
			//memset( logbuf->bufbase , 0x00 , logbuf->buf_size );
		}
		else
		{
			char	*tmp = NULL ;
			
			nret = EnterMutexSection(F_SETLK, g ) ;
			if( nret )
				return nret;
			tmp = (char*)realloc( logbuf->bufbase , buf_size ) ;
			LeaveMutexSection( g );
			if( tmp == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			
			logbuf->buf_remain_len = logbuf->buf_remain_len + ( buf_size - logbuf->buf_size ) ;
			logbuf->bufptr = logbuf->bufptr - logbuf->bufbase + tmp ;
			logbuf->bufbase = tmp ;
			logbuf->buf_size = buf_size ;
		}
	}
	
	return 0;
}

/* 销毁日志句柄 */ /* destruction of log handle */
void DestroyLogHandle( LOG *g )
{
	if( g )
	{
		if( g->logbuf.bufbase )
		{
			free( g->logbuf.bufbase );
		}

		if( g->open_flag == 1 )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				if( g->pfuncCloseLogFinally )
				{
					g->pfuncCloseLogFinally( g , & (g->open_handle) );
					g->open_flag = 0 ;
				}
			}
		}
		
		DestroyMutexSection( g );
		
		memset( g , 0x00 , sizeof(LOG) );
		free(g);
	}
	
	return;
}

/* 创建日志句柄 */ /* create log handle */
LOG *CreateLogHandle()
{
	LOG		_g , *g = NULL ;
	int		nret ;
	
#if ( defined _WIN32 )
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#else
	return LOG_RETURN_ERROR_NOTSUPPORT;
#endif
	
	memset( & _g , 0x00 , sizeof(LOG) );
	nret = CreateMutexSection( & _g ) ;
	if( nret )
		return NULL;
	
	nret = EnterMutexSection(F_SETLKW, & _g ) ;
	if( nret )
		return NULL;
	g = (LOG *)malloc( sizeof(LOG) ) ;
	LeaveMutexSection( & _g );
	if( g == NULL )
		return NULL;
	
	memcpy( g , & _g , sizeof(LOG) );
	
	SetLogLevel( g , LOG_LEVEL_DEFAULT );
	SetLogStyles( g , LOG_STYLES_DEFAULT , LOG_NO_STYLEFUNC );
	SetLogOptions( g , LOG_OPTION_CHANGE_TEST );
	
	g->rotate_mode = LOG_ROTATEMODE_NONE ;
	g->rotate_file_no = 1 ;
	g->rotate_file_count = LOG_ROTATE_SIZE_FILE_COUNT_DEFAULT ;
	g->pressure_factor = LOG_ROTATE_SIZE_PRESSURE_FACTOR_DEFAULT ;
	g->fsync_period = LOG_FSYNC_PERIOD ;
	g->fsync_elapse = g->fsync_period ;
	g->skip_count = 1 ;
	
	g->logbuf.buf_size = 0 ;
	g->logbuf.bufbase = NULL ;
	g->logbuf.bufptr = NULL ;
	g->logbuf.buf_remain_len = 0 ;
	nret = SetLogBufferSize( g , LOG_BUFSIZE_DEFAULT , LOG_BUFSIZE_MAX ) ;
	if( nret )
	{
		DestroyLogHandle( g );
		return NULL;
	}
		
	g->multi_mode = LOG_WRITEMODE_ONE;
	g->write_flag = 0;

	g->error_flag = 0;
	
	g->timingtime = 0;
	g->pfuncTimingInit = NULL;

	return g;
}

/* 复制日志句柄 */ /* copy log handle */
LOG *CopyLogHandle(LOG *p_g)
{
	LOG		*g = NULL ;
	int		i ;
	
	g = CreateLogHandle();
	if( g == NULL )
		return(NULL);
	
	strcpy(g->log_basefilename,p_g->log_basefilename);
	strcpy(g->log_pathfilename,p_g->log_basefilename);
	
	g->pfuncOpenLogFirst = p_g->pfuncOpenLogFirst;
	g->pfuncOpenLog = p_g->pfuncOpenLog;
	g->pfuncWriteLog = p_g->pfuncWriteLog;
	g->pfuncChangeTest = p_g->pfuncChangeTest;
	g->pfuncCloseLog = p_g->pfuncCloseLog;
	g->pfuncCloseLogFinally = p_g->pfuncCloseLogFinally;
	
	g->pfuncFilterLog = p_g->pfuncFilterLog;
	
	g->log_styles = p_g->log_styles;
	g->pfuncLogStyle = p_g->pfuncLogStyle;
	g->style_func_count = p_g->style_func_count;
	for (i = 0; i < g->style_func_count; i++)
		g->pfuncLogStyles[i] = p_g->pfuncLogStyles[i];

	g->log_options = p_g->log_options;
	
	g->file_change_test_interval = p_g->file_change_test_interval;

	g->rotate_mode = p_g->rotate_mode;
	g->log_rotate_size = p_g->log_rotate_size;
	g->rotate_file_count = p_g->rotate_file_count;
	g->pressure_factor = p_g->pressure_factor;

	g->pfuncAfterRotateFile = p_g->pfuncAfterRotateFile;
	g->pfuncBeforeRotateFile = p_g->pfuncBeforeRotateFile;

	g->timingtime = p_g->timingtime;
	g->pfuncTimingInit = p_g->pfuncTimingInit;
	g->multi_mode = p_g->multi_mode;

	return g;
}

/* 打开、输出、关闭日志函数 */ /* open , write , close log functions */
#if ( defined _WIN32 )

#define LOG_WINOPENFILE_TRYCOUNT	1000	/* windows上多线程打开同一文件会有互斥现象 */

static int OpenLog_OpenFile( LOG *g , char *log_pathfilename , void **open_handle )
{
	long	l ;
	
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	for( l = 0 ; l < LOG_WINOPENFILE_TRYCOUNT ; l++ )
	{
		g->hFile = CreateFileA( log_pathfilename , GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE , NULL , OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL ) ;
		if( g->hFile != INVALID_HANDLE_VALUE )
			break;
	}
	if( l >= LOG_WINOPENFILE_TRYCOUNT )
		return LOG_RETURN_ERROR_OPENFILE;
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_WriteFile( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	BOOL	bret ;
	
	if( g->open_flag == 0 )
		return -1;
	
	SetFilePointer( g->hFile , 0 , NULL , FILE_END );
	bret = WriteFile( g->hFile , buf , len , writelen , NULL ) ;
	if( bret != TRUE )
		return LOG_RETURN_ERROR_WRITEFILE;

	return 0;
}

static int CloseLog_CloseHandle( LOG *g , void **open_handle )
{
	if( g->open_flag == 0 )
		return 0;
	
	CloseHandle( g->hFile );
	
	g->open_flag = 0 ;
	return 0;
}

static int OpenLog_RegisterEventSource( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	g->hFile = RegisterEventSource( NULL , g->log_pathfilename ) ;
	if( g->hFile == NULL )
		return LOG_RETURN_ERROR_OPENFILE;
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_ReportEvent( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	unsigned short	event_log_level ;
	char		*ptr = NULL ;
	
	if( g->open_flag == 0 )
		return -1;
	
	if( log_level == LOG_LEVEL_PGM )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_DEBUG )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_INFO )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_ERROR )
		event_log_level = EVENTLOG_ERROR_TYPE ;
	else if( log_level == LOG_LEVEL_OSERR )
		event_log_level = EVENTLOG_ERROR_TYPE ;
	else
		event_log_level = EVENTLOG_ERROR_TYPE ;
	ptr = buf ;
	ReportEvent( g->hFile , event_log_level , 0 , 0 , NULL , 1 , 0 , & ptr , NULL );
	
	return 0;
}

static long CloseLog_DeregisterEventSource( LOG *g , void **open_handle )
{
	if( g->open_flag == 0 )
		return 0;
	
	DeregisterEventSource( g->hFile );
	
	g->open_flag = 0 ;
	return 0;
}

#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )

static int OpenLog_open( LOG *g , char *log_pathfilename , void **open_handle )
{
	int	len;
	char	*ptr = NULL;
	char	pathfilename[MAXLEN_FILENAME ];
	char	tmpPathFileName[MAXLEN_FILENAME], pathFileName[MAXLEN_FILENAME];
	int	lenOfPathFileName = 0;
	
	if( g->log_pathfilename[0] == '\0' )
		return 0;

	if( g->open_flag == 1 )
		return 0;

	if ((ptr = strstr(g->log_basefilename,"(mdc)")) != NULL)
	{
		if ((mdc_get() != NULL) && (strlen(mdc_get()) > 0))
			len = SNPRINTF(pathfilename,sizeof(pathfilename) , "%.*s%s%s" , (int)(ptr - g->log_basefilename), g->log_basefilename,mdc_get(),ptr + 5);
		else
			len = SNPRINTF(pathfilename,sizeof(pathfilename) , "%.*s%s" , (int)(ptr - g->log_basefilename-1), g->log_basefilename,ptr + 5);
		pathfilename[len] = 0;
	}
	else
		strcpy(pathfilename,g->log_basefilename);
	
	if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_PER_DAY ))
	{
		if( g->cache1_tv.tv_sec == 0 )
			g->cache1_tv.tv_sec = time( NULL ) ;
		if( g->cache1_stime.tm_mday == 0 )
			LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )

		if (strcmp(g->log_basefilename + strlen(g->log_basefilename) - 4, ".log") == 0)
			len = SNPRINTF(g->log_pathfilename,sizeof(g->log_pathfilename) , "%.*s.%04d%02d%02d.log" , (int)strlen(pathfilename) - 4, pathfilename, g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday );
		else
			len = SNPRINTF(g->log_pathfilename,sizeof(g->log_pathfilename) , "%s.%04d%02d%02d" , pathfilename, g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday );
		
		g->log_pathfilename[len] = 0;
	}
	else
		strcpy(g->log_pathfilename,pathfilename);

	//add  by linxj 20151111 错误日志写到另一个文件中
	if (g->error_flag == 1)
	{
		lenOfPathFileName = sprintf(tmpPathFileName, "%s", g->log_pathfilename);
		tmpPathFileName[lenOfPathFileName] = 0;
		memcpy(pathFileName, tmpPathFileName, lenOfPathFileName-4);
		sprintf(pathFileName+lenOfPathFileName-4, ".error%s", tmpPathFileName+lenOfPathFileName-4);
		memcpy(g->log_pathfilename, pathFileName, lenOfPathFileName+6);
	}
	//add by linxj 20151111
	
	g->fd = open( g->log_pathfilename , O_CREAT|O_APPEND|O_WRONLY , S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
	if( g->fd == -1 )
		return LOG_RETURN_ERROR_OPENFILE;
	
	g->open_flag = 1 ;
	return 0;
}

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
int fsync(int fildes);
#endif

static int CloseLog_close( LOG *g , void **open_handle )
{
	if( g->open_flag == 0 )
		return 0;
	
	close( g->fd );
	
	g->open_flag = 0 ;

	return 0;
}

static int OpenLog_openlog( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	openlog( g->log_pathfilename , LOG_PID , LOG_USER );
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_syslog( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	int	syslog_log_level ;
	
	if( g->open_flag == 0 )
		return -1;
	
	if( log_level == LOG_LEVEL_PGM )
		syslog_log_level = LOG_DEBUG ;
	else if( log_level == LOG_LEVEL_DEBUG )
		syslog_log_level = LOG_DEBUG ;
	else if( log_level == LOG_LEVEL_INFO )
		syslog_log_level = LOG_INFO ;
	else if( log_level == LOG_LEVEL_ERROR )
		syslog_log_level = LOG_ERR ;
	else if( log_level == LOG_LEVEL_OSERR )
		syslog_log_level = LOG_EMERG ;
	else
		syslog_log_level = LOG_ALERT ;
	syslog( syslog_log_level , buf );
	(*writelen) = len ;
	
	return 0;
}

static int CloseLog_closelog( LOG *g , void **open_handle )
{
	if( g->open_flag == 0 )
		return 0;
	
	closelog();
	
	g->open_flag = 0 ;
	return 0;
}

#endif

static int WriteLog_write( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	if( g->open_flag == 0 )
		return -1;
	g->write_flag = 0;
	(*writelen) = WRITE( g->fd , buf , len ) ;
	if( (*writelen) == -1 )
		return LOG_RETURN_ERROR_WRITEFILE;

	return 0;
}

static int ChangeTest_interval( LOG *g , void **test_handle )
{
	int		nret ;
	
	g->file_change_test_no--;
	if( g->file_change_test_no < 1 )
	{
		struct _STAT	file_change_stat ;
		nret = _STAT( g->log_pathfilename , & file_change_stat ) ;
		if( ( nret == -1 || file_change_stat.st_size < g->file_change_stat.st_size ) && g->pfuncCloseLogFinally )
		{
			nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
			if( nret )
				return nret;
		}
		g->file_change_test_no = g->file_change_test_interval ;
	}
	
	return 0;
}

int ExpandPathFilename( char *pathfilename , long pathfilename_bufsize )
{
	long		pathfilename_len ;
	
	char		*p1 = NULL , *p2 = NULL ;
	char		env_key[ MAXLEN_FILENAME ] ;
	long		env_key_len ;
	char		*env_val = NULL ;
	long		env_val_len ;
	
	pathfilename_len = strlen(pathfilename) ;
	
	p1 = strchr( pathfilename , '$' );
	while( p1 )
	{
		/* 展开环境变量 */ /* expand environment variable */
		p2 = strchr( p1 + 1 , '$' ) ;
		if( p2 == NULL )
			return LOG_RETURN_ERROR_PARAMETER;
		
		//memset( env_key , 0x00 , sizeof(env_key) );
		env_key_len = p2 - p1 + 1 ;
		strncpy( env_key , p1 + 1 , env_key_len - 2 );
		env_key[env_key_len - 2] = 0;
		env_val = getenv( env_key ) ;
		if( env_val == NULL )
			return LOG_RETURN_ERROR_PARAMETER;
		
		env_val_len = strlen(env_val) ;
		if( pathfilename_len + ( env_val_len - env_key_len ) > pathfilename_bufsize-1 )
			return LOG_RETURN_ERROR_PARAMETER;
		
		memmove( p2+1 + ( env_val_len - env_key_len ) , p2+1 , strlen(p2+1) + 1 );
		memcpy( p1 , env_val , env_val_len );
		pathfilename_len += env_val_len - env_key_len ;
		
		p1 = strchr( p1 + ( env_val_len - env_key_len ) , '$' );
	}
	pathfilename[pathfilename_len] = 0;
	
	return 0;
}

/* 设置日志输出 */ /* set log output */
int SetLogOutput( LOG *g , int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	char		pathfilename[ MAXLEN_FILENAME ] ;
	
	int		nret = 0 ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( log_pathfilename == NULL || log_pathfilename[0] == '\0' )
	{
		//memset( pathfilename , 0x00 , sizeof(pathfilename) );
		pathfilename[0] = 0;
	}
	else
	{
		//memset( pathfilename , 0x00 , sizeof(pathfilename) );
		if (strlen(log_pathfilename) < sizeof(pathfilename))
			strcpy(pathfilename,log_pathfilename);
		else
		{
			strncpy( pathfilename , log_pathfilename , sizeof(pathfilename)-1 );
			pathfilename[sizeof(pathfilename)-1] = 0;
		}
		
		if( output != LOG_OUTPUT_CALLBACK )
		{
			nret = ExpandPathFilename( pathfilename , sizeof(pathfilename) ) ;
			if( nret )
				return nret;
		}
	}
	
	if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_SET_OUTPUT_BY_FILENAME ) )
	{
		if( strncmp( pathfilename , "#stdout#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_STDOUT ;
		}
		else if( strncmp( pathfilename , "#stderr#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_STDERR ;
		}
		else if( strncmp( pathfilename , "#syslog#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_SYSLOG ;
		}
		pathfilename[strlen(pathfilename+8)+1] = 0;
	}
	
	if( output == LOG_OUTPUT_FILE && TEST_ATTRIBUTE( g->log_options , LOG_OPTION_FILENAME_APPEND_DOT_LOG ) )
	{
		if( strlen(pathfilename) + 4 > sizeof(pathfilename) - 1 )
			return LOG_RETURN_ERROR_PARAMETER;
		
		strcat( pathfilename , ".log" );
	}
	
	// 日志文件打开并且文件名有变动
	if (( g->open_flag == 1 ) && (strcmp(g->log_basefilename,pathfilename) != 0))
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
			}
		}
	}
	
	if( TEST_NOT_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE )
		&& TEST_NOT_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST )
		&& TEST_NOT_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
	{
		g->log_options |= LOG_OPTION_OPEN_DEFAULT ;
	}
	
	if (strlen(pathfilename) < sizeof(g->log_pathfilename))
	{
		strcpy( g->log_pathfilename , pathfilename );
		strcpy( g->log_basefilename , pathfilename );
	}
	else
	{
		strncpy( g->log_pathfilename , pathfilename , sizeof(g->log_pathfilename)-1 );
		strncpy( g->log_basefilename , pathfilename , sizeof(g->log_basefilename)-1 );
		g->log_pathfilename[sizeof(g->log_pathfilename)-1] = 0;
		g->log_basefilename[sizeof(g->log_pathfilename)-1] = 0;
	}

	if( output != LOG_OUTPUT_NOSET )
	{
		g->output = output ;
		if( output == LOG_OUTPUT_STDOUT )
		{
			g->fd = STDOUT_HANDLE ;
			g->pfuncOpenLogFirst = NULL ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_write ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = NULL ;
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 1 ;
		}
		else if( output == LOG_OUTPUT_STDERR )
		{
			g->fd = STDERR_HANDLE ;
			g->pfuncOpenLogFirst = NULL ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_write ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = NULL ;
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 1 ;
		}
		else if( output == LOG_OUTPUT_SYSLOG )
		{
#if ( defined _WIN32 )
			g->pfuncOpenLogFirst = & OpenLog_RegisterEventSource ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_ReportEvent ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = & CloseLog_DeregisterEventSource ;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
			g->pfuncOpenLogFirst = & OpenLog_openlog ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_syslog ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = & CloseLog_closelog ;
#endif
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 0 ;
		}
		else if( output == LOG_OUTPUT_FILE )
		{
#if ( defined _WIN32 )
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				g->pfuncOpenLogFirst = & OpenLog_OpenFile ;
				g->pfuncOpenLog = NULL ;
				g->pfuncWriteLog = & WriteLog_WriteFile ;
				g->pfuncChangeTest = & ChangeTest_interval ;
				g->pfuncCloseLog = NULL ;
				g->pfuncCloseLogFinally = & CloseLog_CloseHandle ;
			}
			else
			{
				g->pfuncOpenLogFirst = NULL ;
				g->pfuncOpenLog = & OpenLog_OpenFile ;
				g->pfuncWriteLog = & WriteLog_WriteFile ;
				g->pfuncChangeTest = NULL ;
				g->pfuncCloseLog = & CloseLog_CloseHandle ;
				g->pfuncCloseLogFinally = NULL ;
			}
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				g->pfuncOpenLogFirst = & OpenLog_open ;
				g->pfuncOpenLog = NULL ;
				g->pfuncWriteLog = & WriteLog_write ;
				g->pfuncChangeTest = & ChangeTest_interval ;
				g->pfuncCloseLog = NULL ;
				g->pfuncCloseLogFinally = & CloseLog_close ;
			}
			else
			{
				g->pfuncOpenLogFirst = NULL ;
				g->pfuncOpenLog = & OpenLog_open ;
				g->pfuncWriteLog = & WriteLog_write ;
				g->pfuncChangeTest = NULL ;
				g->pfuncCloseLog = & CloseLog_close ;
				g->pfuncCloseLogFinally = NULL ;
			}
#endif
			g->open_flag = 0 ;
		}
		else if( output == LOG_OUTPUT_CALLBACK )
		{
			SetLogOutputFuncDirectly( g , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
			g->open_flag = 0 ;
		}
	}
	
	/*
	if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
	{
		if( g->pfuncOpenLogFirst )
		{
			nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
			if( nret )
				return nret;
		}
	}
	*/
	
	return 0;
}

int SetLogOutput2( LOG *g , int output , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally , char *log_pathfilename_format , ... )
{
	int		len;
	va_list		valist ;
	char		log_pathfilename[ MAXLEN_FILENAME ] ;
	
	//memset( log_pathfilename , 0x00 , sizeof(log_pathfilename) );
	va_start( valist , log_pathfilename_format );
	len = VSNPRINTF( log_pathfilename , sizeof(log_pathfilename)-1 , log_pathfilename_format , valist );
	log_pathfilename[len] = 0;
	va_end( valist );
	
	return SetLogOutput( g , output , log_pathfilename , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
}

/* 设置日志等级 */ /* set log level */
int SetLogLevel( LOG *g , int log_level )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->log_level = log_level ;
	return 0;
}

/* 行格式函数集合 */ /* log style functions */
/*
static int LogStyle_SEPARATOR( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , " | " , 3 );
	return 0;
}

static int LogStyle_SEPARATOR2( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , "| " , 2 );
	return 0;
}
*/
static int LogStyle_BSEPARATOR( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , "[" , 1 );
	return 0;
}

static int LogStyle_ASEPARATOR( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , "]" , 1 );
	return 0;
}

static int LogStyle_DATE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_tv.tv_sec != g->cache2_logstyle_tv.tv_sec )
	{
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
		FormatLogBuffer( g , logbuf , "%04d-%02d-%02d" , g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday );
		g->cache2_logstyle_tv.tv_sec = g->cache1_tv.tv_sec ;
		g->cache2_logstyle_tv.tv_usec = g->cache1_tv.tv_usec ;
		g->cache2_logstyle_date_buf_len = 10 ;
		memcpy( g->cache2_logstyle_date_buf , logbuf->bufptr - g->cache2_logstyle_date_buf_len , g->cache2_logstyle_date_buf_len );
		g->cache2_logstyle_date_buf[g->cache2_logstyle_date_buf_len] = '\0' ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_date_buf , g->cache2_logstyle_date_buf_len );
	}
	return 0;
}

static int LogStyle_DATETIME( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_tv.tv_sec != g->cache2_logstyle_tv.tv_sec )
	{
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
		FormatLogBuffer( g , logbuf , "%04d-%02d-%02d %02d:%02d:%02d" , g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday , g->cache1_stime.tm_hour , g->cache1_stime.tm_min , g->cache1_stime.tm_sec ) ;
		g->cache2_logstyle_tv.tv_sec = g->cache1_tv.tv_sec ;
		g->cache2_logstyle_datetime_buf_len = 19 ;
		memcpy( g->cache2_logstyle_datetime_buf , logbuf->bufptr - g->cache2_logstyle_datetime_buf_len , g->cache2_logstyle_datetime_buf_len ); g->cache2_logstyle_datetime_buf[g->cache2_logstyle_datetime_buf_len] = '\0' ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_datetime_buf , g->cache2_logstyle_datetime_buf_len ) ;
	}
	return 0;
}

static int LogStyle_DATETIMEMS( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
#if ( defined _WIN32 )
	SYSTEMTIME	stNow ;
	GetLocalTime( & stNow );
	SYSTEMTIME2TIMEVAL_USEC( stNow , g->cache1_tv );
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	gettimeofday( & (g->cache1_tv) , NULL );
#endif	
	LogStyle_DATETIME( g , logbuf , c_filename , c_fileline , log_level , format , valist );
	FormatLogBuffer( g , logbuf , " %06ld" , g->cache1_tv.tv_usec ) ;
	return 0;
}

static int LogStyle_LOGLEVEL( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%-5s" , sg_aszLogLevelDesc[log_level] );
	return 0;
}

static int LogStyle_CUSTLABEL1( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[1-1] );
	return 0;
}

static int LogStyle_CUSTLABEL2( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[2-1] );
	return 0;
}

static int LogStyle_CUSTLABEL3( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[3-1] );
	return 0;
}

static int LogStyle_SPACE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , " " , 1 );
	return 0;
}

static int LogStyle_PID( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	unsigned long	pid ;
#if ( defined _WIN32 )
	HWND		hd ;
	DWORD		dw ;
	hd = GetForegroundWindow() ;
	GetWindowThreadProcessId( hd , & dw );
	pid = (long)dw ;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	pid = (long)getpid() ;
#endif
	if( pid != g->cache2_logstyle_pid )
	{
		g->cache2_logstyle_pid_buf_len = FormatLogBuffer( g , logbuf , "%-8u" , pid ) ;
		
		memcpy( g->cache2_logstyle_pid_buf , logbuf->bufptr - g->cache2_logstyle_pid_buf_len , g->cache2_logstyle_pid_buf_len ); g->cache2_logstyle_pid_buf[g->cache2_logstyle_pid_buf_len] = '\0' ;
		
		g->cache2_logstyle_pid = pid ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_pid_buf , g->cache2_logstyle_pid_buf_len ) ;
	}
	return 0;
}

static int LogStyle_TID( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	unsigned long	tid = 0;
#if ( defined _WIN32 )
	tid = (unsigned long)GetCurrentThreadId() ;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#if ( defined _PTHREAD_H )
	tid = (unsigned long)pthread_self() ;
#else
#endif
#endif
	if( tid != g->cache2_logstyle_tid )
	{
		g->cache2_logstyle_tid_buf_len = FormatLogBuffer( g , logbuf , "%-16u" , tid ) ;
		
		memcpy( g->cache2_logstyle_tid_buf , logbuf->bufptr - g->cache2_logstyle_tid_buf_len , g->cache2_logstyle_tid_buf_len ); g->cache2_logstyle_tid_buf[g->cache2_logstyle_tid_buf_len] = '\0' ;
		
		g->cache2_logstyle_tid = tid ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_tid_buf , g->cache2_logstyle_tid_buf_len ) ;
	}
	return 0;
}

static int LogStyle_SOURCE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
#if ( defined _WIN32 )
	char		*pfilename = NULL ;
	pfilename = strrchr( c_filename , '\\' ) ;
	if( pfilename )
	{
		pfilename++;
	}
	else
	{
		pfilename = strrchr( c_filename , '/' ) ;
		if( pfilename )
			pfilename++;
		else
			pfilename = c_filename ;
	}
	FormatLogBuffer( g , logbuf , "%s:%ld" , pfilename , c_fileline ) ;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	FormatLogBuffer( g , logbuf , "%s:%ld" , c_filename , c_fileline ) ;
#endif
	return 0;
}

static int LogStyle_FORMAT( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBufferV( g , logbuf , format , valist );
	return 0;
}

static int LogStyle_NEWLINE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , LOG_NEWLINE , LOG_NEWLINE_LEN );
	return 0;
}

/* 填充行日志段 */ /* fill log segments */
static int LogStyle_FuncArray( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	int		format_func_index ;
	funcLogStyle	**ppfuncLogStyle = NULL ;
	int		nret ;
	/* 遍历格式函数数组 */ /* travel all log style functions */
	for( format_func_index = 0 , ppfuncLogStyle = g->pfuncLogStyles ; format_func_index < g->style_func_count ; format_func_index++ , ppfuncLogStyle++ )
	{
		nret = (*ppfuncLogStyle)( g , logbuf , c_filename , c_fileline , log_level , format , valist ) ;
		if( nret )
			return nret;
	}
	
	return 0;
}

/* 设置行日志风格 */ /* set log styles */
int SetLogStyles( LOG *g , long log_styles , funcLogStyle *pfuncLogStyle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if (g->log_styles == log_styles)
		return 0;
	else
		g->log_styles = log_styles;
	
	if( g->log_styles == LOG_STYLE_CALLBACK )
	{
		SetLogStyleFuncDirectly( g , pfuncLogStyle );
		return 0;
	}
	
	
	/* 构造行风格函数数组 */
	g->style_func_count = 0 ;
	
	// PID
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_PID ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_PID ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
	}
	
	// TID
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_TID ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_TID ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
	}
	
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATETIMEMS ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATETIMEMS ;
	}
	else if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATETIME ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATETIME ;
	}
	else if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATE ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATE ;
	}

	// LEVEL
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_LOGLEVEL ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_LOGLEVEL ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
	}

	// SOURCE
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_SOURCE ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SOURCE ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
	}

	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL1 )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL2 )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL3 ) )
	{
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL1 ) )
		{
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL1 ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL2 ) )
		{
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL2 ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL3 ) )
		{
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_BSEPARATOR ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL3 ;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_ASEPARATOR ;
		}
	}
		
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_FORMAT ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_FORMAT ;
	}
	
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_NEWLINE ) )
	{
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_NEWLINE ;
	}
	
	g->pfuncLogStyle = & LogStyle_FuncArray ;
	
	return 0;
}

/* 转档日志文件 */ /* rotate log file */
static int RotateLogFileSize( LOG *g , long step )
{
	char		rotate_log_pathfilename[ MAXLEN_FILENAME ] ;
	char		rotate_log_pathfilename_access[ MAXLEN_FILENAME ] ;
	char            rotate_log_pathfilename_last[ MAXLEN_FILENAME ] ;
	int		nret ;
		
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
				if( nret )
				{
					LeaveMutexSection( g );
					return nret;
				}
				g->open_flag = 0 ;
			}
		}
	}

	nret = EnterMutexSection(F_SETLK, g ) ;
	if( nret )
		return nret;
	
	nret = _STAT( g->log_pathfilename , & (g->file_change_stat) ) ;
	if( nret == -1 )
	{
		LeaveMutexSection( g );
		return LOG_RETURN_ERROR_INTERNAL;
	}
	
	if( g->file_change_stat.st_size >= g->log_rotate_size )
	{
		for( g->rotate_file_no = 1 ; g->rotate_file_no <= g->rotate_file_count ; g->rotate_file_no++ )
		{
			SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%04ld" , g->log_pathfilename , g->rotate_file_no );
			strcpy( rotate_log_pathfilename_access , rotate_log_pathfilename );
			if( g->pfuncBeforeRotateFile )
				g->pfuncBeforeRotateFile( g , rotate_log_pathfilename_access );
			nret = ACCESS( rotate_log_pathfilename_access , ACCESS_OK ) ;
			if( nret == -1 )
				break;
		}
		if( g->rotate_file_no > g->rotate_file_count )
		{
			for( g->rotate_file_no = 1 ; g->rotate_file_no <= g->rotate_file_count - 1 ; g->rotate_file_no++ )
                        {
                                SNPRINTF( rotate_log_pathfilename_last, sizeof(rotate_log_pathfilename), "%s.%04ld", g->log_pathfilename, g->rotate_file_no );
                                SNPRINTF( rotate_log_pathfilename, sizeof(rotate_log_pathfilename), "%s.%04ld", g->log_pathfilename, g->rotate_file_no + 1);
                                nret = RENAME(rotate_log_pathfilename,rotate_log_pathfilename_last);
                                if( nret )
                                {
                                        UNLINK( rotate_log_pathfilename_last );
                                        RENAME( rotate_log_pathfilename , rotate_log_pathfilename_last ) ;
                                }
                        }

                        g->rotate_file_no = g->rotate_file_count ;
                        SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%04ld" , g->log_pathfilename , g->rotate_file_count);
		}
		
		nret = RENAME( g->log_pathfilename , rotate_log_pathfilename ) ;
		if( nret )
		{
			UNLINK( rotate_log_pathfilename );
			RENAME( g->log_pathfilename , rotate_log_pathfilename ) ;
		}
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		g->skip_count = 1 ;
		/*		
		for( g->rotate_file_no = 0 ; g->rotate_file_no <= g->rotate_file_count ; g->rotate_file_no++ )
		{
			SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%ld" , g->log_pathfilename , g->rotate_file_no );
			strcpy( rotate_log_pathfilename_access , rotate_log_pathfilename );
			if( g->pfuncBeforeRotateFile )
				g->pfuncBeforeRotateFile( g , rotate_log_pathfilename_access );
			nret = ACCESS( rotate_log_pathfilename_access , ACCESS_OK ) ;
			if ( g->rotate_file_no == 0 )
			{
				if ( nret == 0 )
					UNLINK(rotate_log_pathfilename_access);

				continue;
			}
			else
			{
				if ( nret == -1 )
					break;
			}
		}
		
		if( g->rotate_file_no > g->rotate_file_count )
		{
			SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%d" , g->log_pathfilename , 0 );
			SNPRINTF( cmd , sizeof(cmd) , "rm -f %s.* >> %s/rmlog.log 2>> %s/rmlog.log",g->log_pathfilename,getenv("UNIONLOG"),getenv("UNIONLOG"));
			system(cmd);
		}
			
		nret = RENAME( g->log_pathfilename , rotate_log_pathfilename );
		if( nret )
		{
			UNLINK(rotate_log_pathfilename);
			RENAME( g->log_pathfilename , rotate_log_pathfilename ) ;
		}
		
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		g->skip_count = 1 ;
		*/
	}
	else
	{
		if( step == 0 )
			step = 1 ;
		if( g->pressure_factor == 0 )
			g->skip_count = 1 ;
		else
			g->skip_count = ( g->log_rotate_size - g->file_change_stat.st_size ) / step / g->pressure_factor + 1 ;

	}
	
	LeaveMutexSection( g );
	
	return 0;
}

static int RotateLogFilePerDate( LOG *g )
{
	int		nret ;
	
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_stime.tm_mday == 0 )
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
	
	if( g->cache2_rotate_tv.tv_sec == 0 || g->cache2_rotate_stime.tm_mday == 0 )
	{
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	else if
	(
		g->cache1_tv.tv_sec == g->cache2_rotate_tv.tv_sec
		||
		(
			g->cache1_stime.tm_year == g->cache2_rotate_stime.tm_year
			&&
			g->cache1_stime.tm_mon == g->cache2_rotate_stime.tm_mon
			&&
			g->cache1_stime.tm_mday == g->cache2_rotate_stime.tm_mday
		)
	)
	{
	}
	else
	{
		if( g->open_flag == 1 )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				if( g->pfuncCloseLogFinally )
				{
					nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
			}
		}
		
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	
	return 0;
}

/*
static int RotateLogFilePerHour( LOG *g )
{
	char		rotate_log_pathfilename[ MAXLEN_FILENAME ] ;
	int		nret ;
	long		writelen ;
	
	nret = EnterMutexSection(F_SETLK, g ) ;
	if( nret )
		return nret;
	
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_stime.tm_mday == 0 )
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
	
	if( g->cache2_rotate_tv.tv_sec == 0 || g->cache2_rotate_stime.tm_mday == 0 )
	{
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	else if
	(
		g->cache2_rotate_tv.tv_sec == g->cache1_tv.tv_sec
		||
		(
			g->cache2_rotate_stime.tm_year == g->cache1_stime.tm_year
			&&
			g->cache2_rotate_stime.tm_mon == g->cache1_stime.tm_mon
			&&
			g->cache2_rotate_stime.tm_mday == g->cache1_stime.tm_mday
			&&
			g->cache2_rotate_stime.tm_hour == g->cache1_stime.tm_hour
		)
	)
	{
	}
	else
	{
		if( g->open_flag == 1 )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				if( g->pfuncCloseLogFinally )
				{
					nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
					if( nret )
					{
						LeaveMutexSection( g );
						return nret;
					}
				}
			}
		}
	
		SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%04d%02d%02d_%02d" , g->log_pathfilename , g->cache2_rotate_stime.tm_year+1900 , g->cache2_rotate_stime.tm_mon+1 , g->cache2_rotate_stime.tm_mday , g->cache2_rotate_stime.tm_hour );
		RENAME( g->log_pathfilename , rotate_log_pathfilename );
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	
	LeaveMutexSection( g );
	
	return 0;
}
*/

int TimintInit(LOG *g)
{
	int	nret;
	
	if ( g->timingtime <= 0)
		return(0);

	g->cache1_tv.tv_sec = time( NULL ) ;

	if( g->cache2_init_tv.tv_sec == 0 )
	{
		memcpy( & (g->cache2_init_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
	}

	// 间隔5秒加载一次
	if ((g->cache1_tv.tv_sec - g->cache2_init_tv.tv_sec) >= g->timingtime)
	{
		nret = g->pfuncTimingInit( g ,LOG_OUTPUT_NOSET) ;
		if( nret )
			return nret;
		memcpy( & (g->cache2_init_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
	}
	return(0);
}

/* 写日志基函数 */ /* output log */
int WriteLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	long		writelen = 0 ;
	int		nret ;
	LOGBUF		*plogbuf = &g->logbuf;
	
	if( format == NULL )
		return 0;

	//add by linxj 20151112
	if (log_level <= LOG_LEVEL_ERROR && g_log_error_file)
		g->error_flag = 1;
	//add end 20151112

	
	/* 初始化行日志缓冲区 */ /* initialize log buffer  */
	if ((g->multi_mode == LOG_WRITEMODE_ONE) || 
		(g->multi_mode == LOG_WRITEMODE_MULTI_FIRST))
	{
		plogbuf->buf_remain_len = plogbuf->buf_size - 1 - 1 ;
		plogbuf->bufptr = plogbuf->bufbase ;
	}

	if (g->multi_mode == LOG_WRITEMODE_MULTI_FIRST)
		g->multi_mode = LOG_WRITEMODE_MULTI_Middle;
		
	/* 填充行日志缓冲区 */ /* fill log buffer */
	if( g->pfuncLogStyle )
	{
		nret = g->pfuncLogStyle( g , plogbuf , c_filename , c_fileline , log_level , format , valist ) ;
		if( nret )
			return nret;
	}
	
	/* 自定义过滤日志 */ /* filter log */
	if( g->pfuncFilterLog )
	{
		nret = g->pfuncFilterLog( g , & (g->open_handle) , log_level , plogbuf->bufbase , plogbuf->buf_size-1-1 - plogbuf->buf_remain_len ) ;
		if( nret )
			return nret;
	}

	/* 日志先缓存，不写入文件 */
	if (g->multi_mode != LOG_WRITEMODE_ONE)
		goto out_tag;	

	/* 打开文件 */ /* open log */
	if( g->open_flag == 0 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLogFirst )
			{
				nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
		else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLog )
			{
				nret = g->pfuncOpenLog( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	/* 导出日志缓冲区 */ /* output log */
	if( g->pfuncWriteLog )
	{
		nret = g->pfuncWriteLog( g , & (g->open_handle) , log_level , plogbuf->bufbase , plogbuf->buf_size-1-1 - plogbuf->buf_remain_len , & writelen ) ;
		if( nret )
			return nret;
	}
	
#if ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->output == LOG_OUTPUT_FILE )
	{
		g->fsync_elapse--;
		if( g->fsync_elapse < 1 )
		{
			fsync( g->fd );
			g->fsync_elapse = g->fsync_period ;
		}
	}
#endif

	/* 关闭日志 */ /* close log */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				/* 测试日志文件 */
				if( g->pfuncChangeTest )
				{
					nret = g->pfuncChangeTest( g , & (g->test_handle) ) ;
					if( nret )
						return nret;
				}
			}
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
			{
				/* 关闭日志文件 */
				if( g->pfuncCloseLog )
				{
					nret = g->pfuncCloseLog( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
			}
		}
	}
	
	/* 如果输出到文件 */
	if( g->output == LOG_OUTPUT_FILE )
	{
		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_SIZE ) && (g->log_rotate_size > 0) )
		{
			g->skip_count--;
			if( g->skip_count < 1 )
			{
				RotateLogFileSize( g , writelen );
			}
		}

		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_PER_DAY ))
		{
			RotateLogFilePerDate( g );
		}

		/*
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_HOUR )
		{
			RotateLogFilePerHour( g );
		}
		*/
	}

	//add by linxj 20151111
	g->error_flag = 0;
	//add end 20151111
out_tag:	
	/* 清空一级缓存 */ /* clean level 1 cache */
	g->cache1_tv.tv_sec = 0 ;
	g->cache1_stime.tm_mday = 0 ;
	
	return 0;
}

/* 代码宏 */ /* code macros */
#define WRITELOGBASE(_g_,_log_level_) \
	va_list		valist; \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( (_g_)->pfuncTimingInit )	\
		TimintInit(_g_);	\
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) { \
		if( ( (_g_)->multi_mode == LOG_WRITEMODE_ONE) ||	\
			(_log_level_ == LOG_LEVEL_NOLOG) ||		\
			(_log_level_ > LOG_LEVEL_DEFAULT) )		\
			return 0; \
	} else	\
		(_g_)->write_flag = 1;	\
	va_start( valist , format ); \
	nret = WriteLogBase( (_g_) , c_filename , c_fileline , _log_level_ , format , valist ) ; \
	va_end( valist ); \
	if( nret < 0 ) \
		return nret;

/* 带日志等级的写日志 */ /* write log */
int WriteLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , ... )
{	
	WRITELOGBASE( g , log_level )
	return 0;
}

/* 代码宏 */ /* code macros */
#define WRITELOGBASEV(_g_,_log_level_) \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( (_g_)->pfuncTimingInit )	\
		TimintInit(_g_);	\
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) { \
		if( ( (_g_)->multi_mode == LOG_WRITEMODE_ONE) ||	\
			(_log_level_ == LOG_LEVEL_NOLOG) ||		\
			(_log_level_ > LOG_LEVEL_DEFAULT) )		\
			return 0; \
	} else	\
		(_g_)->write_flag = 1;	\
	nret = WriteLogBase( (_g_) , c_filename , c_fileline , _log_level_ , format , valist ) ; \
	if( nret < 0 ) \
		return nret;

/* 带日志等级的写日志 */ /* write log */
int WriteLogV( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist)
{	
	WRITELOGBASEV( g , log_level )
	return 0;
}

/* 写程序员日志 */ /* write debug log */
int PGMLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_PGM )
	return 0;
}

/* 写调试日志 */ /* write debug log */
int DebugLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_DEBUG )
	return 0;
}

/* 写普通信息日志 */ /* write info log */
int InfoLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_INFO )
	return 0;
}

/* 写错误日志 */ /* write error log */
int ErrorLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_ERROR )
	return 0;
}

/* 写致命错误日志 */ /* write fatal log */
int OserrLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_OSERR )
	return 0;
}

/* 写十六进制块日志基函数 */ /* output log */
int WriteHexLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist )
{
	int		row_offset , col_offset ;
	//long		len ;
	long		writelen = 0 ;
	int		nret ;
	LOGBUF		*plogbuf =  &g->logbuf;
	
	/* 初始化十六进制块日志缓冲区 */ /* initialize log buffer  */
	if ((g->multi_mode == LOG_WRITEMODE_ONE) || 
		(g->multi_mode == LOG_WRITEMODE_MULTI_FIRST))
	{
		plogbuf->buf_remain_len = plogbuf->buf_size - 1 - 1 ;
		plogbuf->bufptr = plogbuf->bufbase ;
	}

	if (g->multi_mode == LOG_WRITEMODE_MULTI_FIRST)
		g->multi_mode = LOG_WRITEMODE_MULTI_Middle;

	/* 填充行日志缓冲区 */ /* fill log buffer */
	if( format )
	{
		if( g->pfuncLogStyle )
		{
			nret = g->pfuncLogStyle( g , plogbuf , c_filename , c_fileline , log_level , format , valist ) ;
			if( nret )
				return nret;
		}
	}
	
	/* 填充十六进制块日志缓冲区 */ /* fill hex log buffer */
	if( buffer && buflen > 0 )
	{
		FormatLogBuffer( g , plogbuf , "             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF" );
		FormatLogBuffer( g , plogbuf , LOG_NEWLINE );
		
		row_offset = 0 ;
		col_offset = 0 ;
		while(1)
		{
			FormatLogBuffer( g , plogbuf , "0x%08X   " , row_offset * 16 );

			for( col_offset = 0 ; col_offset < 16 ; col_offset++ )
			{
				if( row_offset * 16 + col_offset < buflen )
					FormatLogBuffer( g , plogbuf , "%02X " , *((unsigned char *)buffer+row_offset*16+col_offset) );
				else
					FormatLogBuffer( g , plogbuf , "   " );
			}
			FormatLogBuffer( g , plogbuf , "  "  );
			for( col_offset = 0 ; col_offset < 16 ; col_offset++ )
			{
				if( row_offset * 16 + col_offset < buflen )
				{
					if( isprint( (int)*(buffer+row_offset*16+col_offset) ) )
						FormatLogBuffer( g , plogbuf , "%c" , *((unsigned char *)buffer+row_offset*16+col_offset) ) ;
					else
						FormatLogBuffer( g , plogbuf , "." ) ;
				}
				else
					FormatLogBuffer( g , plogbuf , "." ) ;
			}
			FormatLogBuffer( g , plogbuf , LOG_NEWLINE ) ;
			if( row_offset * 16 + col_offset >= buflen )
				break;
			row_offset++;
		}
	}
	
	if( plogbuf->bufptr == plogbuf->bufbase )
		return 0;
	
	/* 自定义过滤日志 */ /* filter log */
	if( g->pfuncFilterLog )
	{
		nret = g->pfuncFilterLog( g , & (g->open_handle) , log_level , plogbuf->bufbase , plogbuf->buf_size-1-1 - plogbuf->buf_remain_len ) ;
		if( nret )
			return nret;
	}
	
	/* 日志先缓存，不写入文件 */
	if (g->multi_mode != LOG_WRITEMODE_ONE)
		goto out_tag;
		
	/* 打开文件 */ /* open log */
	if( g->open_flag == 0 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLogFirst )
			{
				nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
		else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLog )
			{
				nret = g->pfuncOpenLog( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	
	/* 导出日志缓冲区 */ /* output file */
	if( g->pfuncWriteLog )
	{
		nret = g->pfuncWriteLog( g , & (g->open_handle) , log_level , plogbuf->bufbase , plogbuf->buf_size-1-1 - plogbuf->buf_remain_len , & writelen ) ;
		if( nret )
			return nret;
	}
	
#if ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->output == LOG_OUTPUT_FILE )
	{
		g->fsync_elapse--;
		if( g->fsync_elapse < 1 )
		{
			fsync( g->fd );
			g->fsync_elapse = g->fsync_period ;
		}
	}
#endif

	/* 关闭日志 */ /* close file */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				/* 测试日志文件 */
				if( g->pfuncChangeTest )
				{
					nret = g->pfuncChangeTest( g , & (g->test_handle) ) ;
					if( nret )
						return nret;
				}
			}
			/*
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
			{
			}
			*/
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
			{
				/* 关闭日志文件 */
				if( g->pfuncCloseLog )
				{
					nret = g->pfuncCloseLog( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
				
			}
		}
	}
	
	/* 如果输出到文件 */
	if( g->output == LOG_OUTPUT_FILE )
	{
		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_SIZE ) && (g->log_rotate_size > 0) )
		{
			g->skip_count--;
			if( g->skip_count < 1 )
			{
				RotateLogFileSize( g , writelen );
			}
		}
		
		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_PER_DAY ))
		{
			RotateLogFilePerDate( g );
		}

		/*
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_HOUR )
		{
			RotateLogFilePerHour( g );
		}
		*/
	}
out_tag:	
	/* 清空一级缓存 */ /* clean level 1 cache */
	g->cache1_tv.tv_sec = 0 ;
	g->cache1_stime.tm_mday = 0 ;
	
	return 0;
}

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGBASE(_g_,_log_level_) \
	va_list		valist; \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( (_g_)->pfuncTimingInit )	\
		TimintInit(_g_);	\
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) { \
		if( ( (_g_)->multi_mode == LOG_WRITEMODE_ONE) ||	\
			(_log_level_ == LOG_LEVEL_NOLOG) ||		\
			(_log_level_ > LOG_LEVEL_DEFAULT) )		\
			return 0; \
	} else	\
		(_g_)->write_flag = 1;	\
	va_start( valist , format ); \
	nret = WriteHexLogBase( (_g_) , c_filename , c_fileline , _log_level_ , buffer , buflen , format , valist ) ; \
	va_end( valist ); \
	if( nret ) \
		return nret;

/* 带日志等级的写十六进制块日志 */ /* write hex log */
int WriteHexLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , log_level )
	return 0;
}

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGBASEV(_g_,_log_level_) \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( (_g_)->pfuncTimingInit )	\
		TimintInit(_g_);	\
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) { \
		if( ( (_g_)->multi_mode == LOG_WRITEMODE_ONE) ||	\
			(_log_level_ == LOG_LEVEL_NOLOG) ||		\
			(_log_level_ > LOG_LEVEL_DEFAULT) )		\
			return 0; \
	} else	\
		(_g_)->write_flag = 1;	\
	nret = WriteHexLogBase( (_g_) , c_filename , c_fileline , _log_level_ , buffer , buflen , format , valist ) ; \
	if( nret ) \
		return nret;

/* 带日志等级的写十六进制块日志 */ /* write hex log */
int WriteHexLogV( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist)
{
	WRITEHEXLOGBASEV( g , log_level )
	return 0;
}

/* 写十六进制块调试日志 */ /* write debug hex log */
int PGMHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_PGM )
	return 0;
}

/* 写十六进制块调试日志 */ /* write debug hex log */
int DebugHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_DEBUG )
	return 0;
}

/* 写十六进制块普通信息日志 */ /* write info hex log */
int InfoHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_INFO )
	return 0;
}

/* 写十六进制块错误日志 */ /* write error hex log */
int ErrorHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_ERROR )
	return 0;
}

/* 写十六进制块致命错误日志 */ /* write fatal hex log */
int OserrHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_OSERR )
	return 0;
}

/* 文件变动测试间隔 */
#define LOG_FILECHANGETEST_INTERVAL_ON_OPEN_ONCE_DEFAULT	10

/* 设置日志选项 */ /* set log options */
int SetLogOptions( LOG *g , int log_options )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->log_options = log_options ;
	
	SetLogFileChangeTest( g , LOG_FILECHANGETEST_INTERVAL_ON_OPEN_ONCE_DEFAULT );
	
	return 0;
}

/* 设置文件改变测试间隔 */
int SetLogFileChangeTest( LOG *g , long interval )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if (g->file_change_test_interval != interval)
	{
		g->file_change_test_interval = interval ;
		g->file_change_test_no = g->file_change_test_interval ;
		memset( & (g->file_change_stat) , 0x00 , sizeof(g->file_change_stat) );
	}
	
	return 0;
}

int SetLogFsyncPeriod( LOG *g , long period )
{
	g->fsync_period = period ;
	g->fsync_elapse = g->fsync_period ;

	return 0;
}

/* 设置日志自定义标签 */ /* set log custom labels */
int SetLogCustLabel( LOG *g , int index , char *cust_label )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( 1 <= index && index <= LOG_MAXCNT_CUST_LABEL )
	{
		memset( g->cust_label[index-1] , 0x00 , sizeof(g->cust_label[index-1]) );
		if( cust_label && cust_label[0] != '\0' )
			strncpy( g->cust_label[index-1] , cust_label , sizeof(g->cust_label[index-1])-1 );
		return 0;
	}
	else
	{
		return LOG_RETURN_ERROR_PARAMETER;
	}
}

/* 设置日志转档模式 */ /* set log rotate mode */
int SetLogRotateMode( LOG *g , int rotate_mode )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if (g->rotate_mode != rotate_mode)
		g->rotate_mode = rotate_mode ;
	return 0;
}

/* 设置日志转档大小 */ /* set rotate size */
int SetLogRotateSize( LOG *g , long log_rotate_size )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( log_rotate_size < 0 )
		log_rotate_size = 0;
	if (g->log_rotate_size != log_rotate_size)
		g->log_rotate_size = log_rotate_size ;
	return 0;
}

/* 设置日志转档紧迫系数 */ /* set rotate pressure fator */
int SetLogRotatePressureFactor( LOG *g , long pressure_factor )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( pressure_factor < 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	if (g->pressure_factor != pressure_factor)
		g->pressure_factor = pressure_factor ;
	return 0;
}

/* 设置日志转档最大文件数 */ /* set rotate file count */
int SetLogRotateFileCount( LOG *g , long rotate_file_count )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( g->rotate_file_count == rotate_file_count )
		return(0);	
	if( rotate_file_count <= 0 )
		g->rotate_file_count = LOG_ROTATE_SIZE_FILE_COUNT_DEFAULT;
	else
		g->rotate_file_count = rotate_file_count ;
	return 0;
}

/* 设置自定义定时初始化回调函数 */ /* set timing int callback function */
int SetTimingInitFunc( LOG *g , long timingtime, funcTimingInit *pfuncTimingInit )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( timingtime <= 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	g->timingtime = timingtime ;
	g->pfuncTimingInit = pfuncTimingInit ;
	return 0;
}

/* 设置自定义日志转档前回调函数 */ /* set custom callback function before rotate log */
int SetBeforeRotateFileFunc( LOG *g , funcBeforeRotateFile *pfuncBeforeRotateFile )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pfuncBeforeRotateFile = pfuncBeforeRotateFile ;
	return 0;
}

/* 设置自定义日志转档后回调函数 */ /* set custom callback function after rotate log */
int SetAfterRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pfuncAfterRotateFile = pfuncAfterRotateFile ;
	return 0;
}

/* 设置自定义检查日志等级回调函数类型 */ /* set custom filter callback function */
int SetFilterLogFunc( LOG *g , funcFilterLog *pfuncFilterLog )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pfuncFilterLog = pfuncFilterLog ;
	return 0;
}

/* 设置行日志缓冲区大小 */ /* set log buffer size */
int SetLogBufferSize( LOG *g , long log_bufsize , long max_log_bufsize )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	return SetBufferSize( g , & (g->logbuf) , log_bufsize , max_log_bufsize );
}

// 设置多条日志开始
int SetMultiLogBegin(LOG *g)
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if (g->output != LOG_OUTPUT_FILE)
		return(g->multi_mode = LOG_WRITEMODE_ONE);

	SetMultiLogEnd( g );
	
	if (g->multi_mode == LOG_WRITEMODE_ONE)
		return(g->multi_mode = LOG_WRITEMODE_MULTI_FIRST);
	else
		return(0);
}

// 设置多条日志结束
int SetMultiLogEnd(LOG *g)
{
	int	nret = 0;
	long	writelen ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if (g->output != LOG_OUTPUT_FILE)
		return(g->multi_mode = LOG_WRITEMODE_ONE);

	/* 每次写 */
	if (g->multi_mode == LOG_WRITEMODE_ONE)
		return(0);
	
	g->multi_mode = LOG_WRITEMODE_ONE;

	/* 检查写标志 */
	if (g->write_flag == 0)
		return(0);

	g->write_flag = 0;

	/* 关闭日志 */ /* close log */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				/* 测试日志文件 */
				if( g->pfuncChangeTest )
				{
					g->file_change_test_no = 1;
					nret = g->pfuncChangeTest( g , & (g->test_handle) ) ;
					if( nret )
						return nret;
				}
			}
		}
	}

	/* 打开文件 */ /* open log */
	if( g->open_flag == 0 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLogFirst )
			{
				nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
		else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLog )
			{
				nret = g->pfuncOpenLog( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	
	/* 导出日志缓冲区 */ /* output log */
	if( g->pfuncWriteLog )
	{
		nret = g->pfuncWriteLog( g , & (g->open_handle) , -1 , g->logbuf.bufbase , g->logbuf.buf_size-1-1 - g->logbuf.buf_remain_len , & writelen ) ;
		if( nret )
			return nret;
	}
	
#if ( defined __unix ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->output == LOG_OUTPUT_FILE )
	{
		g->fsync_elapse--;
		if( g->fsync_elapse < 1 )
		{
			//fsync( g->fd );
			g->fsync_elapse = g->fsync_period ;
		}
	}
#endif
	
	/* 关闭日志 */ /* close log */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
			{
				/* 关闭日志文件 */
				if( g->pfuncCloseLog )
				{
					nret = g->pfuncCloseLog( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
			}
		}
	}
	
	/* 如果输出到文件 */
	if( g->output == LOG_OUTPUT_FILE )
	{
		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_SIZE ) && (g->log_rotate_size > 0) )
		{
			g->skip_count--;
			if( g->skip_count < 1 )
			{
				RotateLogFileSize( g , writelen );
			}
		}

		if (TEST_ATTRIBUTE( g->rotate_mode , LOG_ROTATEMODE_PER_DAY ))
		{
			RotateLogFilePerDate( g );
		}

		/*
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_HOUR )
		{
			RotateLogFilePerHour( g );
		}
		*/
	}

	g->error_flag = 0;
	return(0);
}

/* 直接设置日志输出回调函数 */
int SetLogOutputFuncDirectly( LOG *g , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pfuncOpenLogFirst = pfuncOpenLogFirst ;
	g->pfuncOpenLog = pfuncOpenLog ;
	g->pfuncWriteLog = pfuncWriteLog ;
	g->pfuncChangeTest = pfuncChangeTest ;
	g->pfuncCloseLog = pfuncCloseLog ;
	g->pfuncCloseLogFinally = pfuncCloseLogFinally ;
	return 0;
}

/* 直接设置行日志风格回调函数 */
int SetLogStyleFuncDirectly( LOG *g , funcLogStyle *pfuncLogStyle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pfuncLogStyle = pfuncLogStyle ;

	return 0;
}

int SetOpenFlag( LOG *g , char open_flag )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	g->open_flag = open_flag ;
	return 0;
}

char IsLogOpened( LOG *g )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	return g->open_flag ;
}

int GetLogLevel( LOG *g )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	return g->log_level;
}

LOGBUF *GetLogBuffer( LOG *g )
{
	if( g == NULL )
		return NULL;
	return & (g->logbuf);
}

long FormatLogBuffer( LOG *g , LOGBUF *logbuf , char *format , ... )
{
	va_list		valist ;
	long		len  = -1;
_REDO :
	va_start( valist , format );
	len = VSNPRINTF( logbuf->bufptr , logbuf->buf_remain_len , format , valist ) ;
	va_end( valist );
#if ( defined _WIN32 )
	if( len == -1 )
#elif ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
	if( len >= logbuf->buf_remain_len )
#endif
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
		{
			logbuf->bufptr[0] = '\0' ;
			return LOG_RETURN_ERROR_ALLOC_MAX;
		}
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
			return nret;
		goto _REDO;
	}
	OFFSET_BUFPTR( logbuf , len )
	return len;
}

long FormatLogBufferV( LOG *g , LOGBUF *logbuf , char *format , va_list valist )
{
	long		len = -1;
	va_list		tmpValist;
_REDO :
	va_copy(tmpValist,valist);
	len = VSNPRINTF( logbuf->bufptr , logbuf->buf_remain_len , format , tmpValist ) ;
	va_end(tmpValist);
#if ( defined _WIN32 )
	if( len == -1 )
#elif ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
	if( len >= logbuf->buf_remain_len )
#endif
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
		{
			logbuf->bufptr[0] = '\0' ;
			return LOG_RETURN_ERROR_ALLOC_MAX;
		}
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
			return nret;
		goto _REDO;
	}
	OFFSET_BUFPTR( logbuf , len )
	return len;
}

long MemcatLogBuffer( LOG *g , LOGBUF *logbuf , char *append , long len )
{
_REDO :
	if( len > logbuf->buf_remain_len )
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
			return LOG_RETURN_ERROR_ALLOC_MAX;
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
			return nret;
		goto _REDO;
	}
	memcpy( logbuf->bufptr , append , len );
	OFFSET_BUFPTR( logbuf , len )
	logbuf->bufptr[0] = '\0' ;
	return len;
}

int ConvertLogOutput_atoi( char *output_desc , int *p_log_output )
{
	if( strcmp( output_desc , "STDOUT" ) == 0 )
		(*p_log_output) = LOG_OUTPUT_STDOUT ;
	else if( strcmp( output_desc , "STDERR" ) == 0 )
		(*p_log_output) = LOG_OUTPUT_STDERR ;
	else if( strcmp( output_desc , "SYSLOG" ) == 0 )
		(*p_log_output) = LOG_OUTPUT_SYSLOG ;
	else if( strcmp( output_desc , "FILE" ) == 0 )
		(*p_log_output) = LOG_OUTPUT_FILE ;
	else if( strcmp( output_desc , "CALLBACK" ) == 0 )
		(*p_log_output) = LOG_OUTPUT_CALLBACK ;
	else
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

int ConvertLogLevel_atoi( char *log_level_desc , int *p_log_level )
{
	if( strcmp( log_level_desc , "ALL" ) == 0 )
		(*p_log_level) = LOG_LEVEL_ALL ;
	else if( strcmp( log_level_desc , "PGM" ) == 0 )
		(*p_log_level) = LOG_LEVEL_PGM ;
	else if( strcmp( log_level_desc , "DEBUG" ) == 0 )
		(*p_log_level) = LOG_LEVEL_DEBUG ;
	else if( strcmp( log_level_desc , "INFO" ) == 0 )
		(*p_log_level) = LOG_LEVEL_INFO ;
	else if( strcmp( log_level_desc , "ERROR" ) == 0 )
		(*p_log_level) = LOG_LEVEL_ERROR ;
	else if( strcmp( log_level_desc , "OSERR" ) == 0 )
		(*p_log_level) = LOG_LEVEL_OSERR ;
	else if( strcmp( log_level_desc , "NOLOG" ) == 0 )
		(*p_log_level) = LOG_LEVEL_NOLOG ;
	else
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

int ConvertLogLevel_itoa( int log_level , char **log_level_desc )
{
	(*log_level_desc) = sg_aszLogLevelDesc[log_level] ;
	return 0;
}

int ConvertLogStyle_atol( char *line_style_desc , long *p_line_style )
{
	if( strcmp( line_style_desc , "DATE" ) == 0 )
		(*p_line_style) = LOG_STYLE_DATE ;
	else if( strcmp( line_style_desc , "DATETIME" ) == 0 )
		(*p_line_style) = LOG_STYLE_DATETIME ;
	else if( strcmp( line_style_desc , "DATETIMEMS" ) == 0 )
		(*p_line_style) = LOG_STYLE_DATETIMEMS ;
	else if( strcmp( line_style_desc , "LOGLEVEL" ) == 0 )
		(*p_line_style) = LOG_STYLE_LOGLEVEL ;
	else if( strcmp( line_style_desc , "PID" ) == 0 )
		(*p_line_style) = LOG_STYLE_PID ;
	else if( strcmp( line_style_desc , "TID" ) == 0 )
		(*p_line_style) = LOG_STYLE_TID ;
	else if( strcmp( line_style_desc , "SOURCE" ) == 0 )
		(*p_line_style) = LOG_STYLE_SOURCE ;
	else if( strcmp( line_style_desc , "FORMAT" ) == 0 )
		(*p_line_style) = LOG_STYLE_FORMAT ;
	else if( strcmp( line_style_desc , "NEWLINE" ) == 0 )
		(*p_line_style) = LOG_STYLE_NEWLINE ;
	else if( strcmp( line_style_desc , "CUSTLABEL1" ) == 0 )
		(*p_line_style) = LOG_STYLE_CUSTLABEL1 ;
	else if( strcmp( line_style_desc , "CUSTLABEL2" ) == 0 )
		(*p_line_style) = LOG_STYLE_CUSTLABEL2 ;
	else if( strcmp( line_style_desc , "CUSTLABEL3" ) == 0 )
		(*p_line_style) = LOG_STYLE_CUSTLABEL3 ;
	else
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

int ConvertLogOption_atol( char *log_option_desc , long *p_log_option )
{
	if( strcmp( log_option_desc , "OPEN_AND_CLOSE" ) == 0 )
		(*p_log_option) = LOG_OPTION_OPEN_AND_CLOSE ;
	else if( strcmp( log_option_desc , "CHANGE_TEST" ) == 0 )
		(*p_log_option) = LOG_OPTION_CHANGE_TEST ;
	else if( strcmp( log_option_desc , "OPEN_ONCE" ) == 0 )
		(*p_log_option) = LOG_OPTION_OPEN_ONCE ;
	else if( strcmp( log_option_desc , "SET_OUTPUT_BY_FILENAME" ) == 0 )
		(*p_log_option) = LOG_OPTION_SET_OUTPUT_BY_FILENAME ;
	else if( strcmp( log_option_desc , "FILENAME_APPEND_DOT_LOG" ) == 0 )
		(*p_log_option) = LOG_OPTION_FILENAME_APPEND_DOT_LOG ;
	else
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

int ConvertLogRotateMode_atoi( char *rotate_mode_desc , int *p_rotate_mode )
{
	if( strcmp( rotate_mode_desc , "NONE" ) == 0 )
		(*p_rotate_mode) = LOG_ROTATEMODE_NONE ;
	else if( strcmp( rotate_mode_desc , "SIZE" ) == 0 )
		(*p_rotate_mode) = LOG_ROTATEMODE_SIZE ;
	else if( strcmp( rotate_mode_desc , "PER_DAY" ) == 0 )
		(*p_rotate_mode) = LOG_ROTATEMODE_PER_DAY ;
	/*
	else if( strcmp( rotate_mode_desc , "PER_HOUR" ) == 0 )
		(*p_rotate_mode) = LOG_ROTATEMODE_PER_HOUR ;
	*/
	else
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

int ConvertBufferSize_atol( char *bufsize_desc , long *p_bufsize )
{
	long	bufsize_desc_len ;
	
	bufsize_desc_len = strlen(bufsize_desc) ;
	
	if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "GB" ) == 0 )
		(*p_bufsize) = atol(bufsize_desc) * 1024 * 1024 * 1024 ;
	else if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "MB" ) == 0 )
		(*p_bufsize) = atol(bufsize_desc) * 1024 * 1024 ;
	else if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "KB" ) == 0 )
		(*p_bufsize) = atol(bufsize_desc) * 1024 ;
	else if( bufsize_desc_len > 1 && strcmp( bufsize_desc + bufsize_desc_len - 1 , "B" ) == 0 )
		(*p_bufsize) = atol(bufsize_desc) ;
	else
		(*p_bufsize) = atol(bufsize_desc) ;
	
	if( (*p_bufsize) == 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	
	return 0;
}

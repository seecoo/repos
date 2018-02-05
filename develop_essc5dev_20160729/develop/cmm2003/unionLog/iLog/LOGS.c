/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
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

#include "LOGS.h"

/* 日志句柄集合结构 */ /* log handle collection structure */
// 第0个为默认的
struct tagLOGS
{
	long	g_no;
	char	*g_id[ LOGS_MAXCNT_LOG] ;
	LOG	*g[ LOGS_MAXCNT_LOG] ;
	long	index;
} ;

/* 销毁日志句柄集合 */ /* destruction of logging handle collection */
void DestroyLogsHandle( LOGS *gs )
{
	if( gs )
	{
		long	g_no ;
		
		for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
		{
			if( gs->g_id[g_no] && gs->g[g_no] )
			{
				free( gs->g_id[g_no] );	
				gs->g_id[g_no] = NULL;			
				DestroyLogHandle( gs->g[g_no] );
				gs->g[g_no] = NULL;
			}
		}
		
		free( gs );
	}
	
	return;
}

/* 创建日志句柄集合 */ /* create log handle collection */
LOGS *CreateLogsHandle()
{
	LOGS	*gs = NULL ;
	
	gs = (LOGS *)malloc( sizeof(LOGS) ) ;
	if( gs == NULL )
		return NULL;
	memset( gs , 0x00 , sizeof(LOGS) );
	
	return gs;
}

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 ) /* 为了下面函数的strdup能编译不报警告 */
char *strdup(const char *s);
#endif

int AddLogToLogs( LOGS *gs , char *g_id , LOG *g )
{
	long		g_no ;
	
	if( gs == NULL )
		return LOG_RETURN_ERROR_PARAMETER;

	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;

	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] == NULL && gs->g[g_no] == NULL )
		{
setLogs:
			gs->g_id[g_no] = strdup( g_id ) ;
			if( gs->g_id[g_no] == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			gs->g[g_no] = g ;
			return 0;
		}
	}
	
	// 已经超限了随机删除一个
	if ( LOGS_MAXCNT_LOG > 1 )
	{
		srand( (unsigned)time(NULL) );
		g_no =  rand() % (LOGS_MAXCNT_LOG - 1) + 1 ;
	}
	else
		g_no = 0;
	free( gs->g_id[g_no] ); 
	gs->g_id[g_no] = NULL;
	DestroyLogHandle( gs->g[g_no] );
	gs->g[g_no] = NULL;
	goto setLogs;
	//return LOGS_RETURN_ERROR_TOOMANY_LOG;
}

LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	LOG		*g = NULL ;
	
	if( gs == NULL )
		return NULL;

	if( g_id == NULL )
		return NULL;
	
	for( g_no = 1 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				free( gs->g_id[g_no] ); 
				gs->g_id[g_no] = NULL ;
				g = gs->g[g_no] ;
				gs->g[g_no] = NULL ;
				return g;
			}
		}
	}
	
	return NULL;
}

int DeleteOutLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	
	if( gs == NULL )
		return 0;

	if( g_id == NULL )
		return 0;
	
	for( g_no = 1 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				free( gs->g_id[g_no] ); 
				gs->g_id[g_no] = NULL;
				DestroyLogHandle( gs->g[g_no] );
				gs->g[g_no] = NULL;
				return(0);
			}
		}
	}
	
	return 0;
}

LOG *GetLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	
	if( gs == NULL )
		return NULL;
		
	if( g_id == NULL )
		return gs->g[0];
	
	for( g_no = 1 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				return gs->g[g_no];
			}
		}
	}
	
	return gs->g[0];
}

int FindLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	long		index;
	
	if( gs == NULL )
		return 0;
		
	if( g_id == NULL )
		return 0;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		index = (gs->index + g_no) % LOGS_MAXCNT_LOG;
		if( gs->g_id[index] && gs->g[index] )
		{
			if( STRCMP( gs->g_id[index] , == , g_id ) )
			{
				gs->index = index;
				return 1;
			}
		}
	}
	
	return 0;
}

/*
int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g )
{
	if( gs == NULL )
		return LOG_RETURN_ERROR_PARAMETER;

	for( ++(*p_index) ; (*p_index) >= LOGS_MAXCNT_LOG ; (*p_index)++ )
	{
		if( gs->g_id[(*p_index)] && gs->g[(*p_index)] )
		{
			if( pp_g_id )
				(*pp_g_id) = gs->g_id[(*p_index)] ;
			if( pp_g )
				(*pp_g) = gs->g[(*p_index)] ;
			return 0;
		}
	}
	
	return LOGS_RETURN_INFO_NOTFOUND;
}
*/

extern int WriteLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );

/* 代码宏 */ /* code macro */
#define WRITELOGSBASE(_gs_,_log_level_) \
	LOG		*(_g_) = GetLogFromLogs(_gs_,mdc_get());	\
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

/* 带日志等级的写日志 */ /* write log handle collection */
int WriteLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( gs , log_level )
	return 0;
}

/* 代码宏 */ /* code macro */
#define WRITELOGSBASEV(_gs_,_log_level_) \
	LOG		*(_g_) = GetLogFromLogs(_gs_,mdc_get());	\
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

/* 带日志等级的写日志 */ /* write log handle collection */
int WriteLogsV( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *format , va_list valist)
{
	WRITELOGSBASEV( gs , log_level )
	return 0;
}

/* 写程序员日志 */ /* write debug log handle collection */
int PGMLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_PGM )
	return 0;
}

/* 写调试日志 */ /* write debug log handle collection */
int DebugLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

/* 写普通信息日志 */ /* write info log handle collection */
int InfoLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

/* 写错误日志 */ /* write error log handle collection */
int ErrorLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

/* 写致命错误日志 */ /* write fatal log handle collection */
int OserrLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_OSERR )
	return 0;
}

extern int WriteHexLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist );

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGSBASE(_gs_,_log_level_) \
	LOG		*(_g_) = GetLogFromLogs(_gs_,mdc_get());	\
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

/* 带日志等级的写十六进制块日志 */ /* write hex log handle collection */
int WriteHexLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , log_level )
	return 0;
}

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGSBASEV(_gs_,_log_level_) \
	LOG		*(_g_) = GetLogFromLogs(_gs_,mdc_get());	\
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

/* 带日志等级的写十六进制块日志 */ /* write hex log handle collection */
int WriteHexLogsV( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist )
{
	WRITEHEXLOGSBASEV( gs , log_level )
	return 0;
}

/* 写十六进制块程序员日志 */ /* write debug hex log handle collection */
int PGMHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_PGM )
	return 0;
}

/* 写十六进制块调试日志 */ /* write debug hex log handle collection */
int DebugHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

/* 写十六进制块普通信息日志 */ /* write info hex log handle collection */
int InfoHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

/* 写十六进制块错误日志 */ /* write error hex log handle collection */
int ErrorHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

/* 写十六进制块致命错误日志 */ /* write fatal hex log handle collection */
int OserrHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_OSERR )
	return 0;
}


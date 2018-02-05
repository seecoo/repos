#ifndef _H_LOGS_
#define _H_LOGS_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifdef __cplusplus
extern "C" {
#endif

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

/* ��������ֵ�� */ /* function returns value macros */
#define LOGS_RETURN_ERROR_TOOMANY_LOG	-31	/* ̫����־����� */ /* too many log handles */
#define LOGS_RETURN_ERROR_NOTFOUND	-32	/* û�и���־��� */ /* log handle not found */
#define LOGS_RETURN_INFO_NOTFOUND	32	/* û�и���־����� */ /* log handle not found */

/* ������ */ /* other macros */
#define LOGS_MAXCNT_LOG			20	/* ��־��������������־������� */ /* maximum amount of log handle in one log handle collection */

typedef struct tagLOGS LOGS ;

/* ������־������Ϻ��� */ /* log handle collection functions */
_WINDLL_FUNC LOGS *CreateLogsHandle();
_WINDLL_FUNC void DestroyLogsHandle( LOGS *gs );

_WINDLL_FUNC int AddLogToLogs( LOGS *gs , char *g_id , LOG *g );
_WINDLL_FUNC LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC int DeleteOutLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC LOG *GetLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC int FindLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g );

/* д��־���Ϻ��� */ /* output logs */
_WINDLL_FUNC int WriteLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int WriteLogsV( LOGS *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );
_WINDLL_FUNC int PGMLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int DebugLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int OserrLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );

/* дʮ�����ƿ���־���Ϻ��� */ /* output hex log collection */
_WINDLL_FUNC int WriteHexLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteHexLogsV( LOGS *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist );
_WINDLL_FUNC int PGMHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int OserrHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

/* д��־���Ϻ����Ŀɱ������ */ /* output log collection macros */
#define WRITELOGS( _g_ , _log_level_ , ... )	WriteLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define PGMLOGS( _g_ , ... )			PGMLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define DEBUGLOGS( _g_ , ... )			DebugLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGS( _g_ , ... )			InfoLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGS( _g_ , ... )			ErrorLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define OSERRLOGS( _g_ , ... )			OserrLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );

/* дʮ�����ƿ���־���ϵĿɱ������ */ /* output hex log collection macros */
#define WRITEHEXLOGS( _g_ , _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define PGMHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			PGMHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			DebugHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			InfoHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			ErrorHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define OserrHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			OserrHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );

#endif

/* ����滻�� */
#define create_logs_handle		CreateLogsHandle
#define destroy_logs_handle		DestroyLogsHandle

#define add_log_to_logs			AddLogToLogs
#define remove_out_log_from_logs	RemoveOutLogFromLogs
#define get_log_from_logs		GetLogFromLogs
#define find_log_from_logs		FindLogFromLogs
#define travel_log_from_logs		TravelLogFromLogs

#define write_logs			WriteLogs
#define pgm_logs			PGMLogs
#define debug_logs			DebugLogs
#define info_logs			InfoLogs
#define error_logs			ErrorLogs
#define oserr_logs			OserrLogs

#define write_hex_logs			WriteLogs
#define pgm_hex_logs			PGMHexLogs
#define debug_hex_logs			DebugHexLogs
#define info_hex_logs			InfoHexLogs
#define error_hex_logs			ErrorHexLogs
#define oserr_hex_logs			OserrHexLogs

#define LOG_TRAVELLOG_INDEX_INIT	-1

#ifdef __cplusplus
}
#endif

#endif

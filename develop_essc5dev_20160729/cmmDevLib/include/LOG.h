#ifndef _H_LOG_
#define _H_LOG_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.10
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#if ( defined _WIN32 )
#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <pthread.h>
#endif

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
struct tm *localtime_r(const time_t *timep, struct tm *result);
#endif

/* ��������ֵ�� */ /* function returns value macros */
#define LOG_RETURN_ERROR_ALLOC		-11	/* �����ڴ�ʧ�� */ /* alloc memory failure */
#define LOG_RETURN_ERROR_INTERNAL	-12	/* �ڲ����� */ /* internal error */
#define LOG_RETURN_ERROR_ALLOC_MAX	-13	/* �ڴ�ʹ�ó��� */ /* memory usage transfinite */
#define LOG_RETURN_ERROR_PARAMETER	-14	/* �������� */ /* parameter invalid */
#define LOG_RETURN_ERROR_NOTSUPPORT	-17	/* �ݲ�֧�� */ /* not support */
#define LOG_RETURN_ERROR_CREATEFILE	-21	/* �����ļ�ʧ�� */ /* failed to create file */
#define LOG_RETURN_ERROR_OPENFILE	-22	/* ���ļ�ʧ�� */ /* failed to open file  */
#define LOG_RETURN_ERROR_WRITEFILE	-23	/* д�ļ�ʧ�� */ /* failed to write file */

/********************************************************/
/*   ����Ϊ�������� The basic features are as follows   */
/********************************************************/

/* ��־������� */ /* log output type macros */
#define LOG_OUTPUT_NOSET	-1	/* ������ */ /* no set */
#define LOG_OUTPUT_STDOUT	1	/* ��׼��� */ /* stdout */
#define LOG_OUTPUT_STDERR	2	/* ��׼������� */ /* stderr */
#define LOG_OUTPUT_SYSLOG	3	/* UNIX&Linux��syslog �� Windows��WINDOWS EVENT */
#define LOG_OUTPUT_FILE		11	/* �ļ� */ /* file */
#define LOG_OUTPUT_CALLBACK	21	/* ��ʹ���Զ�����־����ص������� */ /* using custom log output callback function */

/* ��־�ȼ��� */ /* log level macros */
#define LOG_LEVEL_NOLOG		0	/* ����Ҫ�����־ */ /* no log */
#define LOG_LEVEL_OSERR		1	/* ���ش���ȼ� */ /* error level */
#define LOG_LEVEL_ERROR		2	/* ����ȼ� */ /* error level */
#define LOG_LEVEL_INFO		3	/* ��ͨ��Ϣ�ȼ� */ /* info level */
#define LOG_LEVEL_DEBUG		4	/* ���Եȼ� */ /* debug level */
#define LOG_LEVEL_PGM		5	/* ���Եȼ� */ /* debug level */
#define LOG_LEVEL_ALL		6	/* ���� */ /* debug level */

#define LOG_LEVEL_DEFAULT	LOG_LEVEL_INFO

/* ����־�����Ϻ� */ /* Line logging combination style macros */
#define LOG_STYLE_DATE		1	/* ����"YYYY-MM-DD" */ /* date "YYYY-MM-DD" */
#define LOG_STYLE_DATETIME	2	/* ����ʱ��"YYYY-MM-DD hh:mm:ss" */ /* date time "YYYY-MM-DD hh:mm:ss" */
#define LOG_STYLE_DATETIMEMS	4	/* ����ʱ�����"YYYY-MM-DD hh:mm:ss.6ms"������ʱ����껥�⣬�����Զ�ѡ����Ϣ�����ģ� */ /* date time "YYYY-MM-DD hh:mm:ss.6ms" */
#define LOG_STYLE_LOGLEVEL	8	/* ��־�ȼ� */ /* log level */
#define LOG_STYLE_PID		16	/* ����id */ /* pid */
#define LOG_STYLE_TID		32	/* �߳�id */ /* tid */
#define LOG_STYLE_SOURCE	64	/* "Դ�����ļ���:Դ�����к�" */ /* source file name and row number */
#define LOG_STYLE_FORMAT	128	/* Ӧ����־�� */ /* application text */
#define LOG_STYLE_NEWLINE	256	/* ���� */ /* new line */
#define LOG_STYLE_CUSTLABEL1	512	/* �Զ����ǩ1 */ /* custom label1 */
#define LOG_STYLE_CUSTLABEL2	1024	/* �Զ����ǩ2 */ /* custom label2 */
#define LOG_STYLE_CUSTLABEL3	2048	/* �Զ����ǩ3 */ /* custom label3 */
#define LOG_STYLE_CALLBACK	4096	/* ��ʹ���Զ�������־���ص������� */ /* using custom log style callback function  */

#define LOG_STYLES_DEFAULT	0
//#define LOG_STYLES_DEFAULT	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT )

/* ������ */ /* other macros */
#ifndef MAXLEN_FILENAME
#define MAXLEN_FILENAME		256
#endif

typedef struct tagLOG LOG ;
typedef struct tagLOGBUF LOGBUF ;

/* ������־������� */ /* log handle functions */
_WINDLL_FUNC LOG *CreateLogHandle();
_WINDLL_FUNC LOG *CopyLogHandle( LOG *p_g );
_WINDLL_FUNC void DestroyLogHandle( LOG *g );

/* �Զ���򿪡�������ر���־�������� */ /* custom open , write , close log callback functions */
typedef int funcOpenLog( LOG *g , char *log_pathfilename , void **open_handle );
typedef int funcWriteLog( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen );
typedef int funcChangeTest( LOG *g , void **test_handle );
typedef int funcCloseLog( LOG *g , void **open_handle );

#define LOG_NO_OUTPUTFUNC	NULL , NULL , NULL , NULL , NULL , NULL

/* �Զ�������־��������� */ /* custom log style callback functions */
typedef int funcLogStyle( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );

#define LOG_NO_STYLEFUNC	NULL

#define func_log_style		funcLogStyle

/* ����������ú��� */ /* set log handle environment */
_WINDLL_FUNC int SetLogOutput( LOG *g , int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogOutput2( LOG *g , int output , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally , char *log_pathfilename_format , ... );
_WINDLL_FUNC int SetLogLevel( LOG *g , int log_level );
_WINDLL_FUNC int SetLogStyles( LOG *g , long log_styles , funcLogStyle *pfuncLogStyle );

/* д��־���� */ /* output log */
_WINDLL_FUNC int WriteLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int WriteLogV( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );
_WINDLL_FUNC int PGMLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int DebugLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int OserrLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );

/* дʮ�����ƿ���־���� */ /* output hex log */
_WINDLL_FUNC int WriteHexLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteHexLogV( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist);
_WINDLL_FUNC int PGMHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int OserrHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

/* д��־�����Ŀɱ������ */ /* output log macros */
#define WRITELOG( _g_ , _log_level_ , ... )	WriteLog( _g_ , __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define PGMLOG( _g_ , ... )			PGMLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define DEBUGLOG( _g_ , ... )			DebugLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOG( _g_ , ... )			InfoLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOG( _g_ , ... )			ErrorLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define OSERRLOG( _g_ , ... )			OserrLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );

/* дʮ�����ƿ���־�Ŀɱ������ */ /* output hex log macros */
#define WRITEHEXLOG( _g_ , _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLog( _g_ , __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define PGMHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			PGMHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			DebugHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			InfoHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			ErrorHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			FatalHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );

#endif

/* ����滻�� */
#define create_log_handle	CreateLogHandle
#define destroy_log_handle	DestroyLogHandle

#define func_open_log		funcOpenLog
#define func_write_log		funcWriteLog
#define func_change_test	funcChangeTest
#define func_close_log		funcCloseLog

#define set_log_output		SetLogOutput
#define set_log_output2		SetLogOutput2
#define set_log_level		SetLogLevel
#define set_log_styles		SetLogStyles

#define write_log		WriteLog
#define pgm_log			PGMLog
#define debug_log		DebugLog
#define info_log		InfoLog
#define error_log		ErrorLog
#define oserr_log		OserrLog

#define write_hex_log		WriteHexLog
#define pgm_hex_log		PGMHexLog
#define debug_hex_log		DebugHexLog
#define info_hex_log		InfoHexLog
#define error_hex_log		ErrorHexLog
#define oserr_hex_log		OserrHexLog

/********************************************************/
/* ����Ϊ�߼����� The following is a advanced features  */
/********************************************************/

/* �Զ�������־�ȼ��ص��������� */ /* custom check log level callback function */
typedef int funcFilterLog( LOG *g , void **open_handle , int log_level , char *buf , long len );

#define func_filter_log		funcFilterLog

/* �Զ����ǩ���� */ /* custom tag amount */
#define LOG_MAXCNT_CUST_LABEL		3
/* �Զ����ǩ��󳤶� */ /* custom tag maximum length */
#define LOG_MAXLEN_CUST_LABEL		32

/* ��־ѡ�� */ /* log options */
#define LOG_OPTION_OPEN_AND_CLOSE		1	/* ÿ�ζ�����־��д��־���ر���־ */ /* open , write , close log every time */
#define LOG_OPTION_CHANGE_TEST			2	/* ����ļ��䶯 */ /* detect log changed and reopen it */
#define LOG_OPTION_OPEN_ONCE			4	/* ��־��һ�β��ر� */ /* open log once */
#define LOG_OPTION_SET_OUTPUT_BY_FILENAME	8	/* �Զ������ļ�������������� */ /* reset log output type automatically */
							/* "#stdout" -> LOG_OUTPUT_STDOUT */
							/* "#stderr" -> LOG_OUTPUT_STDERR */
							/* "#syslog" -> LOG_OUTPUT_SYSLOG */
#define LOG_OPTION_FILENAME_APPEND_DOT_LOG	16	/* ��־����ļ������Զ�����".log" */ /* append ".log" */

#define LOG_OPTION_OPEN_DEFAULT			LOG_OPTION_CHANGE_TEST

/* ��־ת��ģʽ */ /* log rotate mode */
#define LOG_ROTATEMODE_NONE		0	/* ��ת�� */ /* no rotate */
#define LOG_ROTATEMODE_SIZE		1	/* ����־�ļ���Сת�����ͺ���SetLogRotateSize���ʹ�ã�ת���ļ�����ʽ"ԭ��־�ļ���.���" */ /* according to log file size turn log */
#define LOG_ROTATEMODE_PER_DAY		2	/* ��ÿ��ת����ת���ļ�����ʽ"ԭ��־�ļ���.����������������" */ /* according to daily turn log */
//#define LOG_ROTATEMODE_PER_HOUR	3	/* ��Сʱת����ת���ļ�����ʽ"ԭ��־�ļ���.����������������_Сʱ" */ /* according to hours turn log  */

/* ��־ת��ȱʡֵ */ /* log rotate default macros */
#define LOG_ROTATE_SIZE_FILE_COUNT_DEFAULT		9999
#define LOG_ROTATE_SIZE_PRESSURE_FACTOR_DEFAULT		0	
#define LOG_FSYNC_PERIOD				1000000

// ������־дģʽ
#define LOG_WRITEMODE_ONE		0
#define LOG_WRITEMODE_MULTI_FIRST	1	// ��һ��
#define LOG_WRITEMODE_MULTI_Middle	2	// �м�
//#define LOG_WRITEMODE_MULTI_LAST	3	// ���һ��

#define LOG_ROTATE_SKIP_COUNT_MAX	1000

/* �Զ�����־ת��ǰ��ص��������� */ /* custom turn log file callback function */
typedef int funcBeforeRotateFile( LOG *g , char *rotate_log_pathfilename );
typedef int funcAfterRotateFile( LOG *g , char *rotate_log_pathfilename );

#define func_before_rotate_file		funcBeforeRotateFile
#define func_after_rotate_file		funcAfterRotateFile

/* �Զ��嶨ʱ��ʼ���ص��������� */ /* timing init callback function */
#define LOG_TIMING_TIME_DEFAULT		5
typedef int funcTimingInit( LOG *g , int output);
#define func_timint_init		funcTimingInit

// ���ö�����־��ʼ
int SetMultiLogBegin(LOG *g);

// ���ö�����־����
int SetMultiLogEnd(LOG *g);

/* ��������Сȱʡֵ */ /* default buffer size */
#define LOG_BUFSIZE_DEFAULT		(64*1024)	/* ȱʡ��־��������С */
#define LOG_BUFSIZE_MAX			(512*1024)	/* �����־��������С */

/* �߼�����������ú��� */ /* senior handle environment setting function */
_WINDLL_FUNC int SetLogOptions( LOG *g , int log_options );
_WINDLL_FUNC int SetLogFileChangeTest( LOG *g , long interval );
_WINDLL_FUNC int SetLogFsyncPeriod( LOG *g , long period );
_WINDLL_FUNC int SetLogCustLabel( LOG *g , int index , char *cust_label );
_WINDLL_FUNC int SetLogRotateMode( LOG *g , int rotate_mode );
_WINDLL_FUNC int SetLogRotateSize( LOG *g , long log_rotate_size );
_WINDLL_FUNC int SetLogRotatePressureFactor( LOG *g , long pressure_factor );
_WINDLL_FUNC int SetLogRotateFileCount( LOG *g , long rotate_file_count );
_WINDLL_FUNC int SetBeforeRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetAfterRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetFilterLogFunc( LOG *g , funcFilterLog *pfuncFilterLog );
_WINDLL_FUNC int SetLogBufferSize( LOG *g , long log_bufsize , long max_log_bufsize );
_WINDLL_FUNC int SetLogOutputFuncDirectly( LOG *g , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogStyleFuncDirectly( LOG *g , funcLogStyle *pfuncLogStyle );
_WINDLL_FUNC int SetTimingInitFunc( LOG *g , long timingtime, funcTimingInit *pfuncTimingInit );
_WINDLL_FUNC int TimintInit(LOG *g);

/* ����滻�� */
#define set_log_options			SetLogOptions
#define set_log_file_change_test	SetLogFileChangeTest
#define set_log_fsync_period		SetLogFsyncPeriod
#define set_log_cust_label		SetLogCustLabel
#define set_log_rotate_mode		SetLogRotateMode
#define set_log_rotate_size		SetLogRotateSize
#define set_log_rotate_pressure_factor	SetLogRotatePressureFactor
#define set_log_rotate_file_count	SetLogRotateFileCount
#define set_before_rotate_file_func	SetBeforeRotateFileFunc
#define set_after_rotate_file_func	SetAfterRotateFileFunc
#define set_filter_log_func		SetFilterLogFunc
#define set_log_buffer_size		SetLogBufferSize
#define set_hex_log_buffer_size		SetHexLogBufferSize
#define set_log_output_func_directly	SetLogOutputFuncDirectly
#define set_log_stlye_func_directly	SetLogStyleFuncDirectly

//#define TEST_LOGLEVEL_NOTENOUGH(_log_level_,_g_)	( (_log_level_) < (_g_)->log_level )
#define TEST_LOGLEVEL_NOTENOUGH(_log_level_,_g_)	( (_log_level_) > (_g_)->log_level )
#define TEST_ATTRIBUTE(_param_,_attr_)			( ( _param_ & _attr_ ) == _attr_ )
#define TEST_NOT_ATTRIBUTE(_param_,_attr_)		( ( _param_ & _attr_ ) != _attr_ )
#define TEST_STDSTREAM(_fd_)				( (_fd_) != FD_NULL && ( (_fd_) == STDOUT_HANDLE || (_fd_) == STDERR_HANDLE ) )
#define TEST_FILEFD(_fd_)				( (_fd_) != FD_NULL && (_fd_) != STDOUT_HANDLE && (_fd_) != STDERR_HANDLE )

/********************************************************/
/* ����Ϊ���ӿ��� the following development for the hook*/
/********************************************************/

/* ƽ̨�� */ /* platfrom macros */
#if ( defined _WIN32 )
#define LOG_NEWLINE		"\r\n"
#define LOG_NEWLINE_LEN		2
#define SNPRINTF		_snprintf
#define VSNPRINTF		_vsnprintf
#define STDOUT_HANDLE		1
#define STDERR_HANDLE		2
#define FD_NULL			-1
#define WRITE			_write
#define _STAT			_stat
#define ACCESS			_access
#define ACCESS_OK		00
#define RENAME			rename
#define UNLINK			unlink
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#define LOG_NEWLINE		"\n"
#define LOG_NEWLINE_LEN		1
#define SNPRINTF		snprintf
#define VSNPRINTF		vsnprintf
#define STDOUT_HANDLE		STDOUT_FILENO
#define STDERR_HANDLE		STDERR_FILENO
#define FD_NULL			-1
#define WRITE			write
#define _STAT			stat
#define ACCESS			access
#define ACCESS_OK		F_OK
#define RENAME			rename
#define UNLINK			unlink
#endif



/* ����� */ /* code macros */
#if ( defined _WIN32 )
#define SYSTEMTIME2TIMEVAL_USEC(_syst_,_tv_) \
		(_tv_).tv_usec = (_syst_).wMilliseconds * 1000 ;
#define SYSTEMTIME2TM(_syst_,_stime_) \
		(_stime_).tm_year = (_syst_).wYear - 1900 ; \
		(_stime_).tm_mon = (_syst_).wMonth - 1 ; \
		(_stime_).tm_mday = (_syst_).wDay ; \
		(_stime_).tm_hour = (_syst_).wHour ; \
		(_stime_).tm_min = (_syst_).wMinute ; \
		(_stime_).tm_sec = (_syst_).wSecond ;
#define LOCALTIME(_tt_,_stime_) \
	{ \
		SYSTEMTIME	stNow ; \
		GetLocalTime( & stNow ); \
		SYSTEMTIME2TM( stNow , (_stime_) ); \
	}
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#define LOCALTIME(_tt_,_stime_) \
	localtime_r(&(_tt_),&(_stime_));
#endif


#ifndef STRCMP
#define STRCMP(_a_,_C_,_b_) ( strcmp(_a_,_b_) _C_ 0 )
#define STRNCMP(_a_,_C_,_b_,_n_) ( strncmp(_a_,_b_,_n_) _C_ 0 )
#endif

#ifndef STRICMP
#if ( defined _WIN32 )
#define STRICMP(_a_,_C_,_b_) ( stricmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strnicmp(_a_,_b_,_n_) _C_ 0 )
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#define STRICMP(_a_,_C_,_b_) ( strcasecmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strncasecmp(_a_,_b_,_n_) _C_ 0 )
#endif
#endif

#define OFFSET_BUFPTR(_logbuf_,_offset_len_) \
	if( _offset_len_ != -1 ) \
	{ \
		(_logbuf_)->bufptr += _offset_len_ ; \
		(_logbuf_)->buf_remain_len -= _offset_len_ ; \
	}

_WINDLL_FUNC int SetOpenFlag( LOG *g , char open_flag );
_WINDLL_FUNC char IsLogOpened( LOG *g );

_WINDLL_FUNC int GetLogLevel( LOG *g );

_WINDLL_FUNC LOGBUF *GetLogBuffer( LOG *g );

_WINDLL_FUNC long FormatLogBuffer( LOG *g , LOGBUF *logbuf , char *format , ... );
_WINDLL_FUNC long FormatLogBufferV( LOG *g , LOGBUF *logbuf , char *format , va_list valist );
_WINDLL_FUNC long MemcatLogBuffer( LOG *g , LOGBUF *logbuf , char *append , long len );

#define set_open_flag		SetOpenFlag
#define is_log_opened		IsLogOpened

#define get_log_level		GetLogLevel

#define get_log_buffer		GetLogBuffer
#define get_hex_log_buffer	GetHexLogBuffer

#define format_log_buffer	FormatLogBuffer
#define format_log_buffer_v	FormatLogBufferV
#define memcat_log_buffer	MemcatLogBuffer

// MDC
void mdc_put(char *mdc);
char *mdc_get();

/* ��־�������ṹ */ /* log buffer structure */
struct tagLOGBUF
{
	long			max_buf_size ;
	long			buf_size ;
	char			*bufbase ;
	char			*bufptr ;
	long			buf_remain_len ;
} ;

/* ��־����ṹ */ /* log handle structure */
struct tagLOG
{
	/* ���� */ /* basic */
	int			output ;
	char			log_basefilename[ MAXLEN_FILENAME + 1 ];
	char			log_pathfilename[ MAXLEN_FILENAME + 1 ];
	funcOpenLog		*pfuncOpenLogFirst ;
	funcOpenLog		*pfuncOpenLog ;
	funcWriteLog		*pfuncWriteLog ;
	funcChangeTest		*pfuncChangeTest ;
	funcCloseLog		*pfuncCloseLog ;
	funcCloseLog		*pfuncCloseLogFinally ;
	void			*open_handle ;
	void			*test_handle ;
	
	int			fd ;
#if ( defined _WIN32 )
	HANDLE			hFile ;
#endif
	
	char			open_flag ;
	
	/* ��־�ȼ� */ /* log level */
	int			log_level ;
	
	funcFilterLog		*pfuncFilterLog ;
	
	/* ����־��� */ /* log style */
	long			log_styles ;
	funcLogStyle		*pfuncLogStyle ;
	/* �����и�ʽ��񣬹�����ʽ�������飬����д��־ʱ�����������־������ */
	funcLogStyle		*pfuncLogStyles[ 30 + 1 ] ;
	int			style_func_count ;
	
	/* ��־ѡ�� */ /* log options */
	int			log_options ;
	
	long			file_change_test_interval ;
	long			file_change_test_no ;
	struct _STAT		file_change_stat ;
	long			fsync_period ;
	long			fsync_elapse ;
	
	/* �Զ����ǩ */ /* custom labels */
	char			cust_label[LOG_MAXCNT_CUST_LABEL][ LOG_MAXLEN_CUST_LABEL + 1 ] ;
	
	/* ��־ת�� */ /* log rotate */
	int			rotate_mode ;
	long			log_rotate_size ;
	long			rotate_file_no ;
	long			rotate_file_count ;
	long			pressure_factor ;
	
	long			skip_count ;
	
	funcAfterRotateFile	*pfuncAfterRotateFile ;
	funcBeforeRotateFile	*pfuncBeforeRotateFile ;
	
	// ��ʱ��ʼ���ص�����
	long			timingtime;
	funcTimingInit		*pfuncTimingInit;
	
	/* ����־������ */ /* log buffer */
	int			multi_mode ;
	int			write_flag;
	int			error_flag;
	LOGBUF			logbuf;
	
	/* һ������ */ /* level 1 cache */
	struct timeval		cache1_tv ;
	struct tm		cache1_stime ;
	
	/* �������� */ /* level 2 cache */
	struct timeval		cache2_logstyle_tv ;
	struct tm		cache2_logstyle_stime ;
	char			cache2_logstyle_date_buf[ 10 + 1 ] ;
	long			cache2_logstyle_date_buf_len ;
	char			cache2_logstyle_datetime_buf[ 19 + 1 ] ;
	long			cache2_logstyle_datetime_buf_len ;
	
	unsigned long		cache2_logstyle_pid ;
	char			cache2_logstyle_pid_buf[ 20 + 1 ] ;
	long			cache2_logstyle_pid_buf_len ;
	
	unsigned long		cache2_logstyle_tid ;
	char			cache2_logstyle_tid_buf[ 20 + 1 ] ;
	long			cache2_logstyle_tid_buf_len ;
	
	struct timeval		cache2_rotate_tv ;
	struct tm		cache2_rotate_stime ;

	struct timeval		cache2_init_tv ;
	struct tm		cache2_init_stime ;
	
	/* ת���ļ��� */ /* rotate file lock */
#if ( defined _WIN32 )
	HANDLE			rotate_lock ;
#elif ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	int			rotate_lock ;
	struct flock		lock ;
#endif
} ;

/********************************************************/
/* ����Ϊ�����ӿ� the following for the assist interface*/
/********************************************************/

/* ���ø������� */ /* config assist functions */
_WINDLL_FUNC int ConvertLogOutput_atoi( char *output_desc , int *p_log_output );
_WINDLL_FUNC int ConvertLogLevel_atoi( char *log_level_desc , int *p_log_level );
_WINDLL_FUNC int ConvertLogLevel_itoa( int log_level , char **log_level_desc );
_WINDLL_FUNC int ConvertLogStyle_atol( char *line_style_desc , long *p_line_style );
_WINDLL_FUNC int ConvertLogOption_atol( char *log_option_desc , long *p_log_option );
_WINDLL_FUNC int ConvertLogRotateMode_atoi( char *rotate_mode_desc , int *p_rotate_mode );
_WINDLL_FUNC int ConvertBufferSize_atol( char *bufsize_desc , long *p_bufsize );

void SetErrorLogAloneFile();

#define convert_log_output_atoi		ConvertLogOutput_atoi
#define convert_log_level_atoi		ConvertLogLevel_atoi
#define convert_log_level_itoa		ConvertLogLevel_itoa
#define convert_log_style_atol		ConvertLogStyle_atol
#define cnovert_log_option_atol		ConvertLogOption_atol
#define convert_log_rotate_mode_atoi	ConvertLogRotateMode_atoi
#define convert_buffer_size_atol	ConvertBufferSize_atol

#ifdef __cplusplus
}
#endif

#endif


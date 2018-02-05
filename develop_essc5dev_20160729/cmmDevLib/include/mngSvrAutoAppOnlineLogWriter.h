// Author:	王纯军
// Date:	2010-6-2

#ifndef _mngSvrAutoAppOnlineLogWriter_
#define _mngSvrAutoAppOnlineLogWriter_

#include "unionOperationAuthorization.h"

// 定义一个自动流水表的域赋值方法标识
typedef enum
{
	conAutoAppOnlineLogFldMethodReadReqStrFld = 0,	// 读请求串的域
	conAutoAppOnlineLogFldMethodReadResStrFld = 1,	// 读响应串的域
	conAutoAppOnlineLogFldMethodReadTableFld = 2,	// 读表的域
} TAutoAppOnlineLogFldAssignMethod;

// 定义一个域赋值方法
typedef struct
{
	char					desFldName[40+1];	// 流水表的域名称
	char					oriFldName[40+1];	// 源域名称
	TAutoAppOnlineLogFldAssignMethod	methodTag;		// 域赋值方法标识
} TUnionAutoAppOnlineLogFldDef;
typedef TUnionAutoAppOnlineLogFldDef	*PUnionAutoAppOnlineLogFldDef;

// 定义流水最大的域数目
#define conMaxFldNumOfAutoAppOnlineLogRec	32
// 定义一个流水自动写结构
typedef struct
{
	int				resID;			// 资源ID
	int				serviceID;		// 操作ID
	char				serviceName[40+1];	// 操作名称
	char				oriTableName[40+1];	// 源表名称
	char				desTableName[40+1];	// 目标表名称
	int				fldNum;			// 域数目
	TUnionAutoAppOnlineLogFldDef	desTableFldGrp[conMaxFldNumOfAutoAppOnlineLogRec];
} TUnionAutoAppOnlineLogWriter;
typedef TUnionAutoAppOnlineLogWriter	*PUnionAutoAppOnlineLogWriter;

/*
功能
	拼装由指定方法形成的记录串
输入参数：
	pwriterDef	方法定义
	methodTag	方法标识
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStr(PUnionAutoAppOnlineLogWriter pwriterDef,int methodTag,char *recStr,int sizeOfBuf);

/*
功能
	拼装记录请求串
输入参数：
	pwriterDef	方法定义
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStrForRequest(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf);

/*
功能
	拼装记录响应串
输入参数：
	pwriterDef	方法定义
	sizeOfBuf	缓冲区大小
输出参数
	recStr		记录串
返回值
	>=0	成功，记录串的长度
	<0	错误码
*/
int UnionFormAutoAppOnlineLogRecStrForResponse(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf);

/*
功能
	从一个串，读取域赋值方法
输入参数：
	fldDefStr	域赋值串
	lenOfFldDefStr	域赋值串长度
输出参数
	pfldDef		域赋值定义
返回值
	>=0	成功,返回域定义的数量
	<0	错误码
*/
int UnionInitAutoAppOnlineLogFldWriterFromDefStr(char *fldDefStr,int lenOfFldDefStr,PUnionAutoAppOnlineLogFldDef pfldDef);

/*
功能
	根据授权记录初始化自动日志生成器
输入参数：
	serviceID	操作代码
输出参数
	pwriterDef	日志生成器定义
返回值
	>=0	成功,返回域的数目
	<0	错误码
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRec(PUnionOperationAuthorization pauthRec,PUnionAutoAppOnlineLogWriter pwriterDef);

#endif


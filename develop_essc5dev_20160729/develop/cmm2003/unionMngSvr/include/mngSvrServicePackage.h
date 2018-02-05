// Author:	Wolfgang Wang
// Date:	2006/8/9

// 这个文件定义了资源编号及资源操作命令

#ifndef _mngSvrServicePackage_
#define _mngSvrServicePackage_

#define conMngSvrPackFldNameTellerNo		"TELLERNO"
#define conMngSvrPackFldNameResID		"RESID"
#define conMngSvrPackFldNameResCmd		"RESCMD"
#define conMngSvrPackFldNameData		"DATA"
#define conMngSvrPackFldNameResponseCode	"RESPONSECODE"

#define conMngSvrRequestPackageFlag		'1'
#define conMngSvrResponsePackageFlag		'0'

// 以下定义资源管理请求命令头
typedef struct
{
	char	flag;				// 请求包标识，固定为1
	char	tellerNo[40+1];			// 操作员ID号
	char	resID[3+1];			// 资源ID号
	char	resCmd[3+1];			// 操作命令
	char	lenOfParameter[4+1];		// 参数的长度
} TUnionResMngReqCmdHeader;
typedef TUnionResMngReqCmdHeader	*PUnionResMngReqCmdHeader;
// 以下定义资源管理响应命令头
typedef struct
{
	char	flag;				// 响应包标识，固定为0
	char	tellerNo[40+1];			// 操作员ID号
	char	resID[3+1];			// 资源ID号
	char	resCmd[3+1];			// 操作命令
	char	responseCode[6+1];		// 响应码，负数表示错误代码
	char	lenOfParameter[4+1];		// 参数的长度
} TUnionResMngResCmdHeader;
typedef TUnionResMngResCmdHeader	*PUnionResMngResCmdHeader;

void UnionLogResMngRequestCmdPackage();

int UnionUnpackResMngRequestPackage(char *dataStr,int lenOfDataStr);

int UnionUnpackResMngResponsePackage(char *dataStr,int lenOfDataStr);

int UnionPackResMngResponsePackage(char *buf,int sizeOfBuf);

int UnionPackResMngRequestPackage(char *buf,int sizeOfBuf);

void UnionLogResMngResponseCmdPackage();

int UnionReadResMngRequestPackageFld(char *name,char *buf,int sizeOfBuf);

int UnionReadResMngResponsePackageFld(char *name,char *buf,int sizeOfBuf);

int UnionSetResMngRequestPackageFld(char *name,char *buf,int len);

int UnionSetResMngResponsePackageFld(char *name,char *buf,int len);

int UnionSetResMngClientIPAddr(char *ipAddr);

int UnionSetResMngClientPort(int port);

char *UnionReadResMngClientIPAddr();

int UnionReadResMngClientPort();

// 解开一个监控包
// dataStr,lenOfDataStr是输入参数，分别对应监控包和其长度
// resID,resCmd,buf,sizeOfBuf是输出参数，分别对应资源ID，资源命令，监控数据，接收监控数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是监控数据的长度。
int UnionUnpackSpierPackage(char *dataStr,int lenOfDataStr,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// 打一个监控包
// resID,resCmd,data,lenOfData是输入参数，分别对应资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是监控数据包的长度。
int UnionPackSpierPackage(char *data,int lenOfData,int resID,int resCmd,char *buf,int sizeOfBuf);

// 打一个管理包
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf);

// 解开一个管理包
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// 解开一个管理响应包
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngSvrResponsePackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,int *responseCode,
			char *buf,int sizeOfBuf);

// 打一个请求管理包
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngSvrRequestPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf);

// 解开一个管理请求包
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngSvrRequestPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// 打一个响应管理包
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// responseCode，是响应码
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngSvrResponsePackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,int responseCode,char *buf,int sizeOfBuf);

// 判断一个包是不是请求包
int UnionIsMngSvrResponsePackage(char *dataStr,int lenOfDataStr);

// 2009/9/30，王纯军增加
// 解开一个管理包头
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// isRequest,tellerNo,resID,resCmd,resID,lenOfData分别对应请求标识、柜员号、资源ID，资源命令，响应码,数据长度
// 返回值是负数，解包出错，否则，是数据头的长度。
int UnionUnpackMngSvrPackageHeader(char *dataStr,int lenOfDataStr,int *isRequest,char *tellerNo,int *resID,int *resCmd,int *resCode,int *lenOfData);

#endif





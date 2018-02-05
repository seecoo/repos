//	Wolfgang Wang, 2008/11/10

#ifndef _mngSvrFileSvr_
#define _mngSvrFileSvr_

/* 功能
	根据请求串，获取文件配置的信息
输入参数
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfBuf	关键字缓冲的大小
输出参数
	dirName		文件存储目录,如果指针不为空
	tableName	表名,如果指针不为空
	fileNameFldName	文件名字段名,如果指针不为空
	primaryKey	关键字串,如果指针不为空
返回值
	>=0	成功，返回关键字串的长度
	<0	失败，错误码
*/
int UnionReadFileConfFromReqStr(char *reqStr,int lenOfReqStr,char *dirName,char *tableName,char *fileNameFldName,char *primaryKey,char *primaryKeyValueStr,int *fileType,int sizeOfBuf);

/* 功能
	接收客户端上传的一个文件
输入参数
	handle		socket句柄
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件返回客户端
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionRecvFileFromClient(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

/* 功能
	文件服务器解释器
输入参数
	handle		socket句柄
	resID		资源标识
	serviceID	命令字
	reqStr	请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件接收到
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionExcuteMngSvrFileSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

#endif

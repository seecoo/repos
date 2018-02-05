#ifndef _PKDBSvrOperation_H_
#define _PKDBSvrOperation_H_

/*
功能：密钥库操作命令函数
输入参数：
	serviceID： 命令标识
	reqStr：请求数据
	lenOfReqStr：请求数据的长度
	sizeOfResStr：resStr的缓冲区大小
输出参数：
	resStr：响应数据
fileRecved：1：表示服务器端发送来一个文件，0：表示没有文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionPKDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

#endif


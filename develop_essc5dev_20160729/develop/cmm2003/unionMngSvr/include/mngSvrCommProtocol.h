//	Wolfgang Wang, 2006/08/08

#ifndef _mngSvrCommProtocol_
#define _mngSvrCommProtocol_

// 设置暂时文件	2010-5-18,王纯军
void UnionSetUserSpecMngSvrTempFileName(char *fileName);

// 重置暂时文件	2010-5-18,王纯军
void UnionResetUserSpecMngSvrTempFileName();

// 王纯军，2009/3/18,增加
// 建立mngSvrClient与后台的通讯连接
int UnionCreateMngSvrClientSckHDL(char *ipAddr,int port);

// 王纯军，2009/3/18,增加
// 关闭mngSvrClient与后台的通讯连接
int UnionCloseMngSvrClientSckHDL(int sckHDL);

// 王纯军，2009/3/18,增加
// 设置mngSvrClient与后台使用长连接通讯
void UnionSetMngSvrClientUseLongConn();

// 王纯军，2009/3/18,增加
// 设置mngSvrClient与后台使用短连接通讯
void UnionSetMngSvrClientUseShortConn();

// 2007/12/22 增加
// 判断是否是ESSC返回失败
int UnionIsMngSvrReturnedError();

// 2007/12/22 增加
// 获取mngSvr返回的错误码
char *UnionGetMngSvrErrorInfo();

// 生成一个暂时文件，返回其文件名称
char *UnionGenerateMngSvrTempFile();

// 获取当前暂时文件的名称
char *UnionGetCurrentMngSvrTempFileName();

int UnionDeleteAllMngSvrTempFile();

// 删除暂时文件
int UnionDeleteMngSvrTempFile();

// mngSvr服务器端向客户端发送一个文件
int UnionMngSvrTransferFile(int sckHDL,char *fileName);

// 在服务器和客户端之间，发送一个数据文件
int UnionTransferMngDataFile(int sckHDL,char *fileName,char *tellerNo,int resID);

// 客户端获取服务器的通讯配置
int UnionGetConfOfMngSvr(char *ipAddr,int *port);

// 2009/5/29，王纯军增加，从UnionCommunicationWithSpecMngSvr中折分出这个函数
// 与MngSvr通讯
// reqStr,lenOfReqStr是输入参数请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionExchangeInfoWithSpecMngSvr(char *ipAddr,int port,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// 与MngSvr通讯，客户端使用
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了暂时文件
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionCommunicationWithMngSvr(char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// 2007/12/22,增加
// 与指定的MngSvr通讯，客户端使用
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了暂时文件
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionCommunicationWithSpecMngSvr(char *ipAddr,int port,char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved);
		
// 客户端从服务器申请下载一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadFile(char *tellerNo,int resID,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// 客户端从服务器申请下载一个指定名称的文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// fileName，要下载的文件名称
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadSpecFile(char *tellerNo,int resID,char *fileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// 客户端从服务器申请下载一个指定名称的文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// svrFileName，要下载的文件名称
// localFileName，下载的文件在本地的存储名称
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadSpecFileToLocalFile(char *tellerNo,int resID,char *svrFileName,char *localFileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// 客户端向服务器上传一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileName,是输入参数，是要传输的文件名。
// 返回值是resStr中数据的长度
int UnionMngClientUploadFile(char *fileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr);

// 客户端向服务器上传一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileName,是输入参数，是要传输的文件名。
// svrFileName，输入参数，文件在服务器端的名称
// 返回值是resStr中数据的长度
int UnionMngClientUploadFileWithSvrFileName(char *fileName,char *svrFileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr);
		
// MngSvr从客户端接收数据文件
// fileName是接收数据要覆盖的文件
int UnionMngSvrRecvFileFromClient(int sckHDL,char *overwrittenFileName);

int UnionDeleteSelfTempFile();

void UnionFormSelfTempFileName(char *fileName);

#endif

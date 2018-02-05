//	Wolfgang Wang, 2004/7/23

#ifndef _WIN32_
#ifdef _AIX_
#define _LARGE_FILES
#else
#define _FILE_OFFSET_BITS 64
#endif
//typedef __int64	off_t
#endif

#ifndef _unionFileTransfer_
#define _unionFileTransfer_

#define conFileReceiverCmdTagDownloadFile       1       // 从文件服务器下载文件
#define conFileReceiverCmdTagUpLoadFile         2       //上传文件
#define conFileReceiverCmdTagListFile           3       //
#define conFileReceiverCmdTagCheck              4
#define conFileReceiverCmdTagMoveFile           5
#define conFileReceiverCmdTagExcuteFile         6
typedef struct
{
	int				port;					// 使用的端口
	int				socket;
} TUnionFileReceiveServer;
typedef TUnionFileReceiveServer		*PUnionFileReceiveServer;

typedef struct
{
	// 通讯参数
	PUnionFileReceiveServer		preceiveServer;	// 接收者使用的文件接收服务器；
	char	ipAddrOfCaller[15+1];			// 创建连接者的IP地址
	char	ipAddrOfListener[15+1];			// 接收连接者的IP地址
	int	port;					// 传输使用的端口号
	int	socket;					// Socket 标识
	
	// 文件参数
	char	originFileName[128];			// 发送者一端的文件名称
	char	originDir[256];				// 发送者一端的文件目录
	char	destinitionFileName[128];		// 接收者一端的文件名称
	char	destinitionDir[256];			// 接收者一端的文件目录
	long long	totalFileLength;			// 文件总长度
	long	totalNum;				// 规划传输的报文总数
	long	sizePerTransfer;			// 每个报文传输的长度
	char	checkValue[40+1];			// 文件的校验值
	
	// 文件传输动态参数
	long long lengthTransferred;			// 尚未传输的长度
	long	numTransferred;				// 尚未传输的报文数目
	time_t	startTime,finishTime,currentTime;	// 开始传输的时间，结束传输的时间，当前时间
} TUnionFileTransferHandle;
typedef TUnionFileTransferHandle	*PUnionFileTransferHandle;

/*
功能
	使用引擎发送一个文件
输入参数
	desIPAddr	接收者的IP地址
	desPort		接收者的端口号
	oriDir		源文件目录
	oriFile		源文件名
	desDir		目标文件目录，如果为空，则目标文件目录由对端指定
	desFile		目标文件名，如果为空，则目标文件名与源文件名相同
输出参数
	无
返回值
	>=0		成功
	<0		失败，错误码
*/
int UnionExcuteFileAtFileReceiverByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

/*
 功能
 	执行一个文件
 输入参数
 	phdl	传输引擎指针
 输出参数
 	无
 返回值
 	>=0	成功
 	<0	错误码
*/
int UnionExecuteFileSpecBySpecTransferHandle(PUnionFileTransferHandle phdl);

/*
功能
	将一个文件传输句柄写入到串中
输入参数
	phdl		句柄
	sizeOfRecStr	串缓冲的大小
输出参数
	recStr		串
返回值
	>=0		成功，串长度
	<0		失败，错误码
*/
int UnionPutFileTransferHandleIntoRecStr(PUnionFileTransferHandle phdl,char *recStr,int sizeOfRecStr);

// 呼叫创建一个文件接收连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// oriFileName是要发送端的文件的名称，是输入参数; oriDir是发送端的文件的目录
// desDir是接收端的文件目录，desFileName是接收端的文件名称
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileReceiverHandleForSpecCmd(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName,int receiverCmd);

/*
功能
	从一个串中读取文件传输句柄
输入参数
	recStr		串
	lenOfRecStr	串长度
输出参数
	phdl		句柄
返回值
	>=0		成功，串长度
	<0		失败，错误码
*/
int UnionReadFileTransferHandleFromRecStr(char *recStr,int lenOfRecStr,PUnionFileTransferHandle phdl);

// 设置成文件接收者
void UnionSetAsFileReceiver();

// 设置成非文件接收者
void UnionSetAsNoneFileReceiver();

// 设置成非文件接收者
int UnionIsFileReceiver();

// 呼叫创建一个文件接收连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// oriFileName是要发送端的文件的名称，是输入参数; oriDir是发送端的文件的目录
// desDir是接收端的文件目录，desFileName是接收端的文件名称
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileReceiverHandle(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName);

#ifdef _unionFileTransfer_2_x_
// 呼叫创建一个文件传输连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// fileName是要传输的文件的名称，是输入参数; dir是要传输的文件的目录
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName,char *desDir,char *desFileName);
#else
// 呼叫创建一个文件传输连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// fileName是要传输的文件的名称，是输入参数; dir是要传输的文件的目录
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName);

#endif

/*
 功能
 	设置文件传输属性
 输入参数
 	phdl	传输引擎指针
 输出参数
 	无
 返回值
 	>=0	成功
 	<0	错误码
*/
int UnionSetTransAttrForTransferHandle(PUnionFileTransferHandle phdl);

/*
功能
	使用引擎发送一个文件
输入参数
	desIPAddr	接收者的IP地址
	desPort		接收者的端口号
	oriDir		源文件目录
	oriFile		源文件名
	desDir		目标文件目录，如果为空，则目标文件目录由对端指定
	desFile		目标文件名，如果为空，则目标文件名与源文件名相同
输出参数
	无
返回值
	>=0		成功
	<0		失败，错误码
*/
int UnionSendFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

// 初始化一个文件接收服务器名柄
PUnionFileReceiveServer UnionInitFileReceiveServer(int port);

// 释放文件接收服务器
int UnionReleaseFileReceiveServer(PUnionFileReceiveServer);

// 接收呼叫创建一个文件传输连接
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionAcceptFileTransferCall(int socket_fd,char *ipAddr,PUnionFileReceiveServer preceiveServer);

// 释放文件传输句柄
// 失败返回负值
int UnionReleaseFileTransferHandle(PUnionFileTransferHandle phdl);


// 传输一个文件，phdl是使用的传输句柄
int UnionTransferFile(PUnionFileTransferHandle phdl);

// 接收一个文件，phdl是使用的传输句柄
int UnionReceiveFile(PUnionFileTransferHandle phdl);

// 同意接收文件，并为文件指定接收的名称和目录
// < 0 不同意接收
int UnionAgreeToAccepteFileTransferred(PUnionFileTransferHandle phdl);

/*
功能
	使用引擎发送一个文件
输入参数
	desIPAddr	接收者的IP地址
	desPort		接收者的端口号
	oriDir		源文件目录
	oriFile		源文件名
	desDir		目标文件目录，如果为空，则目标文件目录由对端指定
	desFile		目标文件名，如果为空，则目标文件名与源文件名相同
输出参数
	无
返回值
	>=0		成功
	<0		失败，错误码
*/
int UnionReceiveFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

int UnionFileTransferService(PUnionFileTransferHandle phdl);

#endif

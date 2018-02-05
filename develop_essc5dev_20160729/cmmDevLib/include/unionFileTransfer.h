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

#define conFileReceiverCmdTagDownloadFile       1       // ���ļ������������ļ�
#define conFileReceiverCmdTagUpLoadFile         2       //�ϴ��ļ�
#define conFileReceiverCmdTagListFile           3       //
#define conFileReceiverCmdTagCheck              4
#define conFileReceiverCmdTagMoveFile           5
#define conFileReceiverCmdTagExcuteFile         6
typedef struct
{
	int				port;					// ʹ�õĶ˿�
	int				socket;
} TUnionFileReceiveServer;
typedef TUnionFileReceiveServer		*PUnionFileReceiveServer;

typedef struct
{
	// ͨѶ����
	PUnionFileReceiveServer		preceiveServer;	// ������ʹ�õ��ļ����շ�������
	char	ipAddrOfCaller[15+1];			// ���������ߵ�IP��ַ
	char	ipAddrOfListener[15+1];			// ���������ߵ�IP��ַ
	int	port;					// ����ʹ�õĶ˿ں�
	int	socket;					// Socket ��ʶ
	
	// �ļ�����
	char	originFileName[128];			// ������һ�˵��ļ�����
	char	originDir[256];				// ������һ�˵��ļ�Ŀ¼
	char	destinitionFileName[128];		// ������һ�˵��ļ�����
	char	destinitionDir[256];			// ������һ�˵��ļ�Ŀ¼
	long long	totalFileLength;			// �ļ��ܳ���
	long	totalNum;				// �滮����ı�������
	long	sizePerTransfer;			// ÿ�����Ĵ���ĳ���
	char	checkValue[40+1];			// �ļ���У��ֵ
	
	// �ļ����䶯̬����
	long long lengthTransferred;			// ��δ����ĳ���
	long	numTransferred;				// ��δ����ı�����Ŀ
	time_t	startTime,finishTime,currentTime;	// ��ʼ�����ʱ�䣬���������ʱ�䣬��ǰʱ��
} TUnionFileTransferHandle;
typedef TUnionFileTransferHandle	*PUnionFileTransferHandle;

/*
����
	ʹ�����淢��һ���ļ�
�������
	desIPAddr	�����ߵ�IP��ַ
	desPort		�����ߵĶ˿ں�
	oriDir		Դ�ļ�Ŀ¼
	oriFile		Դ�ļ���
	desDir		Ŀ���ļ�Ŀ¼�����Ϊ�գ���Ŀ���ļ�Ŀ¼�ɶԶ�ָ��
	desFile		Ŀ���ļ��������Ϊ�գ���Ŀ���ļ�����Դ�ļ�����ͬ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ�ܣ�������
*/
int UnionExcuteFileAtFileReceiverByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

/*
 ����
 	ִ��һ���ļ�
 �������
 	phdl	��������ָ��
 �������
 	��
 ����ֵ
 	>=0	�ɹ�
 	<0	������
*/
int UnionExecuteFileSpecBySpecTransferHandle(PUnionFileTransferHandle phdl);

/*
����
	��һ���ļ�������д�뵽����
�������
	phdl		���
	sizeOfRecStr	������Ĵ�С
�������
	recStr		��
����ֵ
	>=0		�ɹ���������
	<0		ʧ�ܣ�������
*/
int UnionPutFileTransferHandleIntoRecStr(PUnionFileTransferHandle phdl,char *recStr,int sizeOfRecStr);

// ���д���һ���ļ���������
// ipAddr,port������������ֱ��Ӧ�ļ����䷽��IP��ַ�ʹ���ʹ�õĶ˿�
// oriFileName��Ҫ���Ͷ˵��ļ������ƣ����������; oriDir�Ƿ��Ͷ˵��ļ���Ŀ¼
// desDir�ǽ��ն˵��ļ�Ŀ¼��desFileName�ǽ��ն˵��ļ�����
// ����ֵΪ�ļ���������NULL����ʾ����ʧ��
PUnionFileTransferHandle UnionCreateFileReceiverHandleForSpecCmd(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName,int receiverCmd);

/*
����
	��һ�����ж�ȡ�ļ�������
�������
	recStr		��
	lenOfRecStr	������
�������
	phdl		���
����ֵ
	>=0		�ɹ���������
	<0		ʧ�ܣ�������
*/
int UnionReadFileTransferHandleFromRecStr(char *recStr,int lenOfRecStr,PUnionFileTransferHandle phdl);

// ���ó��ļ�������
void UnionSetAsFileReceiver();

// ���óɷ��ļ�������
void UnionSetAsNoneFileReceiver();

// ���óɷ��ļ�������
int UnionIsFileReceiver();

// ���д���һ���ļ���������
// ipAddr,port������������ֱ��Ӧ�ļ����䷽��IP��ַ�ʹ���ʹ�õĶ˿�
// oriFileName��Ҫ���Ͷ˵��ļ������ƣ����������; oriDir�Ƿ��Ͷ˵��ļ���Ŀ¼
// desDir�ǽ��ն˵��ļ�Ŀ¼��desFileName�ǽ��ն˵��ļ�����
// ����ֵΪ�ļ���������NULL����ʾ����ʧ��
PUnionFileTransferHandle UnionCreateFileReceiverHandle(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName);

#ifdef _unionFileTransfer_2_x_
// ���д���һ���ļ���������
// ipAddr,port������������ֱ��Ӧ�ļ����䷽��IP��ַ�ʹ���ʹ�õĶ˿�
// fileName��Ҫ������ļ������ƣ����������; dir��Ҫ������ļ���Ŀ¼
// ����ֵΪ�ļ���������NULL����ʾ����ʧ��
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName,char *desDir,char *desFileName);
#else
// ���д���һ���ļ���������
// ipAddr,port������������ֱ��Ӧ�ļ����䷽��IP��ַ�ʹ���ʹ�õĶ˿�
// fileName��Ҫ������ļ������ƣ����������; dir��Ҫ������ļ���Ŀ¼
// ����ֵΪ�ļ���������NULL����ʾ����ʧ��
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName);

#endif

/*
 ����
 	�����ļ���������
 �������
 	phdl	��������ָ��
 �������
 	��
 ����ֵ
 	>=0	�ɹ�
 	<0	������
*/
int UnionSetTransAttrForTransferHandle(PUnionFileTransferHandle phdl);

/*
����
	ʹ�����淢��һ���ļ�
�������
	desIPAddr	�����ߵ�IP��ַ
	desPort		�����ߵĶ˿ں�
	oriDir		Դ�ļ�Ŀ¼
	oriFile		Դ�ļ���
	desDir		Ŀ���ļ�Ŀ¼�����Ϊ�գ���Ŀ���ļ�Ŀ¼�ɶԶ�ָ��
	desFile		Ŀ���ļ��������Ϊ�գ���Ŀ���ļ�����Դ�ļ�����ͬ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ�ܣ�������
*/
int UnionSendFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

// ��ʼ��һ���ļ����շ���������
PUnionFileReceiveServer UnionInitFileReceiveServer(int port);

// �ͷ��ļ����շ�����
int UnionReleaseFileReceiveServer(PUnionFileReceiveServer);

// ���պ��д���һ���ļ���������
// ����ֵΪ�ļ���������NULL����ʾ����ʧ��
PUnionFileTransferHandle UnionAcceptFileTransferCall(int socket_fd,char *ipAddr,PUnionFileReceiveServer preceiveServer);

// �ͷ��ļ�������
// ʧ�ܷ��ظ�ֵ
int UnionReleaseFileTransferHandle(PUnionFileTransferHandle phdl);


// ����һ���ļ���phdl��ʹ�õĴ�����
int UnionTransferFile(PUnionFileTransferHandle phdl);

// ����һ���ļ���phdl��ʹ�õĴ�����
int UnionReceiveFile(PUnionFileTransferHandle phdl);

// ͬ������ļ�����Ϊ�ļ�ָ�����յ����ƺ�Ŀ¼
// < 0 ��ͬ�����
int UnionAgreeToAccepteFileTransferred(PUnionFileTransferHandle phdl);

/*
����
	ʹ�����淢��һ���ļ�
�������
	desIPAddr	�����ߵ�IP��ַ
	desPort		�����ߵĶ˿ں�
	oriDir		Դ�ļ�Ŀ¼
	oriFile		Դ�ļ���
	desDir		Ŀ���ļ�Ŀ¼�����Ϊ�գ���Ŀ���ļ�Ŀ¼�ɶԶ�ָ��
	desFile		Ŀ���ļ��������Ϊ�գ���Ŀ���ļ�����Դ�ļ�����ͬ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ�ܣ�������
*/
int UnionReceiveFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile);

int UnionFileTransferService(PUnionFileTransferHandle phdl);

#endif

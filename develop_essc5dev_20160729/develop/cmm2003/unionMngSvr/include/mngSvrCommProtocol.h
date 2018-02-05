//	Wolfgang Wang, 2006/08/08

#ifndef _mngSvrCommProtocol_
#define _mngSvrCommProtocol_

// ������ʱ�ļ�	2010-5-18,������
void UnionSetUserSpecMngSvrTempFileName(char *fileName);

// ������ʱ�ļ�	2010-5-18,������
void UnionResetUserSpecMngSvrTempFileName();

// ��������2009/3/18,����
// ����mngSvrClient���̨��ͨѶ����
int UnionCreateMngSvrClientSckHDL(char *ipAddr,int port);

// ��������2009/3/18,����
// �ر�mngSvrClient���̨��ͨѶ����
int UnionCloseMngSvrClientSckHDL(int sckHDL);

// ��������2009/3/18,����
// ����mngSvrClient���̨ʹ�ó�����ͨѶ
void UnionSetMngSvrClientUseLongConn();

// ��������2009/3/18,����
// ����mngSvrClient���̨ʹ�ö�����ͨѶ
void UnionSetMngSvrClientUseShortConn();

// 2007/12/22 ����
// �ж��Ƿ���ESSC����ʧ��
int UnionIsMngSvrReturnedError();

// 2007/12/22 ����
// ��ȡmngSvr���صĴ�����
char *UnionGetMngSvrErrorInfo();

// ����һ����ʱ�ļ����������ļ�����
char *UnionGenerateMngSvrTempFile();

// ��ȡ��ǰ��ʱ�ļ�������
char *UnionGetCurrentMngSvrTempFileName();

int UnionDeleteAllMngSvrTempFile();

// ɾ����ʱ�ļ�
int UnionDeleteMngSvrTempFile();

// mngSvr����������ͻ��˷���һ���ļ�
int UnionMngSvrTransferFile(int sckHDL,char *fileName);

// �ڷ������Ϳͻ���֮�䣬����һ�������ļ�
int UnionTransferMngDataFile(int sckHDL,char *fileName,char *tellerNo,int resID);

// �ͻ��˻�ȡ��������ͨѶ����
int UnionGetConfOfMngSvr(char *ipAddr,int *port);

// 2009/5/29�����������ӣ���UnionCommunicationWithSpecMngSvr���۷ֳ��������
// ��MngSvrͨѶ
// reqStr,lenOfReqStr����������������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionExchangeInfoWithSpecMngSvr(char *ipAddr,int port,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// ��MngSvrͨѶ���ͻ���ʹ��
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����ʱ�ļ�
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionCommunicationWithMngSvr(char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// 2007/12/22,����
// ��ָ����MngSvrͨѶ���ͻ���ʹ��
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����ʱ�ļ�
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionCommunicationWithSpecMngSvr(char *ipAddr,int port,char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved);
		
// �ͻ��˴ӷ�������������һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����Դ�ļ�����Դ�ļ��洢��һ����ʱ�ļ���
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientDownloadFile(char *tellerNo,int resID,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// �ͻ��˴ӷ�������������һ��ָ�����Ƶ��ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// fileName��Ҫ���ص��ļ�����
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����Դ�ļ�����Դ�ļ��洢��һ����ʱ�ļ���
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientDownloadSpecFile(char *tellerNo,int resID,char *fileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// �ͻ��˴ӷ�������������һ��ָ�����Ƶ��ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// svrFileName��Ҫ���ص��ļ�����
// localFileName�����ص��ļ��ڱ��صĴ洢����
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����Դ�ļ�����Դ�ļ��洢��һ����ʱ�ļ���
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientDownloadSpecFileToLocalFile(char *tellerNo,int resID,char *svrFileName,char *localFileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved);

// �ͻ�����������ϴ�һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileName,�������������Ҫ������ļ�����
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientUploadFile(char *fileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr);

// �ͻ�����������ϴ�һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileName,�������������Ҫ������ļ�����
// svrFileName������������ļ��ڷ������˵�����
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientUploadFileWithSvrFileName(char *fileName,char *svrFileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr);
		
// MngSvr�ӿͻ��˽��������ļ�
// fileName�ǽ�������Ҫ���ǵ��ļ�
int UnionMngSvrRecvFileFromClient(int sckHDL,char *overwrittenFileName);

int UnionDeleteSelfTempFile();

void UnionFormSelfTempFileName(char *fileName);

#endif

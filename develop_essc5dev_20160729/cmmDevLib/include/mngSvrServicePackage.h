// Author:	Wolfgang Wang
// Date:	2006/8/9

// ����ļ���������Դ��ż���Դ��������

#ifndef _mngSvrServicePackage_
#define _mngSvrServicePackage_

#define conMngSvrPackFldNameTellerNo		"TELLERNO"
#define conMngSvrPackFldNameResID		"RESID"
#define conMngSvrPackFldNameResCmd		"RESCMD"
#define conMngSvrPackFldNameData		"DATA"
#define conMngSvrPackFldNameResponseCode	"RESPONSECODE"

#define conMngSvrRequestPackageFlag		'1'
#define conMngSvrResponsePackageFlag		'0'

// ���¶�����Դ������������ͷ
typedef struct
{
	char	flag;				// �������ʶ���̶�Ϊ1
	char	tellerNo[40+1];			// ����ԱID��
	char	resID[3+1];			// ��ԴID��
	char	resCmd[3+1];			// ��������
	char	lenOfParameter[4+1];		// �����ĳ���
} TUnionResMngReqCmdHeader;
typedef TUnionResMngReqCmdHeader	*PUnionResMngReqCmdHeader;
// ���¶�����Դ������Ӧ����ͷ
typedef struct
{
	char	flag;				// ��Ӧ����ʶ���̶�Ϊ0
	char	tellerNo[40+1];			// ����ԱID��
	char	resID[3+1];			// ��ԴID��
	char	resCmd[3+1];			// ��������
	char	responseCode[6+1];		// ��Ӧ�룬������ʾ�������
	char	lenOfParameter[4+1];		// �����ĳ���
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

// �⿪һ����ذ�
// dataStr,lenOfDataStr������������ֱ��Ӧ��ذ����䳤��
// resID,resCmd,buf,sizeOfBuf������������ֱ��Ӧ��ԴID����Դ���������ݣ����ռ�����ݵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������Ǽ�����ݵĳ��ȡ�
int UnionUnpackSpierPackage(char *dataStr,int lenOfDataStr,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// ��һ����ذ�
// resID,resCmd,data,lenOfData������������ֱ��Ӧ��ԴID����Դ���������ݺ��䳤��
// buf,sizeOfBuf������������ֱ��Ӧ��ذ����ĵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������Ǽ�����ݰ��ĳ��ȡ�
int UnionPackSpierPackage(char *data,int lenOfData,int resID,int resCmd,char *buf,int sizeOfBuf);

// ��һ�������
// tellerNo,resID,resCmd,data,lenOfData������������ֱ��Ӧ����Ա���š���ԴID����Դ���������ݺ��䳤��
// buf,sizeOfBuf������������ֱ��Ӧ��ذ����ĵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݰ��ĳ��ȡ�
int UnionPackMngPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf);

// �⿪һ�������
// dataStr,lenOfDataStr������������ֱ��Ӧ��������䳤��
// tellerNo,resID,resCmd,buf,sizeOfBuf������������ֱ��Ӧ��Ա�š���ԴID����Դ����������ݣ����չ������ݵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݵĳ��ȡ�
int UnionUnpackMngPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// �⿪һ��������Ӧ��
// dataStr,lenOfDataStr������������ֱ��Ӧ��������䳤��
// tellerNo,resID,resCmd,buf,sizeOfBuf������������ֱ��Ӧ��Ա�š���ԴID����Դ����������ݣ����չ������ݵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݵĳ��ȡ�
int UnionUnpackMngSvrResponsePackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,int *responseCode,
			char *buf,int sizeOfBuf);

// ��һ����������
// tellerNo,resID,resCmd,data,lenOfData������������ֱ��Ӧ����Ա���š���ԴID����Դ���������ݺ��䳤��
// buf,sizeOfBuf������������ֱ��Ӧ��ذ����ĵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݰ��ĳ��ȡ�
int UnionPackMngSvrRequestPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf);

// �⿪һ�����������
// dataStr,lenOfDataStr������������ֱ��Ӧ��������䳤��
// tellerNo,resID,resCmd,buf,sizeOfBuf������������ֱ��Ӧ��Ա�š���ԴID����Դ����������ݣ����չ������ݵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݵĳ��ȡ�
int UnionUnpackMngSvrRequestPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf);

// ��һ����Ӧ�����
// tellerNo,resID,resCmd,data,lenOfData������������ֱ��Ӧ����Ա���š���ԴID����Դ���������ݺ��䳤��
// responseCode������Ӧ��
// buf,sizeOfBuf������������ֱ��Ӧ��ذ����ĵĻ������Ĵ�С
// ����ֵ�Ǹ�����������������ǹ������ݰ��ĳ��ȡ�
int UnionPackMngSvrResponsePackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,int responseCode,char *buf,int sizeOfBuf);

// �ж�һ�����ǲ��������
int UnionIsMngSvrResponsePackage(char *dataStr,int lenOfDataStr);

// 2009/9/30������������
// �⿪һ�������ͷ
// dataStr,lenOfDataStr������������ֱ��Ӧ��������䳤��
// isRequest,tellerNo,resID,resCmd,resID,lenOfData�ֱ��Ӧ�����ʶ����Ա�š���ԴID����Դ�����Ӧ��,���ݳ���
// ����ֵ�Ǹ����������������������ͷ�ĳ��ȡ�
int UnionUnpackMngSvrPackageHeader(char *dataStr,int lenOfDataStr,int *isRequest,char *tellerNo,int *resID,int *resCmd,int *resCode,int *lenOfData);

#endif





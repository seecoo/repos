//	Wolfgang Wang, 2006/08/08

/* 2007/12/22,���Ӻ���
 	UnionCommunicationWithSpecMngSvr
	UnionIsMngSvrReturnedError
	UnionGetMngSvrErrorInfo
	�����˱�����
		gunionIsMngSvrReturnedError
		gunionMsgSvrReturnedErrorInfo
	�Կ��ƻ�ȡmngSvr���صĴ���
	�޸�
	UnionCommunicationWithMngSvr,����������еĴ����Ƶ���UnionCommunicationWithSpecMngSvr��Ȼ�������������
	UnionCommunicationWithSpecMngSvr
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#else
#include <process.h>
#include <windows.h>
#endif

#include "UnionSocket.h"		// ʹ��3.x�汾
#include "unionResID.h"
#include "UnionLog.h"			// ʹ��3.x�汾
#include "unionRecFile.h"		
#include "mngSvrCommProtocol.h"
#include "mngSvrServicePackage.h"
#include "UnionStr.h"
#include "simuMngSvrLocally.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif
#include "UnionEnv.h"

// Add by HuangBaoxin 2008/11/12
#ifdef _WIN32
#include "simuMngSvrLocally.h"
#include "errCodeTranslater.h"
#endif
// End Add

#ifndef _WIN32
char	pgMngSvrTmpFileName[256+1];
unsigned long	gunionTmpFileIndex = 0;
int	gunionIsUserSetTmpFileName = 0;		// ��������2010/5/18,����
#endif

char	gunionRemoteMngSvrName[128+1] = "";	// ��������2009/3/18,����
int	gunionMngSvrClientSckHDL = -1;		// ��������2009/3/18,����
int	gunionIsMngSvrClientUseLongConn = 0;	// ��������2009/3/18,����

// 2007/12/22 ����
int 	gunionIsMngSvrReturnedError = 0;
char 	gunionMsgSvrReturnedErrorInfo[128+1];
// 2007/12/22 ���ӽ���

// ��������2009/3/18,����
// ����mngSvrClient���̨��ͨѶ����
int UnionCreateMngSvrClientSckHDL(char *ipAddr,int port)
{
	int	sckHDL;
	
	if (gunionIsMngSvrClientUseLongConn)	// ʹ�ó�����
	{
		if (gunionMngSvrClientSckHDL >= 0)
			return(gunionMngSvrClientSckHDL);
	}
	
	// ʹ�ö����ӻ�ʹ�ó����ӵ�����δ��
	if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionCreateMngSvrClientSckHDL:: UnionCreateSocketClient [%s] [%d]!\n",ipAddr,port);
		return(sckHDL);
	}
	return(gunionMngSvrClientSckHDL = sckHDL);
}

// ��������2009/3/18,����
// �ر�mngSvrClient���̨��ͨѶ����
int UnionCloseMngSvrClientSckHDL(int sckHDL)
{
	// if (gunionIsMngSvrClientUseLongConn)	// ʹ�ó����ӣ����ر�
	//	return(0);
	UnionCloseSocket(sckHDL);
	gunionMngSvrClientSckHDL = -1;
	return(0);
}

// ��������2009/3/18,����
// ����mngSvrClient���̨ʹ�ó�����ͨѶ
void UnionSetMngSvrClientUseLongConn()
{
	gunionIsMngSvrClientUseLongConn = 1;
	return;
}

// ��������2009/3/18,����
// ����mngSvrClient���̨ʹ�ö�����ͨѶ
void UnionSetMngSvrClientUseShortConn()
{
	gunionIsMngSvrClientUseLongConn = 0;
	return;
}

// ������ʱ�ļ�	2010-5-18,������
void UnionSetUserSpecMngSvrTempFileName(char *fileName)
{
	strcpy(pgMngSvrTmpFileName,fileName);
	gunionIsUserSetTmpFileName = 1;
}

// ������ʱ�ļ�	2010-5-18,������
void UnionResetUserSpecMngSvrTempFileName()
{
	memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
	gunionIsUserSetTmpFileName = 0;
}

int UnionSetRemoteMngSvrName(char *name)
{
	memset(gunionRemoteMngSvrName,0,sizeof(gunionRemoteMngSvrName));
	strcpy(gunionRemoteMngSvrName,name);
}

// 2009/3/19����
char *UnionGetCurrentRemoteMngSvrName()
{
	return(gunionRemoteMngSvrName);
}

// 2007/12/22 ����
// �ж��Ƿ���ESSC����ʧ��
int UnionIsMngSvrReturnedError()
{
	return(gunionIsMngSvrReturnedError);
}

// 2007/12/22 ����
// ��ȡmngSvr���صĴ�����
char *UnionGetMngSvrErrorInfo()
{
	if (!gunionIsMngSvrReturnedError)
	{
		memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
	}
	gunionMsgSvrReturnedErrorInfo[sizeof(gunionMsgSvrReturnedErrorInfo)-1] = 0;
	return(gunionMsgSvrReturnedErrorInfo);
}
		
int UnionConvertDosStrIntoUnixStr(char *str,int lenOfStr)
{
	char	tmpBuf[2048+1];
	int	index1,index2 = 0;
	int	isReturnChar;
	
	for (index1 = 0; (index1 < lenOfStr) && (index2 < sizeof(tmpBuf)); index1++)
	{
		if (str[index1] != 0x0d)
		{
			tmpBuf[index2] = str[index1];
			index2++;
			continue;
		}
		index1++;
		if (index1 >= lenOfStr)
		{
			tmpBuf[index2] = str[index1];
			index2++;
			break;
		}
		if (str[index1] == 0x0a)	// �ǻس�����
		{
			tmpBuf[index2] = 0x0a;
			index2++;
			continue;
		}
		else
		{
			tmpBuf[index2] = 0x0d;
			index2++;
			if (index2 >= sizeof(tmpBuf))
				break;
			tmpBuf[index2] = str[index1];
			index2++;
			continue;
		}
	}
	if (index2 >= sizeof(tmpBuf))	// ��ת��
		return(lenOfStr);		
	memcpy(str,tmpBuf,index2);
	return(index2);
}

// ��ȡһ����ʱ�ļ�����
#ifndef _WIN32
void UnionFormSelfTempFileName(char *fileName)
{
	sprintf(fileName,"%s/%d.self.tmp",getenv("UNIONTEMP"),getpid());
	return;
}

// ɾ����ʱ�ļ�
int UnionDeleteSelfTempFile()
{
	char	tmpBuf[256];
	char	fileName[256];
	
	UnionFormSelfTempFileName(fileName);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",fileName);
	system(tmpBuf);
	return(0);
}

char *UnionGenerateMngSvrTempFile()
{
	// 2010-5-18,����������
	if (gunionIsUserSetTmpFileName)		
		return(pgMngSvrTmpFileName);
	// 2010-5-18,���������ӽ���
	memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
	sprintf(pgMngSvrTmpFileName,"%s/%05d.%ld.tmp",getenv("UNIONTEMP"),getpid(),++gunionTmpFileIndex);
	return(pgMngSvrTmpFileName);
}

int UnionDeleteAllMngSvrTempFile()
{
	int	i;
	char	tmpBuf[512];

	sprintf(tmpBuf,"rm %s/*.tmp",getenv("UNIONTEMP"));
	system(tmpBuf);
	return(0);
}

// ��ȡ��ǰ��ʱ�ļ�������
char *UnionGetCurrentMngSvrTempFileName()
{
	return(pgMngSvrTmpFileName);
}

// ɾ����ʱ�ļ�
int UnionDeleteMngSvrTempFile()
{
	char	tmpBuf[512];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",pgMngSvrTmpFileName);
	system(tmpBuf);
	return(0);
}

#endif

// �ڷ������Ϳͻ���֮�䣬����һ�������ļ�
int UnionTransferMngDataFile(int sckHDL,char *fileName,char *tellerNo,int resID)
{
	FILE		*fp = NULL;
	unsigned char	tmpBuf[2048+1+2];
	char		data[2000+1];
	int		ret,sendLen=0;
	long		realSendLen = 0;
	char		resIDBuf[10+1];
	
	if ((sckHDL < 0) || (fileName == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionTransferMngDataFile:: sckHDL = [%d]!\n",sckHDL);
		return(errCodeParameter);
	}
	sprintf(resIDBuf,"%03d",resID);
	//UnionDebugNullLog("\ntransfer [%s] beginning ...\n",fileName);
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// �ȷ���һ�����俪ʼ��
	if ((ret = UnionPackMngSvrRequestPackage(" ",0,tellerNo,atoi(resIDBuf),conResCmdStartSendingData,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngSvrRequestPackage!\n");
		goto errExit;
	}
	tmpBuf[2+ret] = 0;
	//UnionDebugNullLog("[%04d][%s]\n",ret,tmpBuf+2);
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
		goto errExit;
	}
	// ��ʼ�����ļ�	
	while (!feof(fp))
	{
		memset(data,0,sizeof(data));
		if ((sendLen = fread(data,1,sizeof(data)-1,fp)) <= 0)
		{
			//UnionUserErrLog("in UnionTransferMngDataFile:: sendLen = [%d] [%d] data = [%s]\n",sizeof(data),sendLen,data);
			continue;
		}
#ifdef _WIN32
		sendLen = UnionConvertDosStrIntoUnixStr(data,sendLen);
#endif
		if ((ret = UnionPackMngSvrRequestPackage(data,sendLen,tellerNo,atoi(resIDBuf),conResCmdSendingData,
			(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
		{
			UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngSvrRequestPackage!\n");
			goto errExit;
		}
		tmpBuf[2+ret] = 0;
		//UnionDebugNullLog("[%04d][%s]\n",sendLen,tmpBuf+2);
		tmpBuf[0] = ret / 256;
		tmpBuf[1] = ret % 256;
		if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
		{
			UnionUserErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
			goto errExit;
		}
		realSendLen += sendLen;
	}
	// �ٷ���һ�����������
	sprintf(data,"%08ld",realSendLen);
	if ((ret = UnionPackMngPackage(data,strlen(data),tellerNo,atoi(resIDBuf),conResCmdFinishSendingData,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngPackage!\n");
		goto errExit;
	}
	tmpBuf[2+ret] = 0;
	//UnionDebugNullLog("[%04d][%s]\n",ret,tmpBuf+2);
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
		goto errExit;
	}
	//UnionDebugNullLog("transfer [%s] OK! [%08ld] characters send!\n\n",fileName,realSendLen);
errExit:
	if (fp != NULL)
		fclose(fp);
	return(ret);
}

// mngSvr����������ͻ��˷���һ���ļ�
int UnionMngSvrTransferFile(int sckHDL,char *fileName)
{
	char		resID[10+1];
	char		tellerNo[40+1];
	int		ret;
		
	if ((sckHDL < 0) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: sckHDL = [%d]!\n",sckHDL);
		return(errCodeParameter);
	}
	
	// ����Դ��
	memset(resID,0,sizeof(resID));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameResID);
		return(ret);
	}
	// ����Ա��
	memset(tellerNo,0,sizeof(tellerNo));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameTellerNo,tellerNo,sizeof(tellerNo))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameTellerNo);
		return(ret);
	}
	return(UnionTransferMngDataFile(sckHDL,fileName,tellerNo,atoi(resID)));
}

// 2009/3/19������������
int UnionGetConfOfSpecMngSvr(char *mngSvrName,char *ipAddr,int *port)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	
#ifdef _WIN32
	sprintf(fileName,"./esscMngSvr.conf");
#else
	sprintf(fileName,"%s/esscMngSvr.conf",getenv("UNIONETC"));
#endif
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0; i < UnionGetEnviVarNum(); i++)
	{
		// ��ȡ����
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(mngSvrName,p) != 0)	// δ�ҵ�
			continue;
		// ��ȡip��ַ
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		strcpy(ipAddr,p);
			
		// ��ȡ�˿ں�
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) != NULL)
		{
			*port = atoi(p);
		}
		UnionClearEnvi();
		return(0);
	}
	UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: mngSvr [%s] not defined!\n",mngSvrName);
	UnionClearEnvi();
		
	return(errCodeEsscMDL_MngSvrNotDefined);
}

int UnionGetConfOfMngSvr(char *ipAddr,int *port)
{
	int	ret;
	char	tmpBuf[2048+1];
	FILE	*fp;
	char	*ptr;
	char	tmpBuf2[100];
	char	*mngSvrName;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
#ifdef _WIN32
	sprintf(tmpBuf,"./esscMngSvr.conf");
#else
	sprintf(tmpBuf,"%s/esscMngSvr.conf",getenv("UNIONETC"));
#endif
#ifdef _WIN32
	if ((fp = fopen(tmpBuf,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionGetConfOfMngSvr:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	fread(tmpBuf,sizeof(tmpBuf)-1,1,fp);
	fclose(fp);
	if ((ptr = strstr(tmpBuf,"ipAddr=")) == NULL)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: no ipAddr defined!\n");
		return(errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile);
	}
	sscanf(ptr+7,"%s",tmpBuf2);
	if (!UnionIsValidIPAddrStr(tmpBuf2))
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: %s is not valid ipAddr!\n",tmpBuf2);
		return(errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile);
	}
	strcpy(ipAddr,tmpBuf2);
	if ((ptr = strstr(tmpBuf,"port=")) == NULL)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: no port defined!\n");
		return(errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile);
	}
	sscanf(ptr+5,"%d",port);
	if (*port <= 0)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: port [%d] error!\n",*port);
		return(errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile);
	}
#else
	if (((mngSvrName = UnionGetCurrentRemoteMngSvrName()) != NULL) && (strlen(mngSvrName) != 0))
	{
		if ((ret = UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port)) >= 0)
			return(ret);
	}
	if ((ret = UnionInitEnvi(tmpBuf)) < 0)
	{	
		// printf("Fail to UnionInitEnvi [%s]\n",tmpBuf);
		UnionUserErrLog("Fail to UnionInitEnvi [%s]\n",tmpBuf);
		return(ret);
	}
	
	if ((ptr = UnionGetEnviVarByName("ipAddr")) == NULL)
	{
		// printf("Fail to get IPAddr!\n");
		UnionUserErrLog("Fail to get IPAddr!\n");
		UnionClearEnvi();
		return(UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port));
	}
	else
	{
		memcpy(ipAddr,ptr,strlen(ptr));
		// printf("HsmSvr IP = [%s]\n",ptr);
		UnionNullLog("HsmSvr IP = [%s]\n",ptr);
	}

	if ((ptr = UnionGetEnviVarByName("port")) == NULL)
	{
		// printf("Fail to get Port!\n");
		UnionUserErrLog("Fail to get Port!\n");
		UnionClearEnvi();
		return(UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port));
	}
	else
	{
		*port = atoi(ptr);
		// printf("Port = [%d]\n",*port);
		UnionNullLog("Port = [%d]\n",*port);
	}
	UnionClearEnvi();
#endif
	return(0);
}


// ��MngSvrͨѶ
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionCommunicationWithMngSvr(char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved)

{
	char		ipAddr[100];
	int		port;
	int		ret;

	gunionIsMngSvrReturnedError = 0;	// 2007/11/22����

	// ��ȡESSC�Ĺ����ַ�Ͷ˿�
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionGetConfOfMngSvr(ipAddr,&port)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithMngSvr:: UnionGetConfOfMngSvr!\n");
		return(ret);
	}
	return(UnionCommunicationWithSpecMngSvr(ipAddr,port,tellerNo,resID,serviceID,
		reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
}


// 2009/5/29,��������д���������
// ��MngSvrͨѶ
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionCommunicationWithSpecMngSvr(char *ipAddr,int port,char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved)

{
	unsigned char 	tmpBuf[8192*2+1];
	int		len;
	int		ret;
	int		responseCode;

// Add by HuangBaoxin 2008/11/12
#ifdef _WIN32
        if (UnionIsReadMngSvrDataLocally())
        {
                if ((ret = UnionReadMngSvrDataLocally(-1, tellerNo,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
                {
                        UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionReadMngSvrDataLocally!\n");
                        strcpy(resStr,UnionFindRemarkOfErrCode(ret));
                }
		return(ret);
        }
#endif
// End Add

	gunionIsMngSvrReturnedError = 0;	// 2007/11/22����
	
	if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0) ||
		(tellerNo == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: parameter error!\n");
		return(errCodeParameter);
	}
	// �������
	if ((ret = UnionPackMngSvrRequestPackage(reqStr,lenOfReqStr,tellerNo,resID,serviceID,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionPackMngSvrRequestPackage!\n");
		return(ret);
	}
	// �������ͨѶ
	if ((ret = UnionExchangeInfoWithSpecMngSvr(ipAddr,port,tmpBuf,ret,tmpBuf,sizeof(tmpBuf),fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionPackMngSvrRequestPackage!\n");
		return(ret);
	}
	// ����Ӧ��
	len = ret;
	if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionUnpackMngSvrResponsePackage!\n");
		return(ret);
	}
	resStr[ret] = 0;
	if (responseCode < 0)		// ���������ص�ֵ��Ϊ0
	{
		ret = responseCode;
		// 2007/11/22����
		gunionIsMngSvrReturnedError = 1;
		memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
		if (ret >= sizeof(gunionMsgSvrReturnedErrorInfo))
			memcpy(gunionMsgSvrReturnedErrorInfo,resStr,sizeof(gunionMsgSvrReturnedErrorInfo)-1);
		else
			memcpy(gunionMsgSvrReturnedErrorInfo,resStr,ret);
		// 2007/11/22���ӽ���
	}
	return(ret);
}

// 2009/5/29�����������ӣ���UnionCommunicationWithSpecMngSvr���۷ֳ��������
// ��MngSvrͨѶ
// reqStr,lenOfReqStr����������������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionExchangeInfoWithSpecMngSvr(char *ipAddr,int port,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)

{
	unsigned char 	tmpBuf[8192*2+1];
	int		len;
	int		ret;
	int		sckHDL = -1;
	FILE		*fp = NULL;
	int		mngSvrResCmd;
	int		dataRecvFinished = 0;
	char		*recvFileName;
	int		responseCode;
	int		createSckHDLRetryTimes = 0;	// 2010-5-18,����������
	
	if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0))
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: parameter error!\n");
		return(errCodeParameter);
	}
	// ����ͨѶ����
	// 2010-5-18,�����������������
recreateSckHDL:
	++createSckHDLRetryTimes;
	if (createSckHDLRetryTimes >= 2)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: createSckHDLRetryTimes = [%d]\n",createSckHDLRetryTimes);
		return(errCodeTooManyRetryTimes);
	}
	// 2010-5-18,���������Ӷν���
	//if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)	// 2010-5-18,������ɾ��
	if ((sckHDL = UnionCreateMngSvrClientSckHDL(ipAddr,port)) < 0)	// 2010-5-18,����������
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionCreateMngSvrClientSckHDL! [%s] [%d]\n",ipAddr,port);
		return(sckHDL);
	}
	tmpBuf[0] = lenOfReqStr / 256;
	tmpBuf[1] = lenOfReqStr % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionSendToSocket!\n");
		if (createSckHDLRetryTimes < 2)	// 2010-05-18,����������
			goto recreateSckHDL;	// 2010-05-18,����������	
		goto errorExit;
	}
	if ((ret = UnionSendToSocket(sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionSendToSocket!\n");
		goto errorExit;
	}

	// ��������
	recvFileName = NULL;
	*fileRecved = 0;
	for (;;)
	{
		// ����һ�����ݰ�
		memset(tmpBuf,0,2);
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionExchangeInfoWithSpecMngSvr:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: len [%d] too large or too small!\n",len);
			ret = errCodeEsscMDL_DataLenFromMngSvr;
			goto errorExit;
		}
		if (len == 0)
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionExchangeInfoWithSpecMngSvr:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		// �ж��ǲ�����Ӧ��
		if (UnionIsMngSvrResponsePackage((char *)tmpBuf,len))		// ����Ӧ��
		{
			if (recvFileName != NULL)		// �Ƿ������ݴ���
			{
				if (!dataRecvFinished)		// �����ݴ����Ƿ������
				{
					UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: data send not finished yet!\n");
					ret = errCodeEsscMDL_NotFinishSendingData;
					goto errorExit;
				}
			}
			break;
		}
		// �������������������
		if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionUnpackMngSvrRequestPackage!\n");
			ret = len;
			goto errorExit;
		}
		// �ж��ǲ������ݿ�ʼ���Ͱ�
		if (mngSvrResCmd == conResCmdStartSendingData) 
		{
			if (recvFileName != NULL)	// �Ѿ��յ���һ�����ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: another data start sending pack received again!\n");
				ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
				goto errorExit;
			}
			if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)	// ���ܻ��һ����ʱ�洢�ļ���
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionGenerateMngSvrTempFile!\n");
				ret = errCodeEsscMDL_FailToGetTempFile;
				goto errorExit;
			}
			if ((fp = fopen(recvFileName,"wb")) == NULL)	// �򲻿���ʱ�ļ�
			{
				UnionSystemErrLog("in UnionExchangeInfoWithSpecMngSvr:: fopen [%s]!\n",recvFileName);
				ret = errCodeUseOSErrCode;
				goto errorExit;
			}
			continue;
		}
		// �ж��ǲ������ݽ������Ͱ�
		if (mngSvrResCmd == conResCmdFinishSendingData) 
		{
			if (recvFileName == NULL)	// û���յ������ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			dataRecvFinished = 1;
			continue;
		}	
		// �ж��ǲ������ݰ�
		if (mngSvrResCmd == conResCmdSendingData)
		{
			if ((recvFileName == NULL) || (fp == NULL))	// û���յ������ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			fwrite(tmpBuf,len,1,fp);
			continue;
		}
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: wrong request package received! [%03d]\n",mngSvrResCmd);
		ret = errCodeEsscMDL_InvalidOperation;
		goto errorExit;
	}
	if (len > sizeOfResStr)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionUnpackMngSvrResponsePackage!\n");
		ret = errCodeSmallBuffer;
		goto errorExit;
	}
	memcpy(resStr,tmpBuf,len);		
	ret = len;
	resStr[len] = 0;
	if (recvFileName != NULL)
		*fileRecved = 1;
	// 2010-5-18,�����������������
	if (fp != NULL)
		fclose(fp);
	if ((sckHDL >= 0) && (!gunionIsMngSvrClientUseLongConn))
		UnionCloseMngSvrClientSckHDL(sckHDL);	// 2010-05-18,����������
	return(ret);
	// 2010-5-18,���������ӽ���	
errorExit:
	if (fp != NULL)
		fclose(fp);
	if (sckHDL >= 0)
		// UnionCloseSocket(sckHDL);		// 2010-05-18,�������޸�
		UnionCloseMngSvrClientSckHDL(sckHDL);	// 2010-05-18,����������
	return(ret);
}

// �ͻ��˴ӷ�������������һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileRecved,�������������1����ʾ�յ�����Դ�ļ�����Դ�ļ��洢��һ����ʱ�ļ���
//	ͨ������UnionGetCurrentMngSvrTempFileName��������ʱ�ļ���
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientDownloadFile(char *tellerNo,int resID,
		char *resStr,int sizeOfResStr,
		int *fileRecved)
{
	//return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile," ",0,resStr,sizeOfResStr,fileRecved));
	return(UnionMngClientDownloadSpecFile(tellerNo,resID,NULL,resStr,sizeOfResStr,fileRecved));
}

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
		int *fileRecved)
{
	if (fileName == NULL)
		return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile," ",0,resStr,sizeOfResStr,fileRecved));
	else
		return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile,fileName,strlen(fileName),resStr,sizeOfResStr,fileRecved));
}

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
		int *fileRecved)
{
	int	ret;
	
	if ((ret = UnionMngClientDownloadSpecFile(tellerNo,resID,svrFileName,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionMngClientDownloadSpecFileToLocalFile:: UnionMngClientDownloadSpecFile!\n");
		return(ret);
	}
	if ((localFileName != NULL) && (strlen(localFileName) != 0))
		return(UnionRenameFile(UnionGetCurrentMngSvrTempFileName(),localFileName));
	return(ret);
}

// �ͻ�����������ϴ�һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileName,�������������Ҫ������ļ�����
// svrFileName������������ļ��ڷ������˵�����
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientUploadFileWithSvrFileName(char *fileName,char *svrFileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr)
{
	unsigned char 	tmpBuf[2048+1];
	int		len;
	int		ret;
	char		ipAddr[100];
	int		port;
	int		sckHDL = -1;
	int		isFileSended = 0;
	int		responseCode;
	
	if ((fileName == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: parameter error!\n");
		return(errCodeParameter);
	}
	// ��ȡESSC�Ĺ����ַ�Ͷ˿�
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionGetConfOfMngSvr(ipAddr,&port)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionGetConfOfMngSvr!\n");
		return(ret);
	}
	// ����ͨѶ����
	if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionCreateSocketClient! [%s] [%d]\n",ipAddr,port);
		return(sckHDL);
	}
	// �������
	if (svrFileName == NULL)
		ret = UnionPackMngSvrRequestPackage(" ",0,tellerNo,resID,conResCmdUploadFile,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	else
		ret = UnionPackMngSvrRequestPackage(svrFileName,strlen(svrFileName),tellerNo,resID,conResCmdUploadFile,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionPackMngSvrRequestPackage!\n");
		goto errorExit;
	}
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	// ��������
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2+ret)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionSendToSocket!\n");
		goto errorExit;
	}
	goto recvAnswer;
sendFile:
	// �����ļ�
	if ((ret = UnionTransferMngDataFile(sckHDL,fileName,tellerNo,resID)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionTransferMngDataFile!\n");
		goto errorExit;
	}
	isFileSended = 1;	// ���ļ��ѷ��ͱ�ʶ
recvAnswer:	
	// ������Ӧ��
	memset(tmpBuf,0,2);
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionDebugLog("in UnionMngClientUploadFileWithSvrFileName:: svr close socket!\n");
		ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
		goto errorExit;
	}
	if (((len = tmpBuf[0] * 256 + tmpBuf[1]) <= 0) || (len > sizeof(tmpBuf)))
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: len [%d] too large or too small!\n",len);
		ret = errCodeEsscMDL_DataLenFromMngSvr;
		goto errorExit;
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionDebugLog("in UnionMngClientUploadFileWithSvrFileName:: svr close socket!\n");
		ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
		goto errorExit;
	}
	if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionUnpackMngSvrResponsePackage!\n");
		goto errorExit;
	}
	if (responseCode < 0)		// ���������ص�ֵ��Ϊ0
	{
		ret = responseCode;
		goto errorExit;
	}
	// ����ļ��ǲ����ѷ���
	if (!isFileSended)
		goto sendFile;
errorExit:
	if (sckHDL >= 0)
		UnionCloseSocket(sckHDL);
	return(ret);
}


// �ͻ�����������ϴ�һ����Դ�ļ�
// tellerNo,resID,������������ֱ��Ӧ����Ա��ʶ����Դ
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// fileName,�������������Ҫ������ļ�����
// ����ֵ��resStr�����ݵĳ���
int UnionMngClientUploadFile(char *fileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr)
{
	return(UnionMngClientUploadFileWithSvrFileName(fileName,NULL,tellerNo,resID,resStr,sizeOfResStr));
}

// MngSvr�ӿͻ��˽��������ļ�
int UnionMngSvrRecvFileFromClient(int sckHDL,char *overwrittenFileName)
{
	unsigned char 	tmpBuf[2048+1];
	int		len;
	int		ret;
	FILE		*fp = NULL;
	//int		recvDataLen = 0;
	//int		resStrLen = 0;
	int		mngSvrResCmd;
	char		*recvFileName;
	char		resID[10+1];
	char		tellerNo[40+1];
		
	// ����Դ��
	memset(resID,0,sizeof(resID));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameResID);
		return(ret);
	}
	// ����Ա��
	memset(tellerNo,0,sizeof(tellerNo));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameTellerNo,tellerNo,sizeof(tellerNo))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameTellerNo);
		return(ret);
	}
	// ����һ��ͬ����յ����ݰ�
	// �ȴ��
	if ((ret = UnionPackMngSvrResponsePackage(" ",0,tellerNo,atoi(resID),conResCmdUploadFile,0,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionPackMngSvrResponsePackage!\n");
		return(ret);
	}
	// �ٷ���
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionSendToSocket!\n");
		return(ret);
	}
	// ��������
	recvFileName = NULL;
	for (;;)
	{
		// ����һ�����ݰ�
		memset(tmpBuf,0,2);
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionMngSvrRecvFileFromClient:: peer close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: len [%d] too large or too small!\n",len);
			ret = errCodeEsscMDL_DataLenFromMngSvr;
			goto errorExit;
		}
		if (len == 0)
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionMngSvrRecvFileFromClient:: peer close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		// ����������
		if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionUnpackMngSvrRequestPackage!\n");
			ret = len;
			goto errorExit;
		}
		// �ж��ǲ������ݿ�ʼ���Ͱ�
		if (mngSvrResCmd == conResCmdStartSendingData) 
		{
			if (recvFileName != NULL)	// �Ѿ��յ���һ�����ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: another data start sending pack received again!\n");
				ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
				goto errorExit;
			}
			if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)	// ���ܻ��һ����ʱ�洢�ļ���
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionGenerateMngSvrTempFile!\n");
				ret = errCodeEsscMDL_FailToGetTempFile;
				goto errorExit;
			}
			if ((fp = fopen(recvFileName,"wb")) == NULL)	// �򲻿���ʱ�ļ�
			{
				UnionSystemErrLog("in UnionMngSvrRecvFileFromClient:: fopen [%s]!\n",recvFileName);
				ret = errCodeUseOSErrCode;
				goto errorExit;
			}
			continue;
		}
		// �ж��ǲ������ݽ������Ͱ�
		if (mngSvrResCmd == conResCmdFinishSendingData) 
		{
			if (recvFileName == NULL)	// û���յ������ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			ret = 0;
			break;
		}	
		// �ж��ǲ������ݰ�
		if (mngSvrResCmd == conResCmdSendingData)
		{
			if ((recvFileName == NULL) || (fp == NULL))	// û���յ������ݴ��俪ʼ��
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			fwrite(tmpBuf,len,1,fp);
			continue;
		}
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: wrong request package received! [%03d]\n",mngSvrResCmd);
		ret = errCodeEsscMDL_InvalidOperation;
		goto errorExit;
	}
	fclose(fp);
	fp = NULL;
UnionAuditNullLog("in UnionMngSvrRecvFileFromClient:: overwrittenFileName=[%s],recvFileName=[%s]\n",overwrittenFileName,recvFileName);
	// �ȱ���Ҫ���ǵ��ļ�
	if ((ret = UnionBackupFileToTmpDir(overwrittenFileName)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionBackupFileToTmpDir [%s]\n",overwrittenFileName);
		return(ret);
	}
	//sprintf((char *)tmpBuf,"cp %s %s",overwrittenFileName,getenv("UNIONTEMP"));
	//system((char *)tmpBuf);
	// �ٸ����ļ�
	if ((ret = UnionRenameFile(recvFileName,overwrittenFileName)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionRenameFile [%s] to [%s]\n",recvFileName,overwrittenFileName);
		UnionRestoreFileFromTmpDir(overwrittenFileName);
		return(ret);
	}
	//sprintf((char *)tmpBuf,"cp %s %s",recvFileName,overwrittenFileName);
	//system((char *)tmpBuf);
	//UnionDeleteMngSvrTempFile();
errorExit:
	if (fp != NULL)
		fclose(fp);
	return(ret);	
}

int UnionReceiveDataPackage(int sckHDL,char *resStr,int sizeOfResStr,int *fileRecved)
{
        unsigned char   tmpBuf[8192+1];
        int             len;
        int             ret;
        FILE            *fp = NULL;
        int             mngSvrResCmd;
        int             dataRecvFinished = 0;
        char            *recvFileName;
        int             responseCode;

        if(sckHDL < 0 || fileRecved == NULL)
        {
                UnionUserErrLog("in UnionReceiveDataPackage:: parameter error!\n");
                return(errCodeParameter);
        }
        recvFileName = NULL;
        *fileRecved = 0;
        gunionIsMngSvrReturnedError = 0;
        for (;;)
        {
                // ����һ�����ݰ�
                memset(tmpBuf,0,2);

                if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionReceiveFromSocketUntilLen!\n");
                        goto errorExit;
                }

                if (ret == 0)
                {
                        UnionDebugLog("in UnionReceiveDataPackage:: svr close socket!\n");
                        ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
                        goto errorExit;
                }
                if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: len [%d] too large or too small!\n",len);
                        ret = errCodeEsscMDL_DataLenFromMngSvr;
                        goto errorExit;
                }
                if (len == 0)
                        continue;

                memset(tmpBuf,0,sizeof(tmpBuf));
                if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionReceiveFromSocketUntilLen!\n");
                        goto errorExit;
                }

                if (ret == 0)
                {
                        UnionDebugLog("in UnionReceiveDataPackage svr close socket!\n");
                        ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
                        goto errorExit;
                }

                // �ж��ǲ�����Ӧ��
                if (UnionIsMngSvrResponsePackage((char *)tmpBuf,len))  // ����Ӧ��
                {
                        if (recvFileName != NULL)      // �Ƿ������ݴ���
                        {
                                if (!dataRecvFinished) // �����ݴ����Ƿ������
                                {
                                        UnionUserErrLog("in UnionReceiveDataPackage:: data send not finished yet!\n");
                                        ret = errCodeEsscMDL_NotFinishSendingData;
                                        goto errorExit;
                                }
                        }
                        break;
                }
                // �������������������
                if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionUnpackMngSvrRequestPackage!\n");
                        ret = len;
                        goto errorExit;
                }
                // �ж��ǲ������ݿ�ʼ���Ͱ�
                if (mngSvrResCmd == conResCmdStartSendingData)
                {
                        if (recvFileName != NULL)    // �Ѿ��յ���һ�����ݴ��俪ʼ��
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: another data start sending pack received again!\n");
                                ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
                                goto errorExit;
                        }
                        if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)     // ���ܻ��һ����ʱ�洢�ļ���
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: UnionGenerateMngSvrTempFile!\n");
                                ret = errCodeEsscMDL_FailToGetTempFile;
                                goto errorExit;
                        }

                        if ((fp = fopen(recvFileName,"wb")) == NULL)   // �򲻿���ʱ�ļ�
                        {
                                UnionSystemErrLog("in UnionReceiveDataPackage:: fopen [%s]!\n",recvFileName);
                                ret = errCodeUseOSErrCode;
                                goto errorExit;
                        }
                        continue;
                }
                // �ж��ǲ������ݽ������Ͱ�
                if (mngSvrResCmd == conResCmdFinishSendingData)
                {
                        if (recvFileName == NULL)    // û���յ������ݴ��俪ʼ��
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: no start sending pack received ever!\n");
                                ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
                                goto errorExit;
                        }
                        dataRecvFinished = 1;
                        continue;
                }
                // �ж��ǲ������ݰ�
                if (mngSvrResCmd == conResCmdSendingData)
                {
                        if ((recvFileName == NULL) || (fp == NULL))     // û���յ������ݴ��俪ʼ��
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: no start sending pack received ever!\n");
                                ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
                                goto errorExit;
                        }

                        fwrite(tmpBuf,len,1,fp);
                        continue;
                }
                UnionUserErrLog("in UnionReceiveDataPackage:: wrong request package received! [%03d]\n",mngSvrResCmd);
                ret = errCodeEsscMDL_InvalidOperation;
                goto errorExit;
        }
        if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
        {
                UnionUserErrLog("in UnionReceiveDataPackage:: UnionUnpackMngSvrResponsePackage!\n");
                goto errorExit;
        }
        if (responseCode < 0)           // ���������ص�ֵ��Ϊ0
        {
                ret = responseCode;
                // 2007/11/22����
                gunionIsMngSvrReturnedError = 1;
                memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
                if (ret >= sizeof(gunionMsgSvrReturnedErrorInfo))
                        memcpy(gunionMsgSvrReturnedErrorInfo,resStr,sizeof(gunionMsgSvrReturnedErrorInfo)-1);
                else
                        memcpy(gunionMsgSvrReturnedErrorInfo,resStr,ret);
                // 2007/11/22���ӽ���
                goto errorExit;
        }

        if (recvFileName != NULL)
                *fileRecved = 1;
errorExit:
        if (fp != NULL)
                fclose(fp);
        return ret;

}

// ��MngSvrͨѶ   �ϴ��ļ�
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// fileName,���������,Ҫ�ϴ����ļ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionCommunicationWithSpecMngSvrWithUploadFile(char *ipAddr,int port,char *tellerNo,int resID,int serviceID, char *reqStr,int lenOfReqStr, char *fileName, char *resStr,int sizeOfResStr, int *fileRecved)
{
        int             ret;
        int             sckHDL = -1;

        char tmpBuf[8192+1];
        int  tmpfile;
        int len;
        int responseCode;


        if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0) || (tellerNo == NULL) || (tellerNo == NULL))
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile parameter error!\n");
                return(errCodeParameter);
        }

        gunionIsMngSvrReturnedError = 0;        // 2007/11/22����


        if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionCreateSocketClient! [%s] [%d]\n",ipAddr,port);
                return(sckHDL);
        }

	// �������
	memset(tmpBuf, 0, sizeof tmpBuf);
        ret = UnionPackMngSvrRequestPackage(reqStr,lenOfReqStr,tellerNo,resID,serviceID,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSendRequestPackage:: UnionPackMngSvrRequestPackage!\n");
                goto errorExit;
	}
        tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2+ret)) < 0)
	{
		UnionUserErrLog("in UnionSendRequestPackage:: UnionSendToSocket!\n");
                goto errorExit;
	}

        //����һ����Ӧ��
	memset(tmpBuf, 0, sizeof tmpBuf);
        if( (ret = UnionReceiveDataPackage(sckHDL,tmpBuf,sizeof(tmpBuf),&tmpfile)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionReceiveDataPackage not response packge!");
                goto errorExit;
        }

        //�����ļ�
        if ((ret = UnionTransferMngDataFile(sckHDL,fileName,tellerNo,resID)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionTransferMngDataFile!\n");
                goto errorExit;
        }
#ifdef __UserProgressBar__
         if( gunionShowProgressBar )
                UnionShowMainFormProgressBar();
#endif
        // ��������
        if( (ret = UnionReceiveDataPackage(sckHDL,resStr,sizeOfResStr,fileRecved)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile::UnionReceiveDataPackage error!\n");
        }
#ifdef __UserProgressBar__
        if( gunionShowProgressBar )
                UnionHideMainFormProgressBar();
#endif
errorExit:
        UnionCloseSocket(sckHDL);
        return(ret);
}

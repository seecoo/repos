//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <setjmp.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionSocket.h"
#include "unionMDLID.h"

int		gunionLenOfHsmHeader = 8;
int		gunionTimeoutOfHsm = 10;
int		gunionHsmSckHDL = -1;
int             gunionLenOfHsmCmdHeader = 2;
int             gunionIsNotCheckHsmResCode = 0;
char            gunionCmdPrintType[5+1] = "";

jmp_buf		gsjl06CommJmpEnv;

int UnionErrCodeSwitchChnExplain(char *errCode,char *outStr,int sizeofOutStr);

void UnionDealSJL06Timeout()
{
	UnionUserErrLog("in UnionDealSJL06Timeout:: HSM time out!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gsjl06CommJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gsjl06CommJmpEnv,10);
#endif
}

void UnionSetTimeoutOfHsm(int timeout)
{
	gunionTimeoutOfHsm = timeout;
}

void UnionSetLenOfHsmCmdHeader(int len)
{
	gunionLenOfHsmHeader = len;
}

// ʹ��BCD��չ��ӡHSM����ָ��
void UnionSetBCDPrintTypeForHSMReqCmd()
{
        strcpy(gunionCmdPrintType, "PRTT1");
        return;
}
// ʹ��BCD��չ��ӡHSM��Ӧָ��
void UnionSetBCDPrintTypeForHSMResCmd()
{
        strcpy(gunionCmdPrintType, "PRTT2");
        return;
}
// ʹ��BCD��չ��ӡHSMָ��
void UnionSetBCDPrintTypeForHSMCmd()
{
        strcpy(gunionCmdPrintType, "PRTT3");
        return;
}

// ʹ��***��ӡHSM����ָ�����
void UnionSetMaskPrintTypeForHSMReqCmd()
{
        strcpy(gunionCmdPrintType, "PRTT4");
        return;
}
// ʹ��***��ӡHSM��Ӧָ�����
void UnionSetMaskPrintTypeForHSMResCmd()
{
        strcpy(gunionCmdPrintType, "PRTT8");
        return;
}
// ʹ��***��ӡHSMָ�����
void UnionSetMaskPrintTypeForHSMCmd()
{
        strcpy(gunionCmdPrintType, "PRTT<");
        return;
}

void UnionResetPrintTypeForHSMCmd()
{
        memset(gunionCmdPrintType,0,sizeof(gunionCmdPrintType));
        return;
}
char *UnionGetHsmCmdPrintTypeStr()
{
        return(gunionCmdPrintType);
}

// ���ò���Ҫ�����ܻ���Ӧ��
void UnionSetIsNotCheckHsmResCode()
{
        gunionIsNotCheckHsmResCode = 1;
}

// ������Ҫ�����ܻ���Ӧ��
void UnionSetIsCheckHsmResCode()
{
        gunionIsNotCheckHsmResCode = 0;
}


int UnionConnectHsm(char *hsmIPAddr,int hsmPort)
{
	if (gunionHsmSckHDL > 0)
		return(gunionHsmSckHDL);
	
	if ((gunionHsmSckHDL = UnionCreateSocketClient(hsmIPAddr,hsmPort)) < 0)
	{
		UnionUserErrLog("in UnionConnectHsm:: UnionCreateSocketClient [%s] [%d]!\n",hsmIPAddr,hsmPort);
		return(gunionHsmSckHDL);
	}
	return(gunionHsmSckHDL);

}

int UnionCloseHsm()
{
	int		ret;
	
	if (gunionHsmSckHDL <= 0)
		return(0);
	
	if ((ret = UnionCloseSocket(gunionHsmSckHDL)) < 0)
	{
		UnionUserErrLog("in UnionCloseHsm:: UnionCloseSocket!\n");
		return(ret);
	}
	gunionHsmSckHDL = -1;
	return(ret);

}

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	int		len;
	int		lenOfHsmCmdHeader;
	unsigned char	lenBuf[2+1];
	unsigned char	tmpBuf[8192+1];
	char	tmpBuf2[64];
        char    resHsmCmdHeader[4+1];
        char    errCode[2+1];	

	if ((lenOfHsmCmdHeader = gunionLenOfHsmCmdHeader) != 2)
                gunionLenOfHsmCmdHeader = 2;

        // ��Ӧָ��ͷ
        memset(resHsmCmdHeader,0,sizeof(resHsmCmdHeader));
        memcpy(resHsmCmdHeader,reqStr,lenOfHsmCmdHeader - 1);
        resHsmCmdHeader[lenOfHsmCmdHeader - 1] = reqStr[lenOfHsmCmdHeader - 1] + 1;


	memset(tmpBuf,0,sizeof(tmpBuf));
	for (ret = 0; ret < gunionLenOfHsmHeader; ++ret)
	{
		sprintf((char*)tmpBuf + ret, "%d", ret + 1);
	}
	memcpy(tmpBuf + gunionLenOfHsmHeader,reqStr,lenOfReqStr);
	
	len = gunionLenOfHsmHeader + lenOfReqStr;
	
	//UnionProgramerLog("in UnionDirectHsmCmd:: req[%04d][%s]!\n",len,tmpBuf);

	// ���ͳ�����
	memset(lenBuf,0,sizeof(lenBuf));
	lenBuf[0] = len / 256;
	lenBuf[1] = len % 256;
	if ((ret = UnionSendToSocket(gunionHsmSckHDL,lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionSendToSocket!\n");
		return(ret);
	}
	
	// ��������
	if ((ret = UnionSendToSocket(gunionHsmSckHDL,tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionSendToSocket!\n");
		return(ret);
	}
	
	alarm(0);
	
	// �ó�ʱ����
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gsjl06CommJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gsjl06CommJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: HSM device timeout!\n");
		goto abnormalExit;
	}
	
	alarm(gunionTimeoutOfHsm);
	signal(SIGALRM,UnionDealSJL06Timeout);
	
	// ��������
	if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,lenBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionReceiveFromSocketUntilLen!\n");
		alarm(0);
		return(ret);
	}
	if ((len = lenBuf[0] * 256 + lenBuf[1]) >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionReceiveFromSocketUntilLen len [%d] larger than expected [%zu]!\n",len,sizeof(tmpBuf));
		alarm(0);
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,tmpBuf,len)) != len)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionReceiveFromSocketUntilLen!\n");
		alarm(0);
		return(ret);
	}
	alarm(0);
	//UnionProgramerLog("in UnionDirectHsmCmd:: res[%04d][%s]!\n",len,tmpBuf);

        if (!gunionIsNotCheckHsmResCode)
        {
                // �����Ӧ��
                memset(errCode,0,sizeof(errCode));
                memcpy(errCode,tmpBuf + lenOfHsmCmdHeader + gunionLenOfHsmHeader,2);
                if (memcmp(errCode,"00",2) != 0)
                {

                        memset(tmpBuf2,0,sizeof(tmpBuf2));
                        UnionErrCodeSwitchChnExplain(errCode,tmpBuf2,sizeof(tmpBuf2));
                        UnionUserErrLog("in UnionDirectHsmCmd:: resStr = [%s][%s]!\n",tmpBuf,tmpBuf2);
                        //return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
			return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
                }
        }
	memcpy(resStr,tmpBuf+gunionLenOfHsmHeader,len - gunionLenOfHsmHeader);
	alarm(0);
	return(len - gunionLenOfHsmHeader);

abnormalExit:
	alarm(0);
	return(errCodeSJL06MDL_SJL06Abnormal);

}

int UnionErrCodeSwitchChnExplain(char *errCode,char *outStr,int sizeofOutStr)
{
        int     errNum = 0;
        char    tmpBuf[128+1];

        if (errCode == NULL && outStr == NULL)
        {
                UnionUserErrLog("in UnionErrCodeSwitchChnExplain:: errCode or outStr is NULL!\n");
                return(-1);
        }

        memset(tmpBuf,0,sizeof(tmpBuf));
//      memcpy(tmpBuf,errCode + 2,2);
        errNum = atoi(errCode);

        switch(errNum)
        {
                case    1 : strcpy(tmpBuf,"��Կ��żУ����󾯸�");break;
                case    2 : strcpy(tmpBuf,"��Կ���ȴ���");break;
                case    4 : strcpy(tmpBuf,"��Ч��Կ���ʹ���");break;
                case    5 : strcpy(tmpBuf,"��Ч��Կ���ȱ�ʶ");break;
                case    10 : strcpy(tmpBuf,"Դ��Կ��żУ�����");break;
                case    11 : strcpy(tmpBuf,"Ŀ����Կ��żУ�����");break;
                case    12 : strcpy(tmpBuf,"�û��洢�����������Ч���������������д");break;
                case    13 : strcpy(tmpBuf,"����Կ��żУ�����");break;
                case    14 : strcpy(tmpBuf,"LMK��02-03�����µ�PINʧЧ");break;
                case    15 : strcpy(tmpBuf,"��Ч����������");break;
                case    16 : strcpy(tmpBuf,"����̨���ӡ��û��׼���û���û�����Ӻ�");break;
                case    17 : strcpy(tmpBuf,"HSM������Ȩ״̬�����߲������PIN��������������������");break;
                case    18 : strcpy(tmpBuf,"û��װ���ĵ���ʽ����");break;
                case    19 : strcpy(tmpBuf,"ָ����Diebold����Ч");break;
                case    20 : strcpy(tmpBuf,"PIN��û�а�����Чֵ");break;
                case    21 : strcpy(tmpBuf,"��Ч������ֵ����������/�������������");break;
                case    22 : strcpy(tmpBuf,"��Ч���ʺ�");break;
                case    23 : strcpy(tmpBuf,"��Ч��PIN���ʽ����");break;
                case    24 : strcpy(tmpBuf,"PIN����С��4�����12");break;
                case    25 : strcpy(tmpBuf,"ʮ���Ʊ����");break;
                case    26 : strcpy(tmpBuf,"��Ч����Կ����");break;
                case    27 : strcpy(tmpBuf,"��ƥ�����Կ����");break;
                case    28 : strcpy(tmpBuf,"��Ч����Կ����");break;
                case    29 : strcpy(tmpBuf,"��Կ��������ֹ");break;
                case    30 : strcpy(tmpBuf,"�ο�����Ч");break;
                case    31 : strcpy(tmpBuf,"û���㹻������������ṩ��������");break;
                case    33 : strcpy(tmpBuf,"LMK��Կת���洢�����ƻ�");break;
                case    40 : strcpy(tmpBuf,"��Ч�Ĺ̼�У���");break;
                case    41 : strcpy(tmpBuf,"�ڲ���Ӳ��/�����RAM�ѻ�����Ч�Ĵ������ȵ�");break;
                case    42 : strcpy(tmpBuf,"DES����");break;
                case    46 : strcpy(tmpBuf,"�������ģ��");break;
                case    47 : strcpy(tmpBuf,"DSP����;���������Ա");break;
                case    49 : strcpy(tmpBuf,"˽Կ����;���������Ա");break;
                case    60 : strcpy(tmpBuf,"�޴�����");break;
                case    74 : strcpy(tmpBuf,"��ЧժҪ��Ϣ�﷨(�����޹�ϣģʽ)");break;
                case    75 : strcpy(tmpBuf,"��Ч��Կ/˽Կ��");break;
                case    76 : strcpy(tmpBuf,"��Կ���ȴ���");break;
                case    77 : strcpy(tmpBuf,"�������ݿ����");break;
		case    78 : strcpy(tmpBuf,"˽Կ���ȴ���");break;
		case    79 : strcpy(tmpBuf,"��ϣ�㷨�����ʶ����");break;
		case    80 : strcpy(tmpBuf,"���ݳ��ȴ���");break;
		case    81 : strcpy(tmpBuf,"֤��ƫ��ֵ�볤�ȴ�");break;
		case    82 : strcpy(tmpBuf,"�����Ų��ڷ�Χ��");break;
		case    85 : strcpy(tmpBuf,"��Կ������");break;
		case    90 : strcpy(tmpBuf,"HSM���յ�������Ϣ�е�������żУ���");break;
		case    91 : strcpy(tmpBuf,"��������У��(LRC)�ַ���ƥ��������������������ֵ");break;
		case    92 : strcpy(tmpBuf,"����ֵ(����/������)������Ч��Χ�ڣ����߲���ȷ");break;
		default :
			     UnionUserErrLog("in UnionErrCodeSwitchChnExplain:: errCode[%s] not found!\n",tmpBuf);
			     return(errCodeParameter);
	}

	if (sizeofOutStr < strlen(tmpBuf))
	{
		UnionUserErrLog("in UnionErrCodeSwitchChnExplain:: strlen(tmpBuf)[%zu] > sizeofOutStr[%d]!\n",strlen(tmpBuf),sizeofOutStr);
		return(errCodeParameter);
	}
	else
		strcpy(outStr,tmpBuf);

	return 0;
}


//	Author:		张永定
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

// 使用BCD扩展打印HSM请求指令
void UnionSetBCDPrintTypeForHSMReqCmd()
{
        strcpy(gunionCmdPrintType, "PRTT1");
        return;
}
// 使用BCD扩展打印HSM响应指令
void UnionSetBCDPrintTypeForHSMResCmd()
{
        strcpy(gunionCmdPrintType, "PRTT2");
        return;
}
// 使用BCD扩展打印HSM指令
void UnionSetBCDPrintTypeForHSMCmd()
{
        strcpy(gunionCmdPrintType, "PRTT3");
        return;
}

// 使用***打印HSM请求指令报文体
void UnionSetMaskPrintTypeForHSMReqCmd()
{
        strcpy(gunionCmdPrintType, "PRTT4");
        return;
}
// 使用***打印HSM响应指令报文体
void UnionSetMaskPrintTypeForHSMResCmd()
{
        strcpy(gunionCmdPrintType, "PRTT8");
        return;
}
// 使用***打印HSM指令报文体
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

// 设置不需要检查加密机响应码
void UnionSetIsNotCheckHsmResCode()
{
        gunionIsNotCheckHsmResCode = 1;
}

// 设置需要检查加密机响应码
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

        // 响应指令头
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

	// 发送长度域
	memset(lenBuf,0,sizeof(lenBuf));
	lenBuf[0] = len / 256;
	lenBuf[1] = len % 256;
	if ((ret = UnionSendToSocket(gunionHsmSckHDL,lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionSendToSocket!\n");
		return(ret);
	}
	
	// 发送正文
	if ((ret = UnionSendToSocket(gunionHsmSckHDL,tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionHsmSvrTask:: UnionSendToSocket!\n");
		return(ret);
	}
	
	alarm(0);
	
	// 置超时机制
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
	
	// 接收正文
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
                // 检查响应码
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
                case    1 : strcpy(tmpBuf,"密钥奇偶校验错误警告");break;
                case    2 : strcpy(tmpBuf,"密钥长度错误");break;
                case    4 : strcpy(tmpBuf,"无效密钥类型错误");break;
                case    5 : strcpy(tmpBuf,"无效密钥长度标识");break;
                case    10 : strcpy(tmpBuf,"源密钥奇偶校验错误");break;
                case    11 : strcpy(tmpBuf,"目的密钥奇偶校验错误");break;
                case    12 : strcpy(tmpBuf,"用户存储区域的内容无效、重启、掉电或重写");break;
                case    13 : strcpy(tmpBuf,"主密钥奇偶校验错误");break;
                case    14 : strcpy(tmpBuf,"LMK对02-03加密下的PIN失效");break;
                case    15 : strcpy(tmpBuf,"无效的输入数据");break;
                case    16 : strcpy(tmpBuf,"控制台或打印机没有准备好或者没有连接好");break;
                case    17 : strcpy(tmpBuf,"HSM不在授权状态，或者不能清除PIN输出，或者两种情况都有");break;
                case    18 : strcpy(tmpBuf,"没有装载文档格式定义");break;
                case    19 : strcpy(tmpBuf,"指定的Diebold表无效");break;
                case    20 : strcpy(tmpBuf,"PIN块没有包含有效值");break;
                case    21 : strcpy(tmpBuf,"无效的索引值，或者索引/块数导致了溢出");break;
                case    22 : strcpy(tmpBuf,"无效的帐号");break;
                case    23 : strcpy(tmpBuf,"无效的PIN块格式代码");break;
                case    24 : strcpy(tmpBuf,"PIN长度小于4或大于12");break;
                case    25 : strcpy(tmpBuf,"十进制表错误");break;
                case    26 : strcpy(tmpBuf,"无效的密钥方案");break;
                case    27 : strcpy(tmpBuf,"不匹配的密钥长度");break;
                case    28 : strcpy(tmpBuf,"无效的密钥类型");break;
                case    29 : strcpy(tmpBuf,"密钥函数被禁止");break;
                case    30 : strcpy(tmpBuf,"参考数无效");break;
                case    31 : strcpy(tmpBuf,"没有足够的请求入口以提供批量处理");break;
                case    33 : strcpy(tmpBuf,"LMK密钥转换存储区被破坏");break;
                case    40 : strcpy(tmpBuf,"无效的固件校验和");break;
                case    41 : strcpy(tmpBuf,"内部的硬件/软件错：RAM已坏，无效的错误代码等等");break;
                case    42 : strcpy(tmpBuf,"DES错误");break;
                case    46 : strcpy(tmpBuf,"超过最大模长");break;
                case    47 : strcpy(tmpBuf,"DSP错误;报告给管理员");break;
                case    49 : strcpy(tmpBuf,"私钥错误;报告给管理员");break;
                case    60 : strcpy(tmpBuf,"无此命令");break;
                case    74 : strcpy(tmpBuf,"无效摘要信息语法(仅仅无哈希模式)");break;
                case    75 : strcpy(tmpBuf,"无效公钥/私钥对");break;
                case    76 : strcpy(tmpBuf,"公钥长度错误");break;
                case    77 : strcpy(tmpBuf,"明文数据块错误");break;
		case    78 : strcpy(tmpBuf,"私钥长度错误");break;
		case    79 : strcpy(tmpBuf,"哈希算法对象标识错误");break;
		case    80 : strcpy(tmpBuf,"数据长度错误");break;
		case    81 : strcpy(tmpBuf,"证书偏移值与长度错");break;
		case    82 : strcpy(tmpBuf,"索引号不在范围内");break;
		case    85 : strcpy(tmpBuf,"公钥不存在");break;
		case    90 : strcpy(tmpBuf,"HSM接收的请求信息中的数据奇偶校验错");break;
		case    91 : strcpy(tmpBuf,"纵向冗余校验(LRC)字符不匹配对输入数据所计算出的值");break;
		case    92 : strcpy(tmpBuf,"计数值(命令/数据域)不在有效范围内，或者不正确");break;
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


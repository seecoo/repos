//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2006/08/08
//	Version:	1.0

//	除非特殊说明，函数返回0，表示正确执行，返回<0表示出错代码。

#ifndef _UnionSJL06_2_x_Above_
#define _UnionSJL06_2_x_Above_
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "unionErrCode.h"
#include "UnionLog.h"

jmp_buf			gsjl06CommJmpEnv;
//int			gsjl06CommJmpSet = 0;

// 2008/6/20增加，检查是否存在pin明文
/*
输入参数
	cmdStr		命令串（不包括消息头）
	lenOfCmdStr	命令串长度
输出参数
	仅当命令串中今有pin明文时，才有输出参数
	plainPinOffset	pin明文在串中的偏移
	plainPinLen	pin明文的长度
	plainPin	pin明文
返回值
	1,		有pin明文
	0,		无pin明文
*/
int UnionIsPlainPinContained(char *cmdStr,int lenOfCmdStr,int *plainPinOffset,int *plainPinLen,char *plainPin)
{
	int	index;
	
	if (strncmp(cmdStr,"BA",2) == 0)
	{
		*plainPinOffset = 2;
		for (index = *plainPinOffset;index < lenOfCmdStr; index++)
		{
			if (cmdStr[index] == 'F')
				break;
		}
		if ((*plainPinLen = index - 2) > 12)
			*plainPinLen = 12;
	}
	else if (strncmp(cmdStr,"NH00",4) == 0)
	{
		*plainPinOffset = 4;
		for (index = *plainPinOffset; index < lenOfCmdStr; index++)
		{
			if (cmdStr[index] == 'F')
				break;
		}
		if ((*plainPinLen = index - 4) > 12)
			*plainPinLen = 12;
	}
	else if (strncmp(cmdStr,"60",2) == 0)
	{
		*plainPinOffset = 2 + 1 + 3;
		switch (cmdStr[*plainPinOffset])
		{
			case	'X':
				*plainPinOffset += (1+16);
				break;
			case	'Y':
				*plainPinOffset += (1+32);
				break;
			case	'Z':
				*plainPinOffset += (1+48);
				break;
			default:
				*plainPinOffset += 16;
				break;
		}
		*plainPinLen = 16;
	}
	else if (strncmp(cmdStr,"62",2) == 0)
	{
		*plainPinOffset = 2 + 1 + 3;
		*plainPinLen = 16;
	}
	else if (strncmp(cmdStr,"6900",4) == 0)
	{
		*plainPinOffset = 4;
		*plainPinLen = 16;
	}
	else
		return(0);
	if (*plainPinLen + *plainPinOffset > lenOfCmdStr)
		return(0);
	memcpy(plainPin,cmdStr+*plainPinOffset,*plainPinLen);
	return(1);
}
		
// 2008/6/6增加，Wolfgang Wang
int gunionIsSJL06LongConnAbnormal = 0;
// 检查当前连接是否正常
int UnionIsThisSJL06LongConnAbnormal()
{
	//UnionAuditLog("gunionIsSJL06LongConnAbnormal = [%d]\n",gunionIsSJL06LongConnAbnormal);
	return(gunionIsSJL06LongConnAbnormal);
}
// 设置当前连接正常
int UnionSetThisSJL06LongConnOK()
{
	UnionAuditLog("set SJL06LongConn OK!\n");
	gunionIsSJL06LongConnAbnormal = 0;
}
// 设置当前连接异常
int UnionSetThisSJL06LongConnAbnormal()
{
	UnionAuditLog("set SJL06LongConn Abnormal!\n");
	gunionIsSJL06LongConnAbnormal = 1;
}
// 2008/6/6增加结束

void UnionDealSJL06Timeout();

int UnionShortConnSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	int		sckConn;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionShortConnSJL06Cmd:: wrong parameter passed!\n");
		return(errCodeParameter);
	}
	if ((sckConn = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
	{
		UnionUserErrLog("in UnionShortConnSJL06Cmd:: UnionCreateSocketClient [%s] [%d]!\n",pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port);
		return(sckConn);
	}

	if ((ret = UnionLongConnSJL06Cmd(sckConn,pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionShortConnSJL06Cmd:: UnionLongConnSJL06Cmd!\n");
	}
	
	UnionCloseSocket(sckConn);
	return(ret);

}

int UnionLongConnSJL06Cmd(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	unsigned char	tmpBuf[8096+1];
	int		hsmStrLen;
	unsigned char	*p;
	int		timeout;
	int		retLen;
	char		plainPin[48+1];
	int		plainPinOffset;
	int		plainPinLen;
	int		plainPinExists = 0;
	int   trysendFlag = 0;  //add by hzh in 2011.11.7, 增加重新发送标志

		
	if (sckConn < 0)
	{
		UnionLog("in UnionLongConnSJL06Cmd:: hsm connect is short!\n");
		return(UnionShortConnSJL06Cmd(pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	}

	if ((pSJL06 == NULL) || (reqStr == NULL) || (resStr == NULL) || (sizeOfResStr <= 0) || (lenOfReqStr <= 0))
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: wrong parameter passed! [%x] [%x] [%x] [%d] [%d]\n",
				pSJL06,reqStr,resStr,sizeOfResStr,lenOfReqStr);
		return(errCodeParameter);
	}

  // 2009/5/6，增加
	memset(plainPin,0,sizeof(plainPin));
	if (plainPinExists = UnionIsPlainPinContained(reqStr,lenOfReqStr,&plainPinOffset,&plainPinLen,plainPin))
	{
		memset(reqStr+plainPinOffset,'*',plainPinLen);
	}
	// 2009/5/6增加结束
	
#ifndef _spyHsmCmd_
	reqStr[lenOfReqStr] = 0;
	UnionAuditNullLog("[%15s]::[%04d][%s]\n",pSJL06->staticAttr.ipAddr,lenOfReqStr,reqStr);
#else
	UnionSendHsmCmdRequestInfoToTransSpier(lenOfReqStr,reqStr);
#endif	

	// 2009/5/6，增加
	if (plainPinExists)
	{
		memcpy(reqStr+plainPinOffset,plainPin,plainPinLen);
	}
	// 2009/5/6增加结束
	
	//UnionProgramerLog("in UnionLongConnSJL06Cmd:: [%s] status [%c], conOnlineSJL06 = [%c] !\n", pSJL06->staticAttr.ipAddr, pSJL06->dynamicAttr.status, conOnlineSJL06);
	if (pSJL06->dynamicAttr.status != conOnlineSJL06)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: [%s] status is not online!, status[%c] != conOnlineSJL06[%c] !\n", pSJL06->staticAttr.ipAddr, pSJL06->dynamicAttr.status, conOnlineSJL06);
		return(errCodeSJL06MDL_SJL06StillNotOnline);
	}
		
	if (lenOfReqStr + pSJL06->staticAttr.lenOfMsgHeader + 2 > sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: too long command!\n");
		return(errCodeParameter);
	}
	

	memset(tmpBuf,0,sizeof(tmpBuf));
	// 拼装HSM指令	
	p = tmpBuf + 2;
	// 拷贝消息头
	if (pSJL06->staticAttr.lenOfMsgHeader > strlen(pSJL06->staticAttr.msgHeader))
	{
		memset(p, '0', pSJL06->staticAttr.lenOfMsgHeader);
		memcpy(p,pSJL06->staticAttr.msgHeader, strlen(pSJL06->staticAttr.msgHeader));
	}
	else
		memcpy(p,pSJL06->staticAttr.msgHeader, pSJL06->staticAttr.lenOfMsgHeader);
	// 拷贝命令字串
	p += pSJL06->staticAttr.lenOfMsgHeader;
	memcpy(p,reqStr,lenOfReqStr);
	
	hsmStrLen = pSJL06->staticAttr.lenOfMsgHeader + lenOfReqStr;

	// 2010-08-09,xusj增加
#ifdef _client_use_ebcdic_
                UnionAsciiToEbcdic(tmpBuf+2,tmpBuf+2,hsmStrLen);
		UnionLog("in UnionLongConnSJL06Cmd:: ebcdic hsmCmdBuf = [%s]\n", tmpBuf+2);
#endif  // 2007-11-29,xusj增加结束

	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		tmpBuf[0] = hsmStrLen / 256;
		tmpBuf[1] = hsmStrLen % 256;
		hsmStrLen += 2;
		p = tmpBuf;
	}
	else
	{
		p = tmpBuf + 2;
	}
	// 拼装指令结束，p指向发往密码机的字串
	
	// 置超时机制
#ifdef _LINUX_
	if (sigsetjmp(gsjl06CommJmpEnv,1) != 0)
#else
	if (setjmp(gsjl06CommJmpEnv) != 0)
#endif
	{
		//alarm(0);
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: HSM device timeout! [%15s]\n",pSJL06->staticAttr.ipAddr);
		++(pSJL06->dynamicAttr.timeoutTimes);
		goto abnormalExit;
	}
	if ((timeout = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 0)
		timeout = 20;
	alarm(timeout);
	signal(SIGALRM,UnionDealSJL06Timeout);
	// 置超时机制结束
	
	//UnionLog("in UnionLongConnSJL06Cmd::timeoutOfHsm::[%d].\n",timeout);

	// 与密码机通讯
	/*
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionSendToSocket Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
	}
	*/
	
	//modify by hzh in 2011.11.7, 当加密机的tcp服务连接被关闭时，hsmTask的长连接已失效。
	//需UnionSetThisSJL06LongConnAbnormal，并重建(短)连接，以完成该笔交易。
	// 与密码机通讯
trysend:	
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionSetThisSJL06LongConnAbnormal();
		if (!trysendFlag) 
		{
		UnionDebugLog("in UnionLongConnSJL06Cmd::UnionSendToSocket failed. try connect.\n");
		UnionCloseSocket(sckConn);
		if ((sckConn = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionCreateSocketClient [%s] [%d]!\n",pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;;
		}
		//由于UnionCreateSocketClient里面有alarm，及signal，这里需重新设置;
		alarm(timeout);
		signal(SIGALRM,UnionDealSJL06Timeout);
		trysendFlag = 1;
		goto trysend;
		}  
		else {
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionSendToSocket Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
	}
	//modify end
	
	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf,2)) != 2)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if (((retLen = tmpBuf[0] * 256 + tmpBuf[1]) <= 0) || (retLen >= sizeof(tmpBuf)-2))
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: [%15s] retLen = [%d]\n",pSJL06->staticAttr.ipAddr,retLen);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf+2,retLen)) != retLen)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		ret += 2;
	}
	else
	{
		if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionReceiveFromSocket Error!\n");
			++pSJL06->dynamicAttr.connFailTimes;
			goto abnormalExit;
		}
	}
	tmpBuf[ret] = 0;

	// 2010-08-09,xusj增加
#ifdef _client_use_ebcdic_
		UnionLog("in UnionLongConnSJL06Cmd:: ebcdic hsmCmdRes = [%s]\n", tmpBuf+2);
                UnionEbcdicToAscii(tmpBuf+pSJL06->staticAttr.lenOfLenFld,tmpBuf+pSJL06->staticAttr.lenOfLenFld,ret-pSJL06->staticAttr.lenOfLenFld);
		UnionLog("in UnionLongConnSJL06Cmd:: asc hsmCmdRes = [%s]\n", tmpBuf+2);
#endif  // 2007-11-29,xusj增加结束

	// 获得输出结果
	if ((hsmStrLen = ret - pSJL06->staticAttr.lenOfLenFld - pSJL06->staticAttr.lenOfMsgHeader) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: Invalid efficent len [%d] hsmReturnedLen = [%d] retLen = [%d]\n",hsmStrLen,ret,retLen);
		++(pSJL06->dynamicAttr.connFailTimes);
		goto abnormalExit;
	}
	else
	{
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: small receive buffer!\n");
			//add in 2011.11.7
			alarm(0);
			if(trysendFlag) 
				UnionCloseSocket(sckConn);
			//add end
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader,hsmStrLen);
		if (strncmp(resStr+2,"00",2) == 0)
		{
			if (++(pSJL06->dynamicAttr.normalCmdTimes) < 0)
				pSJL06->dynamicAttr.normalCmdTimes = 0;
		}
		else
		{
			if (++(pSJL06->dynamicAttr.abnormalCmdTimes) < 0)
				pSJL06->dynamicAttr.abnormalCmdTimes = 0;
		}
		
	// 2009/5/6，增加
	memset(plainPin,0,sizeof(plainPin));
	if (plainPinExists = UnionIsPlainPinContained(resStr,hsmStrLen,&plainPinOffset,&plainPinLen,plainPin))
	{
		memset(resStr+plainPinOffset,'*',plainPinLen);	
	}
	// 2009/5/6增加结束
	
#ifndef _spyHsmCmd_
		resStr[hsmStrLen] = 0;
		UnionAuditNullLog("[%15s]::[%04d][%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen,resStr);
#else
		UnionSendHsmCmdResponseInfoToTransSpier(hsmStrLen,resStr);
#endif

  // 2009/5/6，增加
	if (plainPinExists)
	{
		memcpy(resStr+plainPinOffset,plainPin,plainPinLen);
	}
	// 2009/5/6增加结束
	}
	
	//add in 2011.11.7
	if(trysendFlag) 
		UnionCloseSocket(sckConn);
	//add end
			
	alarm(0);
	return(hsmStrLen);

abnormalExit:
	
	//add in 2011.11.7
	if(trysendFlag) 
		UnionCloseSocket(sckConn);
	//add end
	
	alarm(0);
	reqStr[lenOfReqStr] = 0;
	UnionUserErrLog("in UnionLongConnSJL06Cmd:: reqLen = [%04d] reqStr = [%s]\n",lenOfReqStr,reqStr);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	UnionSetThisSJL06LongConnAbnormal();
	return(errCodeSJL06MDL_SJL06Abnormal);
}

int UnionLongConn2HexLenSJL06Cmd(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	unsigned char	tmpBuf[8096+1];
	int		retLen;
	int		hsmStrLen;
	unsigned char	*p;
	int		timeout;
	// 2008/6/20增加
	char		plainPin[48+1];
	int		plainPinOffset;
	int		plainPinLen;
	int		plainPinExists = 0;
	// 2008/6/20增加结束
		
	if (sckConn < 0)
		return(UnionShortConn2HexLenSJL06Cmd(pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr));
		
	if ((pSJL06 == NULL) || (reqStr == NULL) || (resStr == NULL) || (sizeOfResStr <= 0) || (lenOfReqStr <= 0))
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: wrong parameter passed!\n");
		return(errCodeParameter);
	}

	
	memset(tmpBuf,0,sizeof(tmpBuf));
	// 拼装HSM指令	
	p = tmpBuf + 2;
	memcpy(p,reqStr,lenOfReqStr);
	hsmStrLen = lenOfReqStr;
	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		tmpBuf[0] = hsmStrLen / 256;
		tmpBuf[1] = hsmStrLen % 256;
		hsmStrLen += 2;
		p = tmpBuf;
	}
	else
	{
		p = tmpBuf + 2;
	}

	// 2008/6/20，增加
	memset(plainPin,0,sizeof(plainPin));
	if (plainPinExists = UnionIsPlainPinContained(p+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader,
			hsmStrLen-pSJL06->staticAttr.lenOfLenFld-pSJL06->staticAttr.lenOfMsgHeader,&plainPinOffset,&plainPinLen,plainPin))
	{
		memset(p+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader+plainPinOffset,'*',plainPinLen);
	}
	// 2008/6/20增加结束
	
#ifndef _spyHsmCmd_
	UnionAuditNullLog("[%15s]::[%04d][%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen-pSJL06->staticAttr.lenOfLenFld,p+pSJL06->staticAttr.lenOfLenFld);
#else
	UnionSendHsmCmdRequestInfoToTransSpier(hsmStrLen-pSJL06->staticAttr.lenOfLenFld,p+pSJL06->staticAttr.lenOfLenFld);
#endif
	// 拼装指令结束，p指向发往密码机的字串

	// 2008/6/20，增加
	if (plainPinExists)
	{
		memcpy(p+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader+plainPinOffset,plainPin,plainPinLen);
	}
	// 2008/6/20增加结束
	
#ifdef _LINUX_
	if (sigsetjmp(gsjl06CommJmpEnv,1) != 0)
#else
	if (setjmp(gsjl06CommJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: HSM device timeout!  [%15s]\n",pSJL06->staticAttr.ipAddr);
		++(pSJL06->dynamicAttr.timeoutTimes);
		//alarm(0);
		goto abnormalExit;
	}
	if ((timeout = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 0)
		timeout = 20;
	alarm(timeout);
	signal(SIGALRM,UnionDealSJL06Timeout);
	
	// 置超时机制结束
	
	// 与密码机通讯
	//p[hsmStrLen] = 0;
	//UnionLog("in UnionLongConn2HexLenSJL06Cmd:: [%04d][%s]\n",hsmStrLen,p);
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionSendToSocket Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
		++(pSJL06->dynamicAttr.connFailTimes);
		goto abnormalExit;
	}
	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if (((retLen = tmpBuf[0] * 256 + tmpBuf[1]) <= 0) || (retLen >= sizeof(tmpBuf)-2))
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: [%15s] retLen = [%d]\n",pSJL06->staticAttr.ipAddr,retLen);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf+2,retLen)) < 0)
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		ret += 2;
	}
	else
	{
		if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionReceiveFromSocket Error!\n");
			++pSJL06->dynamicAttr.connFailTimes;
			goto abnormalExit;
		}
	}
	tmpBuf[ret] = 0;
	
	// 获得输出结果
	if ((hsmStrLen = ret - pSJL06->staticAttr.lenOfLenFld) < 0)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: Invalid efficent len [%d] hsmReturnedLen = [%d]\n",hsmStrLen,ret);
		++(pSJL06->dynamicAttr.connFailTimes);
		goto abnormalExit;
	}
	else
	{
		++(pSJL06->dynamicAttr.normalCmdTimes);
		if (pSJL06->dynamicAttr.normalCmdTimes < 0)
			pSJL06->dynamicAttr.normalCmdTimes = 0;
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: small receive buffer!\n");
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + pSJL06->staticAttr.lenOfLenFld,hsmStrLen);

		// 2008/6/20，增加
		memset(plainPin,0,sizeof(plainPin));
		if (plainPinExists = UnionIsPlainPinContained(tmpBuf+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader,
			hsmStrLen-pSJL06->staticAttr.lenOfMsgHeader,&plainPinOffset,&plainPinLen,plainPin))
		{
			memset(tmpBuf+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader+plainPinOffset,'*',plainPinLen);
		}
		// 2008/6/20增加结束


	
#ifndef _spyHsmCmd_
		UnionAuditNullLog("[%15s]::[%04d][%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader);
#else
		UnionSendHsmCmdResponseInfoToTransSpier(hsmStrLen,tmpBuf + pSJL06->staticAttr.lenOfLenFld);
#endif

		// 2008/6/20，增加
		if (plainPinExists)
		{
			memcpy(tmpBuf+pSJL06->staticAttr.lenOfLenFld+pSJL06->staticAttr.lenOfMsgHeader+plainPinOffset,plainPin,plainPinLen);
		}
		// 2008/6/20增加结束
	}
	alarm(0);
	return(hsmStrLen);

abnormalExit:
	alarm(0);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	UnionSetThisSJL06LongConnAbnormal();
	reqStr[lenOfReqStr] = 0;
	UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: reqLen = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
	return(errCodeSJL06MDL_SJL06Abnormal);
}

int UnionShortConn2HexLenSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	int		sckConn;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionShortConn2HexLenSJL06Cmd:: wrong parameter passed!\n");
		return(errCodeParameter);
	}
	if ((sckConn = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
	{
		UnionUserErrLog("in UnionShortConn2HexLenSJL06Cmd:: UnionCreateSocketClient [%s] [%d]!\n",pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port);
		return(sckConn);
	}

	if ((ret = UnionLongConn2HexLenSJL06Cmd(sckConn,pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionShortConn2HexLenSJL06Cmd:: UnionLongConn2HexLenSJL06Cmd!\n");
	}
	
	UnionCloseSocket(sckConn);
	return(ret);
}

int UnionShortConnSJL06CmdAnyway(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	int		sckConn;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionShortConnSJL06CmdAnyway:: wrong parameter passed!\n");
		return(errCodeParameter);
	}
	
	// added by 2012-10-18
	if(!UnionIsValidIPAddrStr(pSJL06->staticAttr.ipAddr))
	{
		ret = errCodeInvalidIPAddr;
		UnionUserErrLog("in UnionShortConnSJL06CmdAnyway:: illegal ip addr [%s] ret = [%d]!\n", pSJL06->staticAttr.ipAddr, ret);
		return(ret);
	}
	// end of addition 2012-10-18
	
	if ((sckConn = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
	{
		UnionUserErrLog("in UnionShortConnSJL06CmdAnyway:: UnionCreateSocketClient [%s] [%d]!\n",pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port);
		return(sckConn);
	}

	if ((ret = UnionLongConnSJL06CmdAnyway(sckConn,pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionShortConnSJL06CmdAnyway:: UnionLongConnSJL06CmdAnyway!\n");
	}
	
	UnionCloseSocket(sckConn);
	return(ret);

}

int UnionLongConnSJL06CmdAnyway(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	unsigned char	tmpBuf[8096+1];
	int		retLen;
	int		hsmStrLen;
	unsigned char	*p;
	int		timeout;
	
	if (sckConn < 0)
		return(UnionShortConnSJL06CmdAnyway(pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	
	if ((pSJL06 == NULL) || (reqStr == NULL) || (resStr == NULL) || (sizeOfResStr <= 0) || (lenOfReqStr <= 0))
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: wrong parameter passed!\n");
		return(errCodeParameter);
	}

	if (lenOfReqStr + pSJL06->staticAttr.lenOfMsgHeader + 2 > sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: too long command!\n");
		return(errCodeParameter);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	// 拼装HSM指令	
	p = tmpBuf + 2;
	// 拷贝消息头
	memcpy(p,pSJL06->staticAttr.msgHeader,pSJL06->staticAttr.lenOfMsgHeader);
	// 拷贝命令字串
	p += pSJL06->staticAttr.lenOfMsgHeader;
	memcpy(p,reqStr,lenOfReqStr);
	
	hsmStrLen = pSJL06->staticAttr.lenOfMsgHeader + lenOfReqStr;
	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		tmpBuf[0] = hsmStrLen / 256;
		tmpBuf[1] = hsmStrLen % 256;
		hsmStrLen += 2;
		p = tmpBuf;
	}
	else
	{
		p = tmpBuf + 2;
	}
	// 拼装指令结束，p指向发往密码机的字串
	
	/*
	if (gsjl06CommJmpSet)
	{
		UnionLog("in UnionLongConnSJL06Cmd:: Reinitialize JmpSet!\n");
		gsjl06CommJmpSet = 0;
		longjmp(gsjl06CommJmpEnv,0);
	}
	*/

#ifdef _LINUX_
	if (sigsetjmp(gsjl06CommJmpEnv,1) != 0)
#else
	if (setjmp(gsjl06CommJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: HSM device timeout! [%15s]\n",pSJL06->staticAttr.ipAddr);
		//alarm(0);
		goto abnormalExit;
	}
	if ((timeout = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 0)
		timeout = 20;
	alarm(timeout);
	signal(SIGALRM,UnionDealSJL06Timeout);
	
	// 置超时机制结束
	
	// 与密码机通讯
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionSendToSocket Error![%15s]\n",pSJL06->staticAttr.ipAddr);
		goto abnormalExit;
	}
	if (pSJL06->staticAttr.lenOfLenFld == 2)
	{
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if (((retLen = tmpBuf[0] * 256 + tmpBuf[1]) <= 0) || (retLen >= sizeof(tmpBuf) - 2))
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: [%15s] retLen = [%d]\n",pSJL06->staticAttr.ipAddr,retLen);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(sckConn,tmpBuf+2,retLen)) < 0)
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionReceiveFromSocketUntilLen Error! [%15s]\n",pSJL06->staticAttr.ipAddr);
			++(pSJL06->dynamicAttr.connFailTimes);
			goto abnormalExit;
		}
		ret += 2;
	}
	else
	{
		if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionReceiveFromSocket Error!\n");
			++pSJL06->dynamicAttr.connFailTimes;
			goto abnormalExit;
		}
	}
	tmpBuf[ret] = 0;
	
	// 获得输出结果
	if ((hsmStrLen = ret - pSJL06->staticAttr.lenOfLenFld - pSJL06->staticAttr.lenOfMsgHeader) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: Invalid efficent len [%d] hsmReturnedLen = [%d]\n",hsmStrLen,ret);
		goto abnormalExit;
	}
	else
	{
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: small receive buffer!\n");
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader,hsmStrLen);
	}
	alarm(0);
	//pSJL06->dynamicAttr.status = conOnlineSJL06;
		
	return(hsmStrLen);

abnormalExit:
	alarm(0);
	reqStr[lenOfReqStr] = 0;
	UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: reqLen = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	UnionSetThisSJL06LongConnAbnormal();
	return(errCodeSJL06MDL_SJL06Abnormal);
}

void UnionDealSJL06Timeout()
{
	//signal(SIGALRM,SIG_IGN);
	UnionUserErrLog("in UnionDealSJL06Timeout:: HSM time out!\n");
	//gsjl06CommJmpSet = 1;
#ifdef _LINUX_
	siglongjmp(gsjl06CommJmpEnv,10);
#else
	longjmp(gsjl06CommJmpEnv,10);
#endif
}

//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/12
//	Version:	1.0

//	除非特殊说明，函数返回0，表示正确执行，返回<0表示出错代码。

#include <stdio.h>
#include <string.h>
#include <signal.h>
//#include <setjmp.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "unionErrCode.h"
#include "UnionLog.h"

void UnionDealSJL06Timeout();

int gIsSJL06Timeout = 0;

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
	unsigned char	tmpBuf[4000+1];
	int		hsmStrLen;
	unsigned char	*p;

	if (sckConn < 0)
		return(UnionShortConnSJL06Cmd(pSJL06,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	
	
	if (pSJL06->dynamicAttr.status != conOnlineSJL06)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: [%s] status is not online!\n",pSJL06->staticAttr.ipAddr);
		return(errCodeSJL06MDL_SJL06StillNotOnline);
	}
		
	if ((pSJL06 == NULL) || (reqStr == NULL) || (resStr == NULL) || (sizeOfResStr <= 0) || (lenOfReqStr <= 0))
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: wrong parameter passed!\n");
		return(errCodeParameter);
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
	UnionDebugLog("in UnionLongConnSJL06Cmd::[%s]\nhsmStrLen = [%d]\nCmdStr = [%s] \n",
			pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + 2);
	// 拼装指令结束，p指向发往密码机的字串
	
	gIsSJL06Timeout = 0; 
	alarm(20);
	signal(SIGALRM,UnionDealSJL06Timeout);
	if (gIsSJL06Timeout)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: HSM device timeout! [%0x]\n",pSJL06);
		++pSJL06->dynamicAttr.timeoutTimes;
		reqStr[lenOfReqStr] = 0;
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: reqLen = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
		p[hsmStrLen+2] = 0;
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: hsmCmdLen = [%d] hsmCmdBuf = [%s]\n",hsmStrLen,p+2);
		goto abnormalExit;
	}
	
	// 置超时机制结束
	
	// 与密码机通讯
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionSendToSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: UnionReceiveFromSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	tmpBuf[ret] = 0;
	
	// 获得输出结果
	if ((hsmStrLen = ret - pSJL06->staticAttr.lenOfLenFld - pSJL06->staticAttr.lenOfMsgHeader) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06Cmd:: Invalid efficent len [%d] hsmReturnedLen = [%d]\n",hsmStrLen,ret);
		++pSJL06->dynamicAttr.abnormalCmdTimes;
		goto abnormalExit;
	}
	else
	{
		++pSJL06->dynamicAttr.normalCmdTimes;
		if (pSJL06->dynamicAttr.normalCmdTimes < 0)
			pSJL06->dynamicAttr.normalCmdTimes = 0;
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConnSJL06Cmd:: small receive buffer!\n");
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader,hsmStrLen);
		//if (memcmp(resStr,"43",2) != 0)	// Mary add for decrypt command 42, 2002-12-5
		UnionDebugLog("in UnionLongConnSJL06Cmd:: [%s]\nResult length=[%d]\nHSM Result=[%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader);
	}
	alarm(0);
	return(hsmStrLen);

abnormalExit:
	alarm(0);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	return(errSJL06Abnormal);
}

int UnionLongConn2HexLenSJL06Cmd(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	unsigned char	tmpBuf[2000+1];
	int		hsmStrLen;
	unsigned char	*p;

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
	tmpBuf[0] = hsmStrLen / 256;
	tmpBuf[1] = hsmStrLen % 256;
	hsmStrLen += 2;
	p = tmpBuf;

	UnionDebugLog("in UnionLongConnSJL06Cmd::[%s]\nhsmStrLen = [%d]\nCmdStr = [%s] \n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + 2);
	// 拼装指令结束，p指向发往密码机的字串
	
	gIsSJL06Timeout = 0; 
	alarm(20);
	signal(SIGALRM,UnionDealSJL06Timeout);
	if (gIsSJL06Timeout)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: HSM device timeout! [%0x]\n",pSJL06);
		++pSJL06->dynamicAttr.timeoutTimes;
		reqStr[lenOfReqStr] = 0;
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: reqLen = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
		p[hsmStrLen+2] = 0;
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: hsmCmdLen = [%d] hsmCmdBuf = [%s]\n",hsmStrLen,p+2);
		goto abnormalExit;
	}
	
	// 置超时机制结束
	
	// 与密码机通讯
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionSendToSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: UnionReceiveFromSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	tmpBuf[ret] = 0;
	
	// 获得输出结果
	if ((hsmStrLen = ret - 2) < 0)
	{
		UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: Invalid efficent len [%d] hsmReturnedLen = [%d]\n",hsmStrLen,ret);
		++pSJL06->dynamicAttr.abnormalCmdTimes;
		goto abnormalExit;
	}
	else
	{
		++pSJL06->dynamicAttr.normalCmdTimes;
		if (pSJL06->dynamicAttr.normalCmdTimes < 0)
			pSJL06->dynamicAttr.normalCmdTimes = 0;
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConn2HexLenSJL06Cmd:: small receive buffer!\n");
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + 2,hsmStrLen);
		//if (memcmp(resStr,"43",2) != 0)	// Mary add for decrypt command 42, 2002-12-5
		UnionDebugLog("in UnionLongConnSJL06Cmd:: [%s]\nResult length=[%d]\nHSM Result=[%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader);
	}
	alarm(0);
	return(hsmStrLen);

abnormalExit:
	alarm(0);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	return(errSJL06Abnormal);
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

/*
int UnionIsNormalSJL06(PUnionSJL06 pSJL06)
{
	int		ret;
	int		sckConn;
	TUnionSJL06	tmpSJL06;
	char		tmpBuf[20];
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionIsNormalSJL06:: wrong parameter passed!");
		return(0);
	}
	else
	{
		memset(&tmpSJL06,0,sizeof(tmpSJL06));
		memcpy(&tmpSJL06,pSJL06,sizeof(*pSJL06));
	}


	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionShortConnSJL06Cmd(&tmpSJL06,"01",2,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		return(0);
	}
		
	if (strncmp(tmpBuf,"0200",4) == 0)
		return(1);
	else
		return(0);
}
*/


int UnionShortConnSJL06CmdAnyway(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int		ret;
	int		sckConn;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionShortConnSJL06CmdAnyway:: wrong parameter passed!\n");
		return(errCodeParameter);
	}
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
	unsigned char	tmpBuf[4000+1];
	int		hsmStrLen;
	unsigned char	*p;

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
	UnionDebugLog("in UnionLongConnSJL06Cmd::[%s]\nhsmStrLen = [%d]\nCmdStr = [%s] \n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + 2);
	// 拼装指令结束，p指向发往密码机的字串
	
	gIsSJL06Timeout = 0; 
	alarm(20);
	signal(SIGALRM,UnionDealSJL06Timeout);
	if (gIsSJL06Timeout)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: HSM device timeout! [%0x]\n",pSJL06);
		++pSJL06->dynamicAttr.timeoutTimes;
		reqStr[lenOfReqStr] = 0;
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: reqLen = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
		p[hsmStrLen+2] = 0;
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: hsmCmdLen = [%d] hsmCmdBuf = [%s]\n",hsmStrLen,p+2);
		goto abnormalExit;
	}
	
	// 置超时机制结束
	
	// 与密码机通讯
	if ((ret = UnionSendToSocket(sckConn,p,hsmStrLen)) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionSendToSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	if ((ret = UnionReceiveFromSocket(sckConn,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: UnionReceiveFromSocket Error!\n");
		++pSJL06->dynamicAttr.connFailTimes;
		goto abnormalExit;
	}
	tmpBuf[ret] = 0;
	
	// 获得输出结果
	if ((hsmStrLen = ret - pSJL06->staticAttr.lenOfLenFld - pSJL06->staticAttr.lenOfMsgHeader) < 0)
	{
		UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: Invalid efficent len [%d] hsmReturnedLen = [%d]\n",hsmStrLen,ret);
		++pSJL06->dynamicAttr.abnormalCmdTimes;
		goto abnormalExit;
	}
	else
	{
		++pSJL06->dynamicAttr.normalCmdTimes;
		if (pSJL06->dynamicAttr.normalCmdTimes < 0)
			pSJL06->dynamicAttr.normalCmdTimes = 0;
		if (sizeOfResStr < hsmStrLen)
		{
			UnionUserErrLog("in UnionLongConnSJL06CmdAnyway:: small receive buffer!\n");
			return(errCodeParameter);
		}
		memcpy(resStr,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader,hsmStrLen);
		//if (memcmp(resStr,"43",2) != 0)	// Mary add for decrypt command 42, 2002-12-5
		UnionDebugLog("in UnionLongConnSJL06Cmd:: [%s]\nResult length=[%d]\nHSM Result=[%s]\n",pSJL06->staticAttr.ipAddr,hsmStrLen,tmpBuf + pSJL06->staticAttr.lenOfLenFld + pSJL06->staticAttr.lenOfMsgHeader);
	}
	alarm(0);
	pSJL06->dynamicAttr.status = conOnlineSJL06;
		
	return(hsmStrLen);

abnormalExit:
	alarm(0);
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	return(errSJL06Abnormal);
}
void UnionDealSJL06Timeout()
{
	gIsSJL06Timeout = 1;
	return;
}


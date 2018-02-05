#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionSocket.h"
#include "UnionLog.h"
#include "unionMonitorResID.h"
#include "unionTransInfoToMonitor.h"
#include "transSpierBuf.h"

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif // _UnionEnv_3_x_
#include "UnionEnv.h"
#include "UnionStr.h"

#ifndef conMaxNumOfTransSpierConn
#define conMaxNumOfTransSpierConn       10
#endif
#define conEsscIdLen	8
extern int gunionMonitorNum;
extern TUnionConn gunionTransSpierConn[conMaxNumOfTransSpierConn];
extern int gunionLongConnWithMonitor;


int UnionPackTransInfoResponsePackage(const char *data,int lenOfData,const char *errCode, const char *esscID, int resID,int resCmd, char *buf,int sizeOfBuf)
{
	int		offset = 0;

	if ((data == NULL) || (errCode == NULL) || (esscID == NULL) || (buf == NULL))
	{
                UnionUserErrLog("in UnionPackTransInfoResponsePackage:: parameter error!\n");
                return(errCodeParameter);
	}

	// 平台ID
	if (strlen(esscID) == conEsscIdLen)
	{
		memcpy(buf, esscID, conEsscIdLen);
		offset += conEsscIdLen;
	}
	else
	{
                UnionUserErrLog("in UnionPackMngSvrRequestPackage:: parameter error!\n");
                return(errCodeParameter);
	}

        // 资源ID
	snprintf(buf + offset, sizeOfBuf - offset, "%03d", resID);
	offset += 3;

	// 请求/响应标识
	snprintf(buf + offset, sizeOfBuf - offset, "%d", resCmd);
	offset += 1;

	// 错误码
	memcpy(buf + offset, errCode, 6);
	offset += 6;

	// 报文正文
	if (lenOfData + offset >= sizeOfBuf)
	{
                UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
                return(errCodeParameter);
	}
	memcpy(buf + offset, data, lenOfData);
	offset += lenOfData;
	buf[offset] = 0;

        return(offset);
}

int UnionPackTransInfoRequestPackage(const char *data,int lenOfData,const char *esscID,int resourceID,int resCmd, char *buf,int sizeOfBuf)
{
        int                             offset = 0;

        if ((lenOfData < 0) || (data == NULL) || (buf == NULL) || (lenOfData >= 10000) || (esscID == NULL) || (sizeOfBuf < 0))
        {
                UnionUserErrLog("in UnionPackMngSvrRequestPackage:: parameter error!\n");
                return(errCodeParameter);
        }

	// 平台ID
	if (strlen(esscID) == conEsscIdLen)
	{
		memcpy(buf, esscID, conEsscIdLen);
		offset += conEsscIdLen;
	}
	else
	{
                UnionUserErrLog("in UnionPackMngSvrRequestPackage:: parameter error esscID not enght len[%d]!\n", (int)strlen(esscID));
                return(errCodeParameter);
	}

        // 资源ID
	//snprintf(buf + offset, sizeof(buf) - offset, "%03d", resourceID);
	sprintf(buf + offset, "%03d", resourceID);
	offset += 3;

	// 请求/响应标识
	snprintf(buf + offset, sizeof(buf) - offset, "%d", resCmd);
	offset += 1;

	// 报文正文
	if (lenOfData + offset >= sizeOfBuf)
	{
                UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
                return(errCodeParameter);
	}
	memcpy(buf + offset, data, lenOfData);
	offset += lenOfData;
	buf[offset] = 0;

        return(offset);
}

int UnionSendResouceInfoToMonitor(int resID,const char *data,int len, int resCmd)
{
        int                     ret;
        int                     retryTimes = 0;
        char                    tmpBuf[2048];
	char			resBuf[2048];
        int                     lenOfData;
        int                     index;
	char			*esscID;
	unsigned char		lenBuf[8];
	int			recvLen;

	if ((data == NULL) || (len < 0) || (resCmd != 0 && resCmd != 1))
	{
		UnionUserErrLog("in UnionSendResouceInfoToMonitor:: parameter error!\n");
                return(errCodeParameter);
	}

        // 打请求包
        memset(tmpBuf, 0, sizeof(tmpBuf));
        if ((esscID = UnionGetMyMngSvrIDAtMonitor()) == NULL)
        {
                UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionGetMyMngSvrIDAtMonitor return NULL!!\n");
                return(errCodeParameter);
        }
        if ((lenOfData = UnionPackTransInfoRequestPackage(data,len,esscID,resID,resCmd,tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
        {
                UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionPackTransInfoRequestPackage! data[%s] len = [%d]\n",data, len);
                return(lenOfData);
        }
        tmpBuf[0] = lenOfData / 256;
        tmpBuf[1] = lenOfData % 256;
        lenOfData += 2;

        // 建立通讯连接
        if ((ret = UnionInitCommWithMonitor()) < 0)
        {
                UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionInitCommWithMonitor!\n");
                return(ret);
        }

        // 向服务器发包
        for (index = 0; index < gunionMonitorNum; index++)
        {
                UnionLog("in UnionSendTransInfoToMonitor::ipAddr[%s]port[%d]\n",
                        gunionTransSpierConn[index].ipAddr, gunionTransSpierConn[index].port);
                UnionLog("in UnionSendResouceInfoToMonitor::[%04d][%s]\n", lenOfData-2, tmpBuf+2);

                retryTimes = 0;
loop:
                if (retryTimes >= 2)
                        continue;       // 最多向一个服务器重发一次
                // 重连监控服务器
                if ((ret = UnionReconnectMonitorConn(&gunionTransSpierConn[index])) < 0)
                        continue;
                if ((ret = UnionSendToSocket(gunionTransSpierConn[index].sckHDL,(unsigned char *)tmpBuf,lenOfData)) < 0)
                {
                        UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionSendToSocket!\n");
                        UnionCloseSocket(gunionTransSpierConn[index].sckHDL);
                        gunionTransSpierConn[index].sckHDL = -1;
                        retryTimes++;
                        goto loop;      // 发送失败，重发
                }

		// 接受监控服务器响应消息
		if ((ret = UnionReceiveFromSocketUntilLen(gunionTransSpierConn[index].sckHDL,lenBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionReceiveFromSocketUntilLen Error ret = [%d]!\n",ret);
			continue;
		}
		recvLen = lenBuf[0] * 256 + lenBuf[1];
		if (recvLen >= sizeof(resBuf))
		{
			UnionUserErrLog("in UnionSendResouceInfoToMonitor:: resBuf len[%d] is small, recvLen [%d]!\n", (int)sizeof(resBuf), recvLen);
			continue;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(gunionTransSpierConn[index].sckHDL,(unsigned char *)resBuf,recvLen)) < 0)
		{
			UnionUserErrLog("in UnionSendResouceInfoToMonitor:: UnionReceiveFromSocketUntilLen Error ret = [%d]!\n",ret);
			continue;
		}

		//判断响应码
		if (memcmp(resBuf+12, "000000", 6) != 0)
		{
			memcpy(lenBuf, resBuf + 12, 6);
			lenBuf[6] = 0;
			UnionUserErrLog("in UnionSendResouceInfoToMonitor:: response code is [%s]!\n", lenBuf);
			continue;
		}
	}
        if (!gunionLongConnWithMonitor)
                UnionCloseCommWithMonitor();    // 使用短连接与监控服务器通讯
        return(0);
}

int UnionGetDefFileNameOfMonitorSvrList(char *fileName)
{
	sprintf(fileName,"%s/monitorSvrList.conf",getenv("UNIONETC"));
	return(0);
}

int UnionReadAllMonitorConf(char ipAddrList[][15+1], int portList[], int maxCnt)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	int		numOfConf;

	memset(fileName,0,sizeof(fileName));
	UnionGetDefFileNameOfMonitorSvrList(fileName);

	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadAllMonitorConf:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	numOfConf = 0;

	for (i = 0; (i < UnionGetEnviVarNum()) && (i < maxCnt); i++)
	{
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReadAllMonitorConf:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (!UnionIsValidIPAddrStr(p))
			continue;
		strcpy(ipAddrList[i], p);

		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReadAllMonitorConf:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if ((portList[i] = atoi(p)) < 0)
			continue;

		++numOfConf;
	}

	UnionClearEnvi();

	return numOfConf;
}


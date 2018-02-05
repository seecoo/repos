//	Wolfgang Wang, 2006/8/9

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionResID.h"
#include "mngSvrServicePackage.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

TUnionResMngReqCmdHeader	gunionResMngReqCmdHeader;
int				gunionResMngDataLen = 0;
TUnionResMngResCmdHeader	gunionResMngResCmdHeader;
char				gunionResMngData[8192*2+1];
char				gunionResMngClientIPAddr[15+1];
int				gunionResMngClientPort = -1;

int UnionSetResMngClientIPAddr(char *ipAddr)
{
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionSetResMngClientIPAddr:: [%s] not valid ipAddr!\n",ipAddr);
		return(errCodeParameter);
	}
	strcpy(gunionResMngClientIPAddr,ipAddr);
	return(0);
}

int UnionSetResMngClientPort(int port)
{
	gunionResMngClientPort = port;
	return(0);
}

char *UnionReadResMngClientIPAddr()
{
	return(gunionResMngClientIPAddr);
}

int UnionReadResMngClientPort()
{
	return(gunionResMngClientPort);
}

int UnionUnpackResMngRequestPackage(char *dataStr,int lenOfDataStr)
{
	int	ret;
	int	resID;
	int	resCmd;
	
	memset(&gunionResMngReqCmdHeader,0,sizeof(gunionResMngReqCmdHeader));
	if ((gunionResMngDataLen = UnionUnpackMngSvrRequestPackage(dataStr,lenOfDataStr,
			gunionResMngReqCmdHeader.tellerNo,&resID,&resCmd,
			gunionResMngData,sizeof(gunionResMngData))) < 0)
	{
		UnionUserErrLog("in UnionUnpackResMngRequestPackage:: UnionUnpackMngSvrRequestPackage!\n");
		return(gunionResMngDataLen);
	}
	gunionResMngData[gunionResMngDataLen] = 0;
	sprintf(gunionResMngReqCmdHeader.lenOfParameter,"%04d",gunionResMngDataLen % 10000);
	sprintf(gunionResMngReqCmdHeader.resID,"%03d",resID % 1000);
	sprintf(gunionResMngReqCmdHeader.resCmd,"%03d",resCmd % 1000);
	gunionResMngReqCmdHeader.flag = conMngSvrRequestPackageFlag;
	

	UnionAuditNullLog("\nAfter Unpack ResMngReqCmd::\n");
	UnionLogResMngRequestCmdPackage();
		
	return(0);
}

int UnionUnpackResMngResponsePackage(char *dataStr,int lenOfDataStr)
{
	int	ret;
	int	resID;
	int	resCmd;
	int	responseCode;
	
	memset(&gunionResMngResCmdHeader,0,sizeof(gunionResMngResCmdHeader));
	if ((gunionResMngDataLen = UnionUnpackMngSvrResponsePackage(dataStr,lenOfDataStr,
			gunionResMngResCmdHeader.tellerNo,&resID,&resCmd,&responseCode,
			gunionResMngData,sizeof(gunionResMngData))) < 0)
	{
		UnionUserErrLog("in UnionUnpackResMngResponsePackage:: UnionUnpackMngSvrResponsePackage!\n");
		return(gunionResMngDataLen);
	}
	gunionResMngData[gunionResMngDataLen] = 0;
	sprintf(gunionResMngResCmdHeader.lenOfParameter,"%04d",gunionResMngDataLen % 10000);
	sprintf(gunionResMngResCmdHeader.resID,"%03d",resID % 1000);
	sprintf(gunionResMngResCmdHeader.resCmd,"%03d",resCmd % 1000);
	sprintf(gunionResMngResCmdHeader.responseCode,"%06d",responseCode % 1000000);
	gunionResMngResCmdHeader.flag = conMngSvrResponsePackageFlag;
	
	UnionAuditNullLog("\nAfter Unpack ResMngResCmd::\n");
	UnionLogResMngResponseCmdPackage();
		
	return(responseCode);
}

int UnionPackResMngRequestPackage(char *buf,int sizeOfBuf)
{
	UnionAuditNullLog("\nBefore Pack ResMngReqCmd::\n");
	gunionResMngReqCmdHeader.flag = conMngSvrRequestPackageFlag;
	UnionLogResMngRequestCmdPackage();

	return(UnionPackMngSvrRequestPackage(gunionResMngData,gunionResMngDataLen,
			gunionResMngReqCmdHeader.tellerNo,atoi(gunionResMngReqCmdHeader.resID),
			atoi(gunionResMngReqCmdHeader.resCmd),buf,sizeOfBuf));
}

int UnionPackResMngResponsePackage(char *buf,int sizeOfBuf)
{
	// 将请求中的相关域复制到响应中
	strcpy(gunionResMngResCmdHeader.tellerNo,gunionResMngReqCmdHeader.tellerNo);
	strcpy(gunionResMngResCmdHeader.resID,gunionResMngReqCmdHeader.resID);
	strcpy(gunionResMngResCmdHeader.resCmd,gunionResMngReqCmdHeader.resCmd);
	
	UnionAuditNullLog("\nBefore Pack ResMngResCmd::\n");
	gunionResMngResCmdHeader.flag = conMngSvrResponsePackageFlag;
	UnionLogResMngResponseCmdPackage();

	return(UnionPackMngSvrResponsePackage(gunionResMngData,gunionResMngDataLen,
			gunionResMngResCmdHeader.tellerNo,atoi(gunionResMngResCmdHeader.resID),
			atoi(gunionResMngResCmdHeader.resCmd),atoi(gunionResMngResCmdHeader.responseCode),
			buf,sizeOfBuf));
}

void UnionLogResMngRequestCmdPackage()
{
	UnionAuditNullLog("[flag]           [%c]\n",gunionResMngReqCmdHeader.flag);
	UnionAuditNullLog("[tellerNo]       [%s]\n",gunionResMngReqCmdHeader.tellerNo);
	UnionAuditNullLog("[resID]          [%s]\n",gunionResMngReqCmdHeader.resID);
	UnionAuditNullLog("[resCmd]         [%s]\n",gunionResMngReqCmdHeader.resCmd);
	UnionAuditNullLog("[lenOfParameter] [%s]\n",gunionResMngReqCmdHeader.lenOfParameter);
	if (gunionResMngDataLen > 0)
		UnionAuditNullLog("[parameter]      [%s]\n",gunionResMngData);
}

void UnionLogResMngResponseCmdPackage()
{
	UnionAuditNullLog("[flag]           [%c]\n",gunionResMngResCmdHeader.flag);
	UnionAuditNullLog("[tellerNo]       [%s]\n",gunionResMngResCmdHeader.tellerNo);
	UnionAuditNullLog("[resID]          [%s]\n",gunionResMngResCmdHeader.resID);
	UnionAuditNullLog("[resCmd]         [%s]\n",gunionResMngResCmdHeader.resCmd);
	UnionAuditNullLog("[responseCode]   [%s]\n",gunionResMngResCmdHeader.responseCode);
	UnionAuditNullLog("[lenOfParameter] [%s]\n",gunionResMngResCmdHeader.lenOfParameter);
	if (gunionResMngDataLen > 0)
		UnionAuditNullLog("[parameter]      [%s]\n",gunionResMngData);
}

int UnionReadResMngRequestPackageFld(char *name,char *buf,int sizeOfBuf)
{
	if ((name == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionReadResMngRequestPackageFld:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionToUpperCase(name);
	if (strcmp(name,conMngSvrPackFldNameTellerNo) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngReqCmdHeader.tellerNo))
			goto readError;
		memcpy(buf,gunionResMngReqCmdHeader.tellerNo,sizeof(gunionResMngReqCmdHeader.tellerNo)-1);
		return(sizeof(gunionResMngReqCmdHeader.tellerNo)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResID) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngReqCmdHeader.resID))
			goto readError;
		memcpy(buf,gunionResMngReqCmdHeader.resID,sizeof(gunionResMngReqCmdHeader.resID)-1);
		return(sizeof(gunionResMngReqCmdHeader.resID)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResCmd) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngReqCmdHeader.resCmd))
			goto readError;
		memcpy(buf,gunionResMngReqCmdHeader.resCmd,sizeof(gunionResMngReqCmdHeader.resCmd)-1);
		return(sizeof(gunionResMngReqCmdHeader.resCmd)-1);
	}
	if ((strcmp(name,conMngSvrPackFldNameData) == 0) || (strcmp(name,"PARAMETER") == 0))
	{
		if (sizeOfBuf < gunionResMngDataLen)
			goto readError;
		memcpy(buf,gunionResMngData,gunionResMngDataLen);
		return(gunionResMngDataLen);
	}
	UnionUserErrLog("in UnionReadResMngRequestPackageFld:: invalid fld name [%s]\n",name);
	return(errCodeParameter);

readError:
	UnionUserErrLog("in UnionReadResMngRequestPackageFld:: sizeOfBuf too small! [%d]\n",sizeOfBuf);
	return(errCodeParameter);
}

int UnionReadResMngResponsePackageFld(char *name,char *buf,int sizeOfBuf)
{
	if ((name == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionReadResMngResponsePackageFld:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionToUpperCase(name);
	if (strcmp(name,conMngSvrPackFldNameTellerNo) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngResCmdHeader.tellerNo))
			goto readError;
		memcpy(buf,gunionResMngResCmdHeader.tellerNo,sizeof(gunionResMngResCmdHeader.tellerNo)-1);
		return(sizeof(gunionResMngResCmdHeader.tellerNo)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResponseCode) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngResCmdHeader.responseCode))
			goto readError;
		memcpy(buf,gunionResMngResCmdHeader.responseCode,sizeof(gunionResMngResCmdHeader.responseCode)-1);
		return(sizeof(gunionResMngResCmdHeader.responseCode)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResID) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngResCmdHeader.resID))
			goto readError;
		memcpy(buf,gunionResMngResCmdHeader.resID,sizeof(gunionResMngResCmdHeader.resID)-1);
		return(sizeof(gunionResMngResCmdHeader.resID)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResCmd) == 0)
	{
		if (sizeOfBuf < sizeof(gunionResMngResCmdHeader.resCmd))
			goto readError;
		memcpy(buf,gunionResMngResCmdHeader.resCmd,sizeof(gunionResMngResCmdHeader.resCmd)-1);
		return(sizeof(gunionResMngResCmdHeader.resCmd)-1);
	}
	if ((strcmp(name,conMngSvrPackFldNameData) == 0) || (strcmp(name,"PARAMETER") == 0))
	{
		if (sizeOfBuf < gunionResMngDataLen)
			goto readError;
		memcpy(buf,gunionResMngData,gunionResMngDataLen);
		return(gunionResMngDataLen);
	}
	UnionUserErrLog("in UnionReadResMngResponsePackageFld:: invalid fld name [%s]\n",name);
	return(errCodeParameter);

readError:
	UnionUserErrLog("in UnionReadResMngResponsePackageFld:: sizeOfBuf too small! [%d]\n",sizeOfBuf);
	return(errCodeParameter);
}

int UnionSetResMngRequestPackageFld(char *name,char *buf,int len)
{
	if ((name == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionSetResMngRequestPackageFld:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionToUpperCase(name);
	if (strcmp(name,conMngSvrPackFldNameTellerNo) == 0)
	{
		if (len != sizeof(gunionResMngReqCmdHeader.tellerNo) - 1)
			goto setError;
		memcpy(gunionResMngReqCmdHeader.tellerNo,buf,len);
		gunionResMngReqCmdHeader.tellerNo[sizeof(gunionResMngReqCmdHeader.tellerNo)-1] = 0;
		return(sizeof(gunionResMngReqCmdHeader.tellerNo)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResID) == 0)
	{
		if (len != sizeof(gunionResMngReqCmdHeader.resID) - 1)
			goto setError;
		memcpy(gunionResMngReqCmdHeader.resID,buf,sizeof(gunionResMngReqCmdHeader.resID)-1);
		gunionResMngReqCmdHeader.resID[sizeof(gunionResMngReqCmdHeader.resID)-1] = 0;
		return(sizeof(gunionResMngReqCmdHeader.resID)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResCmd) == 0)
	{
		if (len != sizeof(gunionResMngReqCmdHeader.resCmd) - 1)
			goto setError;
		memcpy(gunionResMngReqCmdHeader.resCmd,buf,sizeof(gunionResMngReqCmdHeader.resCmd)-1);
		gunionResMngReqCmdHeader.resCmd[sizeof(gunionResMngReqCmdHeader.resCmd)-1] = 0;
		return(sizeof(gunionResMngReqCmdHeader.resCmd)-1);
	}
	if ((strcmp(name,conMngSvrPackFldNameData) == 0) || (strcmp(name,"PARAMETER") == 0))
	{
		if (len >= sizeof(gunionResMngData))
		{
			UnionUserErrLog("in UnionSetResMngRequestPackageFld:: dataLen = [%d] too long!\n",len);
			return(errCodeParameter);
		}
		if ((gunionResMngDataLen = len % 10000) == 0)
			return(len);
		memcpy(gunionResMngData,buf,gunionResMngDataLen);
		gunionResMngData[gunionResMngDataLen] = 0;
		return(gunionResMngDataLen);
	}
	UnionUserErrLog("in UnionSetResMngRequestPackageFld:: invalid fld name [%s]\n",name);
	return(errCodeParameter);

setError:
	UnionUserErrLog("in UnionSetResMngRequestPackageFld:: fld len error! [%d]\n",len);
	return(errCodeParameter);
}

int UnionSetResMngResponsePackageFld(char *name,char *buf,int len)
{
	if ((name == NULL) || (buf == NULL) || (len < 0))
	{
		UnionUserErrLog("in UnionSetResMngResponsePackageFld:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionToUpperCase(name);
	//buf[len] = 0;
	UnionDebugNullLog("in UnionSetResMngResponsePackageFld:: fld [%20s] [%04d] [%s]\n",name,len,buf);
	if (strcmp(name,conMngSvrPackFldNameTellerNo) == 0)
	{
		if (len != sizeof(gunionResMngResCmdHeader.tellerNo) - 1)
			goto setError;
		memcpy(gunionResMngResCmdHeader.tellerNo,buf,len);
		gunionResMngResCmdHeader.tellerNo[sizeof(gunionResMngReqCmdHeader.tellerNo)-1] = 0;
		return(sizeof(gunionResMngResCmdHeader.tellerNo)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResponseCode) == 0)
	{
		if (len != sizeof(gunionResMngResCmdHeader.responseCode) - 1)
			goto setError;
		memcpy(gunionResMngResCmdHeader.responseCode,buf,sizeof(gunionResMngResCmdHeader.responseCode)-1);
		gunionResMngResCmdHeader.responseCode[sizeof(gunionResMngResCmdHeader.responseCode)-1] = 0;
		if (atoi(gunionResMngResCmdHeader.responseCode) < 0)	// 如果响应码为负数，清空响应数据
		{
			gunionResMngDataLen = 0;
			memset(gunionResMngResCmdHeader.lenOfParameter,'0',sizeof(gunionResMngResCmdHeader.lenOfParameter)-1);
		}
		return(sizeof(gunionResMngResCmdHeader.responseCode)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResID) == 0)
	{
		if (len != sizeof(gunionResMngResCmdHeader.resID) - 1)
			goto setError;
		memcpy(gunionResMngResCmdHeader.resID,buf,sizeof(gunionResMngResCmdHeader.resID)-1);
		gunionResMngResCmdHeader.resID[sizeof(gunionResMngReqCmdHeader.resID)-1] = 0;
		return(sizeof(gunionResMngResCmdHeader.resID)-1);
	}
	if (strcmp(name,conMngSvrPackFldNameResCmd) == 0)
	{
		if (len != sizeof(gunionResMngResCmdHeader.resCmd) - 1)
			goto setError;
		memcpy(gunionResMngResCmdHeader.resCmd,buf,sizeof(gunionResMngResCmdHeader.resCmd)-1);
		gunionResMngResCmdHeader.resCmd[sizeof(gunionResMngReqCmdHeader.resCmd)-1] = 0;
		return(sizeof(gunionResMngResCmdHeader.resCmd)-1);
	}
	if ((strcmp(name,conMngSvrPackFldNameData) == 0) || (strcmp(name,"PARAMETER") == 0))
	{
		if (len >= sizeof(gunionResMngData))
		{
			buf[len] = 0;
			UnionUserErrLog("in UnionSetResMngResponsePackageFld:: dataLen = [%d] too long buf = [%s]!\n",len,buf);
			return(errCodeParameter);
		}
		sprintf(gunionResMngResCmdHeader.lenOfParameter,"%04d",len % 10000);
		if ((gunionResMngDataLen = len % 10000) == 0)	// 由模１０００改为模１００００，王纯军，2008/11/17。
			return(len);
		memcpy(gunionResMngData,buf,gunionResMngDataLen);
		gunionResMngData[gunionResMngDataLen] = 0;
		return(gunionResMngDataLen);
	}
	UnionUserErrLog("in UnionSetResMngResponsePackageFld:: invalid fld name [%s]\n",name);
	return(errCodeParameter);

setError:
	UnionUserErrLog("in UnionSetResMngResponsePackageFld:: fld [%s] [%s] len error! [%d]\n",name,buf,len);
	return(errCodeParameter);
}

// 解开一个监控包
// dataStr,lenOfDataStr是输入参数，分别对应监控包和其长度
// resID,resCmd,buf,sizeOfBuf是输出参数，分别对应资源ID，资源命令，监控数据，接收监控数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是监控数据的长度。
int UnionUnpackSpierPackage(char *dataStr,int lenOfDataStr,int *resID,int *resCmd,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngReqCmdHeader	tmpHeader;
	int				lenOfData;
	
	if ((lenOfDataStr <= 0) || (dataStr == NULL) || (resID == NULL) || (resCmd == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	if (dataStr[0] != conMngSvrRequestPackageFlag)	// 不是请求报文
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: not requestPackage!\n");
		return(errCodeEsscMDL_NotEsscRequestPackage);
	}
	offset = 1;
	// 读柜员号
	if (offset + sizeof(tmpHeader.tellerNo) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	offset += sizeof(tmpHeader.tellerNo)-1;

	// 读资源ID
	if (offset + sizeof(tmpHeader.resID) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.resID,dataStr+offset,sizeof(tmpHeader.resID)-1);
	*resID = atoi(tmpHeader.resID);
	offset += sizeof(tmpHeader.resID)-1;

	// 读指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.resCmd,dataStr+offset,sizeof(tmpHeader.resCmd)-1);
	*resCmd = atoi(tmpHeader.resCmd);
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackSpierPackage:: lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.lenOfParameter,dataStr+offset,sizeof(tmpHeader.lenOfParameter)-1);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 读数据
	if (offset + (lenOfData = atoi(tmpHeader.lenOfParameter)) > lenOfDataStr)
	{
		//UnionUserErrLog("in UnionUnpackSpierPackage:: lenOfDataStr [%d] too short! lenOfData = [%d]\n",lenOfDataStr,lenOfData);
		lenOfData = lenOfDataStr - offset;
		//return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (lenOfData >= sizeOfBuf)
	{
		memcpy(buf,dataStr+offset,sizeOfBuf-4);
		memcpy(buf+sizeOfBuf-4,"...",3);
		return(sizeOfBuf-1);
	}
	else
	{
		if (lenOfData < 0)
			return(errCodeEsscMDL_ClientDataLenTooShort);
		memcpy(buf,dataStr + offset,lenOfData);
		return(lenOfData);
	}	
}

// 打一个监控包
// resID,resCmd,data,lenOfData是输入参数，分别对应资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是监控数据包的长度。
int UnionPackSpierPackage(char *data,int lenOfData,int resID,int resCmd,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngReqCmdHeader	tmpHeader;
	
	if ((lenOfData < 0) || (data == NULL) || (buf == NULL) || (lenOfData >= 10000))
	{
		UnionUserErrLog("in UnionPackSpierPackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	// 报文标识
	buf[0] = conMngSvrRequestPackageFlag;
	offset = 1;
	// 柜员号
	if (offset + sizeof(tmpHeader.tellerNo) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackSpierPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	memset(buf+offset,'0',sizeof(tmpHeader.tellerNo)-1);
	offset += sizeof(tmpHeader.tellerNo)-1;

	// 资源ID
	if (offset + sizeof(tmpHeader.resID) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackSpierPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resID);
	offset += sizeof(tmpHeader.resID)-1;

	// 指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackSpierPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resCmd);
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackSpierPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%04d",lenOfData);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 读数据
	if (offset + lenOfData >= sizeOfBuf)
	{
		memcpy(buf+offset,data,sizeOfBuf-offset-4);
		offset += (sizeOfBuf-offset-4);
		memcpy(buf+offset,"...",3);
		offset += 3;
	}
	else
	{
		memcpy(buf+offset,data,lenOfData);
		offset += lenOfData;
	}
	//buf[offset] = 0;
	//UnionAuditLog("in UnionPackSpierPackage:: [%s]\n",buf);
	return(offset);
}

// 打一个请求管理包
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngSvrRequestPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngReqCmdHeader	tmpHeader;
	
	if ((lenOfData < 0) || (data == NULL) || (buf == NULL) || (lenOfData >= 10000) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	// 报文标识
	buf[0] = conMngSvrRequestPackageFlag;
	offset = 1;
	// 柜员号 modify by xusj 20091125
	//if (offset + sizeof(tmpHeader.tellerNo) -1 >= (unsigned int)sizeOfBuf)
	if (offset + UnionGetLengthOfTellerNo() >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	if (strlen(tellerNo) >= sizeof(tmpHeader.tellerNo))
		memcpy(buf+offset,tellerNo,sizeof(tmpHeader.tellerNo)-1);
	else
	{
		memset(buf+offset,' ',UnionGetLengthOfTellerNo());
		memcpy(buf+offset,tellerNo,strlen(tellerNo));
	}
	offset += UnionGetLengthOfTellerNo();

	// 资源ID
	if (offset + sizeof(tmpHeader.resID) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resID);
	offset += sizeof(tmpHeader.resID)-1;

	// 指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resCmd);
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%04d",lenOfData);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 读数据
	if (offset + lenOfData >= sizeOfBuf)
	{
		memcpy(buf+offset,data,sizeOfBuf-4);
		offset += (sizeOfBuf-4);
		memcpy(buf+offset,"...",3);
		offset += 3;
	}
	else
	{
		memcpy(buf+offset,data,lenOfData);
		offset += lenOfData;
	}
	return(offset);
}

// 打一个请求管理包(客户端使用)
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngPackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,char *buf,int sizeOfBuf)
{
	return(UnionPackMngSvrRequestPackage(data,lenOfData,tellerNo,resID,resCmd,buf,sizeOfBuf));
}

// 解开一个管理响应包
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngSvrResponsePackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,int *responseCode,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngResCmdHeader	tmpHeader;
	int				lenOfData;
	int				retLen;
	
	if ((lenOfDataStr <= 0) || (dataStr == NULL) || (responseCode == NULL))
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	if (dataStr[0] != conMngSvrResponsePackageFlag)	// 不是请求报文
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: not response Package!\n");
		return(errCodeEsscMDL_NotEsscResponsePackage);
	}
	offset = 1;
	// 柜员号 modify by xusj 20091125
	//if (offset + sizeof(tmpHeader.tellerNo) -1 > (unsigned int)lenOfDataStr)
	if (offset + UnionGetLengthOfTellerNo() > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 1. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (tellerNo != NULL)
	{
		memcpy(tellerNo,dataStr+offset,UnionGetLengthOfTellerNo());
		tellerNo[UnionGetLengthOfTellerNo()] = 0;
		UnionFilterRightBlank(tellerNo);
	}
	offset += UnionGetLengthOfTellerNo();

	// 读资源ID
	if (offset + sizeof(tmpHeader.resID) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 2. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resID != NULL)
	{
		memcpy(tmpHeader.resID,dataStr+offset,sizeof(tmpHeader.resID)-1);
		*resID = atoi(tmpHeader.resID);
	}
	offset += sizeof(tmpHeader.resID)-1;

	// 读指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 3. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resCmd != NULL)
	{
		memcpy(tmpHeader.resCmd,dataStr+offset,sizeof(tmpHeader.resCmd)-1);
		*resCmd = atoi(tmpHeader.resCmd);
	}
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读响应码
	if (offset + sizeof(tmpHeader.responseCode) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 4. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.responseCode,dataStr+offset,sizeof(tmpHeader.responseCode)-1);
	if ((ret = atoi(tmpHeader.responseCode)) < 0)
		UnionAuditLog("in UnionUnpackMngSvrResponsePackage:: 5. responseCode = [%d]\n",ret);
	offset += sizeof(tmpHeader.responseCode)-1;
	*responseCode = ret;

	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 6. offset = [%d] lenOfDataStr [%d] too short!\n",offset,lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.lenOfParameter,dataStr+offset,sizeof(tmpHeader.lenOfParameter)-1);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 读数据
	if (offset + (lenOfData = atoi(tmpHeader.lenOfParameter)) > lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: 7. lenOfDataStr [%d] too short! lenOfData = [%d]\n",lenOfDataStr,lenOfData);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	
	//2010-6-11，王纯军重新打开以下这段代码
	// 2008/7/23删除
	if (lenOfData <= 0)
		return(*responseCode);
	if (buf != NULL)
	{
		if (lenOfData >= sizeOfBuf)
		{
			memcpy(buf,dataStr+offset,sizeOfBuf-4);
			memcpy(buf+sizeOfBuf-4,"...",3);
			buf[sizeOfBuf-1] = 0;
			return(sizeOfBuf-1);
		}
		else
		{
			memcpy(buf,dataStr + offset,lenOfData);
			buf[lenOfData] = 0;
			return(lenOfData);
		}
	}
	return(lenOfData);
	// 2008/7/23删除结束
	//2010-6-11，王纯军打开结束

	/* 2010-6-11,王纯军删除以下这段
	// 2008/7/23增加
	if (buf != NULL)
	{
		if (lenOfData >= sizeOfBuf)
		{
			memcpy(buf,dataStr+offset,sizeOfBuf-4);
			memcpy(buf+sizeOfBuf-4,"...",3);
			buf[sizeOfBuf-1] = 0;
                        retLen = sizeOfBuf - 1;
		}
		else
		{
			memcpy(buf,dataStr + offset,lenOfData);
			buf[lenOfData] = 0;
                        retLen = lenOfData;
		}
	}
        else
                retLen = lenOfData;
	if (*responseCode < 0)
		return(*responseCode);
        else
        	return(retLen);
	// 2008/7/23增加结束
	2010-6-11,王纯军删除结束
	*/
}

// 解开一个管理响应包(客户端使用)
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf)
{
	int	responseCode;
	int	lenOfData;
	
	if ((lenOfData = UnionUnpackMngSvrResponsePackage(dataStr,lenOfDataStr,tellerNo,resID,resCmd,&responseCode,buf,sizeOfBuf)) < 0)
		return(lenOfData);
	else
		return(responseCode);
}

// 解开一个管理请求包
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// tellerNo,resID,resCmd,buf,sizeOfBuf是输出参数，分别对应柜员号、资源ID，资源命令，管理数据，接收管理数据的缓冲区的大小
// 返回值是负数，解包出错，否则，是管理数据的长度。
int UnionUnpackMngSvrRequestPackage(char *dataStr,int lenOfDataStr,char *tellerNo,int *resID,int *resCmd,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngReqCmdHeader	tmpHeader;
	int				lenOfData;
	
	if ((lenOfDataStr <= 0) || (dataStr == NULL))
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	if (dataStr[0] != conMngSvrRequestPackageFlag)	// 不是请求报文
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: not Request Package!\n");
		dataStr[lenOfDataStr];
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: req=[%04d][%s]!\n",lenOfDataStr,dataStr);
		return(errCodeEsscMDL_NotEsscRequestPackage);
	}
	offset = 1;
	// 柜员号 modify by xusj 20091125
	//if (offset + sizeof(tmpHeader.tellerNo) -1 > (unsigned int)lenOfDataStr)
	if (offset + UnionGetLengthOfTellerNo() > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: 1. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (tellerNo != NULL)
	{
		memcpy(tellerNo,dataStr+offset,UnionGetLengthOfTellerNo());
		tellerNo[UnionGetLengthOfTellerNo()] = 0;
		UnionFilterRightBlank(tellerNo);
	}
	offset += UnionGetLengthOfTellerNo();

	// 读资源ID
	if (offset + sizeof(tmpHeader.resID) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: 2. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resID != NULL)
	{
		memcpy(tmpHeader.resID,dataStr+offset,sizeof(tmpHeader.resID)-1);
		*resID = atoi(tmpHeader.resID);
	}
	offset += sizeof(tmpHeader.resID)-1;

	// 读指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: 3. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resCmd != NULL)
	{
		memcpy(tmpHeader.resCmd,dataStr+offset,sizeof(tmpHeader.resCmd)-1);
		*resCmd = atoi(tmpHeader.resCmd);
	}
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: 4. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.lenOfParameter,dataStr+offset,sizeof(tmpHeader.lenOfParameter)-1);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 读数据
	if (offset + (lenOfData = atoi(tmpHeader.lenOfParameter)) > lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: 5. lenOfDataStr [%d] too short! lenOfData = [%d]\n",lenOfDataStr,lenOfData);
		dataStr[lenOfDataStr] = 0;
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: dataStr = [%s]\n",dataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (lenOfData <= 0)
		return(lenOfData);
	if (buf != NULL)
	{
		if (lenOfData >= sizeOfBuf)
		{
			memcpy(buf,dataStr+offset,sizeOfBuf-4);
			memcpy(buf+sizeOfBuf-4,"...",3);
			return(sizeOfBuf-1);
		}
		else
		{
			memcpy(buf,dataStr + offset,lenOfData);
			return(lenOfData);
		}
	}
	return(lenOfData);
}

// 打一个响应管理包
// tellerNo,resID,resCmd,data,lenOfData是输入参数，分别对应操作员代号、资源ID、资源命令、监控数据和其长度
// responseCode，是响应码
// buf,sizeOfBuf是输出参数，分别对应监控包报文的缓冲区的大小
// 返回值是负数，打包出错，否则，是管理数据包的长度。
int UnionPackMngSvrResponsePackage(char *data,int lenOfData,char *tellerNo,int resID,int resCmd,int responseCode,char *buf,int sizeOfBuf)
{
	int				ret;
	int				offset = 0;
	TUnionResMngResCmdHeader	tmpHeader;
	
	if ((lenOfData < 0) || (data == NULL) || (buf == NULL) || (lenOfData >= 10000) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	// 报文标识
	buf[0] = conMngSvrResponsePackageFlag;
	offset = 1;
	// 柜员号 modify by xusj 20091125
	//if (offset + sizeof(tmpHeader.tellerNo) -1 >= (unsigned int)sizeOfBuf)
	if (offset + UnionGetLengthOfTellerNo() >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	if (strlen(tellerNo) >= sizeof(tmpHeader.tellerNo))
		memcpy(buf+offset,tellerNo,sizeof(tmpHeader.tellerNo)-1);
	else
	{
		memset(buf+offset,' ',UnionGetLengthOfTellerNo());
		memcpy(buf+offset,tellerNo,strlen(tellerNo));
	}
	offset += UnionGetLengthOfTellerNo();

	// 资源ID
	if (offset + sizeof(tmpHeader.resID) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resID);
	offset += sizeof(tmpHeader.resID)-1;

	// 指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%03d",resCmd);
	offset += sizeof(tmpHeader.resCmd)-1;

	// 响应码
	if (offset + sizeof(tmpHeader.responseCode) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%06d",responseCode);
	offset += sizeof(tmpHeader.responseCode)-1;
	//if (responseCode < 0)
	//	return(offset);
				
	// 参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 >= (unsigned int)sizeOfBuf)
	{
		UnionUserErrLog("in UnionPackMngSvrResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf+offset,"%04d",lenOfData);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	
	// 数据
	if (offset + lenOfData >= sizeOfBuf)
	{
		UnionAuditLog("in UnionPackMngSvrResponsePackage:: warning lenOfData = [%04d] > expected [%04d]\n",lenOfData,sizeOfBuf-offset);
		memcpy(buf+offset,data,sizeOfBuf-4);
		offset += (sizeOfBuf-4);
		memcpy(buf+offset,"...",3);
		offset += 3;
	}
	else
	{
		memcpy(buf+offset,data,lenOfData);
		offset += lenOfData;
	}
	return(offset);
}

// 判断一个包是不是请求包
int UnionIsMngSvrResponsePackage(char *dataStr,int lenOfDataStr)
{
	if ((dataStr == NULL) || (lenOfDataStr <= 0))
		return(0);
	if (dataStr[0] == conMngSvrResponsePackageFlag)
		return(1);
	else
		return(0);
}

// 2009/9/30，王纯军增加
// 解开一个管理包头
// dataStr,lenOfDataStr是输入参数，分别对应管理包和其长度
// isRequest,tellerNo,resID,resCmd,resID,lenOfData分别对应请求标识、柜员号、资源ID，资源命令，响应码,数据长度
// 返回值是负数，解包出错，否则，是数据头的长度。
int UnionUnpackMngSvrPackageHeader(char *dataStr,int lenOfDataStr,int *isRequest,char *tellerNo,int *resID,int *resCmd,int *resCode,int *lenOfData)
{
	int				ret;
	int				offset = 0;
	TUnionResMngResCmdHeader	tmpHeader;
	
	if ((lenOfDataStr <= 0) || (dataStr == NULL))
	{
		UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: parameter error!\n");
		return(errCodeParameter);
	}
	memset(&tmpHeader,0,sizeof(tmpHeader));
	// 请求标识
	if (isRequest != NULL)
		*isRequest = dataStr[0] - '0';
	offset = 1;
	// 柜员号 modify by xusj 20091125
	//if (offset + sizeof(tmpHeader.tellerNo) -1 > (unsigned int)lenOfDataStr)
	if (offset + UnionGetLengthOfTellerNo() > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: 1. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (tellerNo != NULL)
	{
		memcpy(tellerNo,dataStr+offset,UnionGetLengthOfTellerNo());
		tellerNo[UnionGetLengthOfTellerNo()] = 0;
		UnionFilterRightBlank(tellerNo);
	}
	offset += UnionGetLengthOfTellerNo();

	// 读资源ID
	if (offset + sizeof(tmpHeader.resID) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: 2. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resID != NULL)
	{
		memcpy(tmpHeader.resID,dataStr+offset,sizeof(tmpHeader.resID)-1);
		*resID = atoi(tmpHeader.resID);
	}
	offset += sizeof(tmpHeader.resID)-1;

	// 读指令ID
	if (offset + sizeof(tmpHeader.resCmd) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: 3. lenOfDataStr [%d] too short!\n",lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	if (resCmd != NULL)
	{
		memcpy(tmpHeader.resCmd,dataStr+offset,sizeof(tmpHeader.resCmd)-1);
		*resCmd = atoi(tmpHeader.resCmd);
	}
	offset += sizeof(tmpHeader.resCmd)-1;

	// 读响应码
	if (dataStr[0] == conMngSvrResponsePackageFlag)	// 是响应
	{
		// 读响应码
		if (offset + sizeof(tmpHeader.responseCode) -1 > (unsigned int)lenOfDataStr)
		{
			UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: 4. lenOfDataStr [%d] too short!\n",lenOfDataStr);
			return(errCodeEsscMDL_ClientDataLenTooShort);
		}
		if (resCode != NULL)
		{
			memcpy(tmpHeader.responseCode,dataStr+offset,sizeof(tmpHeader.responseCode)-1);
			if ((*resCode = atoi(tmpHeader.responseCode)) < 0)
				UnionAuditLog("in UnionUnpackMngSvrPackageHeader:: 5. responseCode = [%d]\n",*resCode);
			offset += sizeof(tmpHeader.responseCode)-1;
		}
	}
	
	// 读参数长度
	if (offset + sizeof(tmpHeader.lenOfParameter) -1 > (unsigned int)lenOfDataStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrPackageHeader:: 6. offset = [%d] lenOfDataStr [%d] too short!\n",offset,lenOfDataStr);
		return(errCodeEsscMDL_ClientDataLenTooShort);
	}
	memcpy(tmpHeader.lenOfParameter,dataStr+offset,sizeof(tmpHeader.lenOfParameter)-1);
	offset += sizeof(tmpHeader.lenOfParameter)-1;
	if (lenOfData != NULL)
		*lenOfData = atoi(tmpHeader.lenOfParameter);
	return(offset);
}

// add by xusj begin 20091124 取操作员号的长度
int UnionGetLengthOfTellerNo()
{
	int	len;

	if ( (len = UnionReadIntTypeRECVar("lengthOfTellerNo")) <=0 )
		return 8;
	if(len > 40)
		len = 40;
	return len;
}
// add by xusj end 20091124



// Author:	Wolfgang Wang
// Date:	2003/09/12
// Version:	1.0

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sjl06.h"

#include "UnionStr.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "UnionLog.h"

int UnionGetHsmCfgFileName(char *ipAddr,char *fileName)
{
	sprintf(fileName,"%s/HsmCfg/%s.CFG",getenv("UNIONETC"),ipAddr);
	return(0);
}

int UnionCreateSJL06Conf(char *ipAddr)
{
	int			ret;

	TUnionSJL06		sjl06;
	TUnionSJL06StaticAttr	staticAttr;
	TUnionSJL06DynamicAttr	dynamicAttr;		

	dynamicAttr.timeoutTimes = 0;
	dynamicAttr.connFailTimes = 0;
	dynamicAttr.abnormalCmdTimes = 0;
	dynamicAttr.normalCmdTimes = 0;
	dynamicAttr.continueFailTimes = 0;
	dynamicAttr.downTimes = 0;
	strcpy(dynamicAttr.lastDownDate,"00000000");
	dynamicAttr.status = '1';
	dynamicAttr.activeLongConn = 0;
		
	memset(&staticAttr,0,sizeof(staticAttr));
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionCreateSJL06Conf:: UnionIsValidIPAddrStr [%s]\n",ipAddr);
		return(-1);
	}
	strcpy(staticAttr.ipAddr,ipAddr);
	
	strcpy(staticAttr.hsmGrpID,"001");
	strcpy(staticAttr.hsmCmdVersion,"SJL06");
	staticAttr.port = 8;
	staticAttr.lenOfLenFld = 2;
	staticAttr.lenOfMsgHeader = 0;
	memset(staticAttr.msgHeader,0,sizeof(staticAttr.msgHeader));
	staticAttr.maxConLongConn = 10;
	UnionGetFullSystemDate(staticAttr.registerDate);
	memset(staticAttr.remark,0,sizeof(staticAttr.remark));

	sjl06.dynamicAttr = dynamicAttr;
	sjl06.staticAttr = staticAttr;
	
	return(UnionInsertSJL06Rec(&sjl06));
}

int UnionCreateSJL06ConfArtificially()
{
	int			ret;
	char			*p;
	TUnionSJL06		sjl06;
	TUnionSJL06StaticAttr	staticAttr;
	TUnionSJL06DynamicAttr	dynamicAttr;		

	dynamicAttr.timeoutTimes = 0;
	dynamicAttr.connFailTimes = 0;
	dynamicAttr.abnormalCmdTimes = 0;
	dynamicAttr.normalCmdTimes = 0;
	dynamicAttr.continueFailTimes = 0;
	dynamicAttr.downTimes = 0;
	strcpy(dynamicAttr.lastDownDate,"00000000");
	dynamicAttr.status = '1';
	dynamicAttr.activeLongConn = 0;
		
	memset(&staticAttr,0,sizeof(staticAttr));
inputIPAddr:
	if (UnionIsQuit(p = UnionInput("请输入密码机IP地址：")))
		return(0);
	if (!UnionIsValidIPAddrStr(p))
	{
		printf("非法的IP地址！请重输！\n");
		goto inputIPAddr;
	}
	strcpy(staticAttr.ipAddr,p);
inputHsmGrp:
	if (UnionIsQuit(p = UnionInput("请输入密码机工作组编号(3位数字)：")))
		return(0);
	if (strlen(p) != 3)
	{
		printf("非法的工作组！请重输！\n");
		goto inputHsmGrp;
	}
	strcpy(staticAttr.hsmGrpID,p);
	strcpy(staticAttr.hsmCmdVersion,"SJL06");
	staticAttr.port = 8;
	staticAttr.lenOfLenFld = 2;
inputVersion:
	if (UnionIsQuit(p = UnionInput("请输入密码机指令版本(%s/%s/%s):",
		conHsmCmdVersionSJL06,conHsmCmdVersionRacal,conHsmCmdVersionSJL05)))
		return(0);
	if (!UnionIsValidHsmCmdVersion(p))
	{
		printf("非法指令版本！请重输！\n");
		goto inputVersion;
	}
	strcpy(staticAttr.hsmCmdVersion,p);
inputLenOfMsgHeader:
	if (UnionIsQuit(p = UnionInput("请输消息头的长度：")))
		return(0);
	if (atoi(p) < 0)
	{
		printf("非法的消息头长度！请重输！\n");
		goto inputLenOfMsgHeader;
	}
	staticAttr.lenOfMsgHeader = atoi(p);
	memset(staticAttr.msgHeader,0,sizeof(staticAttr.msgHeader));
	memset(staticAttr.msgHeader,'0',sizeof(staticAttr.msgHeader)-1);
	staticAttr.maxConLongConn = 10;
	UnionGetFullSystemDate(staticAttr.registerDate);
inputRemark:
	if (UnionIsQuit(p = UnionInput("请输密码机说明(最多%d个字符)：",sizeof(staticAttr.remark)-1)))
		return(0);
	if (strlen(p) >= sizeof(staticAttr.remark))
	{
		printf("说明过长！请重输！\n");
		goto inputRemark;
	}
	memset(staticAttr.remark,0,sizeof(staticAttr.remark));
	strcpy(staticAttr.remark,p);

	sjl06.dynamicAttr = dynamicAttr;
	sjl06.staticAttr = staticAttr;
	printf("IP地址     [%s]\n",staticAttr.ipAddr);
	printf("工作组     [%s]\n",staticAttr.hsmGrpID);
	printf("消息头长度 [%d]\n",staticAttr.lenOfMsgHeader);
	printf("说明       [%s]\n",staticAttr.remark);
	if (UnionConfirm("确认创建该密码机的配置文件吗？"))
	{
		if ((ret = UnionInsertSJL06Rec(&sjl06)) < 0)
			printf("创建失败！\n");
		else
			printf("创建成功！\n");
	}
	if (UnionConfirm("继续创建吗？"))
		goto inputIPAddr;
	
	return(0);
}

int UnionDeleteSJL06Conf(char *ipAddr)
{
	char			fileName[256];
	char			cmd[256];				
		
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(ipAddr,fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}
	
int UnionPrintSJL06Conf(char *ipAddr)
{
	int		ret;
	TUnionSJL06	sjl06;
	
	memset(&sjl06,0,sizeof(sjl06));
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionPrintSJL06Conf:: invalid ipAddr [%s]\n",ipAddr);
		return(-1);
	}
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in UnionPrintSJL06Conf:: UnionSelectSJL06Rec!\n");
		return(ret);
	}

	return(UnionWriteSJL06RecToFile(stdout,&sjl06));
}

// Record Layer Functions
// 增加一个Hsm
int UnionInsertSJL06Rec(PUnionSJL06 pSJL06)
{
	return(UnionRewriteSJL06Rec(pSJL06));
}

int UnionRewriteSJL06Rec(PUnionSJL06 pSJL06)
{
	int			ret;

	FILE			*fp;
	char			fileName[256];
				
		
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(pSJL06->staticAttr.ipAddr,fileName);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionRewriteSJL06Rec:: fopen [%s]!\n",fileName);
		return(-1);
	}
	
	if ((ret = UnionWriteSJL06RecToFile(fp,pSJL06)) < 0)
		UnionUserErrLog("in UnionRewriteSJL06Rec:: UnionWriteSJL06RecToFile!\n");

	fclose(fp);
	
	return(ret);
}

int UnionPrintSJL06Rec(PUnionSJL06 pSJL06Grp)
{
	return(UnionWriteSJL06RecToFile(stdout,pSJL06Grp));
}

int UnionWriteSJL06RecToFile(FILE *fp,PUnionSJL06 pSJL06)
{
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionWriteSJL06RecToFile:: null pointer!\n");
		return(-1);
	}
	
	fprintf(fp,"# 静态属性\n");
	fprintf(fp,"[hsmGrpID]			[%s]\n",pSJL06->staticAttr.hsmGrpID);
	fprintf(fp,"[hsmCmdVersion]			[%s]\n",pSJL06->staticAttr.hsmCmdVersion);
	fprintf(fp,"[ipAddr]			[%s]\n",pSJL06->staticAttr.ipAddr);
	fprintf(fp,"[port]				[%d]\n",pSJL06->staticAttr.port);
	fprintf(fp,"[lenOfLenFld]			[%d]\n",pSJL06->staticAttr.lenOfLenFld);
	fprintf(fp,"[lenOfMsgHeader]		[%d]\n",pSJL06->staticAttr.lenOfMsgHeader);
	fprintf(fp,"[msgHeader]			[%s]\n",pSJL06->staticAttr.msgHeader);
	fprintf(fp,"[remark]			[%s]\n",pSJL06->staticAttr.remark);
	fprintf(fp,"[maxConLongConn]		[%d]\n",pSJL06->staticAttr.maxConLongConn);
	fprintf(fp,"[registerDate]			[%s]\n",pSJL06->staticAttr.registerDate);
	
	fprintf(fp,"# 动态属性\n");
	fprintf(fp,"[timeoutTimes]			[%ld]\n",pSJL06->dynamicAttr.timeoutTimes);
	fprintf(fp,"[connFailTimes]			[%ld]\n",pSJL06->dynamicAttr.connFailTimes);
	fprintf(fp,"[abnormalCmdTimes]		[%ld]\n",pSJL06->dynamicAttr.abnormalCmdTimes);
	fprintf(fp,"[normalCmdTimes]		[%ld]\n",pSJL06->dynamicAttr.normalCmdTimes);
	fprintf(fp,"[continueFailTimes]		[%ld]\n",pSJL06->dynamicAttr.continueFailTimes);
	fprintf(fp,"[downTimes]			[%d]\n",pSJL06->dynamicAttr.downTimes);
	fprintf(fp,"[lastDownDate]			[%s]\n",pSJL06->dynamicAttr.lastDownDate);
	fprintf(fp,"[status]			[%c]\n",pSJL06->dynamicAttr.status);
	fprintf(fp,"[activeLongConn]		[%d]\n",pSJL06->dynamicAttr.activeLongConn);
	
	return(0);
}

// 删除一个Hsm
int UnionDeleteSJL06Rec(char *pipAddr)
{
	int			ret;	
	char			fileName[256];
	char			cmd[256];
				
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(pipAddr,fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}

int UnionSelectSJL06Rec(PUnionSJL06 pSJL06)
{
	int			ret;
	
	char			fileName[512];
	char			*pVar;
			
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(pSJL06->staticAttr.ipAddr,fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionInitEnvi!\n");
		return(ret);
	}
	
	if ((pVar = UnionGetEnviVarByName("hsmGrpID")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [hsmGrpID]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.hsmGrpID,pVar);
				
	if ((pVar = UnionGetEnviVarByName("hsmCmdVersion")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [hsmCmdVersion]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.hsmCmdVersion,pVar);
				
	if ((pVar = UnionGetEnviVarByName("ipAddr")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [ipAddr]!\n");
		goto abnormalExit;
	}
	if (strcmp(pSJL06->staticAttr.ipAddr,pVar) != 0)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: [%s] [%s] not same!\n",pVar,pSJL06->staticAttr.ipAddr);
		goto abnormalExit;
	}
				
	if ((pVar = UnionGetEnviVarByName("port")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [port]!\n");
		goto abnormalExit;
	}
	pSJL06->staticAttr.port = atoi(pVar);
				
	if ((pVar = UnionGetEnviVarByName("lenOfLenFld")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [lenOfLenFld]!\n");
		goto abnormalExit;
	}
	pSJL06->staticAttr.lenOfLenFld = atoi(pVar);
				
	if ((pVar = UnionGetEnviVarByName("lenOfMsgHeader")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [lenOfMsgHeader]!\n");
		goto abnormalExit;
	}
	pSJL06->staticAttr.lenOfMsgHeader = atoi(pVar);
				
	if ((pVar = UnionGetEnviVarByName("msgHeader")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [msgHeader]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.msgHeader,pVar);
				
	if ((pVar = UnionGetEnviVarByName("remark")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [remark]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.remark,pVar);
				
	if ((pVar = UnionGetEnviVarByName("maxConLongConn")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [maxConLongConn]!\n");
		goto abnormalExit;
	}
	pSJL06->staticAttr.maxConLongConn = atoi(pVar);
				
	if ((pVar = UnionGetEnviVarByName("registerDate")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [registerDate]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.registerDate,pVar);
				
	if ((pVar = UnionGetEnviVarByName("status")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [status]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.status = *pVar;
				
	if ((pVar = UnionGetEnviVarByName("timeoutTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [timeoutTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.timeoutTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("connFailTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [connFailTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.connFailTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("abnormalCmdTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [abnormalCmdTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.abnormalCmdTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("normalCmdTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [normalCmdTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.normalCmdTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("continueFailTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [continueFailTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.continueFailTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("downTimes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [downTimes]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.downTimes = atol(pVar);
				
	if ((pVar = UnionGetEnviVarByName("activeLongConn")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [activeLongConn]!\n");
		goto abnormalExit;
	}
	pSJL06->dynamicAttr.activeLongConn = atoi(pVar);
				
	if ((pVar = UnionGetEnviVarByName("lastDownDate")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [lastDownDate]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->dynamicAttr.lastDownDate,pVar);
	
	UnionClearEnvi();
	
	return(0);
			
abnormalExit:
	UnionClearEnvi();
	return(-1);
}

int UnionUpdateSJL06StaticAttr(PUnionSJL06 pSJL06)
{
	int			ret;
	TUnionSJL06		tmpSJL06;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionUpdateSJL06StaticAttr:: NullPointer\n");
		return(-1);
	}
	
	memset(&tmpSJL06,0,sizeof(tmpSJL06));
	strcpy(tmpSJL06.staticAttr.ipAddr,pSJL06->staticAttr.ipAddr);
	if ((ret = UnionSelectSJL06Rec(&tmpSJL06)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSJL06StaticAttr:: UnionSelectSJL06Rec [%s]!\n",pSJL06->staticAttr.ipAddr);
		return(-1);
	}
	
	memcpy(&(tmpSJL06.staticAttr),&(pSJL06->staticAttr),sizeof(tmpSJL06.staticAttr));

	return(UnionRewriteSJL06Rec(&tmpSJL06));
}

// 更新动态改变的工作域
int UnionUpdateSJL06DynamicAttr(PUnionSJL06 pSJL06)
{
	int			ret;
	TUnionSJL06		tmpSJL06;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionUpdateSJL06DynamicAttr:: NullPointer\n");
		return(-1);
	}
	
	memset(&tmpSJL06,0,sizeof(tmpSJL06));
	strcpy(tmpSJL06.staticAttr.ipAddr,pSJL06->staticAttr.ipAddr);
	if ((ret = UnionSelectSJL06Rec(&tmpSJL06)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSJL06DynamicAttr:: UnionSelectSJL06Rec [%s]!\n",pSJL06->staticAttr.ipAddr);
		return(-1);
	}
	
	memcpy(&(tmpSJL06.dynamicAttr),&(pSJL06->dynamicAttr),sizeof(tmpSJL06.dynamicAttr));

	return(UnionRewriteSJL06Rec(&tmpSJL06));
}

int UnionIsValidHsmCmdVersion(char *version)
{
	if ((strcmp(version,conHsmCmdVersionSJL06) != 0) && (strcmp(version,conHsmCmdVersionSJL05) != 0) && (strcmp(version,conHsmCmdVersionRacal) != 0))
		return(0);
	else
		return(1);
}

int UnionGetHsmCmdVersion(PUnionSJL06 pSJL06)
{
	if (pSJL06 == NULL)
		return(-1);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionSJL06) == 0)
		return(conHsmCmdOfSJL06);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionSJL05) == 0)
		return(conHsmCmdOfSJL05);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionRacal) == 0)
		return(conHsmCmdOfRacal);
	return(-1);
}
		

// Author:	Wolfgang Wang
// Date:	2003/09/12
// Version:	1.0

// 2004/06/22，Wolfgang Wang升级为2.0
// 升级是因为修改了TUnionSJL06StaticAttr的结构

// 2006/7/26 在sjl062.0.c基础上升级

#ifndef _UnionSJL06_2_x_Above_
#define _UnionSJL06_2_x_Above_	// Added By Wolfgang Wang, 2004/06/22，这个开关是必须的
#endif

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sjl06.h"
#include "UnionLog.h"

#include "UnionStr.h"
#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionErrCode.h"

#ifdef _useComplexDB_
#include "unionComplexDBRecord.h"
#include "unionHsmGroup.h"
#include "unionHsm.h"
#include "unionREC.h"
#endif

#ifndef _useComplexDB_
int UnionExistSJL06Def(char *ipAddr)
{
	char		fileName[512];
	FILE		*fp;
	
	if (ipAddr == NULL)
		return(errCodeParameter);
				
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(ipAddr,fileName);

	if ((fp = fopen(fileName,"r")) != NULL)
	{
		fclose(fp);
		return(1);
	}
	else
		return(0);
}	
#else
int UnionExistSJL06Def(char *ipAddr)
{
	TUnionHsm tHsm;
	int ret = 0;

	if (ipAddr == NULL)
		return(errCodeParameter);

	memset(&tHsm, 0, sizeof(TUnionHsm));

	if( (ret = UnionReadHsmRec(ipAddr, &tHsm)) < 0 )
	{
		return(0);
	}
	{
		return(1);
	}
}
#endif

int UnionReadDefaultAttrOfSJL06(PUnionSJL06 psjl06)
{
	int		ret;
	
	if (psjl06 == NULL)
		return(errCodeParameter);
		
	memset(psjl06,0,sizeof(*psjl06));
	strcpy(psjl06->staticAttr.ipAddr,"default");
	if ((ret = UnionSelectSJL06Rec(psjl06)) < 0)	
	{
		UnionUserErrLog("in UnionReadDefaultAttrOfSJL06:: default Hsm Conf [default.CFG] not exist or conf error!\n");
		return(errCodeSJL06MDL_DefaultSJL06Conf);
	}
	return(ret);
}

int UnionGetFileNameOfDefaultHsmDef(char *fileName)
{
	sprintf(fileName,"%s/HsmCfg/default.CFG",getenv("UNIONETC"));
	return(0);
}

// 2006/7/26增加，创建一个密码机配置文件，如果其配置文件不存在
// 根据$UNIONETC/HsmCfg/default.CFG来创建
int UnionCreateSJL06RecWhenNotExist(char *ipAddr)
{
	int		ret;
	TUnionSJL06	sjl06;
	TUnionSJL06	defaultSJL06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionCreateSJL06RecWhenNotExist:: ipAddr [%s] not valid!\n",ipAddr);
		return(errCodeInvalidIPAddr);
	}
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) == 0)	// 密码机的配置文件已存在
		return(0);
	
	// 密码机配置文件不存在
	// 查找缺省密码机配置
	memset(&defaultSJL06,0,sizeof(defaultSJL06));
	strcpy(defaultSJL06.staticAttr.ipAddr,"default");
	if ((ret = UnionSelectSJL06Rec(&defaultSJL06)) < 0)	
	{
		UnionUserErrLog("in UnionCreateSJL06RecWhenNotExist:: default Hsm Conf [default.CFG] not exist or conf error!\n");
		return(errCodeSJL06MDL_DefaultSJL06Conf);
	}
	memcpy(&sjl06,&defaultSJL06,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionRewriteSJL06Rec(&sjl06)) < 0)	
	{
		UnionUserErrLog("in UnionCreateSJL06RecWhenNotExist:: UnionRewriteSJL06Rec ipAddr [%s]!\n",ipAddr);
		return(ret);
	}
	return(ret);
}

#ifndef _useComplexDB_
int UnionGetHsmCfgFileName(char *ipAddr,char *fileName)
{
	sprintf(fileName,"%s/HsmCfg/%s.CFG",getenv("UNIONETC"),ipAddr);
	return(0);
}
#else
int UnionGetHsmCfgFileName(char *ipAddr,char *fileName)
{
	sprintf(fileName,"hsm");
	return(0);
}
#endif

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
		return(errCodeParameter);
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
	
	// Added by Wolfgang Wang, 2004/06/22
	strcpy(staticAttr.testCmdReq,"01");
	strcpy(staticAttr.testCmdSuccessRes,"0200");
	// End of Addition of 2004/06/22
	
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
	// Added by Wolfgang Wang, 2004/06/22
inputTestCmd:
	if (UnionIsQuit(p = UnionInput("请输测试指令(最多%d个字符)：",sizeof(staticAttr.testCmdReq)-1)))
		return(0);
	if (strlen(p) >= sizeof(staticAttr.testCmdReq))
	{
		printf("测试指令过长！请重输！\n");
		goto inputTestCmd;
	}
	strcpy(staticAttr.testCmdReq,p);
inputTestCmdSuccessRes:
	if (UnionIsQuit(p = UnionInput("请输测试指令成功响应标识(最多%d个字符)：",sizeof(staticAttr.testCmdSuccessRes)-1)))
		return(0);
	if (strlen(p) >= sizeof(staticAttr.testCmdSuccessRes))
	{
		printf("测试指令过长！请重输！\n");
		goto inputTestCmdSuccessRes;
	}
	strcpy(staticAttr.testCmdSuccessRes,p);
	// End of Addition of 2004/06/22

	sjl06.dynamicAttr = dynamicAttr;
	sjl06.staticAttr = staticAttr;
	printf("IP地址     [%s]\n",staticAttr.ipAddr);
	printf("工作组     [%s]\n",staticAttr.hsmGrpID);
	printf("消息头长度 [%d]\n",staticAttr.lenOfMsgHeader);
	printf("说明       [%s]\n",staticAttr.remark);
	
	// Added by Wolfgang Wang, 2004/06/22
	printf("测试指令   [%s]\n",staticAttr.testCmdReq);
	printf("指令响应   [%s]\n",staticAttr.testCmdSuccessRes);
	// End of Addition of 2004/06/22

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

int UnionDeleteSJL06ConfArtificially()
{
	char			*p;
	char			ipAddr[20];
	int			ret;
inputIPAddr:
	if (UnionIsQuit(p = UnionInput("Delete>请输入密码机IP地址：")))
		return(0);
	if (!UnionIsValidIPAddrStr(p))
	{
		printf("Delete>非法的IP地址！请重输！\n");
		goto inputIPAddr;
	}
	memset(ipAddr,0,sizeof(ipAddr));
	strcpy(ipAddr,p);
	if (UnionConfirm("Delete>确认删除该密码机[%s]的配置文件吗？",ipAddr))
	{
		if ((ret = UnionDeleteSJL06Conf(ipAddr)) < 0)
			printf("删除失败！\n");
		else
			printf("删除成功！\n");
	}
	if (UnionConfirm("继续删除吗？"))
		goto inputIPAddr;
	
	return(0);
}

#ifndef _useComplexDB_
int UnionDeleteSJL06Conf(char *ipAddr)
{
	char			fileName[256];
	char			cmd[256];				
	
	if (UnionExistSJL06Def(ipAddr) <= 0)
		return(errCodeSJL06MDL_SJL06NotExists);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(ipAddr,fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}
#else
int UnionDeleteSJL06Conf(char *ipAddr)
{
	int ret = 0;

	if (ipAddr == NULL)
		return(errCodeParameter);

	if (UnionExistSJL06Def(ipAddr) <= 0)
		return(errCodeSJL06MDL_SJL06NotExists);

	if( (ret = UnionDeleteHsmRec(ipAddr)) < 0 )
	{
		UnionUserErrLog("in UnionDeleteSJL06Conf:: UnionDeleteHsmRec ipAddr=[%s]\n", ipAddr);
		return (ret);
	}

	return (ret);
}
#endif
	
int UnionPrintSJL06Conf(char *ipAddr)
{
	int		ret;
	TUnionSJL06	sjl06;
	
	memset(&sjl06,0,sizeof(sjl06));
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionPrintSJL06Conf:: invalid ipAddr [%s]\n",ipAddr);
		return(errCodeParameter);
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
	int	ret;
	
	if (pSJL06 == NULL)
		return(errCodeParameter);
	if ((ret = UnionExistSJL06Def(pSJL06->staticAttr.ipAddr)) > 0)
		return(errCodeSJL06MDL_SJL06AlreadyExists);
	return(UnionRewriteSJL06Rec(pSJL06));
}

#ifndef _useComplexDB_
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
		return(errCodeUseOSErrCode);
	}
	
	if ((ret = UnionWriteSJL06RecToFile(fp,pSJL06)) < 0)
		UnionUserErrLog("in UnionRewriteSJL06Rec:: UnionWriteSJL06RecToFile!\n");

	fclose(fp);
	
	return(ret);
}
#else
int UnionRewriteSJL06Rec(PUnionSJL06 pSJL06)
{
	int	ret = 0;
	TUnionHsm tHsm;
	TUnionHsmGroup tHsmGrp;

	memset(&tHsm, 0, sizeof(TUnionHsm));
	memset(&tHsmGrp, 0, sizeof(TUnionHsmGroup));

	// hsm
	strcpy(tHsm.ipAddr, pSJL06->staticAttr.ipAddr);
	strcpy(tHsm.hsmGrpID, pSJL06->staticAttr.hsmGrpID);
	tHsm.port = pSJL06->staticAttr.port;
	tHsm.hsmStatusID = pSJL06->dynamicAttr.status - '0' + 0;
	strcpy(tHsm.remark, pSJL06->staticAttr.remark);

	if( (ret = UnionUpdateHsmRec(&tHsm)) < 0 )
	{
		UnionProgramerLog("in UnionRewriteSJL06Rec:: UnionUpdateHsmRec error ret=[%d]\n", ret);

		if( (ret = UnionInsertHsmRec(&tHsm)) < 0 )
		{
			UnionUserErrLog("in UnionRewriteSJL06Rec:: UnionInsertHsmRec error\n");
			return (ret);
		}
	}

	// hsmGroup
	strcpy(tHsmGrp.hsmGrpID, pSJL06->staticAttr.hsmGrpID);

	if( strcpy(pSJL06->staticAttr.hsmCmdVersion, "SJL05") == 0 )
	{
		tHsmGrp.hsmCmdVersionID = conHsmCmdVerSJL05StandardHsmCmd;
	}
	else
	{
		if( strcpy(pSJL06->staticAttr.hsmCmdVersion, "SJL06") == 0 )
		{
			tHsmGrp.hsmCmdVersionID = conHsmCmdVerSJL06StandardHsmCmd;
		}
		else
		{
			tHsmGrp.hsmCmdVersionID = conHsmCmdVerRacalStandardHsmCmd;
		}
	}

	tHsmGrp.lenOfLenFld = pSJL06->staticAttr.lenOfLenFld;
	tHsmGrp.lenOfMsgHeader = pSJL06->staticAttr.lenOfMsgHeader;
	strcpy(tHsmGrp.msgHeader, pSJL06->staticAttr.msgHeader);
	strcpy(tHsmGrp.testCmdReq, pSJL06->staticAttr.testCmdReq);
	strcpy(tHsmGrp.testCmdSuccessRes, pSJL06->staticAttr.testCmdSuccessRes);

	if( (ret = UnionUpdateHsmGroupRec(&tHsmGrp)) < 0 )
	{
		UnionProgramerLog("in UnionRewriteSJL06Rec:: UnionUpdateHsmGroupRec error ret=[%d]\n", ret);

		if( (ret = UnionInsertHsmGroupRec(&tHsmGrp)) < 0 )
		{
			UnionUserErrLog("in UnionRewriteSJL06Rec:: UnionInsertHsmGroupRec error\n");
			return (ret);
		}
	}

	return(ret);
}
#endif

int UnionPrintSJL06Rec(PUnionSJL06 pSJL06Grp)
{
	return(UnionWriteSJL06RecToFile(stdout,pSJL06Grp));
}

int UnionWriteSJL06RecToFile(FILE *fp,PUnionSJL06 pSJL06)
{
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionWriteSJL06RecToFile:: null pointer!\n");
		return(errCodeParameter);
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
	// Added by Wolfgang Wang, 2004/06/22
	fprintf(fp,"[testCmdReq]			[%s]\n",pSJL06->staticAttr.testCmdReq);
	fprintf(fp,"[testCmdSuccessRes]		[%s]\n",pSJL06->staticAttr.testCmdSuccessRes);
	// End of Addition of 2004/06/22
	
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

#ifndef _useComplexDB_
// 删除一个Hsm
int UnionDeleteSJL06Rec(char *ipAddr)
{
	int			ret;	
	char			fileName[256];
	char			cmd[256];
	
	if (UnionExistSJL06Def(ipAddr) <= 0)
		return(errCodeSJL06MDL_SJL06NotExists);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetHsmCfgFileName(ipAddr,fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}
#else
int UnionDeleteSJL06Rec(char *ipAddr)
{
	int ret = 0;

	if (ipAddr == NULL)
		return(errCodeParameter);

	if (UnionExistSJL06Def(ipAddr) <= 0)
		return(errCodeSJL06MDL_SJL06NotExists);

	if( (ret = UnionDeleteHsmRec(ipAddr)) < 0 )
	{
		UnionUserErrLog("in UnionDeleteSJL06Rec:: UnionDeleteHsmRec ipAddr=[%s]\n", ipAddr);
		return (ret);
	}

	return (ret);
}
#endif

#ifndef _useComplexDB_
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

	// Added by Wolfgang Wang, 2004/06/22	
	if ((pVar = UnionGetEnviVarByName("testCmdReq")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [testCmdReq]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.testCmdReq,pVar);
	
	if ((pVar = UnionGetEnviVarByName("testCmdSuccessRes")) == NULL)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionGetEnviVarByName [testCmdSuccessRes]!\n");
		goto abnormalExit;
	}
	strcpy(pSJL06->staticAttr.testCmdSuccessRes,pVar);
	// End of Addition of 2004/06/22
				

	UnionClearEnvi();
	
	return(0);
			
abnormalExit:
	UnionClearEnvi();
	return(errCodeSJL06MDL_SJL06Def);
}
#else
int UnionSelectSJL06Rec(PUnionSJL06 pSJL06) 
{
	int		ret = 0;
	char	*cpIpAddr;
	TUnionHsm	hsm;
	TUnionHsmGroup	hsmGrp;

	if( pSJL06 == NULL )
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: null pointer!\n");
		return(errCodeParameter);
	}
	memset(&hsm, 0, sizeof(hsm));
	memset(&hsmGrp, 0, sizeof(hsmGrp));

	if( strncmp(pSJL06->staticAttr.ipAddr, "default", 7) == 0 )
	{
		if ((cpIpAddr = UnionReadStringTypeRECVar("defaultKmcHsmIPAddr")) == NULL)
		{
			UnionUserErrLog("in UnionSelectSJL06Rec:: UnionReadStringTypeRECVar defaultKmcHsmIPAddr!\n");
			return(errCodeRECMDL_VarNotExists);
		}
		
		UnionLog("in UnionSelectSJL06Rec: Get defaultKmcHsmIPAddr = [%s] \n", cpIpAddr);
		if (!UnionIsValidIPAddrStr(cpIpAddr))
		{
			UnionUserErrLog("in UnionSelectSJL06Rec:: cpIpAddr [%s] is not valid ipaddr!\n", cpIpAddr);
			return(errCodeInvalidIPAddr);
		}

		strcpy(pSJL06->staticAttr.ipAddr, cpIpAddr);
	}

	ret = UnionReadHsmRec(pSJL06->staticAttr.ipAddr, &hsm);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionReadHsmRec!\n");
		return(ret);
	}

	//UnionUserErrLog("in UnionSelectSJL06Rec:: UnionReadHsmGroupRec [%s]!\n",hsm.hsmGrpID);
	ret = UnionReadHsmGroupRec(hsm.hsmGrpID,&hsmGrp);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSelectSJL06Rec:: UnionReadHsmGroupRec [%s]!\n",hsm.hsmGrpID);
		return(ret);
	}
	
	// 静态属性
	if (strlen(hsm.hsmGrpID) >= sizeof(pSJL06->staticAttr.hsmGrpID))
		memcpy(pSJL06->staticAttr.hsmGrpID,hsm.hsmGrpID,sizeof(pSJL06->staticAttr.hsmGrpID)-1);
	else
		strcpy(pSJL06->staticAttr.hsmGrpID,hsm.hsmGrpID);

	switch (hsmGrp.hsmCmdVersionID)
	{
		case	conHsmCmdVerSJL05StandardHsmCmd:
			strcpy(pSJL06->staticAttr.hsmCmdVersion, "SJL05");
		case	conHsmCmdVerSJL06InternationalCardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			strcpy(pSJL06->staticAttr.hsmCmdVersion, "SJL06");
		default:
			strcpy(pSJL06->staticAttr.hsmCmdVersion, "RACAL");
	}

	strcpy(pSJL06->staticAttr.ipAddr,hsm.ipAddr);
	pSJL06->staticAttr.port = hsm.port;
	pSJL06->staticAttr.lenOfLenFld = hsmGrp.lenOfLenFld;
	pSJL06->staticAttr.lenOfMsgHeader = hsmGrp.lenOfMsgHeader;
	if (strlen(hsmGrp.msgHeader) >= sizeof(pSJL06->staticAttr.msgHeader))
		memcpy(pSJL06->staticAttr.msgHeader,hsmGrp.msgHeader,sizeof(pSJL06->staticAttr.msgHeader)-1);
	else
		strcpy(pSJL06->staticAttr.msgHeader,hsmGrp.msgHeader);
	if (strlen(hsm.remark) >= sizeof(pSJL06->staticAttr.remark))
		memcpy(pSJL06->staticAttr.remark,hsm.remark,sizeof(pSJL06->staticAttr.remark)-1);
	else
		strcpy(pSJL06->staticAttr.remark,hsm.remark);
	pSJL06->staticAttr.maxConLongConn = 128;
	if (strlen(hsm.inputDate) >= sizeof(pSJL06->staticAttr.registerDate))
		memcpy(pSJL06->staticAttr.registerDate,hsm.inputDate,sizeof(pSJL06->staticAttr.registerDate)-1);
	else
		strcpy(pSJL06->staticAttr.registerDate,hsm.inputDate);
	if (strlen(hsmGrp.testCmdReq) >= sizeof(pSJL06->staticAttr.testCmdReq))
		memcpy(pSJL06->staticAttr.testCmdReq,hsmGrp.testCmdReq,sizeof(pSJL06->staticAttr.testCmdReq)-1);
	else
		strcpy(pSJL06->staticAttr.testCmdReq,hsmGrp.testCmdReq);
	if (strlen(hsmGrp.testCmdSuccessRes) >= sizeof(pSJL06->staticAttr.testCmdSuccessRes))
		memcpy(pSJL06->staticAttr.testCmdSuccessRes,hsmGrp.testCmdSuccessRes,sizeof(pSJL06->staticAttr.testCmdSuccessRes)-1);
	else
		strcpy(pSJL06->staticAttr.testCmdSuccessRes,hsmGrp.testCmdSuccessRes);
	
	// 动态属性
	pSJL06->dynamicAttr.status = hsm.hsmStatusID - 0 + '0';
	UnionProgramerLog("in UnionSelectSJL06Rec:: sjl06Status = [%c] hsmStatus = [%d]\n",pSJL06->dynamicAttr.status,hsm.hsmStatusID);

	return(0);
}
#endif

int UnionUpdateSJL06StaticAttr(PUnionSJL06 pSJL06)
{
	int			ret;
	TUnionSJL06		tmpSJL06;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionUpdateSJL06StaticAttr:: NullPointer\n");
		return(errCodeParameter);
	}
	
	memset(&tmpSJL06,0,sizeof(tmpSJL06));
	strcpy(tmpSJL06.staticAttr.ipAddr,pSJL06->staticAttr.ipAddr);
	if ((ret = UnionSelectSJL06Rec(&tmpSJL06)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSJL06StaticAttr:: UnionSelectSJL06Rec [%s]!\n",pSJL06->staticAttr.ipAddr);
		return(ret);
	}
	
	memcpy(&(tmpSJL06.staticAttr),&(pSJL06->staticAttr),sizeof(tmpSJL06.staticAttr));

	return(UnionRewriteSJL06Rec(&tmpSJL06));
}

// 更新动态改变的工作域
int UnionUpdateSJL06DynamicAttr(PUnionSJL06 pSJL06)
{
	/*
	int			ret;
	TUnionSJL06		tmpSJL06;
		
	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionUpdateSJL06DynamicAttr:: NullPointer\n");
		return(errCodeParameter);
	}
	
	memset(&tmpSJL06,0,sizeof(tmpSJL06));
	strcpy(tmpSJL06.staticAttr.ipAddr,pSJL06->staticAttr.ipAddr);
	if ((ret = UnionSelectSJL06Rec(&tmpSJL06)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSJL06DynamicAttr:: UnionSelectSJL06Rec [%s]!\n",pSJL06->staticAttr.ipAddr);
		return(ret);
	}
	
	memcpy(&(tmpSJL06.dynamicAttr),&(pSJL06->dynamicAttr),sizeof(tmpSJL06.dynamicAttr));

	return(UnionRewriteSJL06Rec(&tmpSJL06));
	*/
	return(0);
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
		return(errCodeParameter);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionSJL06) == 0)
		return(conHsmCmdOfSJL06);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionSJL05) == 0)
		return(conHsmCmdOfSJL05);
	if (strcmp(pSJL06->staticAttr.hsmCmdVersion,conHsmCmdVersionRacal) == 0)
		return(conHsmCmdOfRacal);
	return(errCodeSJL06MDL_InvalidSJL06Type);
}

int UnionModifySJL06StaticAttr(PUnionSJL06 psjl06)
{
	TUnionSJL06	sjl06;
	int		ret;
	
	if (psjl06 == NULL)
		return(errCodeParameter);
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,psjl06->staticAttr.ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in UnionModifySJL06StaticAttr:: UnionSelectSJL06Rec [%s]\n",sjl06.staticAttr.ipAddr);
		return(ret);
	}
	if (strlen(psjl06->staticAttr.hsmGrpID) > 0)
		strcpy(sjl06.staticAttr.hsmGrpID,psjl06->staticAttr.hsmGrpID);
	if (strlen(psjl06->staticAttr.hsmCmdVersion) > 0)
		strcpy(sjl06.staticAttr.hsmCmdVersion,psjl06->staticAttr.hsmCmdVersion);
	if (strlen(psjl06->staticAttr.msgHeader) > 0)
		strcpy(sjl06.staticAttr.msgHeader,psjl06->staticAttr.msgHeader);
	if (strlen(psjl06->staticAttr.remark) > 0)
		strcpy(sjl06.staticAttr.remark,psjl06->staticAttr.remark);
	if (strlen(psjl06->staticAttr.testCmdReq) > 0)
		strcpy(sjl06.staticAttr.testCmdReq,psjl06->staticAttr.testCmdReq);
	if (strlen(psjl06->staticAttr.testCmdSuccessRes) > 0)
		strcpy(sjl06.staticAttr.testCmdSuccessRes,psjl06->staticAttr.testCmdSuccessRes);
	if (psjl06->staticAttr.port > 0)
		sjl06.staticAttr.port = psjl06->staticAttr.port;
	if (psjl06->staticAttr.lenOfLenFld >= 0)
		sjl06.staticAttr.lenOfLenFld = psjl06->staticAttr.lenOfLenFld;
	if (psjl06->staticAttr.lenOfMsgHeader >= 0)
		sjl06.staticAttr.lenOfMsgHeader = psjl06->staticAttr.lenOfMsgHeader;
	if (psjl06->staticAttr.maxConLongConn > 0)
		sjl06.staticAttr.maxConLongConn = psjl06->staticAttr.maxConLongConn;
	if ((psjl06->dynamicAttr.status >= conAbnormalSJL06) && (psjl06->dynamicAttr.status <= conColdBackupSJL06))
		sjl06.dynamicAttr.status = psjl06->dynamicAttr.status;

	return(UnionRewriteSJL06Rec(&sjl06));
}
	
int UnionPrintAllSJL06RecToSpecFile(char *fileName)
{
	char	tmpBuf[512+1];
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(errCodeParameter);
	sprintf(tmpBuf,"ls %s/HsmCfg/*.*.*.*.CFG > %s",getenv("UNIONETC"),fileName);
	system(tmpBuf);
	return(0);
}

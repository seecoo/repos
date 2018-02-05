//	Wolfgang Wang
//	2004/06/17

#include <stdio.h>
#include <string.h>
#include <time.h>

#define _UnionEnv_3_x_
#include "UnionEnv.h"

#include "unionLisence.h"

#include "UnionDes.h"
#include "UnionMD5.h"
#include "unionCommand.h"

#include "unionModule.h"
#include "UnionLog.h"


PUnionSharedMemoryModule	pgunionLisenceModule = NULL;
PUnionLisence			pgunionLisence = NULL;

int UnionConnectLisenceModule()
{
	// 已经连接
	if ((pgunionLisenceModule != NULL) && (pgunionLisence != NULL))
		return(0);
		
	if ((pgunionLisenceModule = UnionConnectSharedMemoryModule(conMDLNameOfUnionLisence,sizeof(TUnionLisence))) == NULL)
	{
		UnionUserErrLog("in UnionConnectLisenceModule:: UnionConnectSharedMemoryModule!\n");
		return(-1);
	}
	if ((pgunionLisence = (PUnionLisence)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionLisenceModule)) == NULL)
	{
		UnionUserErrLog("in UnionConnectLisenceModule:: PUnionLisence!\n");
		return(-1);
	}
			
	if (UnionIsNewCreatedSharedMemoryModule(pgunionLisenceModule))
	{
		return(UnionReloadLisenceModule());
	}
	else
		return(0);
}

	
int UnionDisconnectLisenceModule()
{
	pgunionLisenceModule = NULL;
	pgunionLisence = NULL;
	return(UnionDisconnectShareModule(pgunionLisenceModule));
}

int UnionRemoveLisenceModule()
{
	pgunionLisenceModule = NULL;
	pgunionLisence = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionLisence));
}

int UnionReloadLisenceModule()
{
	int		ret;
	char		fileName[512];
	char		*p;
	
	if ((pgunionLisence == NULL) || (pgunionLisenceModule == NULL))
	{
		if ((ret = UnionConnectLisenceModule()) < 0)
		{
			UnionUserErrLog("in UnionReloadLisenceModule:: UnionConnectLisenceModule!\n");
			return(-1);
		}
	}
	
	UnionGetFixedLisence(pgunionLisence);
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionRegistry.INI",getenv("UNIONETC"));
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadLisenceModule:: UnionInitEnvi!\n");
		return(ret);
	}
	if ((p = UnionGetEnviVarByName("registerTime")) == NULL)
	{
		UnionUserErrLog("in UnionReloadLisenceModule:: UnionGetEnviVarByName registerTime!\n");
		UnionClearEnvi();
		return(-1);
	}
	pgunionLisence->registerTime = atol(p);
	if ((p = UnionGetEnviVarByName("lisenceCode")) == NULL)
	{
		UnionUserErrLog("in UnionReloadLisenceModule:: UnionGetEnviVarByName lisenceCode!\n");
		UnionClearEnvi();
		return(-1);
	}
	if (strlen(p) > 16)
	{
		UnionUserErrLog("in UnionReloadLisenceModule:: lisenceCode [%s] too long!\n",p);
		UnionClearEnvi();
		return(-1);
	}
	strcpy(pgunionLisence->lisenceCode,p);
	UnionClearEnvi();
	
	return(0);
}

int UnionGetLisenceKey(char *key)
{
	char		buf[256];
	int		len;
	int		ret;
	TUnionLisence	lisence;
	
	if (key == NULL)
	{
		UnionUserErrLog("in UnionGetLisenceKey:: null pointer!\n");
		return(-1);
	}

	memset(&lisence,0,sizeof(lisence));	
	if ((ret = UnionGetFixedLisence(&lisence)) < 0)
	{
		UnionUserErrLog("in UnionGetLisenceKey:: UnionGetFixedLisence!\n");
		return(ret);
	}
	
	memset(buf,'0',sizeof(buf));
	len = 0;
	memcpy(buf,lisence.userName,40);
	len += 40;
	memcpy(buf+len,lisence.serialNumber,16);
	len += 16;
	memcpy(buf+len,lisence.lisenceData,16);
	len += 16;
	sprintf(buf+len,"%020ld",lisence.effectiveDays);
	len += 20;
	sprintf(buf+len,"%020ld",lisence.lisenceTime);
	len += 20;

	UnionMD5((unsigned char *)buf,len,(unsigned char *)key);
	
	return(0);
}
	
int UnionGenerateLisenceCode(PUnionLisence plisence,char *lisenceCode)
{
	char	buf[256];
	int	ret;
	char	key[48+1];
	
	if ((plisence == NULL) || (lisenceCode == NULL))
	{
		UnionUserErrLog("in UnionGenerateLisenceCode:: null pointer!\n");
		return(-1);
	}
	
	if ((ret = UnionGetLisenceKey(key)) < 0)
	{
		UnionUserErrLog("in UnionGenerateLisenceCode:: UnionGetLisenceKey!\n");
		return(-1);
	}
	
	sprintf(buf,"%016ld",plisence->registerTime);

	if ((ret = Union1DesEncrypt64BitsText(key,buf,lisenceCode)) < 0)
	{
		UnionUserErrLog("in UnionGenerateLisenceCode:: Generate LisenceCode Error!\n");
		return(ret);
	}
	
	return(0);
}

int UnionCreateLisenceFile()
{
	char		fileName[512];
	FILE		*fp;
	int		ret;
	TUnionLisence	fixedLisence;
	
	memset(&fixedLisence,0,sizeof(fixedLisence));
	UnionGetFixedLisence(&fixedLisence);
	
	if (strcmp(UnionInput("请输入用户名称::"),fixedLisence.userName) != 0)
	{
		printf("错误的用户名称!\n");
		return(-1);
	}
	if (strcmp(UnionInput("请输入产品序列号::"),fixedLisence.serialNumber) != 0)
	{
		printf("错误的产品序列号!\n");
		return(-1);
	}
	if (strcmp(UnionInput("请输入校验数据::"),fixedLisence.lisenceData) != 0)
	{
		printf("错误的校验数据!\n");
		return(-1);
	}
	time(&(fixedLisence.registerTime));
	if (fixedLisence.registerTime <= fixedLisence.lisenceTime)
	{
		printf("非法的产品使用日期\n");
		return(-1);
	}
	
	if ((ret = UnionGenerateLisenceCode(&fixedLisence,fixedLisence.lisenceCode)) < 0)
	{
		printf("生成产品校验码出错!\n");
		return(ret);
	}
		
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionRegistry.INI",getenv("UNIONETC"));
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateLisenceCode:: fopen [%s]\n",fileName);
		printf("无法生成产品校验码注册文件!\n");
		return(-1);
	}
	fprintf(fp,"[registerTime]         [%016ld]\n",fixedLisence.registerTime);
	fprintf(fp,"[lisenceCode]          [%s]\n",fixedLisence.lisenceCode);

	fclose(fp);

	printf("生成产品校验码注册文件成功.\n");
	
	return(0);
}

int UnionVerifyLisenceCode()
{
	int		ret;
	char		lisenceCode[16+1];
	time_t		now;

	if (pgunionLisence == NULL)
	{
		if ((ret = UnionConnectLisenceModule()) < 0)
		{
			UnionUserErrLog("in UnionVerifyLisenceCode:: UnionConnectLisenceModule!\n");
			return(-1);
		}
	}

	memset(lisenceCode,0,sizeof(lisenceCode));
	if ((ret = UnionGenerateLisenceCode(pgunionLisence,lisenceCode)) < 0)
	{
		UnionUserErrLog("in UnionVerifyLisenceCode:: UnionGenerateLisenceCode\n");
		return(ret);
	}
	
	if (strncmp(pgunionLisence->lisenceCode,lisenceCode,16) != 0)
	{
		UnionUserErrLog("in UnionVerifyLisenceCode:: Verify LisenceCode Error!\n");
		return(-1);
	}
	
	time(&now);
	if (now - pgunionLisence->registerTime > pgunionLisence->effectiveDays * 24 * 3600)
	{
		UnionUserErrLog("in UnionVerifyLisenceCode:: software out of date!\n");
		return(-1);
	}
	
	return(0);
}

int UnionPrintLisence()
{
	time_t	now;
	int	ret;

	if (pgunionLisence == NULL)
	{
		if ((ret = UnionConnectLisenceModule()) < 0)
		{
			UnionUserErrLog("in UnionPrintLisence:: UnionConnectLisenceModule!\n");
			return(-1);
		}
	}
	
	printf("[userName]              [%s]\n",pgunionLisence->userName);
	printf("[serialNumber]          [%s]\n",pgunionLisence->serialNumber);
	printf("[lisenceData]           [%s]\n",pgunionLisence->lisenceData);
	printf("[effectiveDays]         [%ld]\n",pgunionLisence->effectiveDays);
	printf("[registerTime]		[%ld]\n",pgunionLisence->registerTime);
	printf("[lisenceCode]           [%s]\n",pgunionLisence->lisenceCode);
	time(&now);
	printf("Still be effective in %ld days\n",pgunionLisence->effectiveDays-(now-pgunionLisence->registerTime)/24/3600);
	return(0);
}

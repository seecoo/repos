//	Wolfgang Wang
//	2003/1/19

//	2003/09/23,Wolfgang Wang, 在1.0基础上升级为2.0

#define _UnionEnv_3_x_
#define _UnionCopyrightRegistry_2_x_

#include <stdio.h>
#include <string.h>

#include "UnionMD5.h"
#include "UnionEnv.h"

#include "unionCopyrightRegistry.h"
#include "unionCommand.h"
#include "UnionLog.h"

int UnionSetCopyrightRegistryFixedLisenceData(PUnionCopyrightRegistry pCopyrightRegistry)
{
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionSetCopyrightRegistryFixedLisenceData:: null pointer!\n");
		return(-1);
	}

	strcpy(pCopyrightRegistry->fixedLisenceData,"6A4A42DB7438DC81C9439015682B840C42DBAF5087BB215EECD576A4A438DC81");
	return(0);
}

int UnionVerifyCopyrightRegistryUserNumber(int userNumber)
{
	int	i;
	
	if (userNumber <= 0)
	{
		UnionUserErrLog("in UnionVerifyCopyrightRegistryUserNumber:: userNumber [%d] < 0 !\n",userNumber);
		return(-1);
	}
	
	if (userNumber > 1024)
	{
		UnionUserErrLog("in UnionVerifyCopyrightRegistryUserNumber:: userNumber [%d] > 1024!\n",userNumber);
		return(-1);
	}
	
	for (i = userNumber; i > 1; i = i / 2)
	{
		if (i % 2 != 0)
		{
			UnionUserErrLog("in UnionVerifyCopyrightRegistryUserNumber:: userNumber [%d] must be 2 * 2 * ...!\n",userNumber);
			return(-1);
		}
	}
	
	return(0);
}

int UnionInitCopyrightRegistryStaticData(PUnionCopyrightRegistry pCopyrightRegistry,char *abbrProductName)
{
	int	ret;
	char	*pVar;
	char	fileName[512];
	
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: null pointer!\n");
		return(-1);
	}

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/%s/%s.Lisence.INI",
		getenv("UNIONPRODUCTREGISTRY"),
		abbrProductName,
		abbrProductName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: UnionInitEnvi [%s] < 0 !\n",fileName);
		return(ret);
	}
	
	if ((pVar = UnionGetEnviVarByName("productPrimaryName")) == NULL)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: UnionInitEnvi [%s] < 0 !\n","productPrimaryName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->primaryName,pVar);

	if ((pVar = UnionGetEnviVarByName("productMinorName")) == NULL)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: UnionInitEnvi [%s] < 0 !\n","productMinorName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->minorName,pVar);

	if ((pVar = UnionGetEnviVarByName("version")) == NULL)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: UnionInitEnvi [%s] < 0 !\n","version");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->version,pVar);

	if ((pVar = UnionGetEnviVarByName("userName")) == NULL)
	{
		UnionUserErrLog("in UnionInitCopyrightRegistryStaticData:: UnionInitEnvi [%s] < 0 !\n","userName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->userName,pVar);

	UnionClearEnvi();		
	return(0);

abnormalExit:
	UnionClearEnvi();		
	return(-1);
}

int UnionOrganizeCopyrightRegistryDynamicData(PUnionCopyrightRegistry pCopyrightRegistry)
{
	int	ret;
	char	*p;
	int	len;
	
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionOrganizeCopyrightRegistryDynamicData:: null pointer!\n");
		return(-1);
	}

	for (len = sizeof(pCopyrightRegistry->lisenceData)-1;;)
	{
		if (strlen(p = UnionInput("Input Lisence Data (= %d Characters)::\n%s",
						len,
						"++++++++++++++++\n")) != len)
			continue;
		strcpy(pCopyrightRegistry->lisenceData,p);
		break;
	}
	
	for (;;)
	{
		if (UnionVerifyCopyrightRegistryUserNumber(pCopyrightRegistry->userNumber = atoi(UnionInput("Input Lisenced User Number (1~1024)::"))) < 0 )
			continue;
		break;
	}

	UnionGetFullSystemDateTime(pCopyrightRegistry->registerDateTime);

	if ((ret = UnionSetCopyrightRegistryFixedLisenceData(pCopyrightRegistry)) < 0)
	{
		UnionUserErrLog("in UnionOrganizeCopyrightRegistryDynamicData:: UnionSetCopyrightRegistryFixedLisenceData!\n");
		return(ret);
	}
	
	return(0);
}

int UnionStoreCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry,char *nameOfProduct)
{
	FILE	*fp;
	char	fileName[512];
	
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionStoreCopyrightRegistry:: null pointer!\n");
		return(-1);
	}

	/*
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mkdir $HOME/UNIONPRODUCTREGISTRY");
	printf("%s\n",cmd);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mkdir $HOME/UNIONPRODUCTREGISTRY/%s",nameOfProduct);
	printf("%s\n",cmd);
	system(cmd);
	*/
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/%s/%sLisence%s.CFG",
			getenv("UNIONPRODUCTREGISTRY"),
			nameOfProduct,
			nameOfProduct,
			pCopyrightRegistry->registerDateTime);
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionStoreCopyrightRegistry:: fopen [%s]!\n",fileName);
		return(-1);
	}

	fprintf(fp,"[productPrimaryName]	[%s]\n",pCopyrightRegistry->primaryName);	
	fprintf(fp,"[productMinorName]	[%s]\n",pCopyrightRegistry->minorName);	
	fprintf(fp,"[version]		[%s]\n",pCopyrightRegistry->version);	
	fprintf(fp,"[userName]		[%s]\n",pCopyrightRegistry->userName);	
	fprintf(fp,"[serialNumber]		[%s]\n",pCopyrightRegistry->serialNumber);	
	fprintf(fp,"[verificationCode]	[%s]\n",pCopyrightRegistry->verificationCode);	
	fprintf(fp,"[lisenceData]		[%s]\n",pCopyrightRegistry->lisenceData);	
	fprintf(fp,"[registerDateTime]	[%s]\n",pCopyrightRegistry->registerDateTime);	
	fprintf(fp,"[userNumber]		[%d]\n",pCopyrightRegistry->userNumber);	

	fflush(fp);

	fclose(fp);

	//memset(cmd,0,sizeof(cmd));
	//sprintf(cmd,"cp %s $UNIONETC/unionRegistry.CFG",fileName);
	//printf("%s\n",cmd);
	//system(cmd);			

	return(0);
}

int UnionGenerateCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry)
{
	int			ret;
	
	char			buf[1024];
	char			md5Result[32+1];


	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionGenerateCopyrightRegistry:: null pointer!\n");
		return(-1);
	}

	sprintf(buf,"%s;%s;%s;%s;%s;%s;%06d;%s;%s;",
		pCopyrightRegistry->abbrName,
		pCopyrightRegistry->registerDateTime,
		pCopyrightRegistry->primaryName,	
		pCopyrightRegistry->minorName,
		pCopyrightRegistry->version,
		pCopyrightRegistry->userName,
		pCopyrightRegistry->userNumber,
		pCopyrightRegistry->lisenceData,
		pCopyrightRegistry->fixedLisenceData);
	
	// UnionLog("in UnionGenerateCopyrightRegistry:: len = [%d] buf = [%s]\n",strlen(buf),buf);
	
	if ((ret = UnionMD5((unsigned char *)buf,strlen(buf),(unsigned char *)md5Result)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCopyrightRegistry:: UnionMD5!\n");
		return(ret);
	}
	memcpy(pCopyrightRegistry->serialNumber,md5Result,16);
	memcpy(pCopyrightRegistry->verificationCode,md5Result+16,16);
	
	return(0);
}

int UnionVerifyCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry)
{
	int			ret;

	char			serialNumber[16+1];	// 产品的序列号
	char			verificationCode[16+1];	// 产品的验证码
	int			userNumber;
	char			fixedLisenceData[64+1];
	int			i;
	
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionGenerateCopyrightRegistry:: null pointer!\n");
		return(-1);
	}

	memset(serialNumber,0,sizeof(serialNumber));
	memset(verificationCode,0,sizeof(verificationCode));
	memcpy(serialNumber,pCopyrightRegistry->serialNumber,16);
	memcpy(verificationCode,pCopyrightRegistry->verificationCode,16);
	userNumber = pCopyrightRegistry->userNumber;
	memset(fixedLisenceData,0,sizeof(fixedLisenceData));
	memcpy(fixedLisenceData,pCopyrightRegistry->fixedLisenceData,64);

	// UnionLog("in UnionVerifyCopyrightRegistry:: serialNumber = [%s] verificationCode = [%s]\n",serialNumber,verificationCode);
	
	for (i = 1; i <= 1024; i = i * 2)
	{
		if ((ret = UnionSetCopyrightRegistryFixedLisenceData(pCopyrightRegistry)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCopyrightRegistry:: UnionSetCopyrightRegistryFixedLisenceData!\n");
			goto abnormalExit;
		}
	
		pCopyrightRegistry->userNumber = i;
		if ((ret = UnionGenerateCopyrightRegistry(pCopyrightRegistry)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCopyrightRegistry:: UnionGenerateCopyrightRegistry!\n");
			goto abnormalExit;
		}
		if ((memcmp(pCopyrightRegistry->serialNumber,serialNumber,16) == 0) &&
		    (memcmp(pCopyrightRegistry->serialNumber,serialNumber,16) == 0) && 
		    (pCopyrightRegistry->userNumber == userNumber))
		{
			return(0);
		}
		// UnionLog("in UnionVerifyCopyrightRegistry:: userNumber = [%d] serialNumber = [%s] verificationCode = [%s]\n",
		//	pCopyrightRegistry->userNumber,
		//	pCopyrightRegistry->serialNumber,
		//	pCopyrightRegistry->verificationCode);
	}

abnormalExit:
	memcpy(pCopyrightRegistry->serialNumber,serialNumber,16);
	memcpy(pCopyrightRegistry->verificationCode,verificationCode,16);
	pCopyrightRegistry->userNumber = userNumber;
	memcpy(pCopyrightRegistry->fixedLisenceData,fixedLisenceData,64);
	return(-1);
}

int UnionReadUserCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry)
{
	int	ret;
	char	*pVar;
	char	fileName[512];
	
	if (pCopyrightRegistry == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: null pointer!\n");
		return(-1);
	}

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionRegistry.CFG",getenv("UNIONETC"));
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n",fileName);
		return(ret);
	}
	
	if ((pVar = UnionGetEnviVarByName("productPrimaryName")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","productPrimaryName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->primaryName,pVar);

	if ((pVar = UnionGetEnviVarByName("productMinorName")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","productMinorName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->minorName,pVar);

	if ((pVar = UnionGetEnviVarByName("version")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","version");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->version,pVar);

	if ((pVar = UnionGetEnviVarByName("userName")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","userName");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->userName,pVar);

	if ((pVar = UnionGetEnviVarByName("serialNumber")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","serialNumber");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->serialNumber,pVar);
		
	if ((pVar = UnionGetEnviVarByName("verificationCode")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","verificationCode");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->verificationCode,pVar);

	if ((pVar = UnionGetEnviVarByName("lisenceData")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","lisenceData");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->lisenceData,pVar);

	if ((pVar = UnionGetEnviVarByName("registerDateTime")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","registerDateTime");
		goto abnormalExit;
	}
	else
		strcpy(pCopyrightRegistry->registerDateTime,pVar);

	if ((pVar = UnionGetEnviVarByName("userNumber")) == NULL)
	{
		UnionUserErrLog("in UnionReadUserCopyrightRegistry:: UnionInitEnvi [%s] < 0 !\n","userNumber");
		goto abnormalExit;
	}
	else
		pCopyrightRegistry->userNumber = atoi(pVar);

	UnionClearEnvi();		
	return(0);

abnormalExit:
	UnionClearEnvi();		
	return(-1);
}

int UnionVerifyUserCopyrightRegistry(char *abbrProductName)
{
	int			ret;

	TUnionCopyrightRegistry copyrightRegistry;
	
	memset(&copyrightRegistry,0,sizeof(copyrightRegistry));
	strcpy(copyrightRegistry.abbrName,abbrProductName);

	if ((ret = UnionReadUserCopyrightRegistry(&copyrightRegistry)) < 0)
	{
		UnionUserErrLog("in UnionVerifyUserCopyrightRegistry:: UnionReadUserCopyrightRegistry!\n");
		return(ret);
	}
		
	return(UnionVerifyCopyrightRegistry(&copyrightRegistry));
}

int UnionGenerateUserCopyrightRegistry(char *abbrProductName)
{
	int			ret;
	TUnionCopyrightRegistry copyrightRegistry;
	
	memset(&copyrightRegistry,0,sizeof(copyrightRegistry));
	strcpy(copyrightRegistry.abbrName,abbrProductName);
	
	if ((ret = UnionInitCopyrightRegistryStaticData(&copyrightRegistry,abbrProductName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUserCopyrightRegistry:: UnionInitCopyrightRegistryStaticData!\n");
		return(ret);
	}
	
	if ((ret = UnionOrganizeCopyrightRegistryDynamicData(&copyrightRegistry)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUserCopyrightRegistry:: UnionOrganizeCopyrightRegistryDynamicData!\n");
		return(ret);
	}

	if ((ret = UnionGenerateCopyrightRegistry(&copyrightRegistry)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUserCopyrightRegistry:: UnionGenerateCopyrightRegistry!\n");
		return(ret);
	}

	UnionPrintCopyrightRegistry(&copyrightRegistry);
	
	if (!UnionConfirm("Are you sure of creating this CopyrightRegistry?"))
		return(-1);
		
	return(UnionStoreCopyrightRegistry(&copyrightRegistry,abbrProductName));
}

int UnionPrintCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry)
{
	if (pCopyrightRegistry == NULL)
		return(-1);
	
	printf("\n");
	printf("[abbrName]		[%s]\n",pCopyrightRegistry->abbrName);
	printf("[primaryName]		[%s]\n",pCopyrightRegistry->primaryName);
	printf("[minorName]		[%s]\n",pCopyrightRegistry->minorName);
	printf("[version]		[%s]\n",pCopyrightRegistry->version);
	printf("[userName]		[%s]\n",pCopyrightRegistry->userName);
	printf("[userNumber]		[%d]\n",pCopyrightRegistry->userNumber);
	printf("[serialNumber]		[%s]\n",pCopyrightRegistry->serialNumber);
	printf("[verificationCode]	[%s]\n",pCopyrightRegistry->verificationCode);
	printf("[lisenceData]		[%s]\n",pCopyrightRegistry->lisenceData);
	printf("[registerDateTime]	[%s]\n",pCopyrightRegistry->registerDateTime);
	
	return(0);
}

//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5UIService.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionCertFun.h"
#include "unionVKForm.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"

/***************************************
�������:	8E19
������:		����֤��
��������:	����֤��
***************************************/
/*
int switchAndImportVK(char *VK,char *keyByLmk,char *vkByLmk);

int UnionDealServiceCode8E19(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret=0;
	char				filePath[512];
	char				pfxFullName[256];
	char				x509FullName[256];
	char				pfxFileName[64];
	char				passwd[64];
	char				VK[4096];
	char				keyByLmk[64];	
	char				vkByLmk[4096];
	char				cerBuf[4096];
	char				pix[32];
	char				bankID[64];
	char				certVersion[32];
	char				sql[512];
	char				*ptr = NULL;
	char				caCertBuf[4096];
	char				remark[128];

	TUnionX509Cer			x509Cer;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	memset(&x509Cer,0,sizeof(x509Cer));

	// ��ȡ�ļ�·��
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);

	// ��Կ����
	memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	
	// ֤����
	memset(pfxFileName,0,sizeof(pfxFileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/pfxFileName",pfxFileName,sizeof(pfxFileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: UnionReadRequestXMLPackageValue[%s]!\n","body/pfxFileName");
		UnionSetResponseRemark("��ȡ֤����ʧ��");
		return(ret);
	}

	UnionFilterHeadAndTailBlank(pfxFileName);
	if (strlen(pfxFileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: pfxFileName is null!\n");
		UnionSetResponseRemark("֤��������Ϊ��!");
		return(errCodeParameter);
	}

	// ֤������
	memset(passwd,0,sizeof(passwd));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",passwd,sizeof(passwd))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankVK");
		UnionSetResponseRemark("��ȡ֤������ʧ��");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(passwd);

	memset(pfxFullName,0,sizeof(pfxFullName));
	sprintf(pfxFullName,"%s/%s",filePath,"CNAPS2.01010000-654123-01.cer.pfx");
	
	// ���֤���Ƿ����
	if ((access(pfxFullName,0) < 0))
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: fileName[%s] not found!\n",pfxFullName);
		return(errCodeMngSvrFileNotExists);
	}

	memset(x509FullName,0,sizeof(x509FullName));
	sprintf(x509FullName,"%s/%s.x509",filePath,asymmetricKeyDB.keyName);
//int UnionCombinationPfx(char *keyFilename,char *passwd,char *crtFileName,char *caCrtFileName,char *alias,char *pfxFileName)
//	if ((ret = UnionCombinationPfx(keyFullName,passwd,pkCert

	// ��ȡ˽Կ�͹�Կ֤��
	memset(VK,0,sizeof(VK));
	memset(cerBuf,0,sizeof(cerBuf));
	memset(caCertBuf,0,sizeof(caCertBuf));
	if((ret=UnionGetVKAndCertFromPfx(pfxFullName,passwd,VK,x509FullName,NULL,cerBuf,NULL))<0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E19:: unionGetVKAndCerFromPfX!\n");
		UnionSetResponseRemark("֤�����ʧ��,����֤�������");
		return(ret);
	}
	
	// ɾ�����ɵ��ļ�
	if ((ret = unlink(x509FullName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: delete fileName[%s] is failed!\n",x509FullName);
		//return(ret);
	}

	return 0;
}
*/



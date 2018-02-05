#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

#include "unionSRJ1401Cmd.h"
#include "base64.h"

/***************************************
  �������:	8ED8
  ������:	֤���ѯ(822)by userDn
  ��������:	֤���ѯ(822)by userDn
 ***************************************/
int UnionDealServiceCode8ED8(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			userDN[128];
	char			tmpBuf[128];
	char			hsmGrpID[128];
	char                    fieldList[1024+1];
        char                    fieldListChnName[4096+1];
	char			certBase64[4096];
	char			certAsc[4096];
	int			certExist;

	TUnionX509Certinfo	certInfo;
	TUnionHsmGroupRec	pSpecHsmGroup;

	// ֤��DN
	if ((ret = UnionReadRequestXMLPackageValue("body/userDN",userDN,sizeof(userDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionReadRequestXMLPackageValue[%s]!\n","body/userDN");
		return(ret);
	}
	userDN[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ�õ��������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED8:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES822(userDN,&certInfo,&certExist)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSRJ1401CmdES822,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED8:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
		return(errCodeParameter);
	}

	// ����֤����ڱ�ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%d", certExist);
	if ((ret = UnionSetResponseXMLPackageValue("body/certExist",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s]!\n","body/tmpBuf");
                return(ret);
        }

	// �����ֶ��嵥
        memset(fieldList,0,sizeof(fieldList));
        strcpy(fieldList,"vkindex,algID,appName,bankName,bankCode,startDateTime,endDataTime,isInCrl,userDN,derPK,certDer,certBase64");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
        {
                UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
                return(ret);
        }

        // �����ֶ��嵥������
        memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"˽Կ����,֤������,֤��Ӧ����,������,��������,֤����Ч��ʼ����,֤����Ч��ֹ����,�Ƿ���CRL�б���,"
			"�û�DN,֤��ӵ���ߵĹ�Կ,֤������DER,֤������Base64");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
        {       
                UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
                return(ret);
        }

	if (certExist == 1)
	{
		//UnionSetResponseXMLPackageValue("body/totalNum","0");
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",1);
			return(ret);
		}

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", certInfo.vkindex);
		if ((ret = UnionSetResponseXMLPackageValue("vkindex",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","vkindex",1);
			return(ret);
		}

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", certInfo.algID);
		if ((ret = UnionSetResponseXMLPackageValue("algID",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","algID",1);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("appName",certInfo.certAppname)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","appName",1);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("bankName",certInfo.bankName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","bankName",1);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("bankCode",certInfo.bankCode)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","bankCode",1);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("startDateTime",certInfo.startDateTime)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","startDateTime",1);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("endDataTime",certInfo.endDataTime)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","endDataTime",1);
			return(ret);
		}

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", certInfo.IsInCrl);
		if ((ret = UnionSetResponseXMLPackageValue("isInCrl",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","isInCrl",1);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("userDN",certInfo.userDN)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","userDN",1);
			return(ret);
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("derPK",certInfo.derPK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","derPK",1);
			return(ret);
		}

		bcdhex_to_aschex((char *)certInfo.certDerBuf, certInfo.certDerBufLen, certAsc);
		certAsc[certInfo.certDerBufLen * 2] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("certDer",certAsc)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","certDer",1);
			return(ret);
		}
		
		to64frombits((unsigned char *)certBase64, certInfo.certDerBuf, certInfo.certDerBufLen);
		if ((ret = UnionSetResponseXMLPackageValue("certBase64",certBase64)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED8:: UnionSetResponseXMLPackageValue[%s][%d]!\n","certBase64",1);
			return(ret);
		}

		UnionLocateResponseXMLPackage("",0);
	}

	return(0);
}


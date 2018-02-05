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
  �������:	8ED1
  ������:	��ҳ��ȡ֤���嵥(821)
  ��������:	��ҳ��ȡ֤���嵥(821)
 ***************************************/
int UnionDealServiceCode8ED1(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			totalNum= 0;
	char			tmpBuf[128];
	int			pageNo;
	int			maxCntOfPage;
	int			count;
	char                    fieldList[1024+1];
        char                    fieldListChnName[4096+1];
	int			i;
	char			hsmGrpID[128];

	TUnionX509CertinfoESSC	*pCertInfo;
	TUnionHsmGroupRec	pSpecHsmGroup;

	// �����ֶ��嵥
        memset(fieldList,0,sizeof(fieldList));
        strcpy(fieldList,"vkindex,algID,appName,bankName,bankCode,startDateTime,endDataTime,isInCrl,userDN");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
        {
                UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
                return(ret);
        }

        // �����ֶ��嵥������
        memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"˽Կ����,֤������,֤��Ӧ����,������,��������,֤����Ч��ʼ����,֤����Ч��ֹ����,�Ƿ���CRL�б���,�û�DN");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
        {       
                UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
                return(ret);
        }

	// ��ȡ֤���¼��ҳ��
	if ((ret = UnionReadRequestXMLPackageValue("body/pageNo",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionReadRequestXMLPackageValue[%s]!\n","body/pageNo");
		return(ret);
	}
	tmpBuf[ret] = 0;
	pageNo = atoi(tmpBuf);

	// ÿҳ֤������¼��
	if ((ret = UnionReadRequestXMLPackageValue("body/maxCntOfPage",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionReadRequestXMLPackageValue[%s]!\n","body/maxCntOfPage");
		return(ret);
	}
	tmpBuf[ret] = 0;
	maxCntOfPage = atoi(tmpBuf);

	// ��ȡ���ܻ���ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ���������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);
	
	pCertInfo = (TUnionX509CertinfoESSC *)malloc(sizeof(TUnionX509CertinfoESSC) * maxCntOfPage);
	if (pCertInfo == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: malloc maxCntOfPage[%d]!\n", maxCntOfPage);
		return(errCodeUseOSErrCode);
	}

	if ((ret = UnionSRJ1401CmdES821(pageNo, maxCntOfPage, pCertInfo, &count)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSRJ1401CmdES821,ret = [%d]!\n",ret);
		free(pCertInfo);
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	for (i = 0; i < count; ++i)
	{
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum + 1)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", pCertInfo[i].vkindex);
		if ((ret = UnionSetResponseXMLPackageValue("vkindex",tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","vkindex",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", pCertInfo[i].algID);
		if ((ret = UnionSetResponseXMLPackageValue("algID",tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","algID",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("appName",pCertInfo[i].certAppname)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","appName",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }
		
		if ((ret = UnionSetResponseXMLPackageValue("bankName",pCertInfo[i].bankName)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","bankName",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("bankCode",pCertInfo[i].bankCode)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","bankCode",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("startDateTime",pCertInfo[i].startDateTime)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","startDateTime",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("endDataTime",pCertInfo[i].endDataTime)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","endDataTime",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		snprintf(tmpBuf, sizeof(tmpBuf), "%d", pCertInfo[i].IsInCrl);
		if ((ret = UnionSetResponseXMLPackageValue("isInCrl",tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","isInCrl",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		if ((ret = UnionSetResponseXMLPackageValue("userDN",pCertInfo[i].userDN)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s][%d]!\n","userDN",totalNum+1);
			free(pCertInfo);
                        return(ret);
                }

		totalNum++;
	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
        snprintf(tmpBuf, sizeof(tmpBuf), "%d",totalNum);
        if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED1:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		free(pCertInfo);
                return(ret);
        }

	free(pCertInfo);
	return(0);
}


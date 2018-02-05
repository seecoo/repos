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
#include "unionRecFile.h"

/***************************************
  �������:	8ED7
  ������:	֤������(622)by userDn
  ��������:	֤������(622)by userDn
 ***************************************/
int UnionDealServiceCode8ED7(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			userDN[128];
	char			cert[4096];
	char			hsmGrpID[128];
	char			fileName[128];
	char			filePath[128];
	char			fileStoreDir[512];
	char			fullTime[20];
	char			sql[1024];
	int			certLen;
	FILE			*fp;

	TUnionHsmGroupRec	pSpecHsmGroup;

	// ֤��DN
	if ((ret = UnionReadRequestXMLPackageValue("body/userDN",userDN,sizeof(userDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadRequestXMLPackageValue[%s]!\n","body/userDN");
		return(ret);
	}
	userDN[ret] = 0;

	// ��ȡ�������ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ�õ��������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED7:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES622(userDN, (unsigned char *)cert, &certLen)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionSRJ1401CmdES622,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED7:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
		return(errCodeParameter);
	}

	//��ѯ�ļ��洢·��
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'","CERT");
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }

        // ��ȡ�ļ��洢Ŀ¼
        memset(fileStoreDir,0,sizeof(fileStoreDir));
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED7:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }

	// ��������֤������
	memset(fullTime, 0, sizeof(fullTime));
	UnionGetFullSystemDateTime(fullTime);
	snprintf(fileName, sizeof(fileName), "%s.cer", fullTime);

	memset(filePath,0,sizeof(filePath));
        UnionFormFullFileName(fileStoreDir,fileName,filePath);

	// ��֤������д���ļ���
	if ((fp = fopen(filePath, "wb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED7:: fopen[%s]!\n",filePath);
                return(errCodeUseOSErrCode);
	}
	if ((ret = fwrite(cert, 1, certLen, fp)) != certLen)
	{
                UnionUserErrLog("in UnionDealServiceCode8ED7:: fwrite[%s]!\n",filePath);
		fclose(fp);
                return(errCodeUseOSErrCode);
	}
	fclose(fp);

	// ����֤������
	if ((ret = UnionSetResponseXMLPackageValue("body/fileName",fileName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED3:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }

	// �����ļ�����
	if ((ret = UnionSetResponseXMLPackageValue("body/fileTypeID","CERT")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8ED3:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileTypeID");
                return(ret);
        }
	
	return(0);
}


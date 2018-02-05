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
  �������:	8EDA
  ������:	����֤�������ļ�
  ��������:	����֤�������ļ�
 ***************************************/
int UnionDealServiceCode8EDA(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			userDN[256];
	char			tmpBuf[128];
	char			hsmGrpID[128];
	char			p10buf[8192*4];
	int			algID;
	int			vkIndex;
	int			pkcsLen;

	char			fileName[128];
	char			filePath[128];
	char			fileStoreDir[512];
	char			fullTime[20];
	char			sql[1024];
	FILE			*fp;

	TUnionHsmGroupRec	pSpecHsmGroup;


	// ֤������
	if ((ret = UnionReadRequestXMLPackageValue("body/algID", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionReadRequestXMLPackageValue[%s]!\n","body/algID");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algID = atoi(tmpBuf);

	// ˽Կ������
	if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
		return(ret);
	}
	tmpBuf[ret] = 0;
	vkIndex = atoi(tmpBuf);

	// �û�DN
	if ((ret = UnionReadRequestXMLPackageValue("body/userDN", userDN,sizeof(userDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionReadRequestXMLPackageValue[%s]!\n","body/userDN");
		return(ret);
	}
	userDN[ret] = 0;
	
	// ���ܻ���ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ�õ��������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDA:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	memset(p10buf, 0, sizeof(p10buf));
	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((pkcsLen = UnionSRJ1401CmdES825(vkIndex, algID, userDN, p10buf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionSRJ1401CmdES825,ret = [%d]!\n",pkcsLen);
			return(pkcsLen);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8EDA:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
		return(errCodeParameter);
	}

	//��ѯ�ļ��洢·��
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'","CERT");
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }

        // ��ȡ�ļ��洢Ŀ¼
        memset(fileStoreDir,0,sizeof(fileStoreDir));
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }

	// ��������֤������
	memset(fullTime, 0, sizeof(fullTime));
	UnionGetFullSystemDateTime(fullTime);
	snprintf(fileName, sizeof(fileName), "%s.req", fullTime);

	memset(filePath,0,sizeof(filePath));
        UnionFormFullFileName(fileStoreDir,fileName,filePath);

	// ��֤������д���ļ���
	if ((fp = fopen(filePath, "wb")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCode8EDA:: fopen[%s]!\n",filePath);
                return(errCodeUseOSErrCode);
	}
	if ((ret = fwrite(p10buf, 1, pkcsLen, fp)) != pkcsLen)
	{
                UnionUserErrLog("in UnionDealServiceCode8EDA:: fwrite[%s]!\n",filePath);
		fclose(fp);
                return(errCodeUseOSErrCode);
	}
	fclose(fp);

	// ����֤������
	if ((ret = UnionSetResponseXMLPackageValue("body/fileName",fileName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }

	// �����ļ�����
	if ((ret = UnionSetResponseXMLPackageValue("body/fileTypeID","CERT")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EDA:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileTypeID");
                return(ret);
        }

	return(0);
}


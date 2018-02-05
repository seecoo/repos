//	Author:		����ΰ
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-07-01

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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	8E0D
������:		�ָ���ʼ��Կ
��������:	�ָ���ʼ��Կ
***************************************/
int UnionDealServiceCode8E0D(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				sql[512];
	int				len = 0;
	char				tmpBuf[256];
	FILE				*fp;
	char				fileName[128];
	char				fullFileName[512];
	char				path[512];
	char				fileStoreDir[512];
	char				delimiter;
	char				keyValue[64];
	char				checkValue[32];
	char				*ptr = NULL;
	int				algorithmID = 0;
	char				tmpCheckValue[32];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", symmetricKeyDB.keyName, sizeof(symmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	symmetricKeyDB.keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);
	if (strlen(symmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: keyName can not be null!\n");
		UnionSetResponseRemark("keyName����Ϊ��");
		return(errCodeParameter);
	}

	//��ȡ�Գ���Կ
	if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName, 0, &symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionReadSymmetricKeyDBRec[%s]!\n", symmetricKeyDB.keyName);
		return(ret);

	}
	
	if(symmetricKeyDB.keyType != conZMK)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: keyType err,should be zmk\n");
		UnionSetResponseRemark("��Կ���ʹ�ӦΪZMK");
		return(errCodeParameter);
	}
	
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	
	//�㷨��ʶ
	if((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) <= 0)
		algorithmID = 0;
	else
	{
		tmpBuf[ret] = 0;
		algorithmID = atoi(tmpBuf);
	}
	if(algorithmID != 0 && algorithmID != 1)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: wrong algorithmType[%d]\n", algorithmID);
		UnionSetResponseRemark("��Կ���ʹ�����ΪDES��SM4");
		return(errCodeParameter);
	}
	if(0 == algorithmID)
		strcpy(fileName, "DESZMKkeyOfPOS");
	else
		strcpy(fileName, "SM4ZMKkeyOfPOS");

	//��ȡ��ʼzmk��Կ�ļ�
	len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID = 'POSZ'");
	sql[len] = 0;
	
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionSelectRealDBRecord[%s]\n", sql);
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: fileTypeID[%s] record not found\n", "POSZ");
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionLocateXMLPackage[detail]\n");
		return(ret);
	}
	if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionReadXMLPackageValue[%s]\n", "fileStoreDir");
		return(ret);
	}
	fileStoreDir[ret] = 0;
	
	if((ret = UnionReadXMLPackageValue("delimiter", tmpBuf, sizeof(tmpBuf))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionReadXMLPackageValue[%s]\n", "delimiter");
                return(ret);
        }
        tmpBuf[ret] = 0;
	delimiter = tmpBuf[0];
	
	UnionReadDirFromStr(fileStoreDir, -1 ,path);
	//��Կ�ļ�ȫ·��
	len = snprintf(fullFileName, sizeof(fullFileName), "%s/%s", path, fileName);
	fullFileName[len] = 0;
	UnionLog("fullFileName = %s\n", fullFileName);
	if((fp = fopen(fullFileName, "r")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: fopen [%s] failed\n", fullFileName);	
		return(errCodeParameter);
	}
	if(fgets(tmpBuf, sizeof(tmpBuf), fp) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: file[%s] is empty\n", fullFileName);
		return(errCodeParameter);
	}
	if((ptr = strchr(tmpBuf, '\r')) != NULL)
                tmpBuf[ptr - tmpBuf] = 0;
	if((ptr = strchr(tmpBuf, '\n')) != NULL)
		tmpBuf[ptr - tmpBuf] = 0;
	UnionLog("tmpBuf[%s]\n", tmpBuf);
	if((ptr = strchr(tmpBuf, delimiter)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: file[%s] content[%s] error. example[keyValue%ccheckValue]\n", fullFileName, tmpBuf, delimiter);
		return(errCodeParameter);
	}
	*ptr = 0;
	strcpy(keyValue, tmpBuf);
	strcpy(checkValue, ptr+1);
	fclose(fp);
	if((strlen(keyValue) != 16 && strlen(keyValue) != 32 && strlen(keyValue) != 48) || strlen(checkValue) != 16)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: wrong length of key[%s] or checkValue[%s]\n", keyValue, checkValue);
		UnionSetResponseRemark("��Կ�ļ����ݴ�");
		return(errCodeParameter);
	}
	UnionSetIsUseNormalZmkType();
	if((ret = UnionHsmCmdBU(algorithmID, conZMK, strlen(keyValue)/16-1, keyValue, tmpCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionHsmCmdBU[%s]\n", keyValue);
		return(ret);
	}
	if(strcmp(checkValue, tmpCheckValue) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: checkValue[%s] in file is diff with checkValue[%s] from BU command\n", checkValue, tmpCheckValue);
		UnionUserErrLog("��Կ�ļ�У��ֵУ��ʧ��");
		return(errCodeParameter);
	}

	//��ȡ��Կֵ
	if((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionGetSymmetricKeyValue lmkProtectMode[%s]\n", phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}
	
	symmetricKeyDB.keyLen = strlen(keyValue)*4;
	//����Կ��Ϊ��ǰ��Կ���ָ���ʼ��Կ
	memcpy(symmetricKeyDB.keyValue[0].oldKeyValue, psymmetricKeyValue->keyValue, symmetricKeyDB.keyLen/4+1);
	strcpy(symmetricKeyDB.keyValue[0].keyValue, keyValue);
        strcpy(symmetricKeyDB.oldCheckValue, symmetricKeyDB.checkValue);
	strcpy(symmetricKeyDB.checkValue, checkValue);
	
	if((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0D:: UnionUpdateSymmetricKeyDBKeyValue\n");
		return(ret);
	}

	return(0);
}



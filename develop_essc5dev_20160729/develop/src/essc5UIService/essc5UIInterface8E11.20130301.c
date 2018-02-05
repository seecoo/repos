//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHighCachedAPI.h"

/***************************************
�������:	8E11
������:		�޸ķǶԳ���Կ
��������:	�޸ķǶԳ���Կ
***************************************/
int UnionDealServiceCode8E11(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				enabled = 1;
	char				tmpBuf[128];
	char				highCachedKey[512];
	char				vkIndex[32];
	int				len = 0;
	char				sql[1024];
	//int				vkIdx = 0,vkIdx1 = 0;
	
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
	if (strlen(asymmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��");
		return(errCodeParameter);
	}

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}

	// ��Կ��
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	strcpy(asymmetricKeyDB.keyGroup,tmpBuf);
	if (strlen(asymmetricKeyDB.keyGroup) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: keyGroup can not be null!\n");
		UnionSetResponseRemark("��Կ�鲻��Ϊ��");
		return(errCodeParameter);
	}

	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		// ��Կָ��
		if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkExponent");
			return(ret);	
		}
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (strlen(tmpBuf) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: pkExponent can not be null!\n");
			UnionSetResponseRemark("��Կָ������Ϊ��");
			return(errCodeParameter);
		}

		asymmetricKeyDB.pkExponent = atoi(tmpBuf);
	}


	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: keyLen can not be null!\n");
		UnionSetResponseRemark("��Կ���Ȳ���Ϊ��");
		return(errCodeParameter);
	}
	asymmetricKeyDB.keyLen = atoi(tmpBuf);
	
	// ˽Կ�洢λ��
	if ((ret = UnionReadRequestXMLPackageValue("body/vkStoreLocation",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkStoreLocation");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: vkStoreLocation can not be null!\n");
		UnionSetResponseRemark("˽Կ�洢λ�ò���Ϊ��");
		return(errCodeParameter);
	}

	asymmetricKeyDB.vkStoreLocation = atoi(tmpBuf);
	
	memset(vkIndex,0,sizeof(vkIndex));
	if (asymmetricKeyDB.vkStoreLocation != 0)
	{
		// �������
		if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
			return(ret);
		}
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		strcpy(asymmetricKeyDB.hsmGroupID,tmpBuf);
		if (strlen(asymmetricKeyDB.hsmGroupID) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: hsmGroupID can not be null!\n");
			UnionSetResponseRemark("������鲻��Ϊ��");
			return(errCodeParameter);
		}

		//  ˽Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(vkIndex);
		if (strlen(vkIndex) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: vkIndex can not be null!\n");
			UnionSetResponseRemark("˽Կ��������Ϊ��");
			return(errCodeParameter);
		}

		// �޸ĺ��������ԭ������ͬʱ
		if (strcmp(asymmetricKeyDB.vkIndex,vkIndex) != 0)
		{
			if (!UnionIsDigitString(vkIndex))
			{
				UnionUserErrLog("in UnionDealServiceCode8E11:: vkIndex[%s] error!\n",vkIndex);
				UnionSetResponseRemark("�Ƿ���˽Կ����[%s]",vkIndex);
				return(errCodeHsmCmdMDL_InvalidIndex);	
			}
			
			// ���˽Կ�����Ƿ��ѱ�ʹ��
			snprintf(sql,sizeof(sql),"select vkIndex from vkKeyIndex where hsmGroupID = '%s' and vkIndex = '%s' and status = 0 and algorithmID = %d",asymmetricKeyDB.hsmGroupID,vkIndex,asymmetricKeyDB.algorithmID);

			if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E11:: UnionSelectRealDBRecord[%s]!\n",sql);
				return(ret);
			}
			if (ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E11:: hsmGroupID[%s] vkIndex[%s]!\n",asymmetricKeyDB.hsmGroupID,vkIndex);
				UnionSetResponseRemark("˽Կ����[%s]�ѱ�ʹ��",vkIndex);
				return(errCodeParameter);
			}
		}
	}

	// ����ʹ�þ���Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValuede [%s] error!\n","body/oldVersionKeyIsUsed");
		return(errCodeParameter);
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: oldVersionKeyIsUsed can not be null!\n");
		UnionSetResponseRemark("����ʹ�þ���Կ����Ϊ��");
		return(errCodeParameter);
	}
	asymmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
	if ((asymmetricKeyDB.oldVersionKeyIsUsed != 0) && (asymmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// �������ʶ
	// 0��������
	// 1������
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValuede [%s] error!\n","body/inputFlag");
		return(errCodeParameter);
	}
	else
	{
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (strlen(tmpBuf) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: inputFlag can not be null!\n");
			UnionSetResponseRemark("�������ʶ����Ϊ��");
			return(errCodeParameter);
		}
		asymmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.inputFlag != 0) && (asymmetricKeyDB.inputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��������ʶ
	// 0��������
	// 1������
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValuede [%s]!\n","body/outputFlag");
		return(errCodeParameter);
	}
	else
	{
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (strlen(tmpBuf) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: outputFlag can not be null!\n");
			UnionSetResponseRemark("��������ʶ����Ϊ��");
			return(errCodeParameter);
		}

		asymmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.outputFlag != 0) && (asymmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}
	
	// ��ȡ��Ч����
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValuede [%s]!\n","body/effectiveDays");
		return(errCodeParameter);
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: effectiveDays can not be null!\n");
		UnionSetResponseRemark("��ȡ��Ч��������Ϊ��");
		return(errCodeParameter);
	}

	asymmetricKeyDB.effectiveDays = atoi(tmpBuf);	

	// ���ñ�ʶ
	// 1���������ã�����Ч��Ĭ��ֵ
	// 2���������ã�ָ��ʱ����Ч
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValuede [%s]!\n","body/enabled");
		return(errCodeParameter);
	}
	else
	{
		tmpBuf[ret] = 0;	
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (strlen(tmpBuf) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: enabled can not be null!\n");
			UnionSetResponseRemark("���ñ�ʶ����Ϊ��");
			return(errCodeParameter);
		}

		enabled = atoi(tmpBuf);
	}
		
	if (enabled == 0)
		asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;	// ״̬����Ϊ��ʼ��״̬
	else
		asymmetricKeyDB.status = conAsymmetricKeyStatusOfEnabled;	// ״̬����Ϊ����״̬

	// ��Կ����ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyApplyPlatform");
		return(ret);
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	strcpy(asymmetricKeyDB.keyApplyPlatform,tmpBuf);
	if (strlen(asymmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",asymmetricKeyDB.keyApplyPlatform);
			return(ret);	
		}
	}

	// ��Կ�ַ�ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyDistributePlatform");
		return(ret);
	}
	tmpBuf[ret] = 0;
	UnionFilterHeadAndTailBlank(tmpBuf);
	strcpy(asymmetricKeyDB.keyDistributePlatform,tmpBuf);
	if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",asymmetricKeyDB.keyDistributePlatform);
			return(ret);	
		}
	}

	// ��ȡ��ע
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		return(ret);
	}
	strcpy(asymmetricKeyDB.remark,tmpBuf);
	
	// �����Կ����
	if (!UnionIsValidAsymmetricKeyDB(&asymmetricKeyDB))
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionIsValidAsymmetricKeyDB!\n");
		return(errCodeKeyCacheMDL_WrongKeyName);	
	}
	
	// ��Ч����
	// ��enabledΪ2ʱ������
	if (enabled == 1)
	{
		UnionGetFullSystemDate(asymmetricKeyDB.activeDate);
	}
	else if (enabled == 2)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate,sizeof(asymmetricKeyDB.activeDate))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
			return(ret);	
		}
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.activeDate);
		if (strlen(asymmetricKeyDB.activeDate) == 0)	
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: activeDate can not be null!\n");
			UnionSetResponseRemark("��Ч���ڲ���Ϊ��!");
			return(errCodeParameter);
		}
	}
	
	len = sprintf(sql,"update asymmetricKeyDB set keyGroup = '%s', pkExponent = %d,keyLen = %d,vkStoreLocation = %d,hsmGroupID = '%s',vkIndex = '%s',oldVersionKeyIsUsed = %d,inputFlag = %d,outputFlag = %d,effectiveDays = %d,status = %d,activeDate = '%s',keyApplyPlatform = '%s',keyDistributePlatform = '%s',remark = '%s'  where keyName = '%s'",
			asymmetricKeyDB.keyGroup,
			asymmetricKeyDB.pkExponent,
			asymmetricKeyDB.keyLen,
			asymmetricKeyDB.vkStoreLocation,
			asymmetricKeyDB.hsmGroupID,
			vkIndex,
			asymmetricKeyDB.oldVersionKeyIsUsed,
			asymmetricKeyDB.inputFlag,
			asymmetricKeyDB.outputFlag,
			asymmetricKeyDB.effectiveDays,
			asymmetricKeyDB.status,
			asymmetricKeyDB.activeDate,
			asymmetricKeyDB.keyApplyPlatform,
			asymmetricKeyDB.keyDistributePlatform,
			asymmetricKeyDB.remark,
			asymmetricKeyDB.keyName);
	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E11:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}

	if (strcmp(asymmetricKeyDB.vkIndex,vkIndex) != 0)
	{
		//len = sprintf(sql,"update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s'",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex);
		len = sprintf(sql,"update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		sql[len] = 0;
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			return(ret);
		}

		//len = sprintf(sql,"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s'",asymmetricKeyDB.hsmGroupID,vkIndex);
		len = sprintf(sql,"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = %d",asymmetricKeyDB.hsmGroupID,vkIndex,asymmetricKeyDB.algorithmID);
		sql[len] = 0;
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E11:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			return(ret);
		}
	}

	memset(highCachedKey,0,sizeof(highCachedKey));
	UnionGetKeyOfHighCachedForAsymmetricKeyDB(asymmetricKeyDB.keyName,highCachedKey);	

	// ɾ�����ٻ���
	if ((ret = UnionDeleteHighCachedValue(highCachedKey)) < 0)
	{
		UnionProgramerLog("in UnionDealServiceCode8E11:: UnionDeleteHighCachedValue error!\n");
	}

	return(0);
}

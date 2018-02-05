
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "pinPadDB.h"
#include "remoteKeyPlatform.h"
#include "esscKmsPackage.h"

/* 
���ܣ�	�����������
������	ppinPadDB[in]
����ֵ��>=0	�ɹ�
	<0	ʧ�ܣ����ش�����
*/
int UnionInsertPinPadDB(PUnionPinPadDB ppinPadDB)
{
	int	ret;
	int	len;
	char	sql[512];

	UnionGetFullSystemDateTime(ppinPadDB->regTime);
	len = snprintf(sql,sizeof(sql),"insert into %s(pinPadID,regTime,lastTime,status,branch,teller,remark,producter,producterID,zmkLength,zpkLength,zakLength,appNo) "
					"values('%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,'%s')",
			defTableNameOfPinPadDB,
			ppinPadDB->pinPadID,
			ppinPadDB->regTime,
			ppinPadDB->regTime,
			ppinPadDB->status,
			ppinPadDB->branch,
			ppinPadDB->teller,
			ppinPadDB->remark,
			ppinPadDB->producter,
			ppinPadDB->producterID,
			ppinPadDB->zmkLength,
			ppinPadDB->zpkLength,
			ppinPadDB->zakLength,
			ppinPadDB->appNo);
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionInsertPinPadDB:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}
	
	return(0);
}

/* 
���ܣ�	�����������
������	ppinPadDB[in]
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdatePinPadDB(PUnionPinPadDB ppinPadDB)
{
	int	ret;
	char	sql[8192];
	int	len = 0;
	
	UnionGetFullSystemDateTime(ppinPadDB->lastTime);
	len = snprintf(sql,sizeof(sql),"update %s set status = '%s',lastTime = '%s',producter = '%s', producterID = '%s',remark = '%s' where pinPadID = '%s'",
			defTableNameOfPinPadDB,
			ppinPadDB->status,
			ppinPadDB->lastTime,
			ppinPadDB->producter,
			ppinPadDB->producterID,
			ppinPadDB->remark,
			ppinPadDB->pinPadID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePinPadDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionUpdatePinPadDB:: count[0] sql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	return(0);
}

/* 
���ܣ�	ɾ��һ���������
������	ppinPadDB[in]
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionDropPinPadDB(PUnionPinPadDB ppinPadDB)
{
	int	ret;
	int	len;
	char	sql[8192];

	len = snprintf(sql,sizeof(sql),"delete from %s where pinPadID = '%s'",defTableNameOfPinPadDB,ppinPadDB->pinPadID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDropPinPadDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	return(0);
}

/* 
���ܣ�	��ȡһ���������
������	pinPadID[in]		����ID
	ppinPadDB[out]
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionReadPinPadDB(char *pinPadID,PUnionPinPadDB ppinPadDB)
{
	int		ret;
	char		sql[128];
	char		tmpBuf[16];
	
	snprintf(sql,sizeof(sql),"select * from %s where pinPadID = '%s'",
		defTableNameOfPinPadDB,pinPadID);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionLog("in UnionReadPinPadDB:: pinPadID[%s] not find!\n",pinPadID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);

	if ((ret = UnionReadXMLPackageValue("pinPadID", ppinPadDB->pinPadID, sizeof(ppinPadDB->pinPadID))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","pinPadID");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("regTime", ppinPadDB->regTime, sizeof(ppinPadDB->regTime))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","regTime");
		return(ret);
	}
	
	if ((ret = UnionReadXMLPackageValue("lastTime", ppinPadDB->lastTime, sizeof(ppinPadDB->lastTime))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","lastTime");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("status", ppinPadDB->status, sizeof(ppinPadDB->status))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}
			
	if ((ret = UnionReadXMLPackageValue("branch", ppinPadDB->branch, sizeof(ppinPadDB->branch))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","branch");
		return(ret);
	}
	
	if ((ret = UnionReadXMLPackageValue("teller", ppinPadDB->teller, sizeof(ppinPadDB->teller))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","teller");
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("producter", ppinPadDB->producter, sizeof(ppinPadDB->producter))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","producter");
                return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("producterID", ppinPadDB->producterID, sizeof(ppinPadDB->producterID))) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","producterID");
                return(ret);
        }

	if ((ret = UnionReadXMLPackageValue("zmkLength", tmpBuf, sizeof(tmpBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","zmkLength");
                return(ret);
        }
	tmpBuf[ret] = 0;
	ppinPadDB->zmkLength = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("zpkLength", tmpBuf, sizeof(tmpBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","zpkLength");
                return(ret);
        }
	tmpBuf[ret] = 0;
	ppinPadDB->zpkLength = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("zakLength", tmpBuf , sizeof(tmpBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","zakLength");
                return(ret);
        }
	tmpBuf[ret] = 0;
	ppinPadDB->zakLength = atoi(tmpBuf);

	if ((ret = UnionReadXMLPackageValue("appNo", ppinPadDB->appNo, sizeof(ppinPadDB->appNo))) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","appNo");
                return(ret);
        }

	if ((ret = UnionReadXMLPackageValue("remark", ppinPadDB->remark, sizeof(ppinPadDB->remark))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDB:: UnionReadXMLPackageValue[%s]!\n","remark");
		return(ret);
	}

	return(0);
}

/* 
���ܣ�  ������ȡ�������
������  branch[in]            ����ID
����ֵ��>=0                     �ɹ�
        <0                      ʧ�ܣ����ش�����
*/
int UnionReadPinPadDBbranch(char *branch)
{
	int             ret;
	char            sql[128];
	char            tmpBuf[16];
	char		resStr[512];
	int 		i = 0;
	int		totalNum = 0;
	char		fileName[256];
	FILE		*fp = NULL;
	int		len = 0;

	TUnionPinPadDB	tpinPadDB;

        snprintf(sql,sizeof(sql),"select * from %s where branch = '%s'",
                defTableNameOfPinPadDB,branch);

        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }
        else if (ret == 0)
        {
                UnionLog("in UnionReadPinPadDBbranch:: branch[%s] not find!\n",branch);
                return(errCodeDatabaseMDL_RecordNotFound);
        }

	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue total !\n");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	if ((ret = UnionGenerateTempFile(fileName,sizeof(fileName),1)) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionGenerateTempFile fileName[%s]!\n",fileName);
		return(ret);
	}

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionReadPinPadDBbranch:: fopen fileName[%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	for(i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i)) < 0)
		{
			UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionLocateXMLPackage [%d]!\n",i);
			fclose(fp);
			return(ret);
		}

	        if ((ret = UnionReadXMLPackageValue("pinPadID", tpinPadDB.pinPadID, sizeof(tpinPadDB.pinPadID))) < 0)
	        {
			UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","pinPadID");
			fclose(fp);
			return(ret);
        	}

	        if ((ret = UnionReadXMLPackageValue("regTime", tpinPadDB.regTime, sizeof(tpinPadDB.regTime))) < 0)
	        {
	                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","regTime");
			fclose(fp);
	                return(ret);
	        }

		if ((ret = UnionReadXMLPackageValue("lastTime", tpinPadDB.lastTime,sizeof(tpinPadDB.lastTime))) < 0)
		{
			UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","lastTime");
			fclose(fp);
			return(ret);
		}

	        if ((ret = UnionReadXMLPackageValue("status", tpinPadDB.status, sizeof(tpinPadDB.status))) < 0)
	        {
	                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","status");
			fclose(fp);
	                return(ret);
	        }

        	if ((ret = UnionReadXMLPackageValue("branch", tpinPadDB.branch, sizeof(tpinPadDB.branch))) < 0)
        	{
        	        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","branch");
			fclose(fp);
        	        return(ret);
        	}

        	if ((ret = UnionReadXMLPackageValue("teller", tpinPadDB.teller, sizeof(tpinPadDB.teller))) < 0)
        	{
        	        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","teller");
			fclose(fp);
        	        return(ret);
        	}
		
		if ((ret = UnionReadXMLPackageValue("remark", tpinPadDB.remark, sizeof(tpinPadDB.remark))) < 0)
                {
                        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","remark");
			fclose(fp);
                        return(ret);
                }

	        if ((ret = UnionReadXMLPackageValue("producter", tpinPadDB.producter, sizeof(tpinPadDB.producter))) < 0)
	        {
	                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","producter");
			fclose(fp);
	                return(ret);
	        }

        	if ((ret = UnionReadXMLPackageValue("producterID", tpinPadDB.producterID, sizeof(tpinPadDB.producterID))) < 0)
        	{
        	        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","producterID");
			fclose(fp);
                	return(ret);
        	}

        	if ((ret = UnionReadXMLPackageValue("zmkLength", tmpBuf, sizeof(tmpBuf))) < 0)
        	{
        	        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","zmkLength");
			fclose(fp);
        	        return(ret);
        	}
		tmpBuf[ret] = 0;
		tpinPadDB.zmkLength = atoi(tmpBuf);

	        if ((ret = UnionReadXMLPackageValue("zpkLength", tmpBuf, sizeof(tmpBuf))) < 0)
	        {
	                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","zpkLength");
			fclose(fp);
	                return(ret);
	        }
	        tmpBuf[ret] = 0;
		tpinPadDB.zpkLength = atoi(tmpBuf);

	        if ((ret = UnionReadXMLPackageValue("zakLength", tmpBuf , sizeof(tmpBuf))) < 0)
	        {
	                UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","zakLength");
			fclose(fp);
	                return(ret);
        	}
       		tmpBuf[ret] = 0;
		tpinPadDB.zakLength = atoi(tmpBuf);

        	if ((ret = UnionReadXMLPackageValue("appNo", tpinPadDB.appNo, sizeof(tpinPadDB.appNo))) < 0)
        	{
        	        UnionUserErrLog("in UnionReadPinPadDBbranch:: UnionReadXMLPackageValue[%s]!\n","appNo");
			fclose(fp);
        	        return(ret);
        	}

		len = snprintf(resStr, sizeof(resStr),"pinPadID=%s|regTime=%s|lastTime=%s|status=%s|branch=%s|teller=%s|remark=%s|producter=%s|producterID=%s|zmkLength=%d|zpkLength=%d|zakLength=%d|appNo=%s|", 
				tpinPadDB.pinPadID,
				tpinPadDB.regTime,
				tpinPadDB.lastTime,
				tpinPadDB.status,
				tpinPadDB.branch,
				tpinPadDB.teller,
				tpinPadDB.remark,
				tpinPadDB.producter,
				tpinPadDB.producterID,
				tpinPadDB.zmkLength,
				tpinPadDB.zpkLength,
				tpinPadDB.zakLength,
				tpinPadDB.appNo);
		resStr[len] = 0;
		fprintf(fp, "%s\n",resStr);
	}
	fclose(fp);
        return(0);
}


/* 

���ܣ�	��ȡ�к�
������	branch[out]	�����к�

*/
void UnionGetPinPadBranch(char *branchID)
{
	char	*ptr = NULL;
	int	len = 0;
	if ((ptr = UnionReadStringTypeRECVar("branchNo")) == NULL)
		len = sprintf(branchID,"%s","999999999999");
	else
		len = sprintf(branchID,"%s",ptr);
	branchID[len] = 0;
}

/*

����:	ȡ���з��б�־ 
����ֵ:	 <0 	ʧ�ܣ����ش�����
	 >=0	�ɹ������سɹ�ֵ
*/
int UnionGetPinPadBranchType()
{
	int	ret = 0;

	if ((ret = UnionReadIntTypeRECVar("branchType")) < 0)
        {
                UnionUserErrLog("in UnionGetPinPadBranchType:: UnionReadIntTypeRECVar[%s]!\n","branchType");
                return(errCodeRECMDL_VarNotExists);
        }

	return ret; 
}

/*

����:	��ȡԶ�̱�־
����ֵ:		<0	ʧ��,���ش�����
		>=0	�ɹ�

*/
/*
int UnionGetPinPadRemoteDistributeID(char *distributeID)
{
	int	ret = 0;
	char	sysID[32];
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEE9:: UnionReadRequestXMLPackageValue[head/sysID]!\n");
                return(ret);
        }
	memcpy(distributeID,sysID,ret);
        distributeID[ret] = 0;
	return 0;
}*/

// ��ȡ������KMCԼ������Կ����
/*int UnionGetPinPadZmkNameWithKMC(char *zmkName) 
{
	char	*ptr = NULL;
	int	len = 0;

	if ((ptr = UnionReadStringTypeRECVar("zmkWithKMC")) == NULL)
	{   
		UnionUserErrLog("in UnionGetPinPadZmkNameWithKMC:: UnionReadStringTypeRECVar[%s] not found!\n","zmkWithKMC");
		return (errCodeRECMDL_VarNotExists);
	}   
	len = sprintf(zmkName,"%s",ptr);
	zmkName[len] = 0;

        return 0; 
}*/

/*
int UnionUpdateKeyAndRetKeyByZMK(char *zmkName, char *keyName, char *value, char *checkValue)
{       
	int     ret;

	// ȡ�����Լ���ı���ZMK����
	if (strlen(zmkName) == 0)
	{
		if ((ret = UnionGetPinPadZmkName(zmkName)) < 0)
		{
			UnionUserErrLog("in UnionUpdateKeyAndRetKeyByZMK:: UnionGetPinPadZmkName error!\n");
			return(ret);
		}
	}   

	
	// ������Կ��ʹ��ָ��zmk�������
	if ((ret = UnionInitRequestXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionUpdateKeyAndRetKeyByZMK:: UnionInitRequestXMLPackage error!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateKeyAndRetKeyByZMK:: UnionInitRequestXMLPackage error!\n");
		return(ret);
	}

	return 0;
}*/

/*

���ܣ�	����zmk\zpk\zak��Կ

*/
int UnionPinPadInsertDesKeyIntoKeyDB(char *appNo,char *pinPadID,int zmkLength,int zpkLength,int zakLength,char *branch)
{
	int			ret = 0;
	TUnionSymmetricKeyDB	symmetricKeyDB;	

	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	snprintf(symmetricKeyDB.keyGroup,sizeof(symmetricKeyDB.keyGroup),"%s", "default");
	symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
	symmetricKeyDB.inputFlag = 1;
	symmetricKeyDB.outputFlag = 1;
	symmetricKeyDB.effectiveDays = 36500;
	symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;
	symmetricKeyDB.oldVersionKeyIsUsed = 1;
	UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfKMS;
	snprintf(symmetricKeyDB.creator,sizeof(symmetricKeyDB.creator),"%s",appNo);
	if ((branch != NULL) && (strlen(branch) > 0))
		snprintf(symmetricKeyDB.keyApplyPlatform, sizeof(symmetricKeyDB.keyApplyPlatform), "%s-KEY", branch);
	UnionGetFullSystemDateTime(symmetricKeyDB.createTime);

	// zmk
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s.%s.zmk",appNo,pinPadID);
	// ��Կ����	
	symmetricKeyDB.keyType = conZMK;
	// ��Կ����
	symmetricKeyDB.keyLen = UnionConvertSymmetricKeyDBKeyLen(zmkLength);
	
	// ������Կ
	if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPinPadInsertDesKeyIntoKeyDB:: UnionCreateSymmetricKeyDB keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}

	// zpk
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s.%s.zpk",appNo,pinPadID);
	// ��Կ����	
	symmetricKeyDB.keyType = conZPK;
	// ��Կ����
	symmetricKeyDB.keyLen = UnionConvertSymmetricKeyDBKeyLen(zpkLength);

	// ������Կ
	if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPinPadInsertDesKeyIntoKeyDB:: UnionCreateSymmetricKeyDB keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}
	
	// zak
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s.%s.zak",appNo,pinPadID);
	// ��Կ����	
	symmetricKeyDB.keyType = conZAK;
	// ��Կ����
	symmetricKeyDB.keyLen = UnionConvertSymmetricKeyDBKeyLen(zakLength);

	// ������Կ
	if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPinPadInsertDesKeyIntoKeyDB:: UnionCreateSymmetricKeyDB keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}

	return 0;
}

//�������������Կ��Ч
int UnionActiveKeyOfPinPad(char *appNo, char *pinPadID)
{
	int			ret;
	char    		zmkName[136];
	char    		zpkName[136];
	char   			zakName[136];

	TUnionSymmetricKeyDB	zpkKey;
	TUnionSymmetricKeyDB	zmkKey;
	TUnionSymmetricKeyDB	zakKey;

	memset(&zpkKey, 0, sizeof(zpkKey));
	memset(&zmkKey, 0, sizeof(zmkKey));
	memset(&zakKey, 0, sizeof(zakKey));

	// ����zmk��Կ״̬
	snprintf(zmkName,sizeof(zmkName),"%s.%s.%s", appNo, pinPadID, "zmk");
	if ((ret =  UnionReadSymmetricKeyDBRec(zmkName,0,&zmkKey)) < 0)
	{
		UnionUserErrLog("in UnionActiveKeyOfPinPad:: UnionReadSymmetricKeyDBRec [%s] ERROR!\n", zmkName);
		return(ret);
	}
	// �����Կû����Ч���ڻ�У��ֵ,��������ԿΪ��ʼ��״̬
	if ((strlen(zmkKey.activeDate) == 0) || (strlen(zmkKey.checkValue) == 0))
		zmkKey.status = conSymmetricKeyStatusOfInitial;
	else
		zmkKey.status = conSymmetricKeyStatusOfEnabled;

	if((ret = (UnionUpdateSymmetricKeyDBStatus(&zmkKey))) < 0)
	{
		UnionUserErrLog("in UnionActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", zmkName);
		return ret;
	}

	// ����zpk��Կ״̬
	snprintf(zpkName,sizeof(zpkName),"%s.%s.%s", appNo, pinPadID, "zpk");
	if ((ret =  UnionReadSymmetricKeyDBRec(zpkName,0,&zpkKey)) < 0)
	{
		UnionUserErrLog("in UnionActiveKeyOfPinPad:: UnionReadSymmetricKeyDBRec [%s] ERROR!\n", zpkName);
		return(ret);
	}
	// �����Կû����Ч���ڻ�У��ֵ,��������ԿΪ��ʼ��״̬
	if ((strlen(zpkKey.activeDate) == 0) || (strlen(zpkKey.checkValue) == 0))
		zpkKey.status = conSymmetricKeyStatusOfInitial;
	else
		zpkKey.status = conSymmetricKeyStatusOfEnabled;

	if((ret = (UnionUpdateSymmetricKeyDBStatus(&zpkKey))) < 0)
	{
		UnionUserErrLog("in UnionActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", zpkName);
                return ret;
	}

	// ����zak��Կ״̬
	snprintf(zakName,sizeof(zakName),"%s.%s.%s", appNo, pinPadID, "zak"); 
	if ((ret =  UnionReadSymmetricKeyDBRec(zakName,0,&zakKey)) < 0)
	{
		UnionUserErrLog("in UnionActiveKeyOfPinPad:: UnionReadSymmetricKeyDBRec [%s] ERROR!\n", zakName);
		return(ret);
	}
	// �����Կû����Ч���ڻ�У��ֵ,��������ԿΪ��ʼ��״̬
	if ((strlen(zakKey.activeDate) == 0) || (strlen(zakKey.checkValue) == 0))
		zakKey.status = conSymmetricKeyStatusOfInitial;
	else
		zakKey.status = conSymmetricKeyStatusOfEnabled;

        if((ret = (UnionUpdateSymmetricKeyDBStatus(&zakKey))) < 0)
        {
                UnionUserErrLog("in UnionActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", zakName);
                return ret;
        }

	return 0;
}

int UnionInActiveKeyOfPinPad(char *appNo, char *pinPadID)
{
        int     ret;
        TUnionSymmetricKeyDB    desKey;
        char    zmkName[136];
        char    zpkName[136];
        char    zakName[136];

        memset(&desKey, 0, sizeof desKey);
        memset(zmkName, 0, sizeof zmkName);
        memset(zpkName, 0, sizeof zpkName);
        memset(zakName, 0, sizeof zakName);


        sprintf(zmkName,"%s.%s.%s", appNo, pinPadID, "zmk");
        sprintf(zpkName,"%s.%s.%s", appNo, pinPadID, "zpk");
        sprintf(zakName,"%s.%s.%s", appNo, pinPadID, "zak");

        snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zmkName);
        desKey.status = conSymmetricKeyStatusOfSuspend;
        if((ret = (UnionUpdateSymmetricKeyDBStatus(&desKey))) < 0)
        {
                UnionUserErrLog("in UnionInActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", desKey.keyName);
                return ret;
        }

        snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zpkName);
        desKey.status = conSymmetricKeyStatusOfSuspend;
        if((ret = (UnionUpdateSymmetricKeyDBStatus(&desKey))) < 0)
        {
                UnionUserErrLog("in UnionInActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", desKey.keyName);
                return ret;
        }

        snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zakName);
        desKey.status = conSymmetricKeyStatusOfSuspend;
        if((ret = (UnionUpdateSymmetricKeyDBStatus(&desKey))) < 0)
        {
                UnionUserErrLog("in UnionInActiveKeyOfPinPad:UnionUpdateSymmetricKeyDBStatus [%s] ERROR!\n", desKey.keyName);
                return ret;
        }

        return 0;
}

int UnionDeleteKeyOfPinPad(char *appNo, char *pinPadID)
{
	int     ret;
        TUnionSymmetricKeyDB    desKey;
        char    zmkName[64];
        char    zpkName[64];
        char    zakName[64];

        memset(&desKey, 0, sizeof desKey);
        memset(zmkName, 0, sizeof zmkName);
        memset(zpkName, 0, sizeof zpkName);
        memset(zakName, 0, sizeof zakName);


        sprintf(zmkName,"%s.%s.%s", appNo, pinPadID, "zmk");
        sprintf(zpkName,"%s.%s.%s", appNo, pinPadID, "zpk");
        sprintf(zakName,"%s.%s.%s", appNo, pinPadID, "zak");

        snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zmkName);
	if((ret = (UnionDropSymmetricKeyDB(&desKey))) < 0)
	{
		UnionUserErrLog("in UnionDeleteKeyOfPinPad UnionDropSymmetricKeyDB[%s] ERROR!\n",desKey.keyName);
		return ret;
	}
	snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zpkName);
	if((ret = (UnionDropSymmetricKeyDB(&desKey))) < 0)
        {
                UnionUserErrLog("in UnionDeleteKeyOfPinPad UnionDropSymmetricKeyDB[%s] ERROR!\n",desKey.keyName);
                return ret;
        }
	snprintf(desKey.keyName,sizeof(desKey.keyName),"%s", zakName);
        if((ret = (UnionDropSymmetricKeyDB(&desKey))) < 0)
        {
                UnionUserErrLog("in UnionDeleteKeyOfPinPad UnionDropSymmetricKeyDB[%s] ERROR!\n",desKey.keyName);
                return ret;
        }
	return 0;
}

int UnionReadQueryAllBranchToFile(char *fileName)
{
	int	ret = 0;	
	char	sql[128];
	char	tmpBuf[16];
	FILE	*fp = NULL;
	int	totalNum = 0;
	char	branchNo[16];
	char	branchName[128];
	char	contact[128];
	char	remark[128];
	int	i = 0;

	snprintf(sql,sizeof(sql),"select branchNo,branchName,contact,remark from pinPadBranch");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
                UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionSelectRealDBRecord[%s] error!\n",sql);
                return ret;
	}
	else if (ret == 0)
	{
                UnionLog("in UnionReadQueryAllBranchToFile:: sql[%s] not record!\n",sql);
                return ret;
	}

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
                UnionSystemErrLog("in UnionReadQueryAllBranchToFile:: fopen fileName[%s] error!\n",fileName);
                return(errCodeUseOSErrCode);
	}

	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
                UnionLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[totalNum]!\n");
		goto errorExit;
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)	
		{
			UnionLog("in UnionReadQueryAllBranchToFile:: UnionLocateXMLPackage[detail]!\n");
			goto errorExit;
		}

		if ((ret = UnionReadXMLPackageValue("branchNo",branchNo,sizeof(branchNo))) < 0)
		{
			UnionLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[branchNo]!\n");
			goto errorExit;
		}
		branchNo[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("branchName",branchName,sizeof(branchName))) < 0)
		{
			UnionLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[branchName]!\n");
			goto errorExit;
		}
		branchName[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("contact",contact,sizeof(contact))) < 0)
		{
			UnionLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[contact]!\n");
			goto errorExit;
		}
		contact[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			UnionLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[remark]!\n");
			goto errorExit;
		}
		remark[ret] = 0;

		fprintf(fp,"fld1=%s|fld2=%s|fld3=%s|fld4=%s|\n",branchNo,branchName,contact,remark);
	}

	ret = totalNum;
errorExit:
	if (fp != NULL)
		fclose(fp);
	return (ret);
}

// д���г��̵��ļ�
int UnionReadQueryAllProducterToFile(char *fileName)
{
	int	ret = 0;	
	char	sql[128];
	char	tmpBuf[16];
	FILE	*fp = NULL;
	int	totalNum = 0;
	char	producter[48];
	char	dllName[48];
	char	remark[128];
	int	i = 0;

	snprintf(sql,sizeof(sql),"select producter,dllName,remark from pinPadProducter");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
                UnionUserErrLog("in UnionReadQueryAllProducterToFile:: UnionSelectRealDBRecord[%s] error!\n",sql);
                return ret;
	}
	else if (ret == 0)
	{
                UnionLog("in UnionReadQueryAllProducterToFile:: sql[%s] not record!\n",sql);
                return ret;
	}

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
                UnionSystemErrLog("in UnionReadQueryAllProducterToFile:: fopen fileName[%s] error!\n",fileName);
                return(errCodeUseOSErrCode);
	}

	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
                UnionLog("in UnionReadQueryAllProducterToFile:: UnionReadXMLPackageValue[totalNum]!\n");
		goto errorExit;
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)	
		{
			UnionLog("in UnionReadQueryAllProducterToFile:: UnionLocateXMLPackage[detail]!\n");
			goto errorExit;
		}

		if ((ret = UnionReadXMLPackageValue("producter",producter,sizeof(producter))) < 0)
		{
			UnionLog("in UnionReadQueryAllProducterToFile:: UnionReadXMLPackageValue[producter]!\n");
			goto errorExit;
		}
		producter[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("dllName",dllName,sizeof(dllName))) < 0)
		{
			UnionLog("in UnionReadQueryAllProducterToFile:: UnionReadXMLPackageValue[dllName]!\n");
			goto errorExit;
		}
		dllName[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			UnionLog("in UnionReadQueryAllProducterToFile:: UnionReadXMLPackageValue[remark]!\n");
			goto errorExit;
		}
		remark[ret] = 0;

		fprintf(fp,"fld1=%s|fld2=%s|fld3=%s|\n",producter,dllName,remark);
	}

	ret = totalNum;
errorExit:
	if (fp != NULL)
		fclose(fp);
	return (ret);
}

// д���в����ߵ��ļ�
int UnionReadQueryAllOperatorToFile(char *fileName)
{
	int	ret = 0;	
	char	sql[128];
	char	tmpBuf[16];
	FILE	*fp = NULL;
	int	totalNum = 0;
	char	userID[128];
	char	userName[128];
	char	userPassword[128];
	char	userRoleList[128];
	char	loginFlag[16];
	char	loginSysID[32];
	char	remark[128];
	int	i = 0;

	snprintf(sql,sizeof(sql),"select userID,userName,userPassword,userRoleList,loginFlag,loginSysID,remark from sysUser");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
                UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionSelectRealDBRecord[%s] error!\n",sql);
                return ret;
	}
	else if (ret == 0)
	{
                UnionLog("in UnionReadQueryAllBranchToFile:: sql[%s] not record!\n",sql);
                return ret;
	}

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
                UnionSystemErrLog("in UnionReadQueryAllBranchToFile:: fopen fileName[%s] error!\n",fileName);
                return(errCodeUseOSErrCode);
	}

	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[totalNum]!\n");
		goto errorExit;
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)	
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionLocateXMLPackage[detail]!\n");
			goto errorExit;
		}

		if ((ret = UnionReadXMLPackageValue("userID",userID,sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[userID]!\n");
			goto errorExit;
		}
		userID[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("userName",userName,sizeof(userName))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[userName]!\n");
			goto errorExit;
		}
		userName[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("userPassword",userPassword,sizeof(userPassword))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[userPassword]!\n");
			goto errorExit;
		}
		userPassword[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("userRoleList",userRoleList,sizeof(userRoleList))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[userRoleList]!\n");
			goto errorExit;
		}
		userRoleList[ret] = 0;
		if ((strcmp(userRoleList,"1") != 0) && (strcmp(userRoleList,"2") != 0) && (strcmp(userRoleList,"4") != 0))
			continue;

		if ((ret = UnionReadXMLPackageValue("loginFlag",loginFlag,sizeof(loginFlag))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[loginFlag]!\n");
			goto errorExit;
		}
		loginFlag[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("loginSysID",loginSysID,sizeof(loginSysID))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[loginSysID]!\n");
			goto errorExit;
		}
		loginSysID[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionReadQueryAllBranchToFile:: UnionReadXMLPackageValue[remark]!\n");
			goto errorExit;
		}
		remark[ret] = 0;

		fprintf(fp,"fld1=%s|fld2=%s|fld3=%s|fld4=%s|fld5=%s|fld6=%s|fld7=%s|\n",userID,userName,userPassword,userRoleList,loginFlag,loginSysID,remark);
	}

	ret = totalNum;
errorExit:
	if (fp != NULL)
		fclose(fp);
	return (ret);
}

// ��ȡ������Ϣ
int UnionReadPinPadProducterRec(char *producter,char *dllName,int sizeofDllName,char *remark,int sizeofRemark)
{
	int	ret = 0;
	char	sql[256];

	snprintf(sql,sizeof(sql),"select producter,dllName,remark from pinPadProducter where producter = '%s'",producter);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
                UnionUserErrLog("in UnionReadPinPadProducterRec:: UnionSelectRealDBRecord[%s] error!\n",sql);
                return ret;
	}
	else if (ret == 0)
	{
                UnionUserErrLog("in UnionReadPinPadProducterRec:: sql[%s] producter[%s] not record!\n",sql,producter);
                return (errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadProducterRec:: UnionLocateXMLPackage[detail]!\n");
                return ret;
	}

	if ((ret = UnionReadXMLPackageValue("dllName",dllName,sizeofDllName)) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadProducterRec:: UnionReadXMLPackageValue[dllName]!\n");
                return ret;
	}
	dllName[ret] = 0;

	if ((ret = UnionReadXMLPackageValue("remark",remark,sizeofRemark)) < 0)
	{
		UnionUserErrLog("in UnionReadPinPadProducterRec:: UnionReadXMLPackageValue[remark]!\n");
                return ret;
	}
	remark[ret] = 0;

	return (ret);
}

// ���в���ʧ�ܻع�
int UnionPinPadOperateRollback(char *branch,char *status)
{
	int				ret = 0;
	TUnionRemoteKeyPlatform 	tapplyPlatform;

	memset(&tapplyPlatform,0,sizeof(tapplyPlatform));

	if ((ret = UnionReadRemoteKeyPlatformRec(branch,conRemoteKeyPlatformKeyDirectionOfApply,&tapplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: UnionReadRemoteKeyPlatformRec apply[%s]!\n",branch);
		return(ret);
	}

	// ��ʼ��Զ����Կ����������
	if ((ret = UnionInitRemoteRequestPackage(&tapplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: UnionInitRemoteRequestPackage!\n");
		return(ret);
	}

	// ���÷��к�
	if ((ret = UnionSetRequestRemotePackageFldValue("branch",branch)) < 0)
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: UnionSetRequestRemotePackageFldValue[%s]!\n","body/branch");
		return(ret);
	}

	// ����״̬
	if ((ret = UnionSetRequestRemotePackageFldValue("status",status)) < 0)
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: UnionSetRequestRemotePackageFldValue[%s]!\n","status");
		return(ret);
	}

	// ���÷�����
	if (tapplyPlatform.packageType == 5)
	{
		if ((ret = UnionSetRequestRemotePackageHead("serviceCode","EEEF")) < 0)
		{
			UnionUserErrLog("in UnionPinPadOperateRollback:: UnionSetRequestRemotePackageHead[serviceCode]!\n");
			return(ret);
		}
	}
	else if (tapplyPlatform.packageType == 4)
	{
		if ((ret = UnionSetRequestRemotePackageHead("serviceCode","109")) < 0)
		{
			UnionUserErrLog("in UnionPinPadOperateRollback:: UnionSetRequestRemotePackageHead[serviceCode]!\n");
			return(ret);
		}
	}
	else
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: tapplyPlatform.packageType[%d] is not support!\n",tapplyPlatform.packageType);
		return(errCodeEsscMDL_InvalidOperation);
	}

	
	// �ַ�������ƽ̨
	if ((ret = UnionTransferPackageToRemotePlatform(&tapplyPlatform)) < 0)
	{
		UnionUserErrLog("in UnionPinPadOperateRollback:: UnionTransferPackageToRemotePlatform!\n");
		return(ret);
	}

	return ret;
}

/* 
���ܣ�	��ȡ�ط�������������
������	branchNo[in]		branchNo
	sizeofBranchName[in]	����
	branchName[out]		�ط�����������

����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetBranchNameByBranchNo(char *branchNo,char *branchName,int sizeofBranchName)
{
	int		ret;
	char		sql[128];
	
	snprintf(sql,sizeof(sql),"select branchName from %s where branchNo = '%s'", "pinPadBranch",branchNo);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionGetBranchNameByBranchNo:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionLog("in UnionGetBranchNameByBranchNo:: branchNo[%s] not find!\n",branchNo);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);

	if ((ret = UnionReadXMLPackageValue("branchName", branchName, sizeofBranchName)) < 0)
	{
		UnionUserErrLog("in UnionGetBranchNameByBranchNo:: UnionReadXMLPackageValue[%s]!\n","branchName");
		return(ret);
	}

	return(0);
}

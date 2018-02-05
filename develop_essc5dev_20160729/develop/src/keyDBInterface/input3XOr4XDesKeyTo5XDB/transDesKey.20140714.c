//add by 张树斌 20140718
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "unionDesKey.h"
#include "UnionLog.h"
#include "symmetricKeyDB.h"
#include "symmetricKeyDBJnl.h"
#include "unionRealBaseDB.h"

// add by liwj
#include "sjl06.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "3DesRacalCmd.h"
#include "unionCommand.h"
#include "unionHsmCmd.h"
#include "unionErrCode.h"
#include "unionRealDBCommon.h"
#include "UnionProc.h"
// add end

#include "UnionTask.h"


int UnionTaskActionBeforeExit();
int UnionIsUpdateKeyRec();

int readDesKeyFromFile(FILE *fp, TUnionDesKey *tdeskey, int len)
{
	char	buf[8192];
	int	ret, x = 0;

	while (x < len)
	{
		if (!fgets(buf, sizeof buf, fp))
			return(x);
		if ((ret = UnionReadDesKeyFromRecStr(buf, strlen(buf), tdeskey + x)) <0)
		{
			UnionUserErrLog("in readDesKeyFromFile::UnionReadDesKeyFromRecStr\n!");
			continue;
		}
		if (UnionIsValidDesKey(tdeskey + x))
			x++;
	}
	return(x);
}

//add by chenwd 20150119 读取密钥名称
int readDesKeyNameFromFile(FILE *fp, TUnionDesKey *tdeskey, int len)
{
	char	buf[8192];
	int	ret, x = 0;

	while (x < len)
	{
		if (!fgets(buf, sizeof buf, fp))
			return(x);

		if ((ret = UnionReadRecFldFromRecStr(buf,strlen(buf),"keyName",(tdeskey+x)->fullName,sizeof((tdeskey+x)->fullName))) < 0)
		{
			UnionUserErrLog("in readDesKeyNameFromFile:: UnionReadRecFldFromRecStr keyName!\n");
			return(ret);
		}

		x++;
	}
	return(x);
}
//add end

static int getTimeFromTimet(time_t timet, char *buf)
{
	struct tm *tmptm;
	int	tmpyear;

	tmptm = (struct tm *)localtime(&timet);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;

	return(sprintf(buf,"%04d%02d%02d%02d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec));

}

int UnionIsValidSymmetricKeyName(char *keyName)
{
        int     i;
        int     len;
        int     pointNum = 0;

        len = strlen(keyName);
        for (i = 0; i < len; i++)
        {
                if (keyName[i] == '.')
                        pointNum++;
        }

        if (pointNum != 2)
        {
                return(0);
        }
        else
        {
                if (keyName[len-1] == '.')
                {
                        return(0);
                }
                else
                        return(1);
        }
}
int UnionIsValidSymmetricKeyLength(TUnionSymmetricKeyLength keyLen)
{
        switch (keyLen)
        {
                case    con64BitsSymmetricKey:
                case    con128BitsSymmetricKey:
                case    con192BitsSymmetricKey:
                        return(1);
                default:
                        UnionUserErrLog("in UnionIsValidSymmetricKeyLength:: keyLen[%d]!\n",keyLen);
                        return(0);
        }
}
TUnionSymmetricKeyType UnionConvertSymmetricKeyKeyType(char *keyTypeName)
{
        if (strcasecmp(keyTypeName,"ZPK") == 0)
                return(conZPK);
        if (strcasecmp(keyTypeName,"ZAK") == 0)
                return(conZAK);
        if (strcasecmp(keyTypeName,"ZMK") == 0)
                return(conZMK);
        if (strcasecmp(keyTypeName,"ZEK") == 0)
                return(conZEK);
        if (strcasecmp(keyTypeName,"TPK") == 0)
                return(conTPK);
        if (strcasecmp(keyTypeName,"TAK") == 0)
                return(conTAK);
        if (strcasecmp(keyTypeName,"TMK") == 0)
                return(conTMK);
        if (strcasecmp(keyTypeName,"PVK") == 0)
                return(conPVK);
        if (strcasecmp(keyTypeName,"CVK") == 0)
                return(conCVK);
        if (strcasecmp(keyTypeName,"WWK") == 0)
                return(conWWK);
        if (strcasecmp(keyTypeName,"BDK") == 0)
                return(conBDK);
        if (strcasecmp(keyTypeName,"EDK") == 0)
                return(conEDK);
        if (strcasecmp(keyTypeName,"USER") == 0)
                return(conSelfDefinedKey);
        if (strcasecmp(keyTypeName,"MK-AC") == 0 || (strcasecmp(keyTypeName,"MKAC") == 0))
                return(conMKAC);
        if (strcasecmp(keyTypeName,"MK-SMC") == 0 || (strcasecmp(keyTypeName,"MKSMC") == 0))
                return(conMKSMC);
        if (strcasecmp(keyTypeName,"KMC-Seed") == 0)
                return(conKMCSeed);
        if (strcasecmp(keyTypeName,"KMU-Seed") == 0)
                return(conKMUSeed);
        if (strcasecmp(keyTypeName,"MDK-Seed") == 0)
                return(conMDKSeed);
        return(errCodeParameter);
}
int UnionIsValidSymmetricKeyAlgorithmID(TUnionSymmetricAlgorithmID algorithmID)
{
        switch (algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:
                case    conSymmetricAlgorithmIDOfSM4:
                case    conSymmetricAlgorithmIDOfSM1:
                        return(1);
                default:
                        UnionUserErrLog("in UnionIsValidSymmetricKeyAlgorithmID:: algorithmID[%d]!\n",algorithmID);
                        return(0);
        }
}

int UnionIsValidSymmetricKeyType(TUnionSymmetricKeyType keyType)
{
	switch (keyType)
	{
		case	conZPK:
		case	conZAK:
		case	conZMK:
		case	conTMK:
		case	conTPK:
		case	conTAK:
		case	conPVK:
		case	conCVK:
		case	conZEK:
		case	conWWK:
		case	conBDK:
		case	conEDK:
		case	conSelfDefinedKey:
		case	conMKAC:
		case	conMKSMC:
		case	conKMCSeed:
		case	conKMUSeed:
		case	conMDKSeed:
			return(1);
		default:
			UnionUserErrLog("in UnionIsValidSymmetricKeyType:: keyType[%d]!\n",keyType);
			return(0);
	}
}
int UnionIsValidSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB)
{
        if (psymmetricKeyDB == NULL)
        {
                UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: null pointer!\n");
                return(0);
        }

        if (!UnionIsValidSymmetricKeyName(psymmetricKeyDB->keyName))
        {
                UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyName error!\n");
                return(0);
        }
        if (!UnionIsValidSymmetricKeyLength(psymmetricKeyDB->keyLen))
        {
                UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyLength error!\n");
                return(0);
        }       
        if (!UnionIsValidSymmetricKeyType(psymmetricKeyDB->keyType))
        {               
                UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyType error!\n");
                return(0);
        }       
        if (!UnionIsValidSymmetricKeyAlgorithmID(psymmetricKeyDB->algorithmID))
        {       
                UnionUserErrLog("in UnionIsValidSymmetricKeyDB:: UnionIsValidSymmetricKeyAlgorithmID error!\n");
                return(0); 
        }               

        return(1);
}
//int transDesKeyToSymmetricKeyDB(TUnionDesKey *pdeskey, char *protectID, TUnionSymmetricKeyDB *psymmetricKeyDB)
int transDesKeyToSymmetricKeyDB(TUnionDesKey *pdeskey, char *protectID, TUnionSymmetricKeyDB *psymmetricKeyDB,char *gunionKeyGroup)//modify by lusj 20151218
{
	//char	typeStr[8];

	memset(psymmetricKeyDB, 0, sizeof(*psymmetricKeyDB));

	strcpy(psymmetricKeyDB->keyName, pdeskey->fullName);
//	strcpy(psymmetricKeyDB->keyGroup, "default");
	strcpy(psymmetricKeyDB->keyGroup, gunionKeyGroup); //medify by lusj 20151218 密钥组有"default",改为写入值gunionKeyGroup
	psymmetricKeyDB->algorithmID = conSymmetricAlgorithmIDOfDES;
	
	// modify by leipp 20150310 begin
	//memset(typeStr, 0, sizeof typeStr);
	//UnionGetNameOfDesKeyType(pdeskey->type, typeStr);
	//psymmetricKeyDB->keyType = UnionConvertSymmetricKeyKeyType(typeStr);
	psymmetricKeyDB->keyType = pdeskey->type;
	// modify by leipp 20150310 end
	psymmetricKeyDB->keyLen = UnionGetDesKeyBitsLength(pdeskey->length);	
	psymmetricKeyDB->inputFlag = 1;
	psymmetricKeyDB->outputFlag = 1;
	psymmetricKeyDB->effectiveDays = pdeskey->maxEffectiveDays;
	psymmetricKeyDB->status = pdeskey->value[0]?conSymmetricKeyStatusOfEnabled:conSymmetricKeyStatusOfInitial;
	psymmetricKeyDB->oldVersionKeyIsUsed = pdeskey->oldVerEffective;
	UnionGetFullSystemDate(psymmetricKeyDB->activeDate); 
	strcpy(psymmetricKeyDB->keyValue[0].keyValue, pdeskey->value);
	strcpy(psymmetricKeyDB->keyValue[0].oldKeyValue, pdeskey->oldValue);
	strcpy(psymmetricKeyDB->keyValue[0].lmkProtectMode, protectID);
	strcpy(psymmetricKeyDB->checkValue, pdeskey->checkValue);
	strcpy(psymmetricKeyDB->oldCheckValue, pdeskey->oldCheckValue);
	if(0 == (time_t)pdeskey->lastUpdateTime)
	{
		pdeskey->lastUpdateTime = (long)time(NULL);
	}
	getTimeFromTimet((time_t)pdeskey->lastUpdateTime, psymmetricKeyDB->keyUpdateTime);
	// modify by lisq 20150122
	//psymmetricKeyDB->creatorType = conSymmetricCreatorTypeOfUser;
	//strcpy(psymmetricKeyDB->creator, "USER");
	psymmetricKeyDB->creatorType = conSymmetricCreatorTypeOfMove;
	strcpy(psymmetricKeyDB->creator, "USER");
	// modify by lisq 20150122 end
	UnionGetFullSystemDateTime(psymmetricKeyDB->createTime);
	strcpy(psymmetricKeyDB->remark, "import from essc3 or essc4");
	if (!UnionIsValidSymmetricKeyDB(psymmetricKeyDB))
	{
		UnionUserErrLog("in transDesKeyToSymmetricKeyDB::key[%s] is invallid!\n", pdeskey->fullName);
		return(-1);
	}
	return(1);
	
}

//add by chenwd 20160118 记录插入成功记录
int writeTheSucceedKeyToFile(char *keyName)
{
	FILE	*fp = NULL;
	char	succeedFileName[64];
	int	keyNameBufLen = 0;
	char	keyNameBuf[128];

	snprintf(succeedFileName, sizeof(succeedFileName), "inputSucceedOfKeyName.txt");
	if ((fp = fopen(succeedFileName, "a")) == NULL)
	{
		fprintf(stderr, "in writeTheSucceedKeyToFile:: fopen[%s] error!\n", succeedFileName);
		return -1;
	}

	memset(keyNameBuf,0,sizeof(keyNameBuf));
	keyNameBufLen = snprintf(keyNameBuf,sizeof(keyNameBuf),"SucceedTime=%s|keyName=%s|\n",UnionGetCurrentFullSystemDateTime(),keyName);
	fwrite(keyNameBuf, 1, keyNameBufLen, fp);
	fclose(fp);
	return 0;
}
//add end

extern char	inputSelectAppID[128];//add by chenwd 20160122 根据appID选择性迁移
static int realUpdate(TUnionSymmetricKeyDB *psymmetricKeyDB)
{
	int	len, ret;
	char	sql[10240];

	if(!strcmp(psymmetricKeyDB->keyApplyPlatform,"null"))
	{
		psymmetricKeyDB->keyApplyPlatform[0] = 0;
	}
//add by chenwd 20160122 根据appID选择性迁移
	if ( strlen(inputSelectAppID) > 0)
	{
		if( 0 == strncmp(inputSelectAppID,psymmetricKeyDB->keyName,strlen(inputSelectAppID)))
		{
			goto selectAppIDUpdate;
		}
		return -1;
	}

selectAppIDUpdate:
//add end
	len = snprintf(sql,sizeof(sql),"update %s set keyGroup = '%s', algorithmID = %d,keyType = %d,keyLen = %d,inputFlag = %d,outputFlag = %d,effectiveDays = %d,status = %d,oldVersionKeyIsUsed = %d, keyApplyPlatform = '%s',activeDate = '%s',checkValue = '%s',oldCheckValue = '%s',keyUpdateTime = '%s',creatorType=%d,creator = '%s',createTime = '%s',usingUnit = '%s',remark ='%s' where keyName = '%s'",
		defTableNameOfSymmetricKeyDB,
                psymmetricKeyDB->keyGroup,
                psymmetricKeyDB->algorithmID,
                psymmetricKeyDB->keyType,
                psymmetricKeyDB->keyLen,
                psymmetricKeyDB->inputFlag,
                psymmetricKeyDB->outputFlag,
                psymmetricKeyDB->effectiveDays,
                psymmetricKeyDB->status,
                psymmetricKeyDB->oldVersionKeyIsUsed,
		psymmetricKeyDB->keyApplyPlatform,
                psymmetricKeyDB->activeDate,
                psymmetricKeyDB->checkValue,
                psymmetricKeyDB->oldCheckValue,
                psymmetricKeyDB->keyUpdateTime,
                psymmetricKeyDB->creatorType,
                psymmetricKeyDB->creator,
                psymmetricKeyDB->createTime,
                psymmetricKeyDB->usingUnit,
                psymmetricKeyDB->remark,
                psymmetricKeyDB->keyName);
	sql[len] = 0;

	if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
	{
		UnionUserErrLog("in realUpdate:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}
	
	// modify by leipp 20150720
	if ((strlen(psymmetricKeyDB->keyValue[0].keyValue) > 0) || (strlen(psymmetricKeyDB->keyValue[0].oldKeyValue) > 0))
	{
		len = snprintf(sql,sizeof(sql),"insert into %s(keyName,lmkProtectMode,keyValue,oldKeyValue)"
			"values('%s','%s','%s','%s')",
			defTableNameOfSymmetricKeyValue,
			psymmetricKeyDB->keyName,
			psymmetricKeyDB->keyValue[0].lmkProtectMode,
			psymmetricKeyDB->keyValue[0].keyValue,
			psymmetricKeyDB->keyValue[0].oldKeyValue);
			if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
			{
				len = snprintf(sql,sizeof(sql),"update %s set lmkProtectMode = '%s',keyValue = '%s',oldKeyValue = '%s' where keyName = '%s'",
					defTableNameOfSymmetricKeyValue,
					psymmetricKeyDB->keyValue[0].lmkProtectMode,
					psymmetricKeyDB->keyValue[0].keyValue,
					psymmetricKeyDB->keyValue[0].oldKeyValue,
					psymmetricKeyDB->keyName);
				if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
				{
					UnionUserErrLog("in realUpdate:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
					return(ret);
				}
			}
	}
	else
	{
		sql[len] = 0;
	}
	// modify end

	writeTheSucceedKeyToFile(psymmetricKeyDB->keyName);//add by chenwd 20160118 记录成功密钥名称

	return(1);
}
static int realInsert(TUnionSymmetricKeyDB *psymmetricKeyDB)
{
	int	len, ret;
	char	sql[10240];


	if(UnionIsUpdateKeyRec())
		return(realUpdate(psymmetricKeyDB));

	if(!strcmp(psymmetricKeyDB->keyApplyPlatform,"null"))
	{
		psymmetricKeyDB->keyApplyPlatform[0] = 0;
	}
//add by chenwd 20160122 根据appID选择性迁移
	if ( strlen(inputSelectAppID) > 0)
	{
		if( 0 == strncmp(inputSelectAppID,psymmetricKeyDB->keyName,strlen(inputSelectAppID)))
		{
			goto selectAppIDInsert;
		}
		return -1;
	}

selectAppIDInsert:
//add end
	len = snprintf(sql,sizeof(sql),"insert into %s(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,\
keyApplyPlatform,activeDate,checkValue,oldCheckValue,keyUpdateTime,creatorType,creator,createTime,usingUnit,remark)values('%s','%s',%d, %d,%d,%d,%d,%d,%d,%d,'%s' ,'%s','%s','%s','%s',%d,'%s','%s','%s','%s');",\
		defTableNameOfSymmetricKeyDB,
                psymmetricKeyDB->keyName,
                psymmetricKeyDB->keyGroup,
                psymmetricKeyDB->algorithmID,
                psymmetricKeyDB->keyType,
                psymmetricKeyDB->keyLen,
                psymmetricKeyDB->inputFlag,
                psymmetricKeyDB->outputFlag,
                psymmetricKeyDB->effectiveDays,
                psymmetricKeyDB->status,
                psymmetricKeyDB->oldVersionKeyIsUsed,
		psymmetricKeyDB->keyApplyPlatform,
                psymmetricKeyDB->activeDate,
                psymmetricKeyDB->checkValue,
                psymmetricKeyDB->oldCheckValue,
                psymmetricKeyDB->keyUpdateTime,
                psymmetricKeyDB->creatorType,
                psymmetricKeyDB->creator,
                psymmetricKeyDB->createTime,
                psymmetricKeyDB->usingUnit,
                psymmetricKeyDB->remark);
	
	// modify by leipp 20150720
	if ((strlen(psymmetricKeyDB->keyValue[0].keyValue) > 0) || (strlen(psymmetricKeyDB->keyValue[0].oldKeyValue) > 0))
	{
		len += snprintf(sql + len,sizeof(sql) - len,"insert into %s(keyName,lmkProtectMode,keyValue,oldKeyValue)"
			"values('%s','%s','%s','%s');",
			defTableNameOfSymmetricKeyValue,
			psymmetricKeyDB->keyName,
			psymmetricKeyDB->keyValue[0].lmkProtectMode,
			psymmetricKeyDB->keyValue[0].keyValue,
			psymmetricKeyDB->keyValue[0].oldKeyValue);
	}
	else
	{
		sql[len-1] = 0;
	}
	// modify end

	if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
	{
		UnionUserErrLog("in realInsert:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}

	writeTheSucceedKeyToFile(psymmetricKeyDB->keyName);//add by chenwd 20160118 记录成功密钥名称

	return(1);
}

int checkDesKeyValueAndCheckValue(TUnionDesKey *pdeskey)
{
	int	ret = 0;
	char	checkValue[20];

	if (pdeskey->checkValue[0]) // 有校验值
	{
		if (!(pdeskey->value[0])) // 无密钥值
			return -1;

		// 计算校验值
		UnionSetIsUseNormalZmkType();
		memset(checkValue, 0, sizeof(checkValue));
		if ((ret = UnionHsmCmdBU(0,pdeskey->type,pdeskey->length,pdeskey->value,checkValue)) < 0)
		{
			fprintf(stderr, "in checkDesKeyValueAndCheckValue:: UnionHsmCmdBU keyName[%s] error!\n", pdeskey->fullName);
			return -1;
		}
		if (strcmp(checkValue, pdeskey->checkValue) != 0)
		{
			return -1;
		}
	}

	// 旧密钥不需要校验

	return 1;
}

int writeTheFailCheckKeyToFile(TUnionDesKey *deskey)
{
	FILE	*fp = NULL;
	char	failFileName[64];
	int	keyBufLen = 0;
	char	keyBuf[1024];

	snprintf(failFileName, sizeof(failFileName), "inputDesKeyCheckFail.txt");

	memset(keyBuf, 0, sizeof(keyBuf));
	if ((keyBufLen = UnionPutDesKeyIntoRecStr(deskey,keyBuf,sizeof(keyBuf))) < 0)
	{
		fprintf(stderr, "in writeTheFailCheckKeyToFile:: keyBuf[%s] error!\n", keyBuf);
		return -1;
	}
	keyBuf[keyBufLen++] = '\n';

	if ((fp = fopen(failFileName, "a")) == NULL)
	{
		fprintf(stderr, "in writeTheFailCheckKeyToFile:: fopen[%s] error!\n", failFileName);
		return -1;
	}
	
	fwrite(keyBuf, 1, keyBufLen, fp);
	fclose(fp);
	return 0;
}

//int insertDesKeyToDB(char *protectID, char *keyFile, int checkKey)
int insertDesKeyToDB(char *protectID, char *keyFile, int checkKey,char *gunionKeyGroup) //modify by lusj 20151218
{
	FILE	*fp;
	TUnionDesKey	desKeyList[256];
	TUnionSymmetricKeyDB	symmetricKeyDB;	
	int	x, num, readNum = 0, insertNum = 0;
	char	sql[64];

	sprintf(sql, "modeID='%s'", protectID);
	if ((x = UnionSelectRealDBRecordCounts("lmkProtectMode", NULL, sql)) <0)
	{
		fprintf(stderr, "Read protectID from dataBases fial!\n");
		UnionUserErrLog("in insertDesKeyToDB::UnionSelectRealDBRecordCounts!\n");
		return(x);
	}
	else if (x == 0)
	{
		UnionUserErrLog("in insertDesKeyToDB::UnionSelectRealDBRecordCounts protectID [%s] do not exist\n", protectID);
		fprintf(stderr, "protectID [%s] do not exist\n", protectID);
		return(0);
	}
	if ((fp = fopen(keyFile, "rb")) == NULL)
	{
		UnionUserErrLog("in insertDesKeyToDB::fopen [%s] fail!\n", keyFile);
		fprintf(stderr, "in insertDesKeyToDB::open [%s] fail!\n", keyFile);
		return(-1);
	}
	
	while((num = readDesKeyFromFile(fp, desKeyList, 250)))
	{
		readNum += num;
		
		for (x = 0; x < num; x++)
		{
			if (checkKey)
			{
				if (checkDesKeyValueAndCheckValue(desKeyList + x) < 0)
				{
					writeTheFailCheckKeyToFile(desKeyList + x);
					continue;
				}
			}

		      //if (transDesKeyToSymmetricKeyDB(desKeyList + x, protectID, &symmetricKeyDB) <0)
			//medify by lusj 20151218 密钥组有"default",改为写入值gunionKeyGroup
			if (transDesKeyToSymmetricKeyDB(desKeyList + x, protectID, &symmetricKeyDB,gunionKeyGroup) <0)
			{
				writeTheFailCheckKeyToFile(desKeyList + x);
				UnionUserErrLog("in insertDesKeyToDB::transDesKeyToSymmetricKeyDB\n");
				continue;
			}

			if (realInsert(&symmetricKeyDB) <0)
			{
				writeTheFailCheckKeyToFile(desKeyList + x);
				UnionUserErrLog("in insertDesKeyToDB::realInsert!\n");
				continue;
			}
			insertNum++;
		}
	}
	fprintf(stdout, "read [%d] key from file, [%d] input succeed, [%d] input fail.\n", readNum, insertNum, readNum - insertNum);	
	fclose(fp);
	return(0);
}

// add by liwj 20150505
TUnionSymmetricKeyDB	gunionSymmetricKeyDB[256];
TUnionDesKey		gunionDesKey[256];
int			gunionSymmetricKeyNum=0;
extern char		gunionZmkValue[52];
extern char		gunionProtectID[128];
//int inputAllKeyFromDesKeyFile(const char *fileName)
int inputAllKeyFromDesKeyFile(const char *fileName,char *gunionKeyGroup)//modify by lusj 20151218 增加密钥组
{
	int		ret;
	FILE		*fp = NULL;
	TUnionDesKey	key;
	TUnionDesKey	desKeyList[256];
	TUnionSymmetricKeyDB	symmetricKeyDB;	
	int	x, num, readNum = 0, insertNum = 0;
	int		childNum = 0;
	pid_t		pid;
	int		childStatus;
	char		tmpValue[64];
	char		tmpCheckValue[20];

	if ((fp = fopen(fileName,"r")) == NULL)	
	{
		fprintf(stderr, "in inputAllKeyFromDesKeyFile:: fopen[%s]\n", fileName);
		UnionUserErrLog("in inputAllKeyFromDesKeyFile:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}

	signal(SIGCHLD, SIG_DFL);
	while((num = readDesKeyFromFile(fp, desKeyList, 250)))
	{
		childNum++;
		readNum += num;
		gunionSymmetricKeyNum = 0;

		for (x = 0; x < num; x++)
		{
			key = desKeyList[x];
			memset(tmpValue, 0, sizeof(tmpValue));
			memset(tmpCheckValue, 0, sizeof(tmpCheckValue));
			// 转换密钥值
			if (strlen((desKeyList+x)->value) > 0)
			{
				if ((ret = UnionHsmCmdA6(NULL, (desKeyList+x)->type,gunionZmkValue,(desKeyList+x)->value, tmpValue,tmpCheckValue)) < 0)
				{
					writeTheFailCheckKeyToFile(desKeyList+x);
					UnionUserErrLog("in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 current key errCode[%d]\n", ret);
					fprintf(stdout, "in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 keyName[%s] fail!\n", (desKeyList+x)->fullName);
					UnionUserErrLog("in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 keyName[%s] fail!\n", (desKeyList+x)->fullName);
					continue;
				}
				// 检查密钥校验值
				if (key.checkValue[0])
				{
					if (strcmp(tmpCheckValue, key.checkValue) != 0)
					{
						UnionUserErrLog("in inputAllKeyFromDesKeyFile:: keyName[%s] checkValue was not right!\n",key.fullName);
						//fprintf(stdout, "in inputAllKeyFromDesKeyFile::realInsert keyName[%s] fail!\n", (desKeyList+x)->fullName);
						writeTheFailCheckKeyToFile(desKeyList+x);
						continue;
					}
				}
				memcpy(key.value, tmpValue, sizeof(key.value));
			}

			// 转换旧密钥值
			if (strlen((desKeyList+x)->oldValue) > 0)
			{
				if ((ret = UnionHsmCmdA6(NULL, (desKeyList+x)->type,gunionZmkValue,(desKeyList+x)->oldValue, key.oldValue,key.oldCheckValue)) < 0)
				{
					UnionUserErrLog("in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 old key errCode[%d]\n", ret);
					UnionUserErrLog("in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 keyName[%s] fail!\n", (desKeyList+x)->fullName);
					fprintf(stdout, "in inputAllKeyFromDesKeyFile::UnionHsmCmdA6 keyName[%s] fail!\n", (desKeyList+x)->fullName);
					writeTheFailCheckKeyToFile(desKeyList+x);
					continue;
				}
			}

		//	if (transDesKeyToSymmetricKeyDB(&key, gunionProtectID, &symmetricKeyDB) <0)
	if (transDesKeyToSymmetricKeyDB(&key, gunionProtectID, &symmetricKeyDB,gunionKeyGroup) <0)//medify by lusj 20151218 密钥组有"default",改为写入值gunionKeyGroup
			{
				UnionUserErrLog("in inputAllKeyFromDesKeyFile::transDesKeyToSymmetricKeyDB keyName[%s] fail!\n", key.fullName);
				fprintf(stdout, "in inputAllKeyFromDesKeyFile::transDesKeyToSymmetricKeyDB keyName[%s] fail!\n", key.fullName);
				writeTheFailCheckKeyToFile(desKeyList+x);
				continue;
			}	

			gunionDesKey[gunionSymmetricKeyNum] = key;
			gunionSymmetricKeyDB[gunionSymmetricKeyNum++] = symmetricKeyDB;
			//gunionDesKey[gunionSymmetricKeyNum] = key; //by chenwd 20160222 写错误记录bug
		}
		if (UnionCreateProcess() == 0)
		{
			insertNum = 0;
			for (x = 0; x < gunionSymmetricKeyNum; x++)
			{
				if (realInsert(&gunionSymmetricKeyDB[x]) <0)
				{
					writeTheFailCheckKeyToFile(&gunionDesKey[x]);
					//fprintf(stdout, "in inputAllKeyFromDesKeyFile::realInsert keyName[%s] fail!\n", gunionSymmetricKeyDB[x].keyName);
					UnionUserErrLog("in inputAllKeyFromDesKeyFile::realInsert keyName[%s] fail!\n", gunionSymmetricKeyDB[x].keyName);
					continue;
				}
				insertNum++;
			}
			if (fp)
				fclose(fp);
			fp = NULL;
			UnionCloseDatabase();
			UnionDisconnectTaskTBL();
			UnionDisconnectLogFileTBL();
			exit(insertNum);
		}
	}

	// 父进程
	insertNum = 0;
	for (x = 0; x < childNum; x++)
	{
		pid = wait(&childStatus);
		childStatus = WEXITSTATUS(childStatus);
		if (childStatus >= 0)
			insertNum += childStatus;
	}

	fprintf(stdout, "read [%d] key from file, [%d] input succeed, [%d] input fail.\n", readNum, insertNum, readNum - insertNum);	
	fclose(fp);
	fp = NULL;
	UnionCloseDatabase();
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	fp = NULL;
	return(-1);
}
// add end 

// add by chenwd 20160118 密钥回退
int deleteDesKey(char *keyFile)
{
	int	ret, x, num, len = 0, readNum = 0, deleteNum = 0;
	char	sql[512];
	FILE	*fp;
	TUnionDesKey	desKeyList[128];
	
	if ((fp = fopen(keyFile, "rb")) == NULL)
	{
		UnionUserErrLog("in deleteDesKey::fopen [%s] fail!\n", keyFile);
		fprintf(stderr, "in deleteDesKey::open [%s] fail!\n", keyFile);
		return(-1);
	}

	memset(desKeyList, 0, sizeof(desKeyList));
	while((num = readDesKeyNameFromFile(fp, desKeyList, 128)))
	{
		if (num < 0)
			return(num);

		readNum += num;
		for (x = 0; x < num; x++)
		{
			memset(sql,0,sizeof(sql));
			len = snprintf(sql,sizeof(sql),"delete from %s where keyName = '%s';",defTableNameOfSymmetricKeyDB,(desKeyList+x)->fullName);
			len += snprintf(sql + len,sizeof(sql) - len,"delete from %s where keyName = '%s';",defTableNameOfSymmetricKeyValue,(desKeyList+x)->fullName);

			if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
			{
				UnionUserErrLog("in deleteDesKey:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
				return(ret);
			}
			
			deleteNum++;
		}
	}
	
	fprintf(stdout, "read [%d] key from file, [%d] delete .\n", readNum, deleteNum);
	fclose(fp);
	return(0);
}
// add end


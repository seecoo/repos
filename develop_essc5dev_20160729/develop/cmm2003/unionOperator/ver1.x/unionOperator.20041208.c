//	Author:		Wolfgang Wang
//	Date:		2004/12/08
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "UnionAlgorithm.h"
#include "unionErrCode.h"
#include "UnionStr.h"

#include "unionOperator.h"

PUnionSharedMemoryModule	pgunionOperatorTableMDL = NULL;
PUnionOperatorTable		pgunionOperatorTable = NULL;
PUnionOperator			pgunionOperator = NULL;

PUnionOperator UnionConnectCurrentOperatorTable()
{
	int	ret;
	
	if ((ret = UnionConnectOperatorTable()) < 0)
		return(NULL);
	return(pgunionOperator);
}

int UnionGetCurrentMaxNumOfOperator()
{
	int	ret;
	
	if ((ret = UnionConnectOperatorTable()) < 0)
		return(ret);
	return(pgunionOperatorTable->maxNumOfOperator);
}


int UnionEncryptOperatorPassword(char *tellerNo,char *passwd,unsigned char *epasswd)
{
	unsigned char	tmpBuf[512];
	unsigned char	tmpPasswd[32+1];
	
	if ((tellerNo == NULL) || (passwd == NULL) || (epasswd == NULL))
		return(errCodeParameter);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf((char *)tmpBuf,"%4s%s",tellerNo,passwd);
	UnionMD5(tmpBuf,strlen((char *)tmpBuf),tmpPasswd);
	tmpPasswd[32] = 0;
	//UnionLog("in UnionEncryptOperatorPassword:: tmpPasswd = [%s]\n",tmpPasswd);
	aschex_to_bcdhex((char *)tmpPasswd,32,(char *)epasswd);
	return(0);
}

int UnionFormOperator(char *tellerNo,char *name,char level,char *passwd,PUnionOperator poper)
{
	if ((poper == NULL) || (tellerNo == NULL) || (name == NULL) || (passwd == NULL) || 
		(strlen(tellerNo) >= sizeof(poper->id)) ||
		(strlen(name) >= sizeof(poper->name)) ||
		((level < '0') || (level > '9')))
		return(errCodeParameter);
	memset(poper,0,sizeof(*poper));
	strcpy(poper->id,tellerNo);
	strcpy(poper->name,name);
	poper->level = level;
	UnionEncryptOperatorPassword(tellerNo,passwd,poper->password);
	UnionGetFullSystemDate(poper->registerDate);
	UnionGetFullSystemDate(poper->passwordLastUpdatedDate);
	poper->logonTimes = 0;
	poper->passwordInputWrongTimes = 0;
	poper->isCurrentLogon = 0;
	poper->operationTimes = 0;
	return(0);
}
		
	
int UnionIsOperatorTableConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfOperatorTable)) || (pgunionOperatorTable == NULL) || (pgunionOperator == NULL)) 
		return(0);
	else
		return(1);
}

int UnionGetConfFileNameOfOperatorTable(char *fileName)
{
	sprintf(fileName,"%s/unionOperator.TBL",getenv("UNIONETC"));
	return(0);
}

int UnionCreateOperatorTable(long maxNumOfOperator,long passwordMaxEffectiveDays,long maxIdleTimeBetweenTwoOperations)
{
	TUnionOperator		oper;
	char			fileName[512];
	TUnionOperatorTable	tbl;
	FILE			*fp;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfOperatorTable(fileName);
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateOperatorTable:: fopen!\n");
		return(errCodeUseOSErrCode);
	}
	memset(&tbl,0,sizeof(tbl));
	tbl.maxNumOfOperator = maxNumOfOperator;
	tbl.passwordMaxEffectiveDays = passwordMaxEffectiveDays;
	tbl.maxIdleTimeBetweenTwoOperations = maxIdleTimeBetweenTwoOperations;
	fwrite(&tbl,sizeof(tbl),1,fp);
	
	memset(&oper,0,sizeof(oper));
	UnionFormOperator("88888888","administrator",'9',"administrator",&oper);
	fwrite(&oper,sizeof(oper),1,fp);
		
	fclose(fp);
	
	return(0);
}

int UnionDeleteOperatorTable()
{
	char			tmpBuf[512];
	char			fileName[512];
	FILE			*fp;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfOperatorTable(fileName);
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteOperatorTable:: fopen!\n");
		return(errCodeUseOSErrCode);
	}
	fclose(fp);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",fileName);
	system(tmpBuf);
	return(0);
}
	
long UnionGetMaxVarNumOfOperatorTable()
{
	char		fileName[512];
	TUnionOperatorTable operTbl;
	FILE		*fp;
		
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfOperatorTable(fileName);
	memset(&operTbl,0,sizeof(operTbl));
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionGetMaxVarNumOfOperatorTable:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fread(&operTbl,sizeof(operTbl),1,fp);
	fclose(fp);
	if ((operTbl.maxNumOfOperator < 0) || (operTbl.maxNumOfOperator > 10000))
		return(errCodeOperatorMDL_OperatorNum);
	else
		return(operTbl.maxNumOfOperator);
}

int UnionConnectOperatorTable()
{
	long				num;
	
	if (UnionIsOperatorTableConnected())	// 已经连接
		return(0);
		
	if ((num = UnionGetMaxVarNumOfOperatorTable()) <= 0)
	{
		UnionUserErrLog("in UnionConnectOperatorTable:: UnionGetMaxVarNumOfOperatorTable [%ld]\n",num);
		return(num);
	}
	
	if ((pgunionOperatorTableMDL = UnionConnectSharedMemoryModule(conMDLNameOfOperatorTable,
			sizeof(TUnionOperatorTable) + sizeof(TUnionOperator) * num)) == NULL)
	{
		UnionUserErrLog("in UnionConnectOperatorTable:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionOperatorTable = (PUnionOperatorTable)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionOperatorTableMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectOperatorTable:: PUnionOperatorTable!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionOperator = (PUnionOperator)((unsigned char *)pgunionOperatorTable + sizeof(*pgunionOperatorTable))) == NULL)
	{
		UnionUserErrLog("in UnionConnectOperatorTable:: PUnionOperatorTable!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionOperatorTable->poperatorList = pgunionOperator;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionOperatorTableMDL))
		return(UnionReloadOperatorTable());
	else
		return(0);
}


int UnionDisconnectOperatorTable()
{
	pgunionOperator = NULL;
	pgunionOperatorTable = NULL;
	return(UnionDisconnectShareModule(pgunionOperatorTableMDL));
}

int UnionRemoveOperatorTable()
{
	UnionDisconnectOperatorTable();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfOperatorTable));
}

int UnionReloadOperatorTable()
{
	int			ret;
	char			fileName[512];
	int			i;
	TUnionOperatorTable	tbl;
	FILE			*fp;
		
	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionReloadOperatorTable:: UnionConnectOperatorTable!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfOperatorTable(fileName);
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionReloadOperatorTable:: fileName [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&tbl,0,sizeof(tbl));
	fread(&tbl,sizeof(tbl),1,fp);
	pgunionOperatorTable->maxNumOfOperator = tbl.maxNumOfOperator;
	pgunionOperatorTable->passwordMaxEffectiveDays = tbl.passwordMaxEffectiveDays;
	pgunionOperatorTable->maxIdleTimeBetweenTwoOperations = tbl.maxIdleTimeBetweenTwoOperations;

	for (i = 0; i < tbl.maxNumOfOperator && (!feof(fp)); i++)
	{
		memset(pgunionOperator+i,0,sizeof(TUnionOperator));	
		fread(pgunionOperator+i,sizeof(TUnionOperator),1,fp);
		(pgunionOperator+i)->isCurrentLogon = 0;
	}
	for (; i < tbl.maxNumOfOperator; i++)
		memset(pgunionOperator+i,0,sizeof(TUnionOperator));	
	fclose(fp);
	
	return(0);
}

int UnionPrintOperatorToFile(PUnionOperator poperator,FILE *fp)
{
	char	tmpBuf[100];
	
	if ((poperator == NULL) || (fp == NULL))
		return(errCodeParameter);
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex((char *)poperator->password,16,tmpBuf);
	fprintf(fp,"[柜员标识]       [%s]\n",poperator->id);
	fprintf(fp,"[柜员名称]       [%s]\n",poperator->name);
	fprintf(fp,"[级    别]       [%c]\n",poperator->level);
	fprintf(fp,"[登记日期]       [%s]\n",poperator->registerDate);
	fprintf(fp,"[密码密文]       [%s]\n",tmpBuf);
	fprintf(fp,"[密码更新日期]   [%s]\n",poperator->passwordLastUpdatedDate);
	fprintf(fp,"[最近一次登录]   [%s]\n",poperator->lastLogonDateTime);
	fprintf(fp,"[最近一次操作]   [%s]\n",poperator->lastOperationDateTime);
	fprintf(fp,"[操作次数]       [%ld]\n",poperator->operationTimes);
	fprintf(fp,"[登录次数]       [%ld]\n",poperator->logonTimes);
	fprintf(fp,"[密码连续出错]   [%d]\n",poperator->passwordInputWrongTimes);
	fprintf(fp,"[当前登录状态]   [%d]\n",poperator->isCurrentLogon);
	return(0);
}

int UnionPrintOperatorTableAttrToFile(FILE *fp)
{
	int	ret;

	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionPrintOperatorTableAttrToFile:: UnionConnectOperatorTable!\n");
		return(ret);
	}
	fprintf(fp,"[maxNumOfOperator]			= [%ld]\n",pgunionOperatorTable->maxNumOfOperator);
	fprintf(fp,"[passwordMaxEffectiveDays]		= [%ld]\n",pgunionOperatorTable->passwordMaxEffectiveDays);
	fprintf(fp,"[maxIdleTimeBetweenTwoOperations]	= [%ld]\n",pgunionOperatorTable->maxIdleTimeBetweenTwoOperations);
	//UnionPrintSharedMemoryModuleToFile(pgunionOperatorTableMDL,stdout);
	return(0);
}

// 将指定的域输出到文件，如果fldList为NULL或为空，将所有域输出到文件
// 合法的域名称为ID/NAME/LEVEL/passwordInputWrongTimes/LastLogonDateTime/IsCurrentLogon
// 分别对应标识/姓名/级别/密码连接出错次数/最后一次登录时间/当前是否登录
// 域串中可以包括多个域名
int UnionPrintOperatorTableToSpecFile(char *fileName,char *fldList)
{
	FILE	*fp;
	int	ret;
	int	outputName = 0,outputLevel = 0,outputPasswordInputWrongTimes=0,outputLastOperationTime=0,outputIsCurrentLogon=0;
	int	i,num=0;
		
	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionPrintOperatorTableToSpecFile:: UnionConnectOperatorTable!\n");
		return(ret);
	}

	if ((fileName == NULL) || (strcmp(fileName,"stderr") == 0) || (strcmp(fileName,"stdout") == 0))
		fp = stdout;
	else 
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintOperatorTableToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((fldList == NULL) || (strlen(fldList) == 0))	// 输出所有域
	{
		if ((ret = UnionPrintOperatorTableToFile(fp)) < 0)
			UnionUserErrLog("in UnionPrintOperatorTableToFile:: UnionPrintOperatorTableToFile!\n");
		goto exitNow;
	}
	// 输出指定域
	UnionToUpperCase(fldList);
	if (strstr(fldList,"NAME") != NULL)
		outputName = 1;
	if (strstr(fldList,"LEVEL") != NULL)
		outputLevel = 1;
	if (strstr(fldList,"PASSWORDINPUTWRONGTIMES") != NULL)
		outputPasswordInputWrongTimes = 1;
	if (strstr(fldList,"LASTLOGONDATETIME") != NULL)
		outputLastOperationTime = 1;
	if (strstr(fldList,"ISCURRENTLOGON") != NULL)
		outputIsCurrentLogon = 1;
	for (i = 0; i < pgunionOperatorTable->maxNumOfOperator; i++)
	{
		if (strlen((pgunionOperator+i)->id) == 0)
			continue;
		num++;
		fprintf(fp,"id=%s|",(pgunionOperator+i)->id);
		if (outputLevel)
			fprintf(fp,"level=%c|",(pgunionOperator+i)->level);
		if (outputIsCurrentLogon)
			fprintf(fp,"isCurrentLogon=%d|",(pgunionOperator+i)->isCurrentLogon);
		if (outputPasswordInputWrongTimes)
			fprintf(fp,"passwordInputWrongTimes=%05d|",(pgunionOperator+i)->passwordInputWrongTimes);
		if (outputLastOperationTime)
			fprintf(fp,"lastLogonDateTime=%14s|",(pgunionOperator+i)->lastLogonDateTime);
		if (outputName)
			fprintf(fp,"name=%s|",(pgunionOperator+i)->name);
		fprintf(fp,"\n");		
	}
	ret = num;
exitNow:	
	if (fp != stdout)
		fclose(fp);
	return(ret);
}		
	
int UnionPrintOperatorTableToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num=0;

	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionPrintOperatorTableToFile:: UnionConnectOperatorTable!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionOperatorTable->maxNumOfOperator; i++)
	{
		if (strlen((pgunionOperator+i)->id) == 0)
			continue;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		num++;
		//fprintf(fp,"%4s %20s %c %8s %8s %06ld %14s %14s %2d %d\n",
		fprintf(fp,"%8s %20s %c %06ld %14s %14s %06ld %2d %d\n",
			(pgunionOperator+i)->id,
			(pgunionOperator+i)->name,
			(pgunionOperator+i)->level,
			//(pgunionOperator+i)->registerDate,
			//(pgunionOperator+i)->passwordLastUpdatedDate,
			(pgunionOperator+i)->logonTimes,
			(pgunionOperator+i)->lastLogonDateTime,
			(pgunionOperator+i)->lastOperationDateTime,
			(pgunionOperator+i)->operationTimes,
			(pgunionOperator+i)->passwordInputWrongTimes,
			(pgunionOperator+i)->isCurrentLogon);
		//UnionPrintOperatorToFile((pgunionOperator+i),fp);
	}
	fprintf(fp,"OperatorNum = [%d]\n",num);
	return(0);
}


PUnionOperator UnionReadOperator(char *tellerNo)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionReadOperator:: UnionConnectOperatorTable!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionOperatorTable->maxNumOfOperator; i++)
	{
		if (strcmp(tellerNo,(pgunionOperator+i)->id) == 0)
			return(pgunionOperator+i);
	}
	for (i = 0; i < pgunionOperatorTable->maxNumOfOperator; i++)
	{
		if (strcmp(tellerNo,(pgunionOperator+i)->name) == 0)
			return(pgunionOperator+i);
	}
	return(NULL);
}

int UnionPrintOperator(char *tellerNo)
{
	PUnionOperator	poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionPrintOperator:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	return(UnionPrintOperatorToFile(poperator,stdout));
}

int UnionUpdateOperatorPassword(char *tellerNo,char *newpasswd)
{
	PUnionOperator	poperator;
	
	if ((tellerNo == NULL) || (newpasswd == NULL) || 
		(strlen(tellerNo) >= sizeof(poperator->id)))
	{
		UnionUserErrLog("in UnionUpdateOperatorPassword:: parameter error!\n");
		return(errCodeParameter);
	}
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateOperatorPassword:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	UnionEncryptOperatorPassword(tellerNo,newpasswd,poperator->password);
	UnionGetFullSystemDate(poperator->passwordLastUpdatedDate);
	return(UnionMirrorOperatorTable());
}

int UnionUpdateOperatorPasswordDirectly(char *tellerNo,unsigned char *newpasswdCrytogram)
{
	PUnionOperator	poperator;
	
	if ((tellerNo == NULL) || (newpasswdCrytogram == NULL) || 
		(strlen(tellerNo) >= sizeof(poperator->id)))
	{
		UnionUserErrLog("in UnionUpdateOperatorPasswordDirectly:: parameter error!\n");
		return(errCodeParameter);
	}
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateOperatorPasswordDirectly:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	memcpy(poperator->password,newpasswdCrytogram,16);
	UnionGetFullSystemDate(poperator->passwordLastUpdatedDate);
	return(UnionMirrorOperatorTable());
}

int UnionUpdateOperatorName(char *tellerNo,char *name)
{
	PUnionOperator	poperator;
	
	if ((tellerNo == NULL) || (name == NULL) || (strlen(tellerNo) >= sizeof(poperator->id)) || (strlen(name) >= sizeof(poperator->name)))
	{
		UnionUserErrLog("in UnionUpdateOperatorName:: parameter error!\n");
		return(errCodeParameter);
	}
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateOperatorName:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	strcpy(poperator->name,name);
	return(UnionMirrorOperatorTable());
}

int UnionUpdateOperatorLevel(char *tellerNo,char level)
{
	PUnionOperator	poperator;
	
	if ((tellerNo == NULL) || (strlen(tellerNo) >= sizeof(poperator->id)) || 
		((level < '1') || (level > '9')) )
	{
		UnionUserErrLog("in UnionUpdateOperatorLevel:: parameter error!\n");
		return(errCodeParameter);
	}
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateOperatorLevel:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	poperator->level = level;
	return(UnionMirrorOperatorTable());
}

int UnionOperatorLogon(char *tellerNo,char *password)
{
	unsigned char	tmpBuf[100];
	PUnionOperator	poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionOperatorLogon:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionEncryptOperatorPassword(tellerNo,password,tmpBuf);
	if (memcmp(tmpBuf,poperator->password,16) != 0)
	{
		UnionUserErrLog("in UnionOperatorLogon:: [%s] password wrong!\n",tellerNo);
		poperator->passwordInputWrongTimes += 1;
		UnionMirrorOperatorTable();
		return(errCodeOperatorMDL_WrongPassword);
	}
	if (poperator->passwordInputWrongTimes >= 3)
	{
		UnionUserErrLog("in UnionOperatorLogon:: [%s] password locked!\n",tellerNo);
		return(errCodeOperatorMDL_PasswordLocked);
	}
	if (poperator->isCurrentLogon)
	{
		UnionUserErrLog("in UnionOperatorLogon:: [%s] already logon\n",tellerNo);
		return(errCodeOperatorMDL_AlreadyLogon);
	}
	else
		poperator->isCurrentLogon = 1;
	UnionGetFullSystemDateTime(poperator->lastLogonDateTime);
	UnionGetFullSystemDateTime(poperator->lastOperationDateTime);
	poperator->logonTimes += 1;
	poperator->passwordInputWrongTimes = 0;
	time(&(poperator->lastOperationTime));
	return(UnionMirrorOperatorTable());
}

int UnionOperatorLogoff(char *tellerNo)
{
	PUnionOperator	poperator;

	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionOperatorLogoff:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	if (!poperator->isCurrentLogon)
	{
		UnionUserErrLog("in UnionOperatorLogon:: [%s] not logon\n",tellerNo);
		return(errCodeOperatorMDL_NotLogon);
	}
	poperator->isCurrentLogon = 0;
	return(UnionMirrorOperatorTable());
}

int UnionUnlockOperatorPassword(char *tellerNo)
{
	PUnionOperator	poperator;

	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionUnlockOperatorPassword:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	poperator->passwordInputWrongTimes = 0;
	return(UnionMirrorOperatorTable());
}

int UnionLockOperatorPassword(char *tellerNo)
{
	PUnionOperator	poperator;

	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionLockOperatorPassword:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	poperator->passwordInputWrongTimes = 3;
	return(UnionMirrorOperatorTable());
}

int UnionApplyOperatorOperation(char *tellerNo)
{
	time_t now;
	PUnionOperator	poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionApplyOperatorOperation:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	if (!poperator->isCurrentLogon)
	{
		UnionUserErrLog("in UnionApplyOperatorOperation:: [%s] not logon\n",tellerNo);
		return(errCodeOperatorMDL_NotLogon);
	}
	time(&now);
	if (now - poperator->lastOperationTime >= pgunionOperatorTable->maxIdleTimeBetweenTwoOperations)
	{
		UnionUserErrLog("in UnionApplyOperatorOperation:: [%s] too long no action\n",tellerNo);
		UnionOperatorLogoff(tellerNo);
		return(errCodeOperatorMDL_TooLongTimeWithoutOperation);
	}
	time(&(poperator->lastOperationTime));
	++poperator->operationTimes;
	UnionGetFullSystemDateTime(poperator->lastOperationDateTime);
	return(UnionMirrorOperatorTable());
}

int UnionIsTellerStillActiveLogon(char *tellerNo)
{
	return(UnionApplyOperatorOperation(tellerNo));
}

int UnionMirrorOperatorTable()
{
	FILE	*fp;
	char	fileName[512];

	if (!UnionIsOperatorTableConnected())
	{
		UnionUserErrLog("in UnionMirrorOperatorTable:: !UnionIsOperatorTableConnected!\n");
		return(errCodeOperatorMDL_NotConnected);
	}
	if (pgunionOperatorTable->maxNumOfOperator < 0)
	{
		UnionUserErrLog("in UnionMirrorOperatorTable:: maxNumOfOperator = [%ld] error!\n",pgunionOperatorTable->maxNumOfOperator);
		return(errCodeOperatorMDL_OperatorNum);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfOperatorTable(fileName);
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorOperatorTable:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fwrite(pgunionOperatorTable,sizeof(*pgunionOperatorTable),1,fp);
	fwrite(pgunionOperator,sizeof(*pgunionOperator),pgunionOperatorTable->maxNumOfOperator,fp);
	fclose(fp);
	return(0);
}

int UnionUpdateOperatorPasswordMaxEffectiveDays(long passwordMaxEffectiveDays)
{
	int		ret;
	
	if (passwordMaxEffectiveDays <= 0)
	{
		UnionUserErrLog("in UnionUpdateOperatorPasswordMaxEffectiveDays:: parameter error [%ld]!\n",passwordMaxEffectiveDays);
		return(errCodeParameter);
	}
	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionUpdateOperatorPasswordMaxEffectiveDays:: UnionConnectOperatorTable!\n");
		return(ret);
	}
	
	pgunionOperatorTable->passwordMaxEffectiveDays = passwordMaxEffectiveDays;
	return(UnionMirrorOperatorTable());
}

int UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations(long maxIdleTimeBetweenTwoOperations)
{
	int	ret;

	if (maxIdleTimeBetweenTwoOperations <= 0)
	{
		UnionUserErrLog("in UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations:: parameter error [%ld]!\n",maxIdleTimeBetweenTwoOperations);
		return(errCodeParameter);
	}
	if ((ret = UnionConnectOperatorTable()) < 0)
	{
		UnionUserErrLog("in UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations:: UnionConnectOperatorTable!\n");
		return(ret);
	}
	
	pgunionOperatorTable->maxIdleTimeBetweenTwoOperations = maxIdleTimeBetweenTwoOperations;
	return(UnionMirrorOperatorTable());
}

int UnionInsertOperator(char *tellerNo,char *name,char level,char *passwd)
{
	PUnionOperator poperator;
	int		ret;
	int		i;
	
	if ((poperator = UnionReadOperator(tellerNo)) != NULL)
	{
		UnionUserErrLog("in UnionInsertOperator:: [%s] already exits!\n",tellerNo);
		return(errCodeOperatorMDL_OperatorAlreadyExists);
	}
	
	for (i = 0; i < pgunionOperatorTable->maxNumOfOperator; i++)
	{
		if (strlen((pgunionOperator+i)->id) == 0)
		{
			if ((ret = UnionFormOperator(tellerNo,name,level,passwd,pgunionOperator+i)) < 0)
			{
				UnionUserErrLog("in UnionInsertOperator:: UnionFormOperator [%s]!\n",tellerNo);
				return(ret);
			}
			return(UnionMirrorOperatorTable());
		}		
	}
	UnionUserErrLog("in UnionInsertOperator:: table is full!\n");
	return(errCodeOperatorMDL_TableFull);
}
	
int UnionDeleteOperator(char *tellerNo)
{
	PUnionOperator poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionDeleteOperator:: [%s] not exits!\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	
	memset(poperator,0,sizeof(*poperator));
	return(UnionMirrorOperatorTable());
}

	
long UnionGetOperatorDaysBeforePasswordOutdate(char *tellerNo)
{
	PUnionOperator	poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionGetOperatorDaysBeforePasswordOutdate:: UnionReadOperator [%s]\n",tellerNo);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	return(pgunionOperatorTable->passwordMaxEffectiveDays - UnionDecideDaysBeforeToday(poperator->passwordLastUpdatedDate));
}

int UnionIsOperatorLogon(char *tellerNo)
{
	PUnionOperator	poperator;
	
	if ((poperator = UnionReadOperator(tellerNo)) == NULL)
	{
		UnionUserErrLog("in UnionIsOperatorLogon:: UnionReadOperator [%s]\n",tellerNo);
		return(0);
	}
	return(poperator->isCurrentLogon);
}

char UnionGetOperatorLevel(char *tellerNo)
{
	PUnionOperator	prec;
	
	if ((prec = UnionReadOperator(tellerNo)) == NULL)
		return('-');
	else
		return(prec->level);
}

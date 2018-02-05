#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "unionLogonMode.h"
#include "unionTeller.h"
//#include "kmcDefaultHsm.h"
#include "sjl06Cmd.h"
#include "UnionStr.h"
#include "unionOperatorType.h"
#include "unionREC.h"
#include "UnionSHA1.h"

char *UnionReadResMngClientIPAddr();
char *UnionGetCurrentMngSvrClientReqStr();
char *UnionGetCurrentOperationTellerNo();
int UnionGetHashVal(char *reqStr, int lenOfReqStr, char *operatorID, char *hashVal);

// ��֤����Ա��½
int UnionVerifyOperatorLogin(char *tellerNo, int lenOfReqStr, char *reqStr)
{
	int	ret;
	int	lockFlag=0;
	char	remark[256+1];
	TUnionTeller  operator;

	memset(&operator, 0, sizeof operator);
	memset(remark, 0, sizeof(remark));
		
	// ��ȡ����Ա��¼
	ret = UnionReadOperatorRec(tellerNo,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorLogin:: UnionReadOperatorRec teller=[%s]\n",tellerNo);
		return(ret);
	}

	switch(operator.logonMode)
	{
	case    conLogonModeStaticPassword:	// ��̬����
		ret = UnionVerifyStaticPasswd(operator.passwordCiper, lenOfReqStr, reqStr);
		if (ret < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: UnionVerifyStatsPasswd err!\n");
			return(ret);
		}
		// �ж������Ƿ����
		if ( (ret = UnionIsPasswdOverTime(operator.level, operator.passwdMTime)) < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: passwd is overDate!\n");
			return(errCodeOperatorMDL_OperatorPwdOverTime);
		}
		// �ж�������,2011-01-21
		if (ret > 3)
		{
			UnionIsPasswdTooSimple(tellerNo, operator.passwordCiper);
		}
		// add by wuhy
		else if (ret == 0)
		{
			UnionInitLogonRemark();
			if (operator.logonTimes > 2)
			{
				return(errCodeOperatorMDL_OperatorPwdOverTime);
			}
			else
			{
				sprintf(remark, "ʹ�õ��ǳ�ʼ���룬���ܵ�½%d�Σ����޸�����", 3 - operator.logonTimes);
				UnionSetLogonRemark(remark);
			}
		}
		// end add
		break;
#ifndef _useStaticPwd_
	case    conLogonModeRsaSign:	// RSAǩ��
		ret = UnionVerifySign(tellerNo, operator.PK, operator.passwordCiper, lenOfReqStr, reqStr);
		if (ret < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: UnionVerifyStaticSign err!\n");
			return(ret);
		}
		break;
	case    conLogonModeStaticPasswordAndRsaSign:	// ��̬����+RSAǩ��
		ret = UnionVerifyStaticPasswdAndSign(tellerNo, operator.PK, operator.passwordCiper, lenOfReqStr, reqStr);
		if (ret < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: UnionVerifyStaticPasswdAndSign err!\n");
			return(ret);
		}
		// �ж������Ƿ����
		if ( (ret = UnionIsPasswdOverTime(operator.level, operator.passwdMTime)) < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: passwd is overDate!\n");
			return(errCodeOperatorMDL_OperatorPwdOverTime);
		}
		break;
	// 20140220 ����Ϊ������������
	//case    1146:	// ��̬����+RSAǩ��
	//	ret = UnionVerifyStaticPasswdEncByPK(tellerNo, operator.PK, operator.passwordCiper, lenOfReqStr, reqStr);
	//	if (ret < 0)
	//	{
	//		 UnionUserErrLog("in UnionVerifyOperatorLogin:: UnionVerifyStaticPasswdEncByPK err!\n");
	//		return(ret);
	//	}
		// �ж������Ƿ����
		if ( (ret = UnionIsPasswdOverTime(operator.level, operator.passwdMTime)) < 0)
		{
			 UnionUserErrLog("in UnionVerifyOperatorLogin:: passwd is overDate!\n");
			return(errCodeOperatorMDL_OperatorPwdOverTime);
		}
		break;
	// 	20140220 add end
	case    conLogonModeDynamicPassword:	// ��̬����
	case    conLogonModeDynamicPasswordAndRsaSign:	// ��̬����+RSAǩ��
#endif
	default:
		 UnionUserErrLog("in UnionVerifyOperatorLogin:: err logonMode[%d]!\n",operator.logonMode);
		return (errCodeCDPMDL_InvalidVarType);
	}
	return ret;
}

// ��֤��̬����
int UnionVerifyStaticPasswd(char *localPasswd, int lenOfReqStr, char *reqStr)
{
	int	ret;
	char	password[48+1];
	char    hashVal[128+1];
	char	tellerNo[128+1];

	memset( password, 0, sizeof password );
	memset(hashVal, 0, sizeof(hashVal));
	memset(tellerNo, 0, sizeof(tellerNo));

	// ��ȡ���󴮵�����
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"password", password,sizeof(password));
	if (ret < 0)
	{
		strcpy(password, reqStr);
	}

	// �Ƚ�����
	// add by wuhy at 20131120
	strcpy(tellerNo, UnionGetCurrentOperationTellerNo());
	if (strlen(localPasswd) == 40)
	{
		UnionGetHashVal(password, strlen(password), tellerNo, password);
	}
        // end add
	if (strcmp(localPasswd, password) != 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswd:: localPassword [%s] != remotePassword [%s]\n",localPasswd,password);
		return(errCodeOperatorMDL_WrongPassword);
	}

	return 0;
}

#ifndef _useStaticPwd_
// ��֤��̬����+RSAǩ��
int UnionVerifyStaticPasswdAndSign(char *teller, char *pk, char *localPasswd, int lenOfReqStr, char *reqStr)
{
	int	ret;
	int	algorithmID = 0;
	int	firstFlag = 1;
	char	password[48+1];
	char	sign[2048+1];
	char	data[2048+1];
	char	lastLogonTime[16+1];
	char	tmpBuf[128+1];

	memset( password, 0, sizeof password );
	memset( sign, 0, sizeof sign );
	memset( data, 0, sizeof data );
	memset(lastLogonTime,0,sizeof(lastLogonTime));

	// ��ȡ���󴮵�����
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"password", password,sizeof(password));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdAndSign:: UnionReadRecFldFromRecStr passwd err! reqStr=[%s]!\n",reqStr);
		return(ret);
	}

	// ��ȡǩ��
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"sign", sign,sizeof(sign));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdAndSign:: UnionReadRecFldFromRecStr sign err! reqStr=[%s]!\n",reqStr);
		return(errCodeOnlineKeyMngMDL_GetSignFail);
	}

	// ��֤����
	ret = UnionVerifyStaticPasswd(localPasswd, lenOfReqStr, reqStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdAndSign:: UnionVerifyStaticPasswd err!\n");
		return(ret);
	}
	
	// ��ȡǩ���㷨
	memset(tmpBuf,0,sizeof(tmpBuf));
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"algorithmID", tmpBuf,sizeof(tmpBuf));
	if (ret < 0)
		algorithmID = 0;	// RSA
	else
		algorithmID = atoi(tmpBuf);	

	// 2012-7-5 ǩ�����������ϴε�¼ʱ��
	memset(lastLogonTime,0,sizeof(lastLogonTime));
	if ((ret = UnionReadOperatorRecFld(teller,"lastLogonTime",lastLogonTime,sizeof(lastLogonTime))) < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdAndSign:: UnionReadOperatorRecFld lastLogonTime!\n");
		return(ret);
	}
	//sprintf(data, "%s%s", teller,password);
	sprintf(data, "%s%s%s", teller,password,lastLogonTime);
	
	// ��֤ǩ��
	ret = UnionVerifyOperatorSign(algorithmID,pk, strlen(pk), data, strlen(data),sign, strlen(sign));
	if (ret < 0)
	{
		UnionLog("in UnionVerifyStaticPasswdAndSign:: UnionVerifyOperatorSign[%s]!\n",data);

		memset(data, 0, sizeof(data));
		sprintf(data, "%s%s", teller,password);
		if ((ret = UnionVerifyOperatorSign(algorithmID,pk, strlen(pk), data, strlen(data),sign, strlen(sign))) < 0)
		{
			UnionUserErrLog("in UnionVerifyStaticPasswdAndSign:: UnionVerifyOperatorSign[%s]!\n",data);
			return(errCodeOnlineKeyMngMDL_GetSignFail);
		}
	}

	return 0;
}

// ��֤RSAǩ��
int UnionVerifySign(char *teller, char *pk, char *localPasswd, int lenOfReqStr, char *reqStr)
{
	int	ret;
	int	algorithmID = 0;
	char	password[48+1];
	char	sign[2048+1];
	char	data[2048+1];
	char	tmpBuf[128+1];

	memset( password, 0, sizeof password );
	memset( sign, 0, sizeof sign );
	memset( data, 0, sizeof data );

	// ��ȡ���󴮵�����
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"password", password,sizeof(password));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifySign:: UnionReadRecFldFromRecStr passwd err! reqStr=[%s]!\n",reqStr);
		return(ret);
	}

	// ��ȡǩ��
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"sign", sign,sizeof(sign));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifySign:: UnionReadRecFldFromRecStr sign err! reqStr=[%s]!\n",reqStr);
		return(errCodeOnlineKeyMngMDL_GetSignFail);
	}

	// ��ȡǩ���㷨
	memset(tmpBuf,0,sizeof(tmpBuf));
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"algorithmID", tmpBuf,sizeof(tmpBuf));
	if (ret < 0)
		algorithmID = 0;	// RSA
	else
		algorithmID = atoi(tmpBuf);	

	sprintf(data, "%s%s", teller,password);
	// ��֤ǩ��
	ret = UnionVerifyOperatorSign(algorithmID,pk, strlen(pk), data, strlen(data),sign, strlen(sign));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifySign:: UnionVerifyOperatorSign err!\n");
		return(errCodeOnlineKeyMngMDL_GetSignFail);
	}

	return 0;
}

// ��֤����Ա��ǩ��
int UnionVerifyOperatorSign(int algorithmID,unsigned char *pk, int lenOfPk, unsigned char *signData,int lenOfSignData, unsigned char *sign, int signLength)
{
	int	ret;
	char    mac[16+1];
	char	errCode[2+1];
	unsigned char v_sign[1024+1];
	unsigned char v_data[4096+1];
	char    authData[] = "12345678";
	int     isNullPK = 0;
	char	tmpBuf[1024+1];
	char	bcdPK[2048+1];

	memset(mac, 0, sizeof mac);
	memset(errCode, 0, sizeof errCode);
	memset(v_sign, 0, sizeof v_sign);
	memset(v_data, 0, sizeof v_data);
	memset(tmpBuf, 0, sizeof tmpBuf);

	switch (lenOfPk)
	{
		case    64:
		case    128:
		case    256:
		case    512:
		case    1024:
		case    2048:
			isNullPK = 1;
			break;
		default:
			isNullPK = 0;
			break;
	}

	memset(v_sign,0,sizeof(v_sign));
	aschex_to_bcdhex(sign,signLength,v_sign);

	UnionProgramerLog("in UnionVerifyOperatorSign:: signData=[%d][%s]\n", lenOfSignData, signData);
			
	// ����ժҪ
	if (algorithmID == 0)	// RSA
	{
		UnionSHA1(signData, lenOfSignData, v_data);
		UnionProgramerMemLog("in UnionVerifyOperatorSign:: Data=\n", v_data, 20);
	}
	else			// �����㷨
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		bcdhex_to_aschex(signData,lenOfSignData,tmpBuf);
		if ((ret = RacalCmdM7(-1,UnionFindDefaultKMCSJL06(),"3",strlen(tmpBuf),tmpBuf,v_data,sizeof(v_data),errCode)) < 0)
		{
			UnionUserErrLog("in UnionVerifyOperatorSign:: RacalCmdM7 error!ret[%d]\n",ret);
			return(ret);
		}
		if ( strncmp(errCode,"00",2) )
		{
			UnionUserErrLog("in UnionVerifyOperatorSign:: RacalCmdM7 error!errCode[%s]\n",errCode);
			return (UnionFindSoftErrCodeOfCliErrCode(errCode));
		}
		UnionLog("in UnionVerifyOperatorSign:: v_data[%s]\n",v_data);	
	}

#ifndef _usingEsscService_
	if (algorithmID == 0)	// RSA
	{
		ret = SJL06Cmd38(-1, UnionFindDefaultKMCSJL06(), '1', "", signLength/2, v_sign, 20,
			v_data, pk, errCode);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionVerifyOperatorSign: RacalCmd38 error!ret[%d]\n",ret);
			return ret;
		}
		if ( strncmp(errCode,"00",2) )
		{
			UnionUserErrLog("in UnionVerifyOperatorSign: RacalCmd38 error!errCode[%s]\n",errCode);
			return (UnionFindSoftErrCodeOfCliErrCode(errCode));
		}
	}
	else
	{
		if ((ret = RacalCmdK4(-1,UnionFindDefaultKMCSJL06(),-1,"01",strlen(pk),pk,strlen(sign),sign,strlen(v_data),v_data,errCode)) < 0)
		{
			UnionUserErrLog("in UnionVerifyOperatorSign: RacalCmdK4 error!ret[%d]\n",ret);
			return ret;
		}
		if ( strncmp(errCode,"00",2) )
		{
			UnionUserErrLog("in UnionVerifyOperatorSign: RacalCmdK4 error!errCode[%s]\n",errCode);
			return (UnionFindSoftErrCodeOfCliErrCode(errCode));
		}
	}
#else	// ʹ��ƽ̨���񣬽����ڴ���չ����

	memset(bcdPK,0,sizeof(bcdPK));
	aschex_to_bcdhex(pk,lenOfPk,bcdPK);
	if ((ret = UnionVerifySignatureUsingInputBCDPK(lenOfPk/2,bcdPK,"1",NULL,20,v_data,sign,signLength)) < 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorSign: UnionVerifySignatureUsingInputBCDPK ret=[%d]\n",ret);
		return ret;
	}
#endif
	return  0;
}
#endif

/*
���ܣ�
	�ж�һ����Ա�Ƿ��¼
�������:
	idOfOperator �û�����ģ�����Ψһʶ��һ����¼������
�������:
	��
����ֵ
	>=0		�ɹ������ؼ�¼�Ĵ�С
	<0		ʧ��,������
*/
int UnionIsTellerStillLogon(char *idOfOperator)
{
	TUnionTeller	rec;
	int		ret;

	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadTellerRec(idOfOperator,&rec)) < 0)
	{
		UnionUserErrLog("in UnionIsTellerStillLogon:: UnionReadTellerRec [%s]\n",idOfOperator);
		return(ret);
	}
	if (!UnionIsBackuperMngSvr() && !UnionIsBackMngSvr())
	{
		if ((!rec.isCurrentLogon) && (rec.level != conOperatorTypeRunningSystem))
		{
			UnionUserErrLog("in UnionIsTellerStillLogon:: [%s][%s] not logon. isCurrentLogon = [%d]\n",idOfOperator,rec.name,rec.isCurrentLogon);
			return(errCodeOperatorMDL_NotLogon);
		}
	}
	UnionSetTellerInfo(idOfOperator,rec.name,rec.level+'0');
	UnionProgramerLog("in UnionIsTellerStillLogon:: tellerLevel = [%d] name = [%s] id = [%s]\n",rec.level,rec.name,rec.idOfOperator);
	return(1);
}

// �ж������Ƿ����
// ���룺passwdMTime - ��������޸�����
// ���أ�>=0 - ��ʣ�µ�������<0 - �ѹ���
int UnionIsPasswdOverTime(int level, char *passwdMTime)
{
	int	maxEffictiveDays;  // ����������Ч������
	int	usedDays;	// �����ʹ������
	char	remark[256+1];

	memset(remark, 0, sizeof remark);

/***
	if (level == conOperatorTypeSystemDesigner)
		return 36500;
***/

	if ( (maxEffictiveDays = UnionReadIntTypeRECVar("effictiveDaysOfPwd"))<=0)
		maxEffictiveDays = 30;  // Ĭ��30��

	UnionInitLogonRemark();
	UnionRTrim(passwdMTime);
	if (strlen(passwdMTime) == 0)
	{
		strcpy(remark, "ʹ�õ��ǳ�ʼ���룬���޸�����");
		UnionSetLogonRemark(remark);
		return 0;
	}

	// ȡ������ʹ�õ�����
	usedDays = UnionDecideDaysBeforeToday(passwdMTime);
	if (usedDays < 0)
	{
		UnionUserErrLog("in UnionIsPasswdOverTime:: UnionDecideDaysBeforeToday [%s] \n", passwdMTime);
		return(usedDays);
	}

	if ((maxEffictiveDays-usedDays)<=3 && (maxEffictiveDays-usedDays)>0)
	{
		sprintf(remark, "%s%d%s", "�û����뻹��", (maxEffictiveDays-usedDays), "����ڣ��뼰ʱ�޸�����");
		UnionSetLogonRemark(remark);
	}
	return (maxEffictiveDays-usedDays);
}

// ���²���Ա�ĵ�½�����������½ʱ��
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionUpdateOperationLogTimes(char *tellerNo)
{
	int	ret;
	TUnionTeller  operator;
	char	primaryKey[128+1];
	char	record[256+1];

	memset(&operator, 0, sizeof operator);

	// ��ȡ����Ա��¼
	ret = UnionReadOperatorRec(tellerNo,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateOperationLogTimes:: UnionReadOperatorRec teller=[%s]\n",tellerNo);
		return(ret);
	}

	/***modify by xusj 20100819 begin ***/
	operator.logonTimes++;
	strcpy(operator.lastLogonTime, UnionGetCurrentFullSystemDateTime());
	strcpy(operator.lastOperationTime, UnionGetCurrentFullSystemDateTime());
	/***
	ret = UnionUpdateOperatorRec(&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateOperationLogTimes:: UnionUpdateOperatorRec teller=[%s]\n",tellerNo);
		return(ret);
	}
	***/
	memset(primaryKey, 0, sizeof primaryKey);
	memset(record, 0, sizeof record);
	sprintf(primaryKey, "%s=%s|","idOfOperator", tellerNo);
	sprintf(record, "%s=%d|%s=%s|%s=%s|","logonTimes", operator.logonTimes, "lastLogonTime", operator.lastLogonTime, "lastOperationTime", operator.lastOperationTime);
	ret = UnionUpdateUniqueObjectRecord("operator", primaryKey, record, strlen(record));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionUpdateOperationLogTimes:: UnionUpdateUniqueObjectRecord table=[%s], primaryKey=[%s], record=[%s]\n","operator",primaryKey,record);
		return(ret);
	}
	/***modify by xusj 20100819 end ***/

	return 0;
}

// ��֤����Ա�Ŀ���ʱ���Ƿ���
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionVerifyOperatorFreeTime(char *tellerNo)
{
	int	ret;
	int	maxFreeTimes;  // ������ʱ��
	//int	freeTimes;      // �ѿ��е�ʱ��
	long	freeTimes;      // �ѿ��е�ʱ��
	TUnionTeller  operator;
	char	primaryKey[128+1];
	char	record[256+1];
	memset(&operator, 0, sizeof operator);

	if ( (maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfOperate"))<=0)
		maxFreeTimes = 1800;  // Ĭ��1800��

	// ��ȡ����Ա��¼
	ret = UnionReadOperatorRec(tellerNo,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorFreeTime:: UnionReadOperatorRec teller=[%s]\n",tellerNo);
		return(ret);
	}

	if (operator.level == conOperatorTypeRunningSystem)
		return 0;

	UnionLog("in UnionVerifyOperatorFreeTime::operator.lastOperationTime=[%s].\n",operator.lastOperationTime);
	// ȡ�ϴβ�����Ŀǰ������
	// modified by Fenglk 2011-12-12
	//UnionCalcuSecondsPassedAfterSpecTime(operator.lastOperationTime, (long *)&freeTimes);
	UnionCalcuSecondsPassedAfterSpecTime(operator.lastOperationTime, &freeTimes);
	if ( (freeTimes-maxFreeTimes) > 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorFreeTime:: your free time is too long, please login again!!\n");
		strcpy(operator.lastOperationTime, UnionGetCurrentFullSystemDateTime());
		operator.isCurrentLogon = 0;
		UnionUpdateOperatorRec(&operator);
		return(errCodeOperatorMDL_OperatorOverMaxFreeTime);
	}

	/***modify by xusj 20100819 begin ***/
	/***
	strcpy(operator.lastOperationTime, UnionGetCurrentFullSystemDateTime());
	ret = UnionUpdateOperatorRec(&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorFreeTime:: UnionUpdateOperatorRec teller=[%s]\n",tellerNo);
		return(ret);
	}
	***/
	memset(primaryKey, 0, sizeof primaryKey);
	memset(record, 0, sizeof record);
	sprintf(primaryKey, "%s=%s|","idOfOperator", tellerNo);
	sprintf(record, "%s=%s|","lastOperationTime", UnionGetCurrentFullSystemDateTime());
	ret = UnionUpdateUniqueObjectRecord("operator", primaryKey, record, strlen(record));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyOperatorFreeTime:: UnionUpdateUniqueObjectRecord table=[%s], primaryKey=[%s], record=[%s]\n","operator",primaryKey,record);
		return(ret);
	}
	/***modify by xusj 20100819 end ***/
	
	return 0;
}

// ��֤�Ϸ��ͻ���
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionVerifyLegitimateClient(char *tellerNo)
{
	int	ret;
	int	i,cliNum;
	int	port;
	int	legPort;
	char	ipAddr[15+1];
	char	legIpAddr[15+1];
	char	clientID[20][128+1];
	TUnionTeller  operator;

	memset(&operator, 0, sizeof operator);
	memset(ipAddr, 0, sizeof ipAddr);
	memset(legIpAddr, 0, sizeof legIpAddr);
	memset(clientID, 0, sizeof clientID);

	// ��ȡ����Ա��¼
	ret = UnionReadOperatorRec(tellerNo,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyLegitimateClient:: UnionReadOperatorRec teller=[%s]\n",tellerNo);
		if (ret == -3005)
			return(errCodeOperatorMDL_OperatorNotExists);
		else
			return(ret);
	}

	UnionRTrim(operator.legalClients);
	if (strlen(operator.legalClients) == 0)
		return 0;

	cliNum = UnionSeprateVarStrIntoVarGrp(operator.legalClients, strlen(operator.legalClients), ',', clientID, 20);
	if (cliNum <= 0)
	{
		UnionUserErrLog("in UnionVerifyLegitimateClient:: UnionSeprateVarStrIntoVarGrp teller=[%s]\n",tellerNo);
		return(cliNum);
	}

	port = UnionReadResMngClientPort();
	strcpy(ipAddr,UnionReadResMngClientIPAddr());
	for(i=0; i<cliNum; i++)
	{
		ret = UnionReadLegalClientRecFld(clientID[i], "clientIPAddr", legIpAddr, sizeof(legIpAddr));
		if (ret < 0)
			continue;
		ret = UnionReadLegalClientRecIntTypeFld(clientID[i], "port", &legPort);
		if (ret < 0)
			continue;

		if (strcmp(legIpAddr, ipAddr)==0 && legPort == port)
			return 0;
		memset(legIpAddr, 0, sizeof legIpAddr);
	}
	return errCodeOperatorMDL_NotLegitimateClient;
}

// �жϲ���Ա�Ƿ���
// 1-������0-û�б���
int UnionIsOperatorLocked(char *tellerNo)
{
	int	lockFlag=0;

	// �ж���״̬
	if (UnionReadOperatorRecIntTypeFld(tellerNo, "lockStatus", &lockFlag)>=0)
	{
		if (lockFlag == 1)
			return 1;
	}

	return 0;
}

// ���ò���Ա����������+1
int UnionSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	int	passwordInputWrongTimes;
	int	maxTimes;
	char	lastLogonTime[16];

	// ȡ����������
	if ( UnionReadOperatorRecIntTypeFld(tellerNo, "passwordInputWrongTimes", &passwordInputWrongTimes) >= 0 )
	{
		if (passwordInputWrongTimes < 0)
			passwordInputWrongTimes = 0;

		passwordInputWrongTimes++;
		UnionUpdateIntTypeSpecFldOfOperatorRec(tellerNo, "passwordInputWrongTimes", passwordInputWrongTimes);

		if ( (maxTimes = UnionReadIntTypeRECVar("maxPasswordInputWrongTimes")) <= 0 )
			maxTimes = 3;
		if ( passwordInputWrongTimes >= maxTimes)
		{
			UnionUpdateIntTypeSpecFldOfOperatorRec(tellerNo, "lockStatus", 1);
			//20140106 tanhj add
			memset(lastLogonTime, 0, sizeof(lastLogonTime));
			strcpy(lastLogonTime, UnionGetCurrentFullSystemDateTime());
			UnionUpdateSpecFldOfOperatorRec(tellerNo,"lastLogonTime",lastLogonTime,strlen(lastLogonTime));
                }
	}

	return 0;
}

// ���ò���Ա����������
int UnionReSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	int	passwordInputWrongTimes;

	// ȡ����������
	if ( UnionReadOperatorRecIntTypeFld(tellerNo, "passwordInputWrongTimes", &passwordInputWrongTimes) >= 0 )
	{
		if (passwordInputWrongTimes > 0)
			UnionUpdateIntTypeSpecFldOfOperatorRec(tellerNo, "passwordInputWrongTimes", 0);
	}
	return 0;
}

// �ж������ǲ���������, 0-����; 1-��
int UnionIsPasswdTooSimple(char *tellerNo, char *passwd)
{
	int		ret;
	char		plainPwd[48+1];
	char		remark[256+1];

	UnionInitLogonRemark();
	memset(remark, 0, sizeof remark);
	strcpy(remark, "���ĵ�½����̫��,�뼰ʱ�޸�����");

	memset(plainPwd, 0, sizeof plainPwd);
	ret = UnionDecryptPassword(passwd, plainPwd);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionIsPasswdTooSimple:: UnionDecryptPassword passwd=[%s]\n", passwd);
		return(0);
	}

	// ����С��6λ,��Ϊ������
	if (strlen(plainPwd) < 6)
	{
		UnionSetLogonRemark(remark);
		return(1);
	}

	// ���������ԱID��ͬ,��Ϊ������
	if (strcmp(tellerNo, plainPwd) == 6)
	{
		UnionSetLogonRemark(remark);
		return(1);
	}

	// ����ȫ������,��Ϊ������
	if (UnionIsDigitString(plainPwd))
	{
		UnionSetLogonRemark(remark);
		return(1);
	}

	return 0;
}

// ��֤��Ȩ����Ա
int UnionVerifyAuthOperator(int lenOfReqStr, char *reqStr)
{
	int	ret;
	char	authTeller[40+1];
	char	passwd[40+1];
	TUnionTeller  operator;

	char			*ptr = NULL;
	char			fldGrp[30][128+1];
	int			i, fldNum;
	char			authLevelList[128];
	int			isLegalLevel = 0;

	if ((lenOfReqStr < 0) || (reqStr == NULL))
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: lenOfReqStr[%d] < 0 or  reqStr == NULL\n", lenOfReqStr);
		return(errCodeParameter);
	}

	memset(&operator, 0, sizeof operator);

	memset(authTeller, 0, sizeof authTeller);
	ret = UnionReadFldFromCurrentMngSvrClientReqStr("authUserName", authTeller, sizeof(authTeller));
	if (ret < 0)
	{
		return 0;
	}
	else
	{
		UnionDeleteRecFldFromRecStr((char *)UnionGetCurrentMngSvrClientReqStr(), "authUserName");
	}

	/*
	memset(passwd, 0, sizeof passwd);
	ret = UnionReadFldFromCurrentMngSvrClientReqStr("authPassword", passwd, sizeof(passwd));
	if (ret < 0)
	{
		return 0;
	}
	else
	{
		UnionDeleteRecFldFromRecStr((char *)UnionGetCurrentMngSvrClientReqStr(), "authPassword");
	}
	*/
	
	// �ж��Ƿ��ǵ�ǰ����Ա
	if (strcmp(authTeller, (char *)UnionGetCurrentOperationTellerNo()) == 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: ��Ȩ����Ա�������Լ� teller=[%s]\n",authTeller);
		return(errCodeOperatorMDL_AuthOperatorIsNotValid);
	}

	// ��ȡ����Ա��¼
	ret = UnionReadOperatorRec(authTeller,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: UnionReadOperatorRec teller=[%s]\n",authTeller);
		return(ret);
	}

	/*
	// �Ƚ�����
	if (strcmp(operator.passwordCiper, passwd) != 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: localPassword [%s] != remotePassword [%s]\n",operator.passwordCiper,passwd);
		return(errCodeOperatorMDL_WrongPassword);
	}
	*/
	
	if ((ret = UnionVerifyOperatorLogin(authTeller, lenOfReqStr, reqStr)) < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: authTeller [%s]\n",authTeller);
		return(ret);
	}

	// added by 2012-06-26
	// �ж���Ȩ����
	memset(authLevelList, 0, sizeof(authLevelList));
	if((ptr = UnionReadStringTypeRECVar("operationAuthLevelList")) == NULL)
	{
		strcpy(authLevelList, "0,1,2,3,4,5,6,7,8,9");
	}
	else
	{
		strncpy(authLevelList, ptr, sizeof(authLevelList) - 1);
	}
	UnionProgramerLog("in UnionVerifyAuthOperator::�! authLevelList = [%s] author level = [%d]!!\n", authLevelList, operator.level);
	
	memset(fldGrp, 0, sizeof(fldGrp));
	if((fldNum = UnionSeprateVarStrIntoVarGrp(authLevelList, strlen(authLevelList), ',', fldGrp, 30)) < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: UnionSeprateVarStrIntoVarGrp fldList %d\n", fldNum);
		return(ret);
	}
	for(i = 0; i < fldNum; i++)
	{
		if(fldGrp[i][0] - '0' == operator.level)
		{
			isLegalLevel = 1;
			break;
		}
		
	}
	if(isLegalLevel == 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: ��Ȩ����Ա���𲻺Ϸ�! authLevelList = [%s] author level = [%d]!!\n", authLevelList, operator.level);
		return(errCodeOperatorMDL_AuthOperatorIsNotValid);
	}
	// end of addition 2012-06-26

	//return(0);
	return(1);
}

int UnionVerifyAuthLoginOperator(int lenOfReqStr, char *reqStr,int sizeOfResStr,char *resStr)
{
	int	ret;
	char	authTeller[40+1];
	char	passwd[40+1];
	TUnionTeller  operator;

	char			*ptr = NULL;
	char			fldGrp[30][128+1];
	int			i, fldNum;
	char			authLevelList[128];
	int			isLegalLevel = 0;

	if ((lenOfReqStr < 0) || (reqStr == NULL))
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: lenOfReqStr[%d] < 0 or  reqStr == NULL\n", lenOfReqStr);
		return(errCodeParameter);
	}

	memset(&operator, 0, sizeof operator);

	memset(authTeller, 0, sizeof authTeller);
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"authUserName", authTeller, sizeof(authTeller));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator :: UnionReadFldFromCurrentMngSvrClientReqStr authUserName Err! ret = [%d]",ret);
		return ret;
	}
	
	if (strcmp(authTeller, (char *)UnionGetCurrentOperationTellerNo()) == 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: ��������2������?��2??����?��??o teller=[%s]\n",authTeller);
		return(errCodeOperatorMDL_AuthOperatorIsNotValid);
	}

	ret = UnionReadOperatorRec(authTeller,&operator);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: UnionReadOperatorRec teller=[%s]\n",authTeller);
		return(ret);
	}

	memset(passwd,0,sizeof passwd);
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"password", passwd, sizeof(passwd));
	if(ret < 0)
		{
			UnionUserErrLog("in UnionVerifyAuthOperator :: UnionReadRecFldFromRecStr passwd Err ret = [%d]\n",ret);
			return ret;
		}
	if (strcmp(operator.passwordCiper, passwd) != 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: localPassword [%s] != remotePassword [%s]\n",operator.passwordCiper,passwd);
		return(errCodeOperatorMDL_WrongPassword);
	}
	
	// added by 2012-06-26
	memset(authLevelList, 0, sizeof(authLevelList));
	if((ptr = UnionReadStringTypeRECVar("operationAuthLevelList")) == NULL)
	{
		strcpy(authLevelList, "0,1,2,3,4,5,6,7,8,9");
	}
	else
	{
		strncpy(authLevelList, ptr, sizeof(authLevelList) - 1);
	}
	UnionProgramerLog("in UnionVerifyAuthOperator:: authLevelList = [%s] author level = [%d]!!\n", authLevelList, operator.level);
	
	memset(fldGrp, 0, sizeof(fldGrp));
	if((fldNum = UnionSeprateVarStrIntoVarGrp(authLevelList, strlen(authLevelList), ',', fldGrp, 30)) < 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator:: UnionSeprateVarStrIntoVarGrp fldList %d\n", fldNum);
		return(ret);
	}
	for(i = 0; i < fldNum; i++)
	{
		if(fldGrp[i][0] - '0' == operator.level)
		{
			isLegalLevel = 1;
			break;
		}
		
	}
	if(isLegalLevel == 0)
	{
		UnionUserErrLog("in UnionVerifyAuthOperator::  ��Ȩ����Ա���𲻺Ϸ�! authLevelList = [%s] author level = [%d]!!\n", authLevelList, operator.level);
		return(errCodeOperatorMDL_AuthOperatorIsNotValid);
	}
	memset(resStr,0,sizeof resStr);
	
	// end of addition 2012-06-26

	//return(0);
	return(1);
}

/*
int UnionVerifyStaticPasswdEncByPK(char *teller, char *pk, char *localPasswd, int lenOfReqStr, char *reqStr)
{
	int	ret;
	int	algorithmID = 0;
	int	firstFlag = 1;
	char	password[48+1];
	char	sign[2048+1];
	char	data[2048+1];
	char	lastLogonTime[16+1];
	char	tmpBuf[128+1];

	memset( password, 0, sizeof password );
	memset( sign, 0, sizeof sign );
	memset(lastLogonTime,0,sizeof(lastLogonTime));

	// ��ȡ���󴮵�����
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"password", password,sizeof(password));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdEncByPK:: UnionReadRecFldFromRecStr passwd err! reqStr=[%s]!\n",reqStr);
		return(ret);
	}

	// ��ȡǩ��
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"sign", sign,sizeof(sign));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdEncByPK:: UnionReadRecFldFromRecStr sign err! reqStr=[%s]!\n",reqStr);
		return(errCodeOnlineKeyMngMDL_GetSignFail);
	}

	// ��֤����
	ret = UnionVerifyStaticPasswd(localPasswd, lenOfReqStr, reqStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdEncByPK:: UnionVerifyStaticPasswd err!\n");
		return(ret);
	}
	
	// ��ȡǩ���㷨
	memset(tmpBuf,0,sizeof(tmpBuf));
	ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"algorithmID", tmpBuf,sizeof(tmpBuf));
	if (ret < 0)
		algorithmID = 0;	// RSA
	else
		algorithmID = atoi(tmpBuf);	

	// 2012-7-5 ǩ�����������ϴε�¼ʱ��
	memset(lastLogonTime,0,sizeof(lastLogonTime));
	if ((ret = UnionReadOperatorRecFld(teller,"lastLogonTime",lastLogonTime,sizeof(lastLogonTime))) < 0)
	{
		UnionUserErrLog("in UnionVerifyStaticPasswdEncByPK:: UnionReadOperatorRecFld lastLogonTime!\n");
		return(ret);
	}
	//sprintf(data, "%s%s", teller,password);
	sprintf(data, "%s%s%s", teller,password,lastLogonTime);
	// ��֤ǩ��
	ret = UnionDecUserNameAndPasswd(sign,teller,localPasswd,pk);
	if (ret < 0)
	{
		UnionLog("in UnionVerifyStaticPasswdEncByPK:: UnionDecUserNameAndPasswd[%s][%s][%s][%s]!\n",sign,teller,localPasswd,pk);
		return ret;
	}
	return 0;
}
*/

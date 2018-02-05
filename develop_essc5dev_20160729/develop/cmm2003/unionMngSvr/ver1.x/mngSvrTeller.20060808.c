//	Wolfgang Wang, 2006/08/08

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#ifdef _WIN32
#include "unionSvrConf.h"
#include "unionNotice.h"
#endif
#include "mngSvrTeller.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "unionTeller.h"
#include "unionOperatorType.h"
#include "unionDataTBLList.h"

char    gunionTellerNo[48+1] = "88888888";
char    gunionTellerName[40+1] = "";
char    gunionTellerLevel = '4';
int     gunionIsTellerLogonOK = 0;

/*
���ܣ�
	�жϵ�ǰ����Ա�Ƿ��ǿ��ѿ�����Ա
�������:
	��
�������:
	��
����ֵ
	1		��
	0		��
*/
int UnionCurrentTellerIsUnionDeveloper()
{
	if (gunionTellerLevel == '9')
		return(1);
	else
		return(0);
}

/*
���ܣ�
	��ȡ����Ա�����ʶ
�������:
	level		����Ա����
	sizeOfBuf	��ֵ�����С
�������:
	operatorTypeID	����Ա�����ʶ
����ֵ
	>=0		�ɹ�
	<0		ʧ��,������
*/
int UnionReadOperatorTypemacroValueByLevel(int level,char *operatorTypeID,int sizeOfBuf)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[8192+1];
	char	condition[1024+1];

	//�Ӽ�¼ƴװ��
	// ƴ�ؼ���
	memset(condition,0,sizeof(condition));
	ret = UnionPutIntTypeRecFldIntoRecStr(conOperatorTypeFldNameMacroValue,level,condition+lenOfRecStr,sizeof(condition)-lenOfRecStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionPutIntTypeRecFldIntoRecStr [%s] failure!\n",conOperatorTypeFldNameMacroValue);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	lenOfRecStr += ret;

	//��ȡ��¼
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByUniqueKey(conTBLNameOperatorType,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionSelectUniqueObjectRecordByUniqueKey! [%s]\n",condition);
		return(UnionSetUserDefinedErrorCode(lenOfRecStr));
	}

	//��ָ����
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperatorTypeFldNameID,operatorTypeID,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadOperatorTypemacroValueByLevel:: UnionReadRecFldFromRecStr [%s]!\n",conOperatorTypeFldNameID);
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(ret);
}

int UnionGetTellerLevelTypeID(char *tellerNo,char *tellerTypeID)
{
	int	ret;
	int	level;
	
	// ������Ա����
	if ((ret = UnionReadTellerRecIntTypeFld(tellerNo,conTellerFldNameLevel,&level)) < 0)
	{
		UnionUserErrLog("in UnionGetTellerLevelTypeID:: UnionReadTellerRecIntTypeFld [%s]!\n",tellerNo);
		return(ret);
	}
	if (UnionReadIntTypeRECVar("isVisualControlUseTellerLevel") >= 1)
	{
		gunionTellerLevel = level % 10 + '0';	// 2010/1/26,Wolfgang Wang
		tellerTypeID[0] = gunionTellerLevel;
		tellerTypeID[1] = 0;
		return(0);	// 2010/10/30,���������Ӵ˾䣬ԭ���ڴ˴���һ��BUG
	}
	else
		return(UnionReadOperatorTypemacroValueByLevel(level,tellerTypeID,40+1));
}

int UnionGetTellerLevelName(char *tellerNo,char *tellerTypeID)
{
	int	ret;
	int	level;
	
	// ������Ա����
	if ((ret = UnionReadTellerRecIntTypeFld(tellerNo,conTellerFldNameLevel,&level)) < 0)
	{
		UnionUserErrLog("in UnionGetTellerLevelTypeID:: UnionReadTellerRecIntTypeFld [%s]!\n",tellerNo);
		return(ret);
	}
	return(UnionReadOperatorTypemacroValueByLevel(level,tellerTypeID,40+1));
}

// ��ü���
char UnionGetTellerLevel()
{
        return(gunionTellerLevel);
}

// ��õ�¼״̬
int UnionIsCurrentTellerLogonOK()
{
        return(gunionIsTellerLogonOK);
}

// ���õ�ǰ��Ա��¼�ɹ�
void UnionSetCurrentTellerLogonOK()
{
        gunionIsTellerLogonOK = 1;
        return;
}
/*
����	��õ�ǰ��Ա��
�������
	��
�������
	��
����ֵ
	ָ���ԱԱ��ָ��
*/
char *UnionGetTellerNo()
{
	return(gunionTellerNo);
}

/*
����	���õ�ǰ��Ա��Ϣ
�������
	tellerNo	��ǰ��Ա��
	tellerName	��Ա��
	level           ����
�������
	��
����ֵ
	>=0	��ȷ
	<0	ʧ��
*/
void UnionSetTellerInfo(char *tellerNo,char *tellerName,char level)
{
        if (tellerNo != NULL)
        	strcpy(gunionTellerNo,tellerNo);
        if (tellerName != NULL)
        	strcpy(gunionTellerName,tellerName);
        gunionTellerLevel = level;
	return;
}

#ifdef _WIN32
/*
����	��Ա��¼
�������
	id	��Ա��
	passwd	��Ա����
�������
	��
����ֵ
	true	�ɹ�
        false   ʧ��
*/
bool UnionOperatorLogon(char *id,char *passwd)
{
	char	tmpBuf[512+1];
	int	len;
	int	fileRecved;
	char	*fileName;
        char    level[100];
        int     ret;

	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionEncryptPassword(passwd,tmpBuf);
	if ((len = UnionCommunicationWithSpecMngSvr(UnionGetIPAddrOfCurrentSvr(),UnionGetPortOfCurrentSvr(),id,conSpecTBLIDOfOperator,conDatabaseSpecCmdOfOperatorTBL_logon,
						tmpBuf,strlen(tmpBuf),
						tmpBuf,sizeof(tmpBuf),
						&fileRecved)) < 0)
	{
		UnionErrorNotice("����Ա[%s]��¼����,������[%d]ԭ��[%s]",id,len,tmpBuf);
		return(false);
	}
        tmpBuf[len] = 0;
        if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"level",level,sizeof(level))) < 0)
        {
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"userType",level,sizeof(level))) < 0)
                {
                        UnionUserErrLog("in UnionOperationOfOperatorTable:: UnionReadRecFldFromRecStr level from [%s] errCode = [%d]\n",tmpBuf,ret);
                        return(ret);
                }
        }
	UnionSetTellerInfo(id,NULL,level[0]);
	/*
	if (!fileRecved)
	{
		UnionErrorNotice("���ز���ԱȨ�ޱ�ʧ��!!");
		UnionOperatorLogoff();
		return(false);
	}
	if ((fileName = UnionGetCurrentMngSvrTempFileName()) != NULL)
		RenameFile(fileName,UnionOperationControlFileName);
	*/
	return(true);
}
//---------------------------------------------------------------------------
// �˳���¼
void UnionOperatorLogoff()
{
	char	tmpBuf[128+1];
	int	fileRecved = 0;
	int	len=0;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((len = UnionCommunicationWithSpecMngSvr(UnionGetIPAddrOfCurrentSvr(),UnionGetPortOfCurrentSvr(),UnionGetTellerNo(),conSpecTBLIDOfOperator,conDatabaseSpecCmdOfOperatorTBL_logoff,
		"",0,tmpBuf,sizeof(tmpBuf),&fileRecved)) < 0)
	{
		UnionErrorNotice("����Ա[%s]�˳���¼����,������[%d]ԭ��[%s]",UnionGetTellerNo(),len,tmpBuf);
		return;
	}
	//DeleteFile(UnionOperationControlFileName);
	return;
}

#endif

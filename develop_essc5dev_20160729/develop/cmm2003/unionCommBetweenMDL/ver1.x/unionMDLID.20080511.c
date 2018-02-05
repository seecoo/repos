// Wolfgang Wang
// 2008/9/27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "unionMDLID.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#include <process.h>
#endif

TUnionMDLType	gunionMyModuleType = conMDLTypeDefault;
TUnionModuleID	gunionCurrentDynamicModuleID = -1;
long		gunionMyselfSSN = 1;

// ��ȡָ��������Ĵ���ģ��ID��
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsm(char *hsmIPAddr)
{
	if (hsmIPAddr == NULL)
		return(0);
	else
		return(labs(inet_addr(hsmIPAddr)));
}

// ��ȡָ�������������Ĵ���ģ��ID��
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsmGrp(char *hsmGrpID)
{
	int	i;
	char	hsmID[8+1];
	char	tmpBuf[128+1];
	
	if (hsmGrpID == NULL)
		return(1);
	
	if (UnionIsDigitString(hsmGrpID))	
		return(labs(atol(hsmGrpID)));
	//else if (strcmp(hsmGrpID,"default") == 0)
	//	return(conMDLTypeUnionHsmSvr);
	else
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,hsmGrpID);
		UnionToUpperCase(tmpBuf);
		memset(hsmID,0,sizeof(hsmID));
		//hsmID = conMDLTypeUnionHsmSvr * 1000;
		for (i = 0; i < strlen(tmpBuf); i++)
		{
			if (i == 8)
				break;
			if (UnionIsDigitChar(tmpBuf[i]))
				hsmID[i] = tmpBuf[i];
			else
				hsmID[i] = (tmpBuf[i] - 'A') % 10 + '0';
		}
		return(labs(atol(hsmID)));
	}
}

int UnionGetHsmGrpIDOutOfPort(int port,char *hsmGrpID)
{
	sprintf(hsmGrpID,"%03d",port % 1000);
	return(0);
}

// ����ģ������
int UnionSetMyModuleType(TUnionMDLType myMDLType)
{
	if (myMDLType <= 0)
	{
		UnionUserErrLog("in UnionSetMyModuleType:: myMDLType [%d]\n",myMDLType);
		return(errCodeEsscMDL_InvalidMDLID);
	}
	gunionMyModuleType = myMDLType;
	return(0);
}

// ��ȡģ������
TUnionMDLType UnionGetMyModuleType()
{
	return(gunionMyModuleType);
}

// ��ģ���ʶ���л�ȡģ������
TUnionMDLType UnionGetMDLTypeOutOfMDLID(TUnionModuleID id)
{
	return(id % conMaxNumOfMDL);
}

// ��ģ���ʶ���л�ȡ���̺�
int UnionGetPIDOutOfMDLID(TUnionModuleID id)
{
	// return(id / conMaxNumOfMDL); // 2007/11/15 ɾ��
	return(id); // 2007/11/15 ����	
}

// ��ģ���ʶ���л�ȡ��̬ģ���ʶ��
// 2007/11/15 �޸��������
int UnionGetDynamicIDOutOfMDLID(TUnionModuleID id)
{
	return(id);
} 

// ����ģ��Ķ�̬��ʶ��
TUnionModuleID UnionGenerateMyDynamicMDLID()
{
	return(gunionCurrentDynamicModuleID = getpid());
}

// ��ȡ��ǰ��ģ��Ķ�̬��ʶ��
TUnionModuleID UnionGetMyCurrentDynamicMDLID()
{
	return(gunionCurrentDynamicModuleID);
}

// ��ȡģ��ľ�̬��ʶ��
TUnionModuleID UnionGetMyFixedMDLID()
{
	return(UnionGetMyModuleType());
}

// ��ȡģ��Ĵ����̺ŵľ�̬��ʶ��
TUnionModuleID UnionGetMySpecifiedFixedMDLID()
{
	return(UnionGetMyModuleType() + getpid() * conMaxNumOfMDL);
}

// ��ȡָ�����͵�ģ��ľ�̬��ʶ��
TUnionModuleID UnionGetFixedMDLIDOfMDLType(TUnionMDLType mdlType)
{
	return(mdlType % conMaxNumOfMDL);
}

// ��ȡָ��ģ���ID��
TUnionModuleID UnionGetFixedMDLIDOfSpecSvr(int specID)
{
	return(specID * conMaxNumOfMDL + 1000000000);
}

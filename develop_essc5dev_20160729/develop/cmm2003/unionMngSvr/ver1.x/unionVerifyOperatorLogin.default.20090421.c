#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionLogonMode.h"
#include "unionTeller.h"
//#include "kmcDefaultHsm.h"
#include "sjl06Cmd.h"
#include "UnionStr.h"

char *UnionReadResMngClientIPAddr();

// ��֤����Ա��½
int UnionVerifyOperatorLogin(char *tellerNo, int lenOfReqStr, char *reqStr)
{
	return 0;
}

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
	return(0);
}

// �ж������Ƿ����
// ���룺passwdMTime - ��������޸�����
// ���أ�>=0 - ��ʣ�µ�������<0 - �ѹ���
int UnionIsPasswdOverTime(char *passwdMTime)
{
	return (0);
}

// ���²���Ա�ĵ�½�����������½ʱ��
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionUpdateOperationLogTimes(char *tellerNo)
{
	return 0;
}

// ��֤����Ա�Ŀ���ʱ���Ƿ���
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionVerifyOperatorFreeTime(char *tellerNo)
{
	return 0;
}

// ��֤�Ϸ��ͻ���
// ���룺tellerNo - ����ԱID
// ���أ�=0 - �ɹ���<0 - ʧ��
int UnionVerifyLegitimateClient(char *tellerNo)
{
	return 0;
}

// �жϲ���Ա�Ƿ���
// 1-������0-û�б���
int UnionIsOperatorLocked(char *tellerNo)
{
        return 0;
}

// �ж������ǲ���������, 0-����; 1-��
int UnionIsPasswdTooSimple(char *tellerNo, char *passwd)
{
	return 0;
}

int UnionSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	return 0;
}

int UnionReSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	return 0;
}

int UnionVerifyAuthOperator(int lenOfReqStr, char *reqStr)
{
	return 0;
}

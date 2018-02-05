//	Wolfgang Wang, 2006/08/08

#ifndef _teller_
#define _teller_

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
int UnionReadOperatorTypemacroValueByLevel(int level,char *operatorTypeID,int sizeOfBuf);

int UnionGetTellerLevelTypeID(char *tellerNo,char *tellerTypeID);

// ��õ�¼״̬
int UnionIsCurrentTellerLogonOK();

// ���õ�ǰ��Ա��¼�ɹ�
void UnionSetCurrentTellerLogonOK();

// ��ü���
char UnionGetTellerLevel();

/*
����	��õ�ǰ��Ա��
�������
	��
�������
	��
����ֵ
	ָ���ԱԱ��ָ��
*/
char *UnionGetTellerNo();

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
void UnionSetTellerInfo(char *tellerNo,char *tellerName,char level);

#ifdef _WIN32_
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
bool UnionOperatorLogon(char *id,char *passwd);

// �˳���¼
void UnionOperatorLogoff();
#endif

#endif

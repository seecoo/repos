//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _CommWithHsmSvr_
#define _CommWithHsmSvr_

// ����Գ���Կ����
typedef struct
{
	char	hsmGroupID[8+1];	// �������ID
	char	lmkProtectMode[16+1];	// LMK������ʽ
	int	hsmCmdVersionID;	// �����ָ������
} TUnionHsmGroupRec;
typedef TUnionHsmGroupRec		*PUnionHsmGroupRec;

// ʹ��BCD��չ��ӡHSM����ָ��
void UnionSetBCDPrintTypeForHSMReqCmd();
// ʹ��BCD��չ��ӡHSM��Ӧָ��
void UnionSetBCDPrintTypeForHSMResCmd();
// ʹ��BCD��չ��ӡHSMָ��
void UnionSetBCDPrintTypeForHSMCmd();

// ʹ��***��ӡHSM����ָ�����
void UnionSetMaskPrintTypeForHSMReqCmd();
// ʹ��***��ӡHSM��Ӧָ�����
void UnionSetMaskPrintTypeForHSMResCmd();
// ʹ��***��ӡHSMָ�����
void UnionSetMaskPrintTypeForHSMCmd();

void UnionResetPrintTypeForHSMCmd();
char *UnionGetHsmCmdPrintTypeStr();

// ���ü��ܻ�ָ��ͷ�ĳ���
void UnionSetLenOfHsmCmdHeader(int len);

// ���ò���Ҫ�����ܻ���Ӧ��
void UnionSetIsNotCheckHsmResCode();

// ������Ҫ�����ܻ���Ӧ��
void UnionSetIsCheckHsmResCode();

// ����ʹ�õļ��ܻ���ID
void UnionSetHsmGroupIDForHsmSvr(char *hsmGrpID);

// ��ȡʹ�õļ��ܻ���ID
char *UnionGetHsmGroupIDForHsmSvr();

// ���÷���ָ��IP��ַ�ļ��ܻ�
// ʹ�����������ÿ��ָ��ֻ����Чһ�Σ��¸�ָ��ָ�ʹ�����ڼ��ܻ�
int UnionSetUseSpecHsmIPAddrForOneCmd(char *hsmIPAddr);

// ���÷���ָ����ļ��ܻ�
// ʹ�����������ÿ��ָ��ֻ����Чһ�Σ��¸�ָ��ָ�ʹ��Ĭ�����ڼ��ܻ�
int UnionSetUseSpecHsmGroupForOneCmd(char *hsmGrpID);

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

int UnionGetHsmGroupRecByHsmGroupID(char *hsmGroupID,PUnionHsmGroupRec phsmGroupRec);

#endif

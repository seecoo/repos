//	Wolfgang Wang
//	2004/11/16
//	Version 1.0

#ifndef _UnionSoftHsmKeySpace_
#define _UnionSoftHsmKeySpace_

#define conMDLNameOfSoftHsmKeySpace		"UnionSoftHsmKeySpaceMDL"

typedef struct
{
	char			hsmGrp[3+1];	// ��
	// ���ƣ�
	// BMKXXX ��ʾ ��������BMK������XXX��BMK��������
	// ICVGGII ��ʾ IC���Ŀ�Ƭ����Կ������V��ʾ�汾��GG��ʾ��ţ�II��ʾ������
	// LMKPPPP ��ʾ RACAL�ı�������Կ������PPPP��ʾ��Կ��
	// RSAXX ��ʾRSA˽Կ�������ţ�����XX��ʾ������ 
	char			name[10+1];	// ��Կ����
	char			value[32+1];	// ��Կ����ֵ
} TUnionSoftHsmKey;
typedef TUnionSoftHsmKey	*PUnionSoftHsmKey;

typedef struct
{
	int			maxNum;		// ��Կ���������
	int			realNum;	// ��Կ��ʵ������
	PUnionSoftHsmKey	pkeyTBL;	// ��Կ�б�
} TUnionSoftHsmKeySpace;
typedef TUnionSoftHsmKeySpace		*PUnionSoftHsmKeySpace;

int UnionGetConfFileNameOfSoftHsmKeySpace(char *fileName);
int UnionConnectSoftHsmKeySpace();
int UnionDisconnectSoftHsmKeySpace();
int UnionRemoveSoftHsmKeySpace();

int UnionReloadSoftHsmKeySpace();
int UnionPrintSoftHsmKeyToFile(PUnionSoftHsmKey pkey,FILE *fp);
int UnionPrintSoftHsmKeySpaceOfSpecifiedHsmGrpToFile(char *hsmGrp,FILE *fp);
int UnionPrintSoftHsmKeySpaceToFile(FILE *fp);

int UnionGetMaxKeyNumOfSoftHsmKeySpace();

PUnionSoftHsmKey UnionReadSoftHsmKey(char *hsmGrp,char *name);
char *UnionReadSoftHsmICCardTransKey(char *hsmGrp,char *version,char *group,char *index);
char *UnionReadSoftHsmRacalLMK(char *hsmGrp,char *lmkPair);
char *UnionReadSoftHsmBMK(char *hsmGrp,char *bmkIndex);

int UnionSoftHsmCmdInterprotor(char *hsmGrpID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

#endif 			

//	Wolfgang Wang
//	2004/11/16
//	Version 1.0

#ifndef _UnionSoftHsmKeySpace_
#define _UnionSoftHsmKeySpace_

#define conMDLNameOfSoftHsmKeySpace		"UnionSoftHsmKeySpaceMDL"

typedef struct
{
	char			hsmGrp[3+1];	// 组
	// 名称：
	// BMKXXX 表示 磁条卡的BMK，其中XXX是BMK的索引号
	// ICVGGII 表示 IC卡的卡片主密钥，其中V表示版本，GG表示组号，II表示索引号
	// LMKPPPP 表示 RACAL的本地主密钥，其中PPPP表示密钥对
	// RSAXX 表示RSA私钥的索引号，其中XX表示索引号 
	char			name[10+1];	// 密钥名称
	char			value[32+1];	// 密钥明文值
} TUnionSoftHsmKey;
typedef TUnionSoftHsmKey	*PUnionSoftHsmKey;

typedef struct
{
	int			maxNum;		// 密钥的最大数量
	int			realNum;	// 密钥的实际数量
	PUnionSoftHsmKey	pkeyTBL;	// 密钥列表
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

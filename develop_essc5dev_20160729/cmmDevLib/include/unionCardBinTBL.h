#ifndef _UNIONCARDBIN_
#define _UNIONCARDBIN_

#define conMaxNumOfCardBin 	                 1024
#define conConfFileNameOfCardBinTBL  		"cardBin.Def"
#define conMDLNameOfCardBinTBL 		        "cardBinTBLMDL"

typedef struct
{
	char	cardBin[9+1];
	char	cvkName[40+1];
	char	algorithmOfCVV[2+1];
	char	pvkName[40+1];
	char	algorithmOfPVV[2+1];
	char	remark[40+1];
} TUnionCardBin;
typedef TUnionCardBin *PUnionCardBin;

typedef struct
{
	int	realNum;
	char	defaultCvkName[40+1];
	char	defaultAlOfCVV[2+1];
	char	defaultPvkName[40+1];
	char	defaultAlOfPVV[2+1];
	TUnionCardBin	rec[conMaxNumOfCardBin];
} TUnionCardBinTBL;
typedef TUnionCardBinTBL *PUnionCardBinTBL;

int UnionGetNameOfCardBinTBL(char *fileName);
int UnionIsCardBinTBLConnected();
int UnionGetMaxNumOfCardBin();
int UnionConnectCardBinTBL();
int UnionDisconnectCardBinTBL();
int UnionRemoveCardBinTBL();
int UnionReloadCardBinTBL();
int UnionPrintCardBinTBLToFile(FILE *fp);
int UnionPrintCardBinToFile(PUnionCardBin cardBin,FILE *fp);
int UnionGetCvkByCardNo(char *cardNo, char *cvkName, char *algorithmOfCVV);
int UnionGetPvkByCardNo(char *cardNo, char *pvkName, char *algorithmOfPVV);
int UnionGetPvkCvkByCardNo(char *cardNo, char *cvkName, char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV);
int UnionGetCardInfoFromStack2(char * Stack2, int lenOfStack2, char *cardNo,char *effectiveDates,char *service,char *CVN);
int UnionPrintCardBinTBLToSpecFile(char *fileName);
int UnionExistCardBinDef(char *name);
int UnionInsertCardBin(char *cardBin,char *cvkName,char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV, char *remark);
int UnionDeleteCardBinDef(char *cardBin);
int UnionUpdateCardBinDef(char *cardBin,char *cvkName,char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV, char *remark);


#endif


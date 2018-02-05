// Wolfgang Wang
// 2008/1/17

#ifndef _probility_
#define _probility_

typedef struct tmpDef
{
	long			key;		// ���ʼ������ֵ
	long			echoNum;	// ��ֵ���ֵĴ���
	double			percent;	// ��ֵ���ֵĸ���
	struct tmpDef		*left;		// ��ڵ�
	struct tmpDef		*right;		// �ҽڵ�
	struct tmpDef		*parent;	// ���ڵ�
} TUnionProbilityRec;
typedef TUnionProbilityRec	*PUnionProbilityRec;

typedef struct
{
	PUnionProbilityRec	precList;	// �м��¼
	long			totalEchoNum;	// ����ֵ���ֵĴ���
	long			keyStep;	// ������ֵ֮��Ĳ���
	long			keyNum;		// ��¼��Ŀ
} TUnionProbilityGrp;
typedef TUnionProbilityGrp	*PUnionProbilityGrp;

// ��ʼ��һ���µļ�¼
PUnionProbilityRec UnionNewProbilityREC(long key);

// ����һ��ֵ��Ӧ�ļ�¼
PUnionProbilityRec UnionFindProbilityRec(PUnionProbilityRec precList,long key);

// ���ҷ�ֵ
// �����ҵ��ķ�ֵ
long UnionFindLevelKeyOfProbilityRec(PUnionProbilityRec precList,long minTotalEchoNum,long *currentTotalNum);

// ��һ��ֵ�������
PUnionProbilityRec UnionInsertProbilityRec(PUnionProbilityRec precList,long key,int *isNew);

// ����һ����¼���ֵĸ���
void UnionPercentOfProbilityRec(PUnionProbilityRec precList,long totalEchoNum);

// �ͷ�һ����¼
void UnionDeleteProbilityRec(PUnionProbilityRec precList);

// ��ʾһ����¼
void UnionPrintProbilityRecToFile(PUnionProbilityRec precList,FILE *fp);

// ��һ��ֵ���ӵ����ʱ���
// ����ֵ��
// ���ر��еļ�¼��	
long UnionAddToProbilityGrp(PUnionProbilityGrp pgrp,long key);

// ��ʼ��һ���µĸ��ʱ�
PUnionProbilityGrp UnionNewProbilityGrp();

// ɾ�����ʱ�
void UnionDeleteProbilityGrp(PUnionProbilityGrp pgrp);

// ��ʼͳ�Ƹ���
void UnionPercentAllRecOfProbilityGrp(PUnionProbilityGrp pgrp);

// ��ȡĳ����¼
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key);

// ĳ����¼
PUnionProbilityRec UnionFindRecInProbilityGrp(PUnionProbilityGrp pgrp,long key);

// ��ӡ������ϸ��
void UnionPrintProbilityGrpToFile(PUnionProbilityGrp pgrp,FILE *fp);

// ��ӡ����ͳ�Ʊ�
int UnionGenerateProbilityReport(PUnionProbilityGrp pprobGrp,FILE *outFp);

// ������Сֵ
long UnionFindMinKeyOfProbilityRec(PUnionProbilityRec precList);

// �������ֵ
long UnionFindMaxKeyOfProbilityRec(PUnionProbilityRec precList);

#endif

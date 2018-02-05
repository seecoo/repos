//	Author:		Wolfgang Wang
//	Date:		2002/8/17

// 2003/08/11,Wolfgang Wang, add UnionReadCurrentSerialNo

#ifndef _UnionSerialNo_
#define _UnionSerialNo_

#ifdef _UnionSerailNo_2_x_Above_
typedef struct
{
	long		userIDOfSerialNoMDL;
	char		currentSerialNo[20];
} TUnionSerialNo;
typedef TUnionSerialNo	*PUnionSerialNo;
#endif

typedef struct
{
	int		IDOfSerialNoMDL;
	int		IDOfSharedMemory;
	long		SerialNo;	// ���к�
	short		Locked;		// ������ʶ
	long		LargestSN;	// ������к�
	long		LowestSN;	// ��С���к�
} TUnionSerialNoMDL;
typedef TUnionSerialNoMDL	*PUnionSerialNoMDL;

// Functions definition
PUnionSerialNoMDL UnionConnectSerialNoMDL(int IDOfSerialNoMDL,long LargestSN,long LowestSN);
int UnionDisconnectSerialNoMDL(PUnionSerialNoMDL);
int UnionRemoveSerialNoMDL(int IDOfSerialNoMDL);
int UnionPrintStatusOfSerialNoMDL(PUnionSerialNoMDL);
long UnionApplyNewSerialNo(PUnionSerialNoMDL);
int UnionUnlockSerialNoMDL(PUnionSerialNoMDL);
int UnionLockSerialNoMDL(PUnionSerialNoMDL);

long UnionReadCurrentSerialNo(PUnionSerialNoMDL pSerialNoMDL);	// Added by Wolfgang Wang, 2003/08/11

int UnionGetSerialNoFromMirrorOfSerialNoMDL(PUnionSerialNoMDL);
int UnionRefreshMirrorOfSerialNoMDL(PUnionSerialNoMDL);
// Error Codes

#endif

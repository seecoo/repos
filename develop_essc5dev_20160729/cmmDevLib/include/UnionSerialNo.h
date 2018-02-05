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
	long		SerialNo;	// 序列号
	short		Locked;		// 锁定标识
	long		LargestSN;	// 最大序列号
	long		LowestSN;	// 最小序列号
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

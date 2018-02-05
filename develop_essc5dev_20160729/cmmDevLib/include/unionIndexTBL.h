//	Author:	Wolfgang Wang
//	Date:	2003/11/03

#ifndef _UnionIndexTBL_
#define _UnionIndexTBL_

#define errIndexTBLTimeout		-20

#define	conAvailabeIndex		100
#define conOccupiedIndex		101

typedef struct
{
	long 	status;		// = conAvailabeIndex,表示空闲位置
				// = conOccupiedIndex + 用户赋值的状态
	char	index[4];	// 索引
} TUnionIndexStatus;
typedef TUnionIndexStatus	*PUnionIndexStatus;

typedef struct
{
	int	userID;		// 用户赋值的状态表ID号
	int	maxNumOfIndex;	// 最大的索引数目
} TUnionIndexStatusTBLDef;
typedef TUnionIndexStatusTBLDef	*PUnionIndexStatusTBLDef;

typedef struct
{
	TUnionIndexStatusTBLDef	tblDef;
	int			id;
} TUnionIndexStatusTBL;
typedef TUnionIndexStatusTBL	*PUnionIndexStatusTBL;

PUnionIndexStatusTBL UnionConnectIndexStatusTBL(int userID,int maxNumOfIndex);
int UnionDisconnectIndexStatusTBL(PUnionIndexStatusTBL);
int UnionRemoveIndexStatusTBL(int userID);

// 将index置为可用
int UnionSetIndexAvailable(PUnionIndexStatusTBL,int index);

// 获得可用的位置
// 返回值为获得的位置
int UnionGetAvailableIndexNoWait(PUnionIndexStatusTBL ptbl);
// 获得可用的位置
// 返回值为获得的位置
int UnionGetAvailableIndex(PUnionIndexStatusTBL);

// 将指定索引的状态置为用户指定的状态
int UnionSetIndexWithUserStatus(PUnionIndexStatusTBL,int index,long status);

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatusUntilSuccess(PUnionIndexStatusTBL,long status);

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatusNoWait(PUnionIndexStatusTBL ptbl,long status);

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatus(PUnionIndexStatusTBL,long status);

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatusUntilTimeout(PUnionIndexStatusTBL,long status,int timeout);

// 将状态为指定状态的位置置为可用
int UnionSetIndexOfUserStatusAvailable(PUnionIndexStatusTBL,long status);

// 将所有位置置为可用
int UnionResetAllIndexAvailable(PUnionIndexStatusTBL ptbl);

// 重置最大索引数目
int UnionResetMaxIndexNumOfIndexStatusTBL(PUnionIndexStatusTBL ptbl,int maxNumOfIndex);

// 将状态为指定状态的位置置为可用
int UnionSetIndexOfUserStatusAndIndexAvailable(PUnionIndexStatusTBL,long status,int index);

int UnionSetIndexWithUserStatusNoWait(PUnionIndexStatusTBL ptbl,int index,long status);


#endif

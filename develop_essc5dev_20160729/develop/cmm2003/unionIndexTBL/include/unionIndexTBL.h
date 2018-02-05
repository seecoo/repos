//	Author:	Wolfgang Wang
//	Date:	2003/11/03

#ifndef _UnionIndexTBL_
#define _UnionIndexTBL_

#define errIndexTBLTimeout		-20

#define	conAvailabeIndex		100
#define conOccupiedIndex		101

typedef struct
{
	long 	status;		// = conAvailabeIndex,��ʾ����λ��
				// = conOccupiedIndex + �û���ֵ��״̬
	char	index[4];	// ����
} TUnionIndexStatus;
typedef TUnionIndexStatus	*PUnionIndexStatus;

typedef struct
{
	int	userID;		// �û���ֵ��״̬��ID��
	int	maxNumOfIndex;	// ����������Ŀ
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

// ��index��Ϊ����
int UnionSetIndexAvailable(PUnionIndexStatusTBL,int index);

// ��ÿ��õ�λ��
// ����ֵΪ��õ�λ��
int UnionGetAvailableIndexNoWait(PUnionIndexStatusTBL ptbl);
// ��ÿ��õ�λ��
// ����ֵΪ��õ�λ��
int UnionGetAvailableIndex(PUnionIndexStatusTBL);

// ��ָ��������״̬��Ϊ�û�ָ����״̬
int UnionSetIndexWithUserStatus(PUnionIndexStatusTBL,int index,long status);

// ��õ�һ��״̬Ϊָ��״̬λ�ã����ػ�õ�λ��
int UnionGetFirstIndexOfUserStatusUntilSuccess(PUnionIndexStatusTBL,long status);

// ��õ�һ��״̬Ϊָ��״̬λ�ã����ػ�õ�λ��
int UnionGetFirstIndexOfUserStatusNoWait(PUnionIndexStatusTBL ptbl,long status);

// ��õ�һ��״̬Ϊָ��״̬λ�ã����ػ�õ�λ��
int UnionGetFirstIndexOfUserStatus(PUnionIndexStatusTBL,long status);

// ��õ�һ��״̬Ϊָ��״̬λ�ã����ػ�õ�λ��
int UnionGetFirstIndexOfUserStatusUntilTimeout(PUnionIndexStatusTBL,long status,int timeout);

// ��״̬Ϊָ��״̬��λ����Ϊ����
int UnionSetIndexOfUserStatusAvailable(PUnionIndexStatusTBL,long status);

// ������λ����Ϊ����
int UnionResetAllIndexAvailable(PUnionIndexStatusTBL ptbl);

// �������������Ŀ
int UnionResetMaxIndexNumOfIndexStatusTBL(PUnionIndexStatusTBL ptbl,int maxNumOfIndex);

// ��״̬Ϊָ��״̬��λ����Ϊ����
int UnionSetIndexOfUserStatusAndIndexAvailable(PUnionIndexStatusTBL,long status,int index);

int UnionSetIndexWithUserStatusNoWait(PUnionIndexStatusTBL ptbl,int index,long status);


#endif

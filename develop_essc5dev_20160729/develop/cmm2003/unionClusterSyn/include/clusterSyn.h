#ifndef _ClusterSyn_
#define _ClusterSyn_

#define conConfFileNameOfClusterDef	"unionCluster.Def"

#define defMaxNumOfCluster		10
#define defMaxNumOfSynContent		128

typedef struct
{
	char	tableName[40+1];	// ����
	char	cmd[512+1];		// ����
} TUnionClusterDef;
typedef TUnionClusterDef	*PUnionClusterDef;

typedef struct
{
	int			clusterNo;		// ������Ⱥ���
	int			realNum;
	TUnionClusterDef	rec[defMaxNumOfSynContent];
} TUnionClusterDefTBL;
typedef TUnionClusterDefTBL	*PUnionClusterDefTBL;

typedef struct
{
	int	sn;
	int	clusterNo;
	char	cmd[128+1];
	char	regTime[14+1];
} TUnionClusterSyn;
typedef TUnionClusterSyn	*PUnionClusterSyn;

int UnionGetNameOfClusterDef(char *fileName);

int UnionInitClusterDef(PUnionClusterDefTBL pdef);

int UnionReadClusterSyn(int clusterNo,TUnionClusterSyn clusterSyn[],int maxNum);

int UnionDeleteClusterSynBySN(int sn);

int UnionDeleteClusterSynByClusterNo(int clusterNo);

int UnionInsertClusterSyn(PUnionClusterSyn pclusterSyn);

// ��ȡ�����ļ�Ⱥ���
int UnionGetClusterNoOfLocalHost();

//add by zhouxw 20150911
// ��ȡ������ָ�������ID�Ľ����Ƿ�����
int UnionGetStatusOfHsmID(char  *hsmID);
//add end
// ���ݼ�Ⱥ����ж��Ƿ������������
int UnionIsRunThisTaskByClusterNo();

// �������ݿ��Ĺ����ڴ�
int UnionUpdateShareMemoryForUpdateDB(char *tableName);

#endif

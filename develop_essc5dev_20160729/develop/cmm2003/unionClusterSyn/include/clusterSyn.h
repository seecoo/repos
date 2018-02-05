#ifndef _ClusterSyn_
#define _ClusterSyn_

#define conConfFileNameOfClusterDef	"unionCluster.Def"

#define defMaxNumOfCluster		10
#define defMaxNumOfSynContent		128

typedef struct
{
	char	tableName[40+1];	// 表名
	char	cmd[512+1];		// 命令
} TUnionClusterDef;
typedef TUnionClusterDef	*PUnionClusterDef;

typedef struct
{
	int			clusterNo;		// 本机集群编号
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

// 读取本机的集群编号
int UnionGetClusterNoOfLocalHost();

//add by zhouxw 20150911
// 读取本机下指定密码机ID的进程是否运行
int UnionGetStatusOfHsmID(char  *hsmID);
//add end
// 根据集群编号判断是否允许这个任务
int UnionIsRunThisTaskByClusterNo();

// 更新数据库表的共享内存
int UnionUpdateShareMemoryForUpdateDB(char *tableName);

#endif

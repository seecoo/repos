#ifndef _HighCached_
#define _HighCached_

#define conConfFileNameOfHighCached	"highCached.Def"
#define conMDLNameOfHighCachedTBL	"highCachedTBLMDL"

#define conMaxNumOfHighCached		128

typedef struct
{
	char	ipAddr[32+1];		// IP地址
	int	port;			// 端口
	int	master;			// 主备标识
	int	enabled;		// 激活
//	int	conStats;		//状态  add by lusj 20151027
	char	remark[128+1];		// 备注
} TUnionHighCached;
typedef TUnionHighCached	*PUnionHighCached;
typedef struct
{
	int			realNum;
	TUnionHighCached	rec[conMaxNumOfHighCached];
} TUnionHighCachedTBL;
typedef TUnionHighCachedTBL	*PUnionHighCachedTBL;

int UnionGetNameOfHighCached(char *fileName);

int UnionIsHighCachedConnected();

int UnionConnectHighCached();
	
int UnionDisconnectHighCached();

int UnionRemoveHighCached();

int UnionReloadHighCached();

int UnionPrintHighCachedTBLToFile(FILE *fp);

int UnionPrintHighCachedToFile(PUnionHighCached phighCached,FILE *fp);

PUnionHighCachedTBL UnionGetHighCachedTBL();

#endif

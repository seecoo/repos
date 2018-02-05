#ifndef _HighCached_
#define _HighCached_

#define conConfFileNameOfHighCached	"highCached.Def"
#define conMDLNameOfHighCachedTBL	"highCachedTBLMDL"

#define conMaxNumOfHighCached		128

typedef struct
{
	char	ipAddr[32+1];		// IP��ַ
	int	port;			// �˿�
	int	master;			// ������ʶ
	int	enabled;		// ����
//	int	conStats;		//״̬  add by lusj 20151027
	char	remark[128+1];		// ��ע
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

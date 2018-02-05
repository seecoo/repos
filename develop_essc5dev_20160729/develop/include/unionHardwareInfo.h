#ifndef _unionHardwareInfo_
#define _unionHardwareInfo_

typedef struct _UnionDiskInfo
{
	char	fileSystem[512];
	long	total;
	long	used;
	long	free;
	int	usedPercent;
}TUnionDiskInfo;
typedef TUnionDiskInfo	*PUnionDiskInfo;

typedef struct _UnionDiskIOStats
{
	int			major;		// ���豸��
	int			minor;		// ���豸��
	char			name[256];	// �豸��
	unsigned long long	rd_ios;		// �����̵Ĵ������ɹ���ɶ����ܴ���(�����еĲ���)  
	unsigned long long	rd_merges;	// �ϲ����Ĵ���(�������ڵ�IO��д���ܱ��ϳ�һ����)
	unsigned long long	rd_sectors;	// ����������
	unsigned long long 	rd_ticks;	// �����ѵĺ�����
	unsigned long long 	wr_ios;		// д���̵Ĵ������ɹ����д���ܴ���(�����еĲ���)
	unsigned long long 	wr_merges;	// �ϲ�д�Ĵ���
	unsigned long long 	wr_sectors;	// д��������
	unsigned long long 	wr_ticks;	// д���ѵĺ�����
	unsigned long long 	ticks;		// ����I/O�����ϵĺ�����
	unsigned long long 	aveq;		// �ڶ������ܵĵȴ��ĺ�����
}TUnionDiskIOStats;
typedef TUnionDiskIOStats	*PUnionDiskIOStats;

typedef struct _UnionDeviceIOStats
{
	char			name[256];	// �豸��
	double			inKB;		// kB_read/s,ÿ���ȡ������
	double			outKB;		// kB_read/s,ÿ��д�������
	int			tps;		// ÿ���д����
}TUnionDeviceIOStats;
typedef TUnionDeviceIOStats	*PUnionDeviceIOStats;

typedef struct _UnionNetIOStats
{
	char			name[128];      // ����ӿ�
        double			inKB;           // kB_read/s,ÿ���յ�������
        double			outKB;          // kB_read/s,ÿ�뷢�͵�����
        //int			tps;            // ÿ���д����
}TUnionNetIOStats;
typedef TUnionNetIOStats	*PUnionNetIOStats;


int UnionGetCpuInfo(int *cpuUserUsed, int *cpuSystemUsed, int *cpuIdle);

int UnionGetMemInfo(long *memoryTotal, long *memoryUesed, long *memoryFree);

int UnionGetDiskInfo(PUnionDiskInfo diskInfo, int maxNum);

int UnionGetDiskIOStats(PUnionDeviceIOStats deviceIOStats, int maxNum);

int UnionGetNetIOStats(PUnionNetIOStats netIOStats, int maxNum);

#endif // _unionHardwareInfo_

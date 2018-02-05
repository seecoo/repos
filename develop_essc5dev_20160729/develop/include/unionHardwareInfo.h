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
	int			major;		// 主设备号
	int			minor;		// 从设备号
	char			name[256];	// 设备名
	unsigned long long	rd_ios;		// 读磁盘的次数，成功完成读的总次数(队列中的不算)  
	unsigned long long	rd_merges;	// 合并读的次数(两次相邻的IO读写可能被合成一次做)
	unsigned long long	rd_sectors;	// 读的扇区数
	unsigned long long 	rd_ticks;	// 读花费的毫秒数
	unsigned long long 	wr_ios;		// 写磁盘的次数，成功完成写的总次数(队列中的不算)
	unsigned long long 	wr_merges;	// 合并写的次数
	unsigned long long 	wr_sectors;	// 写的扇区数
	unsigned long long 	wr_ticks;	// 写花费的毫秒数
	unsigned long long 	ticks;		// 花在I/O操作上的毫秒数
	unsigned long long 	aveq;		// 在队列中总的等待的毫秒数
}TUnionDiskIOStats;
typedef TUnionDiskIOStats	*PUnionDiskIOStats;

typedef struct _UnionDeviceIOStats
{
	char			name[256];	// 设备名
	double			inKB;		// kB_read/s,每秒读取的数据
	double			outKB;		// kB_read/s,每秒写入的数据
	int			tps;		// 每秒读写次数
}TUnionDeviceIOStats;
typedef TUnionDeviceIOStats	*PUnionDeviceIOStats;

typedef struct _UnionNetIOStats
{
	char			name[128];      // 网络接口
        double			inKB;           // kB_read/s,每秒收到的数据
        double			outKB;          // kB_read/s,每秒发送的数据
        //int			tps;            // 每秒读写次数
}TUnionNetIOStats;
typedef TUnionNetIOStats	*PUnionNetIOStats;


int UnionGetCpuInfo(int *cpuUserUsed, int *cpuSystemUsed, int *cpuIdle);

int UnionGetMemInfo(long *memoryTotal, long *memoryUesed, long *memoryFree);

int UnionGetDiskInfo(PUnionDiskInfo diskInfo, int maxNum);

int UnionGetDiskIOStats(PUnionDeviceIOStats deviceIOStats, int maxNum);

int UnionGetNetIOStats(PUnionNetIOStats netIOStats, int maxNum);

#endif // _unionHardwareInfo_

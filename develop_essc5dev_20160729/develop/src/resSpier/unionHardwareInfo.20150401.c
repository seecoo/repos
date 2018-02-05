#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mntent.h>
#include <sys/vfs.h> 

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionHardwareInfo.h"

int UnionGetCpuInfo(int *cpuUserUsed, int *cpuSystemUsed, int *cpuIdle)
{
	long			cpuTotal1 = 0,cpuTotal2 = 0;
	char			hardValue[16][128+1];
	FILE			*fp;

	if ((cpuUserUsed == NULL) || (cpuSystemUsed == NULL) || (cpuIdle == NULL))
	{
		UnionUserErrLog("in UnionGetCpuInfo:: parameter error!\n");
		return(errCodeParameter);
	}

	// cpu信息
	// 第一次获取
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in UnionGetCpuInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}

	memset(hardValue, 0, sizeof(hardValue));
	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[1],hardValue[2],hardValue[3],hardValue[4],hardValue[5],hardValue[6],hardValue[7]);
	fclose(fp);

	usleep(50000);

	// 第二次获取
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in UnionGetCpuInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}

	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[8],hardValue[9],hardValue[10],hardValue[11],hardValue[12],hardValue[13],hardValue[14]);

	fclose(fp);

	// 第一次获取CPU总时间
	cpuTotal1 = atol(hardValue[1]) + atol(hardValue[2]) + atol(hardValue[3]) + atol(hardValue[4]) + atol(hardValue[5]) + atol(hardValue[6]) + atol(hardValue[7]);

	// 第二次获取CPU总时间
	cpuTotal2 = atol(hardValue[8]) + atol(hardValue[9]) + atol(hardValue[10]) + atol(hardValue[11]) + atol(hardValue[12]) + atol(hardValue[13]) + atol(hardValue[14]);

	// CPU用户使用率
	*cpuUserUsed = 100.0 * (atol(hardValue[8]) - atol(hardValue[1])) / (cpuTotal2 - cpuTotal1);

	// CPU系统使用率
	*cpuSystemUsed = 100.0 * (atol(hardValue[10]) - atol(hardValue[3])) / (cpuTotal2 - cpuTotal1);

	// CPU空闲率
	*cpuIdle = 100.0 * (atol(hardValue[11])  - atol(hardValue[4])) / (cpuTotal2 - cpuTotal1); 	

	return 0;
}

int UnionGetMemInfo(long *memoryTotal, long *memoryUesed, long *memoryFree)
{
	FILE		*fp;
	char		hardValue[3][128];

	if ((memoryTotal == NULL) || (memoryUesed == NULL) || (memoryFree == NULL))
	{
		UnionUserErrLog("in UnionGetMemInfo:: parameter error!\n");
		return(errCodeParameter);
	}
	
	if ((fp = fopen("/proc/meminfo", "r")) == NULL)
	{
		UnionUserErrLog("in UnionGetMemInfo:: fopen[/proc/meminfo] error!\n");
		return(errCodeUseOSErrCode);
	}

	memset(hardValue, 0, sizeof(hardValue));
	fscanf(fp, "%s%s%s", hardValue[0], hardValue[1], hardValue[2]);
	*memoryTotal = atol(hardValue[1]);

	memset(hardValue, 0, sizeof(hardValue));
        fscanf(fp, "%s%s%s", hardValue[0], hardValue[1], hardValue[2]);
	*memoryFree = atol(hardValue[1]);
	
	*memoryUesed = *memoryTotal - *memoryFree;

	fclose(fp);
	return 0;
}

int UnionGetDiskInfo(PUnionDiskInfo diskInfo, int maxNum)
{
	int		ret;
	int		count;
	int		percent;
	long		usedSize = 0;
	long		freeSize = 0;
	long		blockSize = 0,totalSize = 0;
	FILE		*fp;
	struct mntent	*mountDevice;
	struct statfs	tmpDiskInfo;

	if (diskInfo == NULL)
	{
		UnionUserErrLog("in UnionGetDiskInfo:: parameter error!\n");	
		return(errCodeParameter);
	}
	
	// 硬盘信息
	if ((fp = setmntent("/etc/mtab","r")) == NULL)
	{
		UnionUserErrLog("in UnionGetDiskInfo:: setmntent[/etc/mtab]!\n");	
		return(errCodeUseOSErrCode);
	}

	count = 0;
	while((count < maxNum) && (mountDevice = getmntent(fp)))
	{
		if ((strcmp(mountDevice->mnt_fsname,"rootfs") == 0) || (strcmp(mountDevice->mnt_fsname,"udev") == 0))
			continue;

		if ((ret = statfs(mountDevice->mnt_dir,&tmpDiskInfo)) < 0)
		{
			UnionUserErrLog("in UnionGetDiskInfo:: statfs[%s] error!\n",mountDevice->mnt_dir);
			//return(ret);
		}
		
		if ((tmpDiskInfo.f_blocks > 0))
		{
			// 总空间大小
			blockSize = tmpDiskInfo.f_bsize / 1024;
			totalSize = blockSize * tmpDiskInfo.f_blocks / 1024;

			// 可用空间大小
			freeSize = blockSize * tmpDiskInfo.f_bavail / 1024;
			
			// 已用空间大小
			usedSize = totalSize - (blockSize * tmpDiskInfo.f_bfree / 1024);

			// 已用空间占用百分比
			percent = 100 * usedSize / totalSize;
			UnionLog("in UnionGetDiskInfo:: hardInfo[%s] total[%ld] used[%ld]\n",mountDevice->mnt_dir,totalSize,usedSize);

			snprintf(diskInfo[count].fileSystem, sizeof(diskInfo[count].fileSystem), "%s", mountDevice->mnt_fsname);
			diskInfo[count].total = totalSize;
			diskInfo[count].used = usedSize;
			diskInfo[count].free = freeSize;
			diskInfo[count].usedPercent = percent;
			count ++;
		}
	} 
	endmntent(fp);
	return count;
}


#define conDiskIOStatsMaxNum	10
#define conIoInterval		1000000

int UnionGetDiskIOStats(PUnionDeviceIOStats deviceIOStats, int maxNum)
{
	int			i;
	int			count = 0;
	int			item;
	char			buf[1024];
	TUnionDiskIOStats	diskIOStats[2][conDiskIOStatsMaxNum];
	FILE			*fp;

	if ((deviceIOStats == NULL) || (maxNum > conDiskIOStatsMaxNum))
	{
		UnionUserErrLog("in UnionGetDiskIOStats:: parameter error!\n");
		return(errCodeParameter);
	}
	// 第一次读取
	if ((fp = fopen("/proc/diskstats", "r")) == NULL)
	{
		UnionUserErrLog("in UnionGetDiskIOStats:: fopen[/proc/diskstats] error!\n");
		return(errCodeUseOSErrCode);
	}
	count = 0;
	memset(diskIOStats, 0, sizeof(diskIOStats));
	while (fgets(buf, sizeof(buf), fp) && count < maxNum)
	{
		item = sscanf(buf, "%4d %4d %s %llu %llu %llu %llu %llu %llu %llu %llu %*u %llu %llu", &diskIOStats[0][count].major, &diskIOStats[0][count].minor, diskIOStats[0][count].name, \
					&diskIOStats[0][count].rd_ios, &diskIOStats[0][count].rd_merges, &diskIOStats[0][count].rd_sectors, &diskIOStats[0][count].rd_ticks, \
					&diskIOStats[0][count].wr_ios, &diskIOStats[0][count].wr_merges, &diskIOStats[0][count].wr_sectors, &diskIOStats[0][count].wr_ticks, \
					&diskIOStats[0][count].ticks, &diskIOStats[0][count].aveq);
		if ((item != 13) || (memcmp(diskIOStats[0][count].name, "ram", 3) == 0) || (memcmp(diskIOStats[0][count].name, "loop", 4) == 0) || 
		    (memcmp(diskIOStats[0][count].name, "fd0", 3) == 0) || (memcmp(diskIOStats[0][count].name, "md0", 3) == 0))
		{
			continue;
		}
		count++;
	}
	fclose(fp);
	usleep(conIoInterval);
	
	// 第二次读取
	if ((fp = fopen("/proc/diskstats", "r")) == NULL)
	{
		UnionUserErrLog("in UnionGetDiskIOStats:: fopen[/proc/diskstats] error!\n");
		return(errCodeUseOSErrCode);
	}
	count = 0;
	while (fgets(buf, sizeof(buf), fp) && count < maxNum)
	{
		item = sscanf(buf, "%4d %4d %s %llu %llu %llu %llu %llu %llu %llu %llu %*u %llu %llu", &diskIOStats[1][count].major, &diskIOStats[1][count].minor, diskIOStats[1][count].name, \
					&diskIOStats[1][count].rd_ios, &diskIOStats[1][count].rd_merges, &diskIOStats[1][count].rd_sectors, &diskIOStats[1][count].rd_ticks, \
					&diskIOStats[1][count].wr_ios, &diskIOStats[1][count].wr_merges, &diskIOStats[1][count].wr_sectors, &diskIOStats[1][count].wr_ticks, \
					&diskIOStats[1][count].ticks, &diskIOStats[1][count].aveq);
		if ((item != 13) || (memcmp(diskIOStats[1][count].name, "ram", 3) == 0) || (memcmp(diskIOStats[1][count].name, "loop", 4) == 0) || 
		    (memcmp(diskIOStats[1][count].name, "fd0", 3) == 0) || (memcmp(diskIOStats[1][count].name, "md0", 3) == 0))
		{
			continue;
		}
		count++;
	}
	fclose(fp);

	// 计算IO
	for (i = 0; i < count; i++)
	{
		snprintf(deviceIOStats[i].name, sizeof(deviceIOStats[i].name), "%s", diskIOStats[0][i].name);
		deviceIOStats[i].tps = diskIOStats[1][i].rd_ios + diskIOStats[1][i].wr_ios - diskIOStats[0][i].rd_ios - diskIOStats[0][i].wr_ios;
		deviceIOStats[i].inKB = (diskIOStats[1][i].rd_sectors - diskIOStats[0][i].rd_sectors) * 1000000 / conIoInterval /2;
		deviceIOStats[i].outKB = (diskIOStats[1][i].wr_sectors - diskIOStats[0][i].wr_sectors) * 1000000 / conIoInterval /2;
	}
	return(count);
}

#define conNetIOStatsMaxNum	10

int UnionGetNetIOStats(PUnionNetIOStats netIOStats, int maxNum)
{
	int			i;
	int			count;
	char			buf[512];
	char			*ptr;
	char			interface[conNetIOStatsMaxNum][64];
	unsigned long long	receveLen[2][conNetIOStatsMaxNum];
	unsigned long long	sendLen[2][conNetIOStatsMaxNum];
	FILE			*fp;

	if ((netIOStats == NULL) || (maxNum > conNetIOStatsMaxNum))
	{
		UnionUserErrLog("in UnionGetNetIOStats:: parameter error!\n");
		return(errCodeParameter);
	}

	// 第一次采集
	if ((fp = fopen("/proc/net/dev", "r")) == NULL)
	{
		UnionUserErrLog("in UnionGetNetIOStats:: fopen[/proc/net/dev] error!\n");
		return(errCodeUseOSErrCode);
	}

	memset(interface, 0, sizeof(interface));
	count = 0;
	while (fgets(buf, sizeof(buf), fp) && count < maxNum)
	{
		if ((memcmp(buf, "Inter", 5) == 0) || (memcmp(buf, " face", 5) == 0) || (memcmp(buf, "    lo", 6) == 0))
		{
			continue;
		}
		if ((ptr = strchr(buf, ':')) == NULL)
		{
			UnionUserErrLog("in UnionGetNetIOStats:: strchr error!\n");
			return(errCodeParameter);
		}
		++ptr;
		sscanf(ptr, "%llu %*u %*u %*u %*u %*u %*u %*u %llu", &receveLen[0][count], &sendLen[0][count]);
		*(ptr - 1) = 0;
		sscanf(buf, "%s", interface[count]);
		++count;
	}
	fclose(fp);
	usleep(conIoInterval);
	

	// 第二次采集
	if ((fp = fopen("/proc/net/dev", "r")) == NULL)
	{
		UnionUserErrLog("in UnionGetNetIOStats:: fopen[/proc/net/dev] error!\n");
		return(errCodeUseOSErrCode);
	}

	count = 0;
	while (fgets(buf, sizeof(buf), fp) && count < maxNum)
	{
		if ((memcmp(buf, "Inter", 5) == 0) || (memcmp(buf, " face", 5) == 0) || (memcmp(buf, "    lo", 6) == 0))
		{
			continue;
		}
		if ((ptr = strchr(buf, ':')) == NULL)
		{
			UnionUserErrLog("in UnionGetNetIOStats:: strchr error!\n");
			return(errCodeParameter);
		}
		++ptr;
		sscanf(ptr, "%llu %*u %*u %*u %*u %*u %*u %*u %llu", &receveLen[1][count], &sendLen[1][count]);
		++count;
	}
	fclose(fp);

	for (i = 0; i < count; ++i)
	{
		snprintf(netIOStats[i].name, sizeof(netIOStats[i].name), "%s", interface[i]);
		netIOStats[i].inKB = (receveLen[1][i] - receveLen[0][i]) / 1024.00 / conIoInterval * 1000000.0;
		netIOStats[i].outKB = (sendLen[1][i] - sendLen[0][i]) / 1024.00 / conIoInterval * 1000000.0;
	}
	return(count);
}

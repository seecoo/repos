// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/03
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h>
#include <fcntl.h>

#include "unionVersion.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionOSLockMachinism.h"

int	gunionLockFD = 0;

static int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len)
{
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return(fcntl(fd,cmd,&lock));
}

static pid_t lock_test(int fd,int type,off_t offset,int whence,off_t len)
{
	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	if(fcntl(fd,F_GETLK,&lock) == -1)
	{
		return -1;
	}
	
	if(lock.l_type == F_UNLCK)
		return 0;
	else
		return lock.l_pid;
}

int read_lock(int fd,off_t offset,int whence,off_t len)
{
	return lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len);
}

int read_lock_try(int fd,off_t offset,int whence,off_t len)
{
	return lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len);
}

int write_lock(int fd,off_t offset,int whence,off_t len)
{
	return lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len);
}

int write_lock_try(int fd,off_t offset,int whence,off_t len)
{
	return lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len);
}

int unlock(int fd,off_t offset, int whence,off_t len)
{
	return lock_reg(fd,F_SETLK,F_UNLCK,offset,whence,len);
}

int is_read_lockable(int fd, off_t offset,int whence,off_t len)
{
	return !lock_test(fd,F_RDLCK,offset,whence,len);
}

int is_write_lockable(int fd, off_t offset,int whence,off_t len)
{
	return !lock_test(fd,F_WRLCK,offset,whence,len);
} 


// 申请一个系统互斥
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionApplyOsLock(char *resName)
{
	int	ret = 0;
	char	fileName[256];
	
	if (gunionLockFD <= 0)
	{
		snprintf(fileName,sizeof(fileName),"%s/%s.lock",getenv("UNIONTEMP"),resName);

		if ((gunionLockFD = open(fileName,O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0)
		{
			UnionUserErrLog("in UnionApplyOsLock:: open[%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	
	if ((ret = write_lock(gunionLockFD,0,SEEK_SET,0)) < 0)
	{
		UnionUserErrLog("in UnionApplyOsLock:: write_lock!\n");
		return(ret);
	}

	return(ret);
}	

// 释放系统互斥
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionFreeOsLock(char *resName)
{
	int	ret = 0;
	
	if (gunionLockFD > 0)
	{
		if ((ret = unlock(gunionLockFD,0,SEEK_SET,0)) < 0)
		{
			UnionUserErrLog("in UnionFreeOsLock:: unlock!\n");
			return(ret);
		}
		close(gunionLockFD);
		gunionLockFD = 0;
	}
	return(ret);
}

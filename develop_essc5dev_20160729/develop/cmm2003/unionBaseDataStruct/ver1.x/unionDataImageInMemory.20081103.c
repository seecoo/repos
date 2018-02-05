// Wolfgang Wang
// 2008/11/03

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "union2DirectionQueue.h"
#include "unionDataImageInMemory.h"
#include "unionLockTBL.h"

PUnion2DirectionQueueRec	pgunionDataImageInMemory = NULL;

// 删除所有数据映像
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0，删除的数据映像的数量
	失败		<0,错误码
*/
int UnionDeleteAllDataImageInMemory()
{
	int				ret;
	
	// 申请锁
	if ((ret = UnionApplyResWritingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllDataImageInMemory:: UnionApplyResWritingLockOfSpecResWithTimeout!\n");
		return(ret);
	}
		
	if ((ret = UnionDeleteAll2DirectionQueueRec(pgunionDataImageInMemory)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllDataImageInMemory:: UnionDeleteAll2DirectionQueueRec failure! ret = [%d]\n",ret);
	}
	UnionFreeResWritingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);
}	

// 增加一个数据映像
/*
输入参数
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionInsertDataImageInMemory(char *key,unsigned char *data,int lenOfData)
{
	int	ret;
	
	// 申请操作系统级的锁
	if ((ret = UnionApplyResWritingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)
	{
		UnionUserErrLog("in UnionInsertDataImageInMemory:: UnionApplyResWritingLockOfSpecResWithTimeout!\n");
		return(ret);
	}
		
	if (pgunionDataImageInMemory == NULL)
	{
		if ((pgunionDataImageInMemory = UnionNew2DirectionQueueRec(key,data,lenOfData)) == NULL)
		{
			UnionUserErrLog("in UnionInsertDataImageInMemory:: UnionNew2DirectionQueueRec failure!\n");
			ret = errCodeDataImageInMemoryInsertError;
			goto errExit;
		}
	}
	else
	{
		if ((ret = UnionInsert2DirectionQueueRec(pgunionDataImageInMemory,key,data,lenOfData)) < 0)
		{
			UnionUserErrLog("in UnionInsertDataImageInMemory:: UnionInsert2DirectionQueueRec failure! ret = [%d]\n",ret);
			goto errExit;
		}
	}
	ret = 0;
errExit:
	UnionFreeResWritingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);
}

// 判断一个数据映像是否存在
/*
输入参数
	key		关键字
输出参数
	无
返回值
	存在		1
	不存在		0
	出错		负值
*/
int UnionExistDataImageInMemory(char *key)
{
	int	exists = 1;
	int	ret;
	
	// 申请操作系统级的锁
	if ((ret = UnionApplyResReadingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)	
	{
		UnionUserErrLog("in UnionExistDataImageInMemory:: UnionApplyResReadingLockOfSpecResWithTimeout!\n");
		return(ret);
	}	
	if (UnionFind2DirectionQueueRec(pgunionDataImageInMemory,key) == NULL)
		exists = 0;
	UnionFreeResReadingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(exists);
}

// 修改一个数据映像
/*
输入参数
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionModifyDataImageInMemory(char *key,unsigned char *data,int lenOfData)
{
	int	ret;
	
	if ((ret = UnionExistDataImageInMemory(key)) <= 0)
	{
		if (ret == 0)
			ret = errCodeDataImageInMemoryNotExists;
		UnionUserErrLog("in UnionModifyDataImageInMemory:: UnionFind2DirectionQueueRec [%s]!\n",key);
		return(ret);
	}
	else
	{
		// 存在数据映像,先删除
		if ((ret = UnionDeleteDataImageInMemory(key)) < 0)
		{
			UnionUserErrLog("in UnionModifyDataImageInMemory:: UnionDeleteDataImageInMemory [%s]!\n",key);
			return(ret);
		}
	}	
	if ((ret = UnionInsertDataImageInMemory(key,data,lenOfData)) < 0)
	{
		UnionUserErrLog("in UnionModifyDataImageInMemory:: UnionInsertDataImageInMemory [%s]!\n",key);
		return(ret);
	}
	return(ret);	
}

// 读一个数据映像
/*
输入参数
	key		关键字
	sizeOfBuf	数据缓冲的大小
输出参数
	data		数据
返回值
	成功		>=0，读到的数据长度
	失败		<0,错误码
*/
int UnionReadDataImageInMemory(char *key,unsigned char *data,int sizeOfBuf)
{
	int				ret;
	PUnion2DirectionQueueRec	ptmpRec;
	
	// 申请操作系统级的锁
	if ((ret = UnionApplyResReadingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)	
	{
		UnionUserErrLog("in UnionDeleteDataImageInMemory:: UnionApplyResReadingLockOfSpecResWithTimeout!\n");
		return(ret);
	}	
	if ((ptmpRec = UnionFind2DirectionQueueRec(pgunionDataImageInMemory,key)) == NULL)
	{
		ret = errCodeDataImageInMemoryNotExists;
		goto errExit;
	}
	// 找到了记录
	if ((data == NULL) || (ptmpRec->reserved == NULL) || (ptmpRec->lenOfData <= 0))
	{
		ret = 0;
		UnionProgramerLog("in UnionDeleteDataImageInMemory:: *** not copy data!\n");
		goto errExit;
	}
	if (sizeOfBuf < ptmpRec->lenOfData)
	{
		UnionUserErrLog("in UnionReadDataImageInMemory:: buffer size [%d] < expected [%d]\n",sizeOfBuf,ptmpRec->lenOfData);
		ret = errCodeSmallBuffer;
		goto errExit;
	}
	memcpy(data,ptmpRec->reserved,ptmpRec->lenOfData);
	ret = ptmpRec->lenOfData;
	//printf("*** data copyed ok [%04d] [%s]!\n",ret,data);
errExit:
	UnionFreeResReadingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);
}

// 删除一个数据映像
/*
输入参数
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionDeleteDataImageInMemory(char *key)
{
	int				ret;
	PUnion2DirectionQueueRec	ptmpRec;

	if ((ret = UnionExistDataImageInMemory(key)) < 0)
		return(ret);
	if (ret == 0)
	{
		UnionAuditLog("in UnionDeleteDataImageInMemory:: [%s] not exists!\n",key);
		return(errCodeDataImageInMemoryNotExists);	
	}
	// 申请操作系统级的锁
	if ((ret = UnionApplyResWritingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)	
	{
		UnionUserErrLog("in UnionDeleteDataImageInMemory:: UnionApplyResWritingLockOfSpecResWithTimeout!\n");
		return(ret);
	}
		
	if ((ret = UnionDelete2DirectionQueueRec(pgunionDataImageInMemory,key)) < 0)
	{
		if (ret != errCodeKeyWordIsMyself)
		{
			UnionUserErrLog("in UnionDeleteDataImageInMemory:: UnionDelete2DirectionQueueRec failure! ret = [%d]\n",ret);
			goto errExit;
		}
		ptmpRec = pgunionDataImageInMemory->next;
		UnionFree2DirectionQueueRec(pgunionDataImageInMemory);
		pgunionDataImageInMemory = ptmpRec;
	}
	ret = 0;
errExit:
	UnionFreeResWritingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);
}	

// 判断一个记录数据映像是否存在
/*
输入参数
	objectName	表名		
	key		关键字
输出参数
	无
返回值
	存在		1
	不存在		0
	出错		负值
*/
int UnionExistRecDataImageInMemory(char *objectName,char *key)
{
	char	tmpBuf[512];
	
	if ((objectName == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionExistRecDataImageInMemory:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"%s::%s",objectName,key);
	return(UnionExistDataImageInMemory(tmpBuf));
}

// 增加一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionInsertRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData)
{
	char	tmpBuf[512];
	
	if ((objectName == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionInsertRecDataImageInMemory:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"%s::%s",objectName,key);
	return(UnionInsertDataImageInMemory(tmpBuf,data,lenOfData));
}

// 修改一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionModifyRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData)
{
	char	tmpBuf[512];
	
	if ((objectName == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionInsertRecDataImageInMemory:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"%s::%s",objectName,key);
	return(UnionModifyDataImageInMemory(tmpBuf,data,lenOfData));
}

// 读一个记录的数据映像
/*
输入参数
	key		关键字
	sizeOfBuf	数据缓冲的大小
输出参数
	data		数据
返回值
	成功		>=0，读到的数据长度
	失败		<0,错误码
*/
int UnionReadRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int sizeOfBuf)
{
	char	tmpBuf[512];
	
	if ((objectName == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionInsertRecDataImageInMemory:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"%s::%s",objectName,key);
	return(UnionReadDataImageInMemory(tmpBuf,data,sizeOfBuf));
}

// 删除一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionDeleteRecDataImageInMemory(char *objectName,char *key)
{
	char	tmpBuf[512];
	
	if ((objectName == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionDeleteRecDataImageInMemory:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"%s::%s",objectName,key);
	return(UnionDeleteDataImageInMemory(tmpBuf));
}

// 显示所有数据映像
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionPrintAllDataImageInMemoryToFile(FILE *fp)
{
	int				ret;
	
	// 申请操作系统级的锁
	if ((ret = UnionApplyResReadingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)	
	{
		UnionUserErrLog("in UnionPrintAllDataImageInMemoryToFile:: UnionApplyResReadingLockOfSpecResWithTimeout!\n");
		return(ret);
	}
	ret = UnionPrintAll2DirectionQueueRecToFile(pgunionDataImageInMemory,fp);
	UnionFreeResReadingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);	
}

// 显示所有数据
/*
输入参数
	resID		资源号
输出参数
	无
返回值
	>=0		数据数目
	<0		错误码
*/
int UnionSpierAllDataImageInMemory(int resID)
{
	return(UnionSpierAllRecIn2DirectionQueue(pgunionDataImageInMemory,resID));
}

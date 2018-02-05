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

// ɾ����������ӳ��
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0��ɾ��������ӳ�������
	ʧ��		<0,������
*/
int UnionDeleteAllDataImageInMemory()
{
	int				ret;
	
	// ������
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

// ����һ������ӳ��
/*
�������
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionInsertDataImageInMemory(char *key,unsigned char *data,int lenOfData)
{
	int	ret;
	
	// �������ϵͳ������
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

// �ж�һ������ӳ���Ƿ����
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	����		1
	������		0
	����		��ֵ
*/
int UnionExistDataImageInMemory(char *key)
{
	int	exists = 1;
	int	ret;
	
	// �������ϵͳ������
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

// �޸�һ������ӳ��
/*
�������
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
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
		// ��������ӳ��,��ɾ��
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

// ��һ������ӳ��
/*
�������
	key		�ؼ���
	sizeOfBuf	���ݻ���Ĵ�С
�������
	data		����
����ֵ
	�ɹ�		>=0�����������ݳ���
	ʧ��		<0,������
*/
int UnionReadDataImageInMemory(char *key,unsigned char *data,int sizeOfBuf)
{
	int				ret;
	PUnion2DirectionQueueRec	ptmpRec;
	
	// �������ϵͳ������
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
	// �ҵ��˼�¼
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

// ɾ��һ������ӳ��
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
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
	// �������ϵͳ������
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

// �ж�һ����¼����ӳ���Ƿ����
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	����		1
	������		0
	����		��ֵ
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

// ����һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
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

// �޸�һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
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

// ��һ����¼������ӳ��
/*
�������
	key		�ؼ���
	sizeOfBuf	���ݻ���Ĵ�С
�������
	data		����
����ֵ
	�ɹ�		>=0�����������ݳ���
	ʧ��		<0,������
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

// ɾ��һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
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

// ��ʾ��������ӳ��
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionPrintAllDataImageInMemoryToFile(FILE *fp)
{
	int				ret;
	
	// �������ϵͳ������
	if ((ret = UnionApplyResReadingLockOfSpecResWithTimeout(conResNameOfDataImageInMemory,1)) < 0)	
	{
		UnionUserErrLog("in UnionPrintAllDataImageInMemoryToFile:: UnionApplyResReadingLockOfSpecResWithTimeout!\n");
		return(ret);
	}
	ret = UnionPrintAll2DirectionQueueRecToFile(pgunionDataImageInMemory,fp);
	UnionFreeResReadingLockOfSpecRes(conResNameOfDataImageInMemory);
	return(ret);	
}

// ��ʾ��������
/*
�������
	resID		��Դ��
�������
	��
����ֵ
	>=0		������Ŀ
	<0		������
*/
int UnionSpierAllDataImageInMemory(int resID)
{
	return(UnionSpierAllRecIn2DirectionQueue(pgunionDataImageInMemory,resID));
}

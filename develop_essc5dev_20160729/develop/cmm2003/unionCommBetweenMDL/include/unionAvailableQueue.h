// Wolfgang Wang
// 2010-12-22

#ifndef	_unionAvailableQueue_
#define _unionAvailableQueue_

#include "unionErrCode.h"

// ����һ���������
typedef struct
{
	int	userRequestQueueID;		// �û�ָ�������󻺳����
	int	sysRequestQueueID;		// ���󻺳���е�OS��ʶ��
	int	userResponseQueueID;		// �û�ָ������Ӧ�������
	int	sysResponseQueueID;		// ��Ӧ������е�OS��ʶ��
} TUnionServiceQueue;
typedef TUnionServiceQueue	*PUnionServiceQueue;

// ����һ��������г�
// һ�����г��п��԰����Ķ��е��������
#define conMaxNumOfQueuePerPool			64
typedef struct
{
	char			poolID[40+1];			// ���гر�ʶ
	int			queueNum;			// ���г��а����Ķ�������
	PUnionServiceQueue	queueGrp[conMaxNumOfQueuePerPool];	// ����
} TUnionQueuePool;
typedef TUnionQueuePool		*PUnionQueuePool;

#endif

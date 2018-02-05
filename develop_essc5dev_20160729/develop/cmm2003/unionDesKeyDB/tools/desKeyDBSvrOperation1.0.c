
#include "UnionLog.h"
#include "unionErrCode.h"

#include "unionDesKeyDB.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "desKeyDBRecord.h"

#include "desKeyDBSvrOperation.h"

/*
���ܣ���Կ����������
���������
	serviceID�� �����ʶ
	reqStr����������
	lenOfReqStr���������ݵĳ���
	sizeOfResStr��resStr�Ļ�������С
���������
	resStr����Ӧ����
fileRecved��1����ʾ�������˷�����һ���ļ���0����ʾû���ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionDesKeyDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	*fileRecved		= 0;
	
	// ����һ����Կ
	if (serviceID == conDatabaseCmdInsert)
	{
		return UnionInsertDesKeyDBRecord(NULL, reqStr, lenOfReqStr);
	}
	// ���ݹؼ���ɾ��һ����Կ
	else if (serviceID == conDatabaseCmdDelete) 
	{
		return UnionDeleteUniqueDesKeyDBRecord(NULL, reqStr);
	}
	// �޸�һ����Կ
	else if (serviceID == conDatabaseCmdUpdate)
	{
		return UnionUpdateUniqueDesKeyDBRecord(NULL, "", reqStr, lenOfReqStr);
	}
	// ���ݹؼ��ֲ�ѯһ����Կ
	else if (serviceID == conDatabaseCmdQuerySpecRec)
	{
		return UnionSelectUniqueDesKeyDBRecordByPrimaryKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ����Ψһֵ��ѯһ����Կ
	else if (serviceID == conDatabaseCmdQuerySpecRecWithUnion)
	{
		return UnionSelectUniqueDesKeyDBRecordByUniqueKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ����ɾ����Կ
	else if (serviceID == conDatabaseCmdConditionDelete)
	{
		return UnionBatchDeleteDesKeyDBRecord(NULL, reqStr);
	}
	// �����޸���Կ
	else if (serviceID == conDatabaseCmdConditionUpdate)
	{
		return UnionBatchUpdateDesKeyDBRecord(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ������ѯ��Կ
	else if (serviceID == conDatabaseCmdQueryAllRec)
	{
		*fileRecved		= 1;
		return UnionBatchSelectDesKeyDBRecord(NULL, reqStr, UnionGenerateMngSvrTempFile());
	}
	// ��ʱ��֧��
	else
	{
		UnionLog("in UnionExcuteUnionDesKeyDBSvrOperation, the serviceID: [%d] is not support now.\n", serviceID);
		return 0;
	}
}

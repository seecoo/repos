
#include "UnionLog.h"
#include "unionErrCode.h"

#include "unionPKDB.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "pkDBRecord.h"

#include "pkDBSvrOperation.h"

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
int UnionExcuteUnionPKDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	*fileRecved		= 0;
	
	// ����һ����Կ
	if (serviceID == conDatabaseCmdInsert)
	{
		return UnionInsertPKDBRecord(NULL, reqStr, lenOfReqStr);
	}
	// ���ݹؼ���ɾ��һ����Կ
	else if (serviceID == conDatabaseCmdDelete) 
	{
		return UnionDeleteUniquePKDBRecord(NULL, reqStr);
	}
	// �޸�һ����Կ
	else if (serviceID == conDatabaseCmdUpdate)
	{
		return UnionUpdateUniquePKDBRecord(NULL, "", reqStr, lenOfReqStr);
	}
	// ���ݹؼ��ֲ�ѯһ����Կ
	else if (serviceID == conDatabaseCmdQuerySpecRec)
	{
		return UnionSelectUniquePKDBRecordByPrimaryKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ����Ψһֵ��ѯһ����Կ
	else if (serviceID == conDatabaseCmdQuerySpecRecWithUnion)
	{
		return UnionSelectUniquePKDBRecordByUniqueKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ����ɾ����Կ
	else if (serviceID == conDatabaseCmdConditionDelete)
	{
		return UnionBatchDeletePKDBRecord(NULL, reqStr);
	}
	// �����޸���Կ
	else if (serviceID == conDatabaseCmdConditionUpdate)
	{
		return UnionBatchUpdatePKDBRecord(NULL, reqStr, resStr, sizeOfResStr);
	}
	// ������ѯ��Կ
	else if (serviceID == conDatabaseCmdQueryAllRec)
	{
		*fileRecved		= 1;
		return UnionBatchSelectPKDBRecord(NULL, reqStr, UnionGenerateMngSvrTempFile());
	}
	// ��ʱ��֧��
	else
	{
		UnionLog("in UnionExcuteUnionPKDBSvrOperation, the serviceID: [%d] is not support now.\n", serviceID);
		return 0;
	}
}

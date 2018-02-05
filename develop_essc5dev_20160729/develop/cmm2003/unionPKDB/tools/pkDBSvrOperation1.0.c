
#include "UnionLog.h"
#include "unionErrCode.h"

#include "unionPKDB.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "pkDBRecord.h"

#include "pkDBSvrOperation.h"

/*
功能：密钥库操作命令函数
输入参数：
	serviceID： 命令标识
	reqStr：请求数据
	lenOfReqStr：请求数据的长度
	sizeOfResStr：resStr的缓冲区大小
输出参数：
	resStr：响应数据
fileRecved：1：表示服务器端发送来一个文件，0：表示没有文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionPKDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	*fileRecved		= 0;
	
	// 插入一把密钥
	if (serviceID == conDatabaseCmdInsert)
	{
		return UnionInsertPKDBRecord(NULL, reqStr, lenOfReqStr);
	}
	// 根据关键字删除一把密钥
	else if (serviceID == conDatabaseCmdDelete) 
	{
		return UnionDeleteUniquePKDBRecord(NULL, reqStr);
	}
	// 修改一把密钥
	else if (serviceID == conDatabaseCmdUpdate)
	{
		return UnionUpdateUniquePKDBRecord(NULL, "", reqStr, lenOfReqStr);
	}
	// 根据关键字查询一把密钥
	else if (serviceID == conDatabaseCmdQuerySpecRec)
	{
		return UnionSelectUniquePKDBRecordByPrimaryKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 根据唯一值查询一把密钥
	else if (serviceID == conDatabaseCmdQuerySpecRecWithUnion)
	{
		return UnionSelectUniquePKDBRecordByUniqueKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 批量删除密钥
	else if (serviceID == conDatabaseCmdConditionDelete)
	{
		return UnionBatchDeletePKDBRecord(NULL, reqStr);
	}
	// 批量修改密钥
	else if (serviceID == conDatabaseCmdConditionUpdate)
	{
		return UnionBatchUpdatePKDBRecord(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 批量查询密钥
	else if (serviceID == conDatabaseCmdQueryAllRec)
	{
		*fileRecved		= 1;
		return UnionBatchSelectPKDBRecord(NULL, reqStr, UnionGenerateMngSvrTempFile());
	}
	// 暂时不支持
	else
	{
		UnionLog("in UnionExcuteUnionPKDBSvrOperation, the serviceID: [%d] is not support now.\n", serviceID);
		return 0;
	}
}

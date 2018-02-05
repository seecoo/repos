
#include "UnionLog.h"
#include "unionErrCode.h"

#include "unionDesKeyDB.h"
#include "mngSvrCommProtocol.h"
#include "unionDatabaseCmd.h"
#include "desKeyDBRecord.h"

#include "desKeyDBSvrOperation.h"

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
int UnionExcuteUnionDesKeyDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	*fileRecved		= 0;
	
	// 插入一把密钥
	if (serviceID == conDatabaseCmdInsert)
	{
		return UnionInsertDesKeyDBRecord(NULL, reqStr, lenOfReqStr);
	}
	// 根据关键字删除一把密钥
	else if (serviceID == conDatabaseCmdDelete) 
	{
		return UnionDeleteUniqueDesKeyDBRecord(NULL, reqStr);
	}
	// 修改一把密钥
	else if (serviceID == conDatabaseCmdUpdate)
	{
		return UnionUpdateUniqueDesKeyDBRecord(NULL, "", reqStr, lenOfReqStr);
	}
	// 根据关键字查询一把密钥
	else if (serviceID == conDatabaseCmdQuerySpecRec)
	{
		return UnionSelectUniqueDesKeyDBRecordByPrimaryKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 根据唯一值查询一把密钥
	else if (serviceID == conDatabaseCmdQuerySpecRecWithUnion)
	{
		return UnionSelectUniqueDesKeyDBRecordByUniqueKey(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 批量删除密钥
	else if (serviceID == conDatabaseCmdConditionDelete)
	{
		return UnionBatchDeleteDesKeyDBRecord(NULL, reqStr);
	}
	// 批量修改密钥
	else if (serviceID == conDatabaseCmdConditionUpdate)
	{
		return UnionBatchUpdateDesKeyDBRecord(NULL, reqStr, resStr, sizeOfResStr);
	}
	// 批量查询密钥
	else if (serviceID == conDatabaseCmdQueryAllRec)
	{
		*fileRecved		= 1;
		return UnionBatchSelectDesKeyDBRecord(NULL, reqStr, UnionGenerateMngSvrTempFile());
	}
	// 暂时不支持
	else
	{
		UnionLog("in UnionExcuteUnionDesKeyDBSvrOperation, the serviceID: [%d] is not support now.\n", serviceID);
		return 0;
	}
}

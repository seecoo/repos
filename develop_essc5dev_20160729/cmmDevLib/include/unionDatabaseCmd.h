#ifndef _unionDatabaseCmd_
#define _unionDatabaseCmd_

#define conSpecTBLIDOfOperator		7	//操作员表

// 以下定义资源操作命令
#define conDatabaseCmdInsert		1	// 插入记录
#define conDatabaseCmdDelete		2	// 删除记录
#define conDatabaseCmdUpdate		3	// 更新整个记录
#define conDatabaseCmdLock		10	// 锁住记录
#define conDatabaseCmdUnLock		11	// 解锁记录
#define conDatabaseCmdUnLockAllRec	12	// 解锁所有记录
#define conEsscRecCmdUpdate		20	// 更新整个记录
#define conEsscRecCmdUpdateAttrFld	21	// 更新属性域
#define conEsscRecCmdUpdateValueFld	22	// 更新值域
#define conDatabaseCmdRead		50	// 读取记录
#define conDatabaseCmdQuerySpecRec	conDatabaseCmdRead	// 读取记录
#define conDatabaseCmdQueryAllRec	51	// 查询所有记录
#define conDatabaseCmdQueryWithCondition	52	// 查询符合条件所有记录
#define conDatabaseCmdQuerySpecRecWithUnion	53	// 查询所有记录
#define conDatabaseCmdQueryAllRecWithUnion	54	// 查询符合条件所有记录
#define conDatabaseCmdQueryByRealSQL		55	// 根据真实SQL查询
#define conDatabaseCmdQueryBySpecFieldList	56	// 查询指定清单对应的数据
#define conDatabaseCmdConditionDelete	62	// 删除符合条件的记录
#define conDatabaseCmdConditionUpdate	63	// 更新符合条件的记录

// 以下是通知命令
// 以下是系统运行情况
#define conDatabaseCmdApplicationStoped		999	// 应用程序停止运行

// 以下是监控命令
#define conDatabaseCmdSendingSpiedRec		100	// 发送监控记录
#define conDatabaseCmdStartSendingSpiedRec	98	// 开始发送监控记录
#define conDatabaseCmdFinishSendingSpiedRec	99	// 结束发送监控记录

// 以下是文件传输命令
#define conDatabaseCmdStartSendingData		200	// 开始发送文件数据
#define conDatabaseCmdSendingData		201	// 发送数据记录
#define conDatabaseCmdFinishSendingData		202	// 结束发送文件数据
#define conDatabaseCmdDownloadFile		203	// 下载数据文件
#define conDatabaseCmdUploadFile			204	// 上载数据文件
#define conDatabaseCmdReloadResFile		205	// 在服务器端重新加载资源文件
#define conDatabaseCmdQueryResStatus		206	// 查询资源的状态
#define conDatabaseCmdUnlockRes			207	// 解锁资源

// 更新某个域，以下命令由不同的资源自行赋予其物理意义。
#define conDatabaseCmdSpec001		101	
#define conDatabaseCmdSpec002		102	
#define conDatabaseCmdSpec003		103	
#define conDatabaseCmdSpec004		104	
#define conDatabaseCmdSpec005		105	
#define conDatabaseCmdSpec006		106	
#define conDatabaseCmdSpec007		107
#define conDatabaseCmdSpec008		108
#define conDatabaseCmdSpec009		109
#define conDatabaseCmdSpec010		110
#define conDatabaseCmdSpec011		111

#define conDatabaseCmdReserved01	9999	// 保留命令
#define conDatabaseCmdReserved02	9998	// 保留命令
#define conDatabaseCmdReserved03	9997	// 保留命令

// 监控客户端控制表的特殊命令
// 操作员登录
#define conDatabaseSpecCmdOfOperatorTBL_logon		conDatabaseCmdSpec001
// 操作员退出登录
#define conDatabaseSpecCmdOfOperatorTBL_logoff		conDatabaseCmdSpec002
// 强制操作员退出登录
#define conDatabaseSpecCmdOfOperatorTBL_ForceLogoff	conDatabaseCmdSpec003

#endif

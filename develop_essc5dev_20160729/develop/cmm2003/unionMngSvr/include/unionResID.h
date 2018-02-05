// 2008/9/27 wolfgang wang
#ifndef _unionResID_
#define _unionResID_

typedef enum
{
	conResTypeREC = 0,		// 运行环境配置资源
	conResTransPack = 1,	// 交易报文资源
	conResErrorInfo = 2,	// 错误信息资源
} TUnionResType;
	
// 以下定义资源ID号
#define conResIDUsingTableName		970	//使用串中的表名查询资源ID
#define conResIDComplexField		73	//复杂域赋值方法表
#define conResIDSecondaryMenu		817	//二级菜单
#define conResIDViewList		823	//视图
#define conResIDMainMenu		812	//主菜单
#define conResIDMenuDef			847	//菜单组定义
#define conResIDEnumValueDef		72	//枚举值定义
#define conResIDSecurityServiceDef	838	//安全服务定义

#define conResIDDynamicTBL		431	//动态表，采用动态表名
#define conResIDMsgBuf			888	//消息交换区
#define conResIDHsmGrp			1	//密码机工作组
#define conResIDTaskTBL			2	//任务注册表
#define conResIDLogTBL			3	//日志注册表
#define conResIDREC			4	//客户化参数表
#define conResIDKeyCache		5	//密钥缓冲区
#define conResIDCommConf		6	//通讯客户端表
#define conResIDOperatorTBL		7	//操作员表
//#define conResIDDesKeyDB		8	//DESKey库
#define conResIDDesKeyDB		222	//DESKey库 //hzh in 2011.8.9 宏定义由8改为222，其与unionDataTBLList.h里定义的一致
#define conResIDPKDB			9	//PK库
#define conResIDRSADB			107	//RSA库
#define conResIDTransSpierBuf		10	//监控信息缓冲区
#define conResIDLockTBL			11	// 锁表
#define conResIDHsmSckConnPool		12	// 密码机通讯连接池表
#define conResIDClientSckConnPool	13	// 客户端通讯连接池表
#define conResIDSecurityService		30	//安全服务
#define conResIDKeyMngService		31	//密钥管理服务
#define conResIDHsmCmd			32	//密码机指令服务
#define conResIDMngService		33	//系统管理服务
#define conResIDCompareDesKey           34      //两个平台之间的密钥比对结果
#define conResIDRunningError		50	//系统运行错误
#define conResIDSpierClientTBL		60	//监控客户端控制表
#define conResIDTransSpierSvr		62	//ESSC交易监控服务器，是一类特殊资源
#define conResIDResSpierSvr		63	//ESSC资源监控服务器，是一类特殊资源
#define conResIDTransSprerThread	64	//交易服务线程	Add By Huangbaoxin,20081231
#define conResIDSessionContainer	65	//会话池	Add By Huangbaoxin,20081231
#define conResIDDataImageInMemory	66	//数据映像	Add By wolfgang wang 2009/1/8
#define conResIDPinPad			67	// 密码键盘	Add By xusj, 2009-2-5
#define conResIDKDBService		68	// 密钥服务
#define conResIDKDBSynchronizerService	69	// 密钥同步服务
#define conResIDMsgQueue		70	// 消息队列资源ID
#define conResIDKDBBackuper		71	// 密钥备份服务
#define conResIDKDBSvr			72	// 密钥映像

#define conResIDHsmError		100	//密码机错误
#define conResIDError			101	//ESSC错误
#define conResIDAppConf			102	//应用配置
#define conResIDLocalKMSvr		103	//本地密钥管理服务器配置
#define conResIDRemoteKMSvr		104	//远程密钥管理服务器配置
#define conResIDKeyDBBackuper		105	//密钥备份服务器
#define conResIDKDBSvrBuf		106	//密钥服务器的缓冲

#define conResIDOperationController	200	//操作控制
#define conResIDServiceDef		201	//安全服务定义
#define conResIDMngServiceDef		202	//管理服务定义
#define conResIDFldTagDef		203	//域标识定义
#define conResIDSharedMemoryDef		204	//共享内存定义
#define conResIDAutoRemoteKeyMngDef	205	//远程密钥自动管理定义
#define conResIDDefaultKeyPrintFormat	206	//缺省密钥打印格式文件
#define conResIDDefaultHsmConf		207	//缺省密码机配置文件

#define conResIDKeyMng			300	// 密钥管理

// add in 20100825
#define conResIDDEAN			401

// add end

#define conResIDVarType			971	//变量类型清单
#define conResIDTableField		972	//表域清单
#define conResIDTableList		813	//表清单
#define conResIDTableType		875	//表分类

#define conResIDConfFileManager		999	//配置文件管理员

// 以下定义资源操作命令
#define conResCmdInsert			1	// 插入记录
#define conResCmdDelete			2	// 删除记录
#define conResCmdUpdate			3	// 更新整个记录
#define conResCmdLock			10	// 锁住记录
#define conResCmdUnLock			11	// 解锁记录
#define conResCmdUnLockAllRec		12	// 解锁所有记录
#define conRecCmdUpdate			20	// 更新整个记录
#define conRecCmdUpdateAttrFld		21	// 更新属性域
#define conRecCmdUpdateValueFld		22	// 更新值域
#define conResCmdRead			50	// 读取记录
#define conResCmdQuerySpecRec		conResCmdRead	// 读取记录
#define conResCmdQueryAllRec		51	// 查询所有记录
#define conResCmdQueryWithCondition	52	// 查询符合条件所有记录

#define conResCmdGenerateTBLDef		741	// 生成表定义
#define conResCmdGenerateTBLCreateSQL	742	// 生成建表的SQL语句
#define conResCmdGenerateTBLQueryInterface	743	// 生成表视图的界面
#define conResCmdGenerateTBLIncFile	744	// 生成头文件
#define conResCmdGenerateTBLCFile	745	// 生成C代码
#define conResCmdGenerateTBLIncAndCFile	746	// 生成C代码和头文件
#define conResCmdGenerateEnumDef	747	// 生成枚举
#define conResCmdGenerateRecDoubleClickMenu	748	// 生成记录双击菜单
#define conResCmdGenerateRecPopupMenu	749	// 生成记录右键菜单
#define conResCmdGenerateFormPopupMenu	750	// 生成界面右键菜单
#define conResCmdGenerateTBLOperation	751	// 生成表操作清单
#define conResCmdCreateTBLAnyway	752	// 重新建表
#define conResCmdGenerateMngSvrTBLCreateFile	753	// 生成mngSvr建表的SQL语句
#define conResCmdInsertInitRec		754	// 插入初始记录
#define conResCmdGenerateAllConfForTBL	755	// 生成表需要的所有配置
#define conResCmdGenerateAllMenu	756	// 生成所有菜单
#define conResCmdGenerateMainMenu	757	// 生成主菜单
//#define conResCmdGenerateSecondaryMenuu	757	// 生成二级菜单
#define conResCmdGenerateSpec2LevelMenu	758	// 生成指定二级菜单菜单
#define conResCmdGenerateCreateAllTBLSQL	759	// 生成所有的建表SQL语句
#define conResCmdGenerateOperationInfo	780	// 生成操作提示文件
#define conResCmdGenerateTBLListIncFile	781	// 生成表清单定义文件
#define conResCmdGenerateOperationDefIncFile	782	// 生成操作定义文件

#define conResCmdOutputTBLRec		783	// 从表中导出记录
#define conResCmdInputTBLRec		784	// 向表中导入记录
#define conResCmdPrintRelationsOfSpecTBL	785	// 显示指定表的关系
#define conResCmdPrintRelationsOfAllTBL	786	// 显示所有表的关系
#define conResCmdPrintAllTBLInOrder	787	// 按引用顺序关系显示所有表
#define conResCmdCreateDropAllTBLFile	788	// 创建删除所有表的语句
#define conResCmdCreateDefaultViewOfTBL	789	// 创建表缺省的视图
#define conResCmdDownloadEnumDef	780	// 读取枚举定义文件
#define conResCmdGetViewAuthorization	791	// 读取视图的操作授权
#define conResCmdCreateTableOnRealDB	642	// 数据库上创建或重创建表
#define conResCmdGenerateTBLStructDefIncFile	792	// 生成表结构定义的头文件
	
// 以下是系统运行情况
#define conResCmdApplicationStoped	999	// 应用程序停止运行

// 以下是监控命令
#define conResCmdSendingSpiedRec	100	// 发送监控记录
#define conResCmdStartSendingSpiedRec	98	// 开始发送监控记录
#define conResCmdFinishSendingSpiedRec	99	// 结束发送监控记录

// 以下是文件传输命令
#define conResCmdStartSendingData	200	// 开始发送文件数据
#define conResCmdSendingData		201	// 发送数据记录
#define conResCmdFinishSendingData	202	// 结束发送文件数据
#define conResCmdDownloadFile		203	// 下载数据文件
#define conResCmdUploadFile		204	// 上载数据文件
#define conResCmdReloadResFile		205	// 在服务器端重新加载资源文件
#define conResCmdQueryResStatus		206	// 查询资源的状态
#define conResCmdUnlockRes		207	// 解锁资源
#define conResCmdUploadFileOnCondition	208	// 条件上载数据文件
#define conResCmdCreateFileDir		209	// 创建文件目录

// 以下是通知命令

// 更新某个域，以下命令由不同的资源自行赋予其物理意义。
#define conResCmdSpec001		101	
#define conResCmdSpec002		102	
#define conResCmdSpec003		103	
#define conResCmdSpec004		104	
#define conResCmdSpec005		105	
#define conResCmdSpec006		106	
#define conResCmdSpec007		107
#define conResCmdSpec008		108
#define conResCmdSpec009		109
#define conResCmdSpec010		110
#define conResCmdSpec011		111

// 安全服务表功能
// 生成安全服务设计
#define conResSpecCmdOfSecuServiceDef_GenerateServiceDef	conResCmdSpec001

// 密钥管理命令
// 本地生成一个密钥，并输出ZMK加密的密文
#define conResSpecCmdOfKeyMng_GenerateKeyLocally		conResCmdSpec001
// 本地存储一个ZMK加密的密钥
#define conResSpecCmdOfKeyMng_StoreKeyLocally		conResCmdSpec002
// 本地读取一个ZMK加密的密钥
#define conResSpecCmdOfKeyMng_ReadKeyLocally		conResCmdSpec003
// 本地随机生成一个ZMK加密的密钥
#define conResSpecCmdOfKeyMng_GenerateRandomKeyLocally	conResCmdSpec004
// 从远程申请下载当前密钥
#define conResSpecCmdOfKeyMng_ApplyCurrentKeyRemotely	conResCmdSpec005
// 从远程申请下载新密钥
#define conResSpecCmdOfKeyMng_ApplyNewKeyRemotely		conResCmdSpec006
// 向远程分发当前密钥
#define conResSpecCmdOfKeyMng_DeployCurrentKeyRemotely	conResCmdSpec007
// 向远程分发新密钥
#define conResSpecCmdOfKeyMng_DeployNewKeyRemotely		conResCmdSpec008
// 打印密钥信封
#define conResSpecCmdOfKeyMng_PrintKeyEnvolop		conResCmdSpec009
// 设置密钥信封格式
#define conResSpecCmdOfKeyMng_SetKeyPrintFormat		conResCmdSpec010
// 下载所密钥信封格式文件名称
#define conResSpecCmdOfKeyMng_DownloadAllKeyPrintFormatFile		conResCmdSpec011

// 密钥库备份服务器特殊命令
// 重置本地备份服务器的配置
#define conResSpecCmdOfHsmGrp_SetMyself		conResCmdSpec001
// 重置远程备份服务器的配置
#define conResSpecCmdOfHsmGrp_SetBrother		conResCmdSpec002
// 激活密钥备份服务器
#define conResSpecCmdOfHsmGrp_Active		conResCmdSpec003
// 关闭密钥备份服务器
#define conResSpecCmdOfHsmGrp_Inactive		conResCmdSpec004

// 加密机组特殊命令
// 查询所有记录文件
#define conResSpecCmdOfHsmGrp_QueryAllRecFile	conResCmdSpec001
// 设置密码机为异常状态
#define conResSpecCmdOfHsmGrp_SetHsmAbnormal	conResCmdSpec002
// 设置密码机为正常状态
#define conResSpecCmdOfHsmGrp_SetHsmNormal		conResCmdSpec003
// 设置密码机为备份状态
#define conResSpecCmdOfHsmGrp_SetHsmBackup		conResCmdSpec004
// 重置密码机的次数统计
#define conResSpecCmdOfHsmGrp_ResetHsmCounts	conResCmdSpec005
// 重置所有密码机的次数统计
#define conResSpecCmdOfHsmGrp_ResetAllHsmCounts	conResCmdSpec006
// 对密码机授权
#define conResSpecCmdOfHsmGrp_AuthorizeHsm		conResCmdSpec007
// 对密码机取消授权
#define conResSpecCmdOfHsmGrp_DisauthorizeHsm	conResCmdSpec008

// 活动客户端特殊命令
// 查询所有指定IP的客户端
#define conResSpecCmdOfCommConf_QueryAllOfIPAddr	conResCmdSpec001
// 查询所有指定说明的客户端
#define conResSpecCmdOfCommConf_QueryAllOfRemark	conResCmdSpec002
// 查询所有指定端口的客户端
#define conResSpecCmdOfCommConf_QueryAllOfPort	conResCmdSpec003
// 删除空闲的客户端
#define conResSpecCmdOfCommConf_DeleteIdleClient	conResCmdSpec004
// 重置指定客户端的属性
#define conResSpecCmdOfCommConf_ResetSpecRecAttr	conResCmdSpec005
//  重置所有客户端的属性
#define conResSpecCmdOfCommConf_ResetAllRecAttr	conResCmdSpec006

// 消息交换区特殊命令
// 重置消息等待处理的最大时间
#define conResSpecCmdOfMsgBuf_ResetMaxStayTime	conResCmdSpec001
// 重置所有消息位置可用
#define conResSpecCmdOfMsgBuf_ResetAllPosAvailable	conResCmdSpec002
// 显示所有等待处理的消息
#define conResSpecCmdOfMsgBuf_QueryAllWaitingMsg	conResCmdSpec003

// 日志特殊命令
// 重置用户数
#define conResSpecCmdOfLogTBL_ResetUsers		conResCmdSpec001

// 监控客户端控制表的特殊命令
// 打开对资源的监控
#define conResSpecCmdOfSpierClientTBL_OpenRes	conResCmdSpec001
// 关闭对资源的监控
#define conResSpecCmdOfSpierClientTBL_CloseRes	conResCmdSpec002
// 增加一个资源
#define conResSpecCmdOfSpierClientTBL_InsertRes	conResCmdSpec003
// 删除一个资源
#define conResSpecCmdOfSpierClientTBL_DeleteRes	conResCmdSpec004
// 修改一个资源
#define conResSpecCmdOfSpierClientTBL_ModifyRes	conResCmdSpec005
// 增加一个缺省客户端定义
#define conResSpecCmdOfSpierClientTBL_InsertDefaultClient	conResCmdSpec006
// 删除一个缺省客户端定义
#define conResSpecCmdOfSpierClientTBL_DeleteDefaultClient	conResCmdSpec007
// 查询所有资源
#define conResSpecCmdOfSpierClientTBL_QueryAllRes	conResCmdSpec008
// 查询指定资源
#define conResSpecCmdOfSpierClientTBL_QuerySpecRes	conResCmdSpec009

// 密码键盘管理
// 增加密码键盘
#define conResSpecCmdOfPinPad_Add		conResCmdSpec001
#define conResSpecCmdOfPinPad_Update		conResCmdSpec002
#define conResSpecCmdOfPinPad_Delete		conResCmdSpec003
// 初始化密码键盘
#define conResSpecCmdOfPinPad_Init		conResCmdSpec007
// 启用密码键盘
#define conResSpecCmdOfPinPad_Active		conResCmdSpec008
// 停用密码键盘
#define conResSpecCmdOfPinPad_InActive		conResCmdSpec004
// 查询所有密码键盘
#define conResSpecCmdOfPinPad_QueryAll		conResCmdSpec005
// 查询密码键盘
#define conResSpecCmdOfPinPad_QuerySpec 	conResCmdSpec006

// 监控客户端控制表的特殊命令
// 操作员登录
#define conResSpecCmdOfOperatorTBL_logon		conResCmdSpec001
// 操作员退出登录
#define conResSpecCmdOfOperatorTBL_logoff		conResCmdSpec002
// 强制操作员退出登录
#define conResSpecCmdOfOperatorTBL_ForceLogoff	conResCmdSpec003

// 监控任务的特殊命令
// 查询所有活动的任务类型
#define conResSpecCmdOfTaskTBL_QueryActiveClass	conResCmdSpec001
// 查询所有活动的任务实例
#define conResSpecCmdOfTaskTBL_QueryAllInstance	conResCmdSpec002
// 查询所有异常的任务类型
#define conResSpecCmdOfTaskTBL_QueryAbnormalClass	conResCmdSpec003
// 查询所有必须在线的任务类型
#define conResSpecCmdOfTaskTBL_QueryMustOnlineClass	conResCmdSpec004
//中止指定的任务类型程序
#define conResSpecCmdOfTaskTBL_KillTaskOfSpecClass	conResCmdSpec005
// 关闭指定的任务类型程序
#define conResSpecCmdOfTaskTBL_CloseTaskOfSpecClass	conResCmdSpec006
// 根据进程号中止指定的任务
#define conResSpecCmdOfTaskTBL_KillSpecTaskInstance	conResCmdSpec007

//工作类型定义表管理
//根据工作类型自动增加文档规划和文档档案
#define conResSpecCmdOfWorkType_AutoAddDocRecByID	conResCmdSpec001

// 加载共享内存映像
#define conResCmdReloadSharedMemoryImage                                 881
// 显示共享内存映像
#define conResCmdDisplaySharedMemoryImage                                882

// 读取主菜单的菜单项
#define conResCmdReadMainMenuItem	241
// 读取二级菜单的菜单项
#define conResCmdReadSecondaryMenuItem	242
// 读取界面右键菜单项
#define conResCmdReadInterfacePopupMenuItem	243
// 读取记录右键菜单项
#define conResCmdReadRecordPopupMenuItem	244
// 读取记录双击菜单项
#define conResCmdReadRecordDoubleClickMenuItem	245
// 读取所有主菜单
#define conResCmdReadAllMainMenuDef	246
// 读取菜单项的执行命令
#define conResCmdReadMenuItemCmd	251
// 读取右键菜单项的执行命令
#define conResCmdReadPopupMenuItem	252
// 修改密码
#define conResCmdUpdatePassword		231
// 从后台获取一个随机数
#define conResCmdGetOneRandomNumber	721

int UnionSetMyTransInfoResID(int resID);

int UnionGetMyTransInfoResID();

int UnionIsValidResType(TUnionResType type);

#endif

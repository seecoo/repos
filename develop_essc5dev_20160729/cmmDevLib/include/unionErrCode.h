#ifndef _UnionErrCode_
#define _UnionErrCode_

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// 错误码偏移量定义
#define errCodeOffsetOfDesKeyDBMDL		-11000	// DES密钥库模块
#define errCodeOffsetOfPKDBMDL			-12000	// 公开密钥库模块
#define errCodeOffsetOfTaskMDL			-13000	// 任务模块
#define errCodeOffsetOfEnviMDL			-14000	// 环境变量模块
#define errCodeOffsetOfKeyDBBackuperMDL		-15000	// 密钥库备份模块
#define errCodeOffsetOfLogMDL			-16000	// 日志模块
#define errCodeOffsetOfSharedMemoryMDL		-17000	// 共享内存模块
#define errCodeOffsetOfMsgBufMDL		-18000	// 消息交换区模块
#define errCodeOffsetOfOpertatorMDL		-19000	// 操作员模块
#define errCodeOffsetOfRECMDL			-20000	// REC模块
#define errCodeOffsetOfSckCommMDL		-21000	// 通讯模块
#define errCodeOffsetOfSocketMDL		-22000	// Socket模块
#define errCodeOffsetOfSJL06MDL			-23000	// SJL06工作组模块
#define errCodeOffsetOfTransClassDefMDL		-24000	// 交易定易模块
#define errCodeOffsetOfPackageDefMDL		-25000	// 报文模块
#define errCodeOffsetOfISO8583MDL		-26000	// 8583模块
#define errCodeOffsetOfEsscMDL			-29000	// 密码服务平台模块
#define errCodeOffsetOfYLQZMDL			-30000	// 银联前置机模块
#define errCodeOffsetOfKDBSvr			-31000	// 密钥库服务器模块
#define errCodeOffsetOfDatabase			-32000	// 数据库的错误码
#define errCodeOffsetOfBankBasicPackageMDL	-33000	// 一般银行报文模块
#define errCodeOffsetOfSJL08			-34000	// sjl08机的错误
#define errCodeOffsetOfSQL			-35000	// SQL模块的错误	// 2008/7/24增加
#define errCodeOffsetOfCmmPack			-36000	// 公共报文模块的错误	// 2008/9/26增加
#define errCodeOffsetOfCDP			-37000	// 公共设计平台模块的错误	// 2008/10/3增加
#define errCodeOffsetOfFileSvr			-38000	// 文件服务器模块的错误	// 2008/11/10增加
#define errCodeOffsetOfObject			-39000	// 对象的错误码		// 2008/3/12增加
#define errCodeOffsetOfLockMachanism		-40000	// 锁机制是		// 2008/3/12增加
#define errCodeOffsetOfHsm			-49000	// 密码机返回的错误码	// 2008/11/19增加
#define errCodeOffsetOfSimuUnixSharedMemory	-50000	// 仿真共享内存	// 2008/11/21增加
#define errCodeOffsetOfPBOC			-52000  // PBOC模块
#define errCodeOffsetOfOTPS			-51000	// 动态口令系统
#define errCodeOffsetOfDPS                      -53000  // 数据准备系统 
#define errCodeOffsetOfPKCS11			-59000	// pkcs11模块		// 2008/3/12增加
#define errCodeOffsetOfDataImageInMemory	-60000	// 数据映像		// 2009/1/8增加
#define errCodeOffsetOfBinaryFileDB		-61000	// 二进制文件模块	// 2009/3/23增加
#define errCodeOffsetOfOnlineKeyMng		-62000	// 在线密钥管理模块	// 2009/8/23增加
#define errCodeOffsetOfFileTransSvrMDL		-63000	// 文件传输模板的偏移
#define errCodeOffsetOfCertificate		-64000	// 证书模块		// 2012/3/30增加
#define errCodeOffsetOfMemcached		-65000	// memcached模块
#define errCodeOffsetOfCertMDL			-69000	// 证书模块
#define errCodeOffsetOfSJL05MDL			-70000	// SJL05工作组模块
#define errCodeOffsetOfHsmCmdMDL		-9000	// 密码机指令模块
#define errCodeOffsetOfCmbcEuspPackageMDL	-8000	// 民生银行报文模块
#define errCodeOffsetOfYLCommConfMDL		-7000	// 银联通讯模块
#define errCodeOffsetOfKeyCacheMDL		-6000	// 密钥缓冲模块
#define errCodeOffsetOfAPI			-5000	// API模块
#define errCodeOffsetOfTransSpierBufMDL		-4000	// 交易监控模块
#define errCodeOffset2OfEsscMDL			-3000	// 密码服务平台模块2
#define errCodeOffsetOfErrCodeMDL		-2000	// 错误代码模块
#define errCodeOffsetOfHsmReturnCodeMDL		-1000	// 密码机返回码
#define errCodeOffsetOfOracle			-900000	// ORACLE响应码偏移
#define errCodeOffsetOfDB2			-800000	// DB2响应码偏移
#define errCodeOffsetOfInformix			-700000	// INFORMIX响应码偏移
#define errCodeOffsetOfMysql			-600000	// Mysql响应码偏移
#define errCodeOffsetOfODBC			-500000	// ODBC响应码偏移
#define errCodeOffsetOfSqlite			-400000	// Sqlite响应码偏移
#define errCodeOffsetOfSRJ1401			-100000	// SRJ1401响应码偏移

// 以下是共用错误码
#define errCodeUseOSErrCode			(0-abs(errno))	// 使用操作系统的错误码
#define errCodeParameter			-10001	// 参数错误
#define errCodeSharedMemoryModule		-10002	// 共享内存错
#define errCodeCreateTaskInstance		-10003	// 创建任务出错
#define errCodeSmallBuffer			-10004	// 太小的缓冲区
#define errCodeInvalidIPAddr			-10005	// 非法的IP地址
#define errCodeCallThirdPartyFunction		-10006	// 调用第三方函数出错
#define errCodeTooShortLength			-10007	// 长度太小
#define errCodeUserSelectExit			-10008	// 用户选择了退出
#define errCodeDefaultErrCode			-10009	// 缺省错误码
#define errCodeIsRemarkLine			-10010	// 是注释行
#define errCodeNoneOperationDefined		-10011	// 没定义操作命令
#define errCodeNoneVarDefined			-10012	// 没定义参数
#define errCodeTimeout				-10013	// 超时
#define errCodeOutRange				-10014	// 超界
#define errCodeUserForgetSetErrCode		-10015	// 程序员忘记了置错误码
#define errCodeUserRequestHelpInfo		-10016	// 用户要求帮助信息 2007/11/30增加
#define errCodeTimerNotStart			-10017	// 计时没有开始 2008/6/20增加
#define errCodeLiscenceCodeInvalid		-10018	// 授权码错，2008/7/16增加
#define errCodeTcpipSvrNotDefined		-10019	// tcpipSvr没有定义
#define errCodeFileEnd				-10020	// 文件结束了
#define errCodeRequestAndResponseNotSame	-10021	// 请求与响应不匹配
#define errCodeKeyWordAlreadyExists		-10022	// 关键字已存在
#define errCodeKeyWordNotExists			-10023	// 关键字不存在
#define errCodeKeyWordIsMyself			-10024	// 关键字是本身
#define errCodeLockKeyWord			-10025	// 锁关键字出错
#define errCodeVarNameNotCorrect		-10026	// 变量名称错
#define errCodeTestFileContentError		-10027	// 测试数据文件内容有错
#define errCodeSckConnNoData			-10028	// socket连接上没有传送数据	Mary add, 20081225
#define errCodeMarcoDefNameNotDefined		-10029	// 宏定义名称没定义
#define errCodeMarcoDefValueNotDefined		-10030	// 宏定义值没定义
#define errCodeDefaultValueTooLong		-10031	// 缺省值太长
#define errCodeFileAlreadyExists		-10032	// 文件已存在
#define errCodeNullPointer			-10033	// 空指针
#define errCodeNullRecPointer			-10034	// 空记录指针
#define errCodeNullRecStrPointer		-10035	// 空记录串指针
#define errCodeTableNameReserved		-10036	// 表名是保留表名
#define errCodeTooManyLoopTimes			-10037	// 循环太多次了
#define errCodeNodeIDNotFoundInNetDef		-10038	// 节点定义没有找到
#define errCodeTooManyParentsNode		-10039	// 父节点太多
#define errCodeTheTwoNodeIsTheSame		-10040	// 同一节点
#define errCodeNodeNotParentOfTheSpecNode	-10041	// 不是该节点的父节点
#define errCodeWrongNetLayer			-10042	// 节点层错误
#define errCodeTooManyNetNodes			-10043	// 节点太多
#define errCodeInvalidKeyDBType			-10044	// 非法的密钥库类型
#define errCodeErrCounterGrpIsFull		-10045	// 错误码计数器表满了
#define errCodeErrCounterGrpNotConnected	-10046	// 错误码计数器表未连接
#define errCodeNameIsUsed			-10047	// 名称已被使用
#define errCodeTooManyRetryTimes		-10048	// 太多重试次数
#define errCodeUserInputWrongParametersNum	-10049	// 用户输入了错误的参数数量
#define errCodeHelpToUser			-10050	// 向用户提供联机帮助
#define errCodePeerCloseSckConn			-10051	// 对端关闭了连接
#define errCodeAlgorithmUsingNonKey		-10052	// 算法不需要用到密钥
#define errCodeMngSvrClientParameter		-10053	// 客户端参数错误
#define errCodeMngSvrHsmIPAddrNotSet		-10054	// 没有为配置服务器指定加密机IP地址
#define errCodeMngSvrFileNotExists		-10055	// 服务器上文件不存在
#define errCodeFunctionNotSupported		-10056	// 不支持此函数
#define errCodeInvalidDataInFile		-10057	// 文件中有非法数据
#define errCodeInvalidReportMethod		-10058	// 非法的报表函数
#define errCodeNoDataFoundInQueue		-10059	// 队列中已无数据
#define errCodeReportMethodNotDefined		-10060	// 报表生成方法没有定义
#define errCodeTestingResultNotSameAsExpected	-10061	// 测试结果与预期不符
#define errCodeInvalidFileName			-10062	// 非法文件名
#define errCodeInvalidService			-10063	// 非法服务
#define errCodeHashValueErr			-10064	// 哈希值错误
#define errCodeRecordNotExists                  -10065  //记录不存在 
#define errCodeDPFileCheck			-10066  // 抽检失败

#define errCodeAPIParameter						(errCodeOffsetOfAPI-1)		// API参数错误
#define errCodeAPIReqStrTooLong						(errCodeOffsetOfAPI-2)		// 请求串太长
#define errCodeAPIReqStrTooShort					(errCodeOffsetOfAPI-3)		// 请求串太短
#define errCodeAPIWrongPackRecveived					(errCodeOffsetOfAPI-4)		// 收到错误的包
#define errCodeAPICommWithEssc						(errCodeOffsetOfAPI-5)		// 与ESSC通讯出错
#define errCodeAPIBufferSmallForRecvData				(errCodeOffsetOfAPI-6)		// 数据缓冲不足接收收到的数据
#define errCodeAPIRecvDataLenNotEqualDefinedLen				(errCodeOffsetOfAPI-7)		// 收到的数据长度与定义的长度不符
#define errCodeAPISvrReturnZeroLenAnswer				(errCodeOffsetOfAPI-8)		// 服务器返回了长度为0的响应
#define errCodeAPISvrReturnTooShortAnswer				(errCodeOffsetOfAPI-9)		// 服务器返回了长度太短的响应
#define errCodeAPISvrReturnWrongKeyLen					(errCodeOffsetOfAPI-10)		// 服务器返回的密钥长度错
#define errCodeAPIShouldReturnMinusButRetIsNotMinus			(errCodeOffsetOfAPI-11)		// 应该返回一个错误，但程序中的返回值不是负数
#define errCodeAPIEsscSvrIPAddrNotDefined				(errCodeOffsetOfAPI-12)		// ESSCIPAddr没有定义
#define errCodeAPIEsscSvrPortNotDefined					(errCodeOffsetOfAPI-13)		// ESSC端口没有定义
#define errCodeAPIEsscSvrPortInvalid					(errCodeOffsetOfAPI-14)		// ESSC端口错误
#define errCodeAPIEsscSvrTimeoutInvalid					(errCodeOffsetOfAPI-15)		// ESSC的超时值非法
#define errCodeAPIEsscSvrIDOfAppApiInvalid				(errCodeOffsetOfAPI-16)		// ESSC的客户端应用ID
#define errCodeAPIClientReqLen						(errCodeOffsetOfAPI-17)		// 客户端请求长度错
#define errCodeAPIRecvClientReqTimeout					(errCodeOffsetOfAPI-18)		// 接收客户端请求超时
#define errCodeAPIErrCodeNotSet						(errCodeOffsetOfAPI-19)		// 未设置出错码
#define errCodeAPIPackageTooShort					(errCodeOffsetOfAPI-20)		// 包长度太短
#define errCodeAPIPackageNotRequest					(errCodeOffsetOfAPI-21)		// 不是请求包
#define errCodeAPIPackageNotResponse					(errCodeOffsetOfAPI-22)		// 不是响应包
#define errCodeAPIInvalidService					(errCodeOffsetOfAPI-23)		// 非法服务

// 文件传输模板的错误码
#define errCodeFileTransSvrNoFileActionStrategyPrimaryKeyDefinedInReqStr		(errCodeOffsetOfFileTransSvrMDL-1)	// 请求串中未定义文件传输动作的关键字
#define errCodeFileTransSvrReadFileActionStrategy			(errCodeOffsetOfFileTransSvrMDL-2)	// 读取文件传输策略出错
#define errCodeFileTransSvrThisLevelNotAuthorized			(errCodeOffsetOfFileTransSvrMDL-3)	// 该级别未被授权
#define errCodeFileTransSvrThisPersonNotAuthorized			(errCodeOffsetOfFileTransSvrMDL-4)	// 该人未被授权
#define errCodeFileTransSvrInvalidClient				(errCodeOffsetOfFileTransSvrMDL-5)	// 非法的发起节点
#define errCodeFileTransSvrNoFileName					(errCodeOffsetOfFileTransSvrMDL-6)	// 未指定文件名
#define errCodeFileTransSvrDataStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-7)	// 文件加密定义有错
#define errCodeFileTransSvrMacStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-8)	// 文件校验定义有错
#define errCodeFileTransSvrSignStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-9)	// 文件认证定义有错
#define errCodeFileTransSvrResendForbidden				(errCodeOffsetOfFileTransSvrMDL-10)	// 不允许重新发送
#define errCodeFileTransSvrTransferFinished				(errCodeOffsetOfFileTransSvrMDL-11)	// 已发送完成
#define errCodeFileTransSvrTransferStillActive				(errCodeOffsetOfFileTransSvrMDL-12)	// 传输仍在进行中
#define errCodeFileTransSvrPackAuthResStr				(errCodeOffsetOfFileTransSvrMDL-13)	// 拼装授权响应包出错
#define errCodeFileTransSvrInsertOnlineTransLog				(errCodeOffsetOfFileTransSvrMDL-14)	// 插入传输流水出错
#define errCodeFileTransSvrInvalidOriNode				(errCodeOffsetOfFileTransSvrMDL-15)	// 非法的源节点
#define errCodeFileTransSvrInvalidDesNode				(errCodeOffsetOfFileTransSvrMDL-16)	// 非法的目标节点
#define errCodeFileTransSvrFileSizeNotDefined				(errCodeOffsetOfFileTransSvrMDL-17)	// 文件大小未定义
#define errCodeFileTransSvrFileSizeSmallThanOffset			(errCodeOffsetOfFileTransSvrMDL-18)	// 文件大小小于文件偏移
#define errCodeFileTransSvrSenderMustApplyAuthorizationFirst		(errCodeOffsetOfFileTransSvrMDL-19)	// 发送方还未申请授权
#define errCodeFileTransSvrInvalidServiceCode				(errCodeOffsetOfFileTransSvrMDL-20)	// 非法服务代码
#define errCodeFileTransSvrInvalidTransDirection			(errCodeOffsetOfFileTransSvrMDL-21)	// 非法的传输方向
#define errCodeFileTransSvrReadCurrentLogRec				(errCodeOffsetOfFileTransSvrMDL-22)	// 读取当前流水记录
#define errCodeFileTransSvrAuthorizationForbiddednOnCurrentStatus	(errCodeOffsetOfFileTransSvrMDL-23)	// 不允许对当前状态的流水进行授权
#define errCodeFileTransSvrInvalidTransStatusChange			(errCodeOffsetOfFileTransSvrMDL-24)	// 非法的传输状态变化
#define errCodeFileTransSvrInvalidTransStatus				(errCodeOffsetOfFileTransSvrMDL-25)	// 非法的传输状态
#define errCodeFileTransSvrInvalidFileOffset				(errCodeOffsetOfFileTransSvrMDL-26)	// 非法的偏移位置
#define errCodeFileTransSvrInvalidDataLen				(errCodeOffsetOfFileTransSvrMDL-27)	// 非法的传输长度
#define errCodeFileTransSvrInvalidFileSize				(errCodeOffsetOfFileTransSvrMDL-28)	// 文件大小出错
#define errCodeFileTransSvrInvalidFileControlType			(errCodeOffsetOfFileTransSvrMDL-29)	// 非法的文件授权控制
#define errCodeFileTransSvrOneSideNotFinishTransmitting			(errCodeOffsetOfFileTransSvrMDL-30)	// 有一方还没有完成传输
#define errCodeFileTransSvrCancelByCancelService			(errCodeOffsetOfFileTransSvrMDL-31)	// 调用中止服务中止了传输
#define errCodeFileTransSvrCancelArtificially				(errCodeOffsetOfFileTransSvrMDL-32)	// 人工中止了传输
#define errCodeFileTransSvrInvalidEngineInstance			(errCodeOffsetOfFileTransSvrMDL-33)	// 不同的传输实例
#define errCodeFileTransSvrOriNodeDirNotSame				(errCodeOffsetOfFileTransSvrMDL-34)	// 控制中心定义的源节点目录与本地的不一致
#define errCodeFileTransSvrInvalidPackNo				(errCodeOffsetOfFileTransSvrMDL-35)	// 非法的包序列
#define errCodeFileTransSvrWithoutFinishedPack				(errCodeOffsetOfFileTransSvrMDL-36)	// 不是结束包
#define errCodeFileTransSvrSenderNotCloseConn				(errCodeOffsetOfFileTransSvrMDL-37)	// 发送方未关闭连接
#define errCodeFileTransSvrReceiverWriteFile				(errCodeOffsetOfFileTransSvrMDL-38)	// 接收方写文件出错
#define errCodeFileTransSvrSendDataAndRecvDataLenNotSame		(errCodeOffsetOfFileTransSvrMDL-39)	// 接收方和发送方长度不一致
#define errCodeFileTransSvrPlainDataLenErrorAfterDecrypt		(errCodeOffsetOfFileTransSvrMDL-40)	// 解密后的数据长度错
#define errCodeFileTransSvrFunTypeNotDefined				(errCodeOffsetOfFileTransSvrMDL-41)	// 非法的功能标识
#define errCodeFileTransSvrSecurityProtocolNotDefined			(errCodeOffsetOfFileTransSvrMDL-42)	// 非法的安全协议
#define errCodeFileTransSvrMaxClientNo					(errCodeOffsetOfFileTransSvrMDL-43)	// 超过最大的客户端数量
#define errCodeFileTransSvrCreateFileReceiverHandleFail			(errCodeOffsetOfFileTransSvrMDL-44)	//文件传输句柄创建失败
#define errCodeFileTransSvrReciveFileFail				(errCodeOffsetOfFileTransSvrMDL-45)	//接收文件失败
#define errCodeFileTransSvrMkdirFail					(errCodeOffsetOfFileTransSvrMDL-45)	//创建目录失败
// 数据映像	// 2009/1/8增加
#define errCodeDataImageInMemoryNotExists				(errCodeOffsetOfDataImageInMemory-1)	// 数据映像不存在
#define errCodeDataImageInMemoryInsertError				(errCodeOffsetOfDataImageInMemory-2)	// 数据映像增加失败

// // 文件服务器模块的错误	// 2008/11/10增加
#define errCodeFileSvrMDL_InvalidCmd					(errCodeOffsetOfFileSvr-1)		// 非法的文件服务器指令

// 锁机制
#define errCodeLockMachanism_WritingLockedAlready			(errCodeOffsetOfLockMachanism-1)		// 已经被写锁
#define errCodeLockMachanism_InvalidLockRecPos				(errCodeOffsetOfLockMachanism-2)		// 非法的锁记录位置
#define errCodeLockMachanism_ResNotLocked				(errCodeOffsetOfLockMachanism-3)		// 资源没有被锁住
#define errCodeLockMachanism_ResWritingLockedNotExists			(errCodeOffsetOfLockMachanism-4)		// 资源没有被写锁住
#define errCodeLockMachanism_ResReadingLockedNotExists			(errCodeOffsetOfLockMachanism-5)		// 资源没有被读锁住
#define errCodeLockMachanism_ResWritingLockedNotPermittedWhenRecWritingLocked	(errCodeOffsetOfLockMachanism-6)		// 记录被写锁时，不允许写锁资源
#define errCodeLockMachanism_RecWritingLockedAlready			(errCodeOffsetOfLockMachanism-7)		// 已经被写锁
#define errCodeLockMachanism_RecNotLocked				(errCodeOffsetOfLockMachanism-8)		// 记录没有被锁住
#define errCodeLockMachanism_RecWritingLockedNotExists			(errCodeOffsetOfLockMachanism-9)		// 记录没有被写锁住
#define errCodeLockMachanism_RecReadingLockedNotExists			(errCodeOffsetOfLockMachanism-10)		// 记录没有被读锁住
#define errCodeLockMachanism_RecockedNotPermittedWhenResWritingLocked	(errCodeOffsetOfLockMachanism-11)		// 资源被写锁时，不允许锁记录
#define errCodeLockMachanism_WritingLockedNotPermittedWhenReadingLocked	(errCodeOffsetOfLockMachanism-12)		// 资源被读锁时，不允许写锁


// 仿真共享内存
#define errCodeSimuUnixSHM_SHMNotInitialized				(errCodeOffsetOfSimuUnixSharedMemory-1)		// 共享内存没有定义
#define errCodeSimuUnixSHM_SizeLargerThanExisted			(errCodeOffsetOfSimuUnixSharedMemory-2)		// 申请的共享内存尺寸比预期的大
#define errCodeSimuUnixSHM_SHMBufferIsFull				(errCodeOffsetOfSimuUnixSharedMemory-3)		// 共享内存尺满了
#define errCodeSimuUnixSHM_NotExists					(errCodeOffsetOfSimuUnixSharedMemory-4)		// 共享内存不存在

// 公共设计平台模块的错误	2008/10/3增加
#define errCodeCDPMDL_TooManyRequestDatagramFld				(errCodeOffsetOfCDP-1)		// 定义了太多报文请求域
#define errCodeCDPMDL_TooManyResponseDatagramFld			(errCodeOffsetOfCDP-2)		// 定义了太多报文响应域
#define errCodeCDPMDL_TooManyFunVarDefined				(errCodeOffsetOfCDP-3)		// 定义了太多函数参数
#define errCodeCDPMDL_VarTypeNotDefined					(errCodeOffsetOfCDP-4)		// 没有定义变量类型
#define errCodeCDPMDL_VarNameNotDefined					(errCodeOffsetOfCDP-5)		// 没有定义变量名称
#define errCodeCDPMDL_ArrayDimisionNotDefined				(errCodeOffsetOfCDP-6)		// 数组的维度没有定义
#define errCodeCDPMDL_ArrayDimisionSizeNotDefined			(errCodeOffsetOfCDP-7)		// 数组的大小没有定义
#define errCodeCDPMDL_ReadVarFailure					(errCodeOffsetOfCDP-8)		// 读变量失败
#define errCodeCDPMDL_InvalidVarType					(errCodeOffsetOfCDP-9)		// 非法变量类型
#define errCodeCDPMDL_NotBaseVarType					(errCodeOffsetOfCDP-10)		// 不是基础变量类型
#define errCodeCDPMDL_ConstNameNotDefined				(errCodeOffsetOfCDP-11)		// 常量名称没有定义
#define errCodeCDPMDL_ConstNotDefined					(errCodeOffsetOfCDP-12)		// 常量没有定义
#define errCodeCDPMDL_PointerNotDefined					(errCodeOffsetOfCDP-13)		// 指针类型没有定义
#define errCodeCDPMDL_ArrayNotDefined					(errCodeOffsetOfCDP-14)		// 数组类型没有定义
#define errCodeCDPMDL_SimpleTypeNotDefined				(errCodeOffsetOfCDP-15)		// 简单类型没有定义
#define errCodeCDPMDL_NotIntConst					(errCodeOffsetOfCDP-16)		// 不是整型常量
#define errCodeCDPMDL_ArraySizeNotSet					(errCodeOffsetOfCDP-17)		// 没有设置数组大小
#define errCodeCDPMDL_VarValueTagNotDefined				(errCodeOffsetOfCDP-18)		// 没有定义变量取值的标志
#define errCodeCDPMDL_VarBaseTypeTagNotDefined				(errCodeOffsetOfCDP-19)		// 没有定义变量取值的类型标志
#define errCodeCDPMDL_NoValueForSpecVarValueTagDefined			(errCodeOffsetOfCDP-20)		// 没有为指定的变量取值标识定义值
#define errCodeCDPMDL_NoValidConstType					(errCodeOffsetOfCDP-21)		// 非法的常数类型
#define errCodeCDPMDL_InvalidKeyWord					(errCodeOffsetOfCDP-22)		// 非法关键字
#define errCodeCDPMDL_GlobalVariableNotDefined				(errCodeOffsetOfCDP-23)		// 全局变量没定义
#define errCodeCDPMDL_DatagramFldTagNotDefined				(errCodeOffsetOfCDP-24)		// 报文域标识没有定义
#define errCodeCDPMDL_DatagramHeaderDefinedError			(errCodeOffsetOfCDP-25)		// 报文头定义错
#define errCodeCDPMDL_DatagramClassNotDefined				(errCodeOffsetOfCDP-26)		// 报文类型没有定义
#define errCodeCDPMDL_ModuleNotDefined					(errCodeOffsetOfCDP-27)		// 模块没有定义
#define errCodeCDPMDL_ModuleDevDirNotDefined				(errCodeOffsetOfCDP-28)		// 模块开发目录没有定义
#define errCodeCDPMDL_ProgramNotDefined					(errCodeOffsetOfCDP-29)		// 程序没有定义
#define errCodeCDPMDL_DatagramFldAlaisNotDefined			(errCodeOffsetOfCDP-30)		// 报文域别名没有定义
#define errCodeCDPMDL_InvalidCalcuOperator				(errCodeOffsetOfCDP-31)		// 非法运算符
#define errCodeCDPMDL_InvalidRelationCalcuOperator			(errCodeOffsetOfCDP-32)		// 非法关系运算符

// 公共报文模块的错误	// 2008/9/26增加
#define errCodeCmmPackMDL_DatatFldLength				(errCodeOffsetOfCmmPack-1)		// 数据域长度错
#define errCodeCmmPackMDL_DatatLengthToShort				(errCodeOffsetOfCmmPack-2)		// 数据域太短
#define errCodeCmmPackMDL_FldNotDefined					(errCodeOffsetOfCmmPack-3)		// 域没有定义
#define errCodeCmmPackMDL_PackIDNotIdentified				(errCodeOffsetOfCmmPack-4)		// 不是匹配包

// SQL模块的错误	2008/7/24 增加
#define errCodeSQLInvalidFldCompareCondition				(errCodeOffsetOfSQL-1)		// 非法的比较条件
#define errCodeSQLInvalidSQLStr						(errCodeOffsetOfSQL-2)		// 非法的SQL条件串
#define errCodeSQLRecFldNameTooLong					(errCodeOffsetOfSQL-3)		// 域名太长
#define errCodeSQLInvalidFldName					(errCodeOffsetOfSQL-4)		// 域名非法
#define errCodeSQLNullFldName						(errCodeOffsetOfSQL-5)		// 域名为空
#define errCodeSQLFldAssignMethodNotSet					(errCodeOffsetOfSQL-6)		// 未定义域赋值方法
#define errCodeSQLFldAssignMethodNotSupport				(errCodeOffsetOfSQL-7)		// 不支持的域赋值方法
#define errCodeSQLPrimaryKeyIsNull					(errCodeOffsetOfSQL-8)		// 关键字是空串
#define errCodeSQLPrimaryKeyUpdateNotPermitted				(errCodeOffsetOfSQL-9)		// 关键字不允许被修改
#define errCodeSQLFldAssignMethodInvalid				(errCodeOffsetOfSQL-10)		// 域赋值方法错误

// 对象错误码，2008/3/12 增加
#define errCodeObjectMDL_ReadObjectDefinition				(errCodeOffsetOfObject-1)	// 读对象定义
#define errCodeObjectMDL_ObjectDefinitionAlreadyExists			(errCodeOffsetOfObject-2)	// 对象定义已存在
#define errCodeObjectMDL_ObjectDefinitionFileLen			(errCodeOffsetOfObject-3)	// 对象定义文件长度错
// Mary add begin, 2008-8-18
#define errCodeObjectMDL_ObjectDefinitionNotExists			(errCodeOffsetOfObject-4)	// 对象定义不存在
#define errCodeObjectMDL_ObjectNameError				(errCodeOffsetOfObject-5)	// 对象名称错误
#define errCodeObjectMDL_ObjectRecordNotExist				(errCodeOffsetOfObject-6)	// 对象实例未找到
#define errCodeObjectMDL_ForeignObjectNotExist				(errCodeOffsetOfObject-7)	// 外部对象不存在
#define errCodeObjectMDL_ForeignRecordNotExist				(errCodeOffsetOfObject-8)	// 外部引用实例不存在
#define errCodeObjectMDL_ObjectIsInvalid				(errCodeOffsetOfObject-9)	// 对象定义不合法
#define errCodeObjectMDL_RecordExistAlready				(errCodeOffsetOfObject-10)	// 对象的实例仍然存在
#define errCodeObjectMDL_ObjectIsReferenced				(errCodeOffsetOfObject-11)	// 对象的仍被其它对象引用
#define errCodeObjectMDL_FieldNumberError				(errCodeOffsetOfObject-12)	// 域数目不正确
#define errCodeObjectMDL_FieldValueIsInvalid				(errCodeOffsetOfObject-13)	// 域值定义不合法
#define errCodeObjectMDL_FieldNotExist					(errCodeOffsetOfObject-14)	// 域不存在
#define errCodeObjectMDL_PrimaryKeyIsRepeat				(errCodeOffsetOfObject-15)	// 键值重复
#define errCodeObjectMDL_UniqueKeyIsRepeat				(errCodeOffsetOfObject-16)	// 唯一值重复
#define errCodeObjectMDL_RecordIsReferenced				(errCodeOffsetOfObject-17)	// 实例仍然被其它对象实例引用
#define errCodeObjectMDL_ObjectHasNoRecord				(errCodeOffsetOfObject-18)	// 该对象没有实例
#define errCodeObjectMDL_FieldCanNotUpdate				(errCodeOffsetOfObject-19)	// 该域不能修改
#define errCodeObjectMDL_FieldNumberTooMuch				(errCodeOffsetOfObject-20)	// 域数目太多
#define errCodeObjectMDL_FieldTypeInvalid				(errCodeOffsetOfObject-21)	// 域类型不对
#define errCodeObjectMDL_FieldValueTooLong				(errCodeOffsetOfObject-22)	// 域值太长
#define errCodeObjectMDL_FieldListDefError				(errCodeOffsetOfObject-23)	// 域清单定义错
#define errCodeObjectMDL_RecordNumverTooMuch				(errCodeOffsetOfObject-24)	// 记录数据太多
// Mary add end, 2008-8-18

// pkcs11模块
#define errCodeOffsetOfPkcs11						errCodeOffsetOfPKCS11

// 密钥库模块
#define errCodeKDBBufMDL_ConnectIndexTBL				(errCodeOffsetOfKDBSvr-1)	// 连接模块
#define errCodeKDBBufMDL_InitDef					(errCodeOffsetOfKDBSvr-2)	// 初始化模块

// 错误代码模块
#define errCodeOffsetOfErrCodeMDL_CliErrCodeMustSpecified		(errCodeOffsetOfErrCodeMDL-1)	// 客户端错误代码必须指明
#define errCodeOffsetOfErrCodeMDL_CodeNotDefined			(errCodeOffsetOfErrCodeMDL-2)	// 错误码没定义
#define errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined			(errCodeOffsetOfErrCodeMDL-3)	// 错误码已经定义

// 银联通讯模块
// offset = -7000
#define errCodeYLCommConfMDL_CityIDNotDefined				(errCodeOffsetOfYLCommConfMDL-1)	// 城市码未定义
#define errCodeYLCommConfMDL_CommConfNotDefined				(errCodeOffsetOfYLCommConfMDL-2)	// 通讯配置未定义
#define errCodeYLCommConfMDL_AddCommConf				(errCodeOffsetOfYLCommConfMDL-3)	// 增加指定的通讯配置

// offset = -8000
// 民生银行主机报文Cmbc错误
#define errCodeCmbcEuspPackageMDL_InvalidDDU				(errCodeOffsetOfCmbcEuspPackageMDL-1)	// 非法的DDU
#define errCodeCmbcEuspPackageMDL_LenOfValueLargerThanMaxLenDefined	(errCodeOffsetOfCmbcEuspPackageMDL-2)	// 值的长度大于定义的值
#define errCodeCmbcEuspPackageMDL_DDULength				(errCodeOffsetOfCmbcEuspPackageMDL-3)	// DDU的长度错
#define errCodeCmbcEuspPackageMDL_DDUValueIsNull			(errCodeOffsetOfCmbcEuspPackageMDL-4)	// DDU的值是空
#define errCodeCmbcEuspPackageMDL_LenOfValueLargerThanStr		(errCodeOffsetOfCmbcEuspPackageMDL-5)	// 值的长度比串的长度大
#define errCodeCmbcEuspPackageMDL_FldOfNameNotDefinedInSpecForm		(errCodeOffsetOfCmbcEuspPackageMDL-6)	// 在表格中未定义该域
#define errCodeCmbcEuspPackageMDL_InvalidFormPointer			(errCodeOffsetOfCmbcEuspPackageMDL-7)	// 非法的表格指针
#define errCodeCmbcEuspPackageMDL_FormIsFull				(errCodeOffsetOfCmbcEuspPackageMDL-8)	// 表格为空
#define errCodeCmbcEuspPackageMDL_FormInvalid				(errCodeOffsetOfCmbcEuspPackageMDL-9)	// 表格非法
#define errCodeCmbcEuspPackageMDL_CreateFormRec				(errCodeOffsetOfCmbcEuspPackageMDL-10)	// 创建表格记录出错
#define errCodeCmbcEuspPackageMDL_FormCurrentRecNotInitialized		(errCodeOffsetOfCmbcEuspPackageMDL-11)	// 表格的当前记录未初始化
#define errCodeCmbcEuspPackageMDL_LocalAndRemoteFormNameNotSame		(errCodeOffsetOfCmbcEuspPackageMDL-12)	// 本地和远端的表格名不相同
#define errCodeCmbcEuspPackageMDL_LocalAndRemoteFldNameNotSame		(errCodeOffsetOfCmbcEuspPackageMDL-13)	// 本地和远端的域名不相同
#define errCodeCmbcEuspPackageMDL_DDUOfNameNotFoundInTransData		(errCodeOffsetOfCmbcEuspPackageMDL-14)	// 交易数据中没有该名称的DDU
#define errCodeCmbcEuspPackageMDL_FormOfNameNotFoundInTransData		(errCodeOffsetOfCmbcEuspPackageMDL-15)	// 交易数据中没有该名称的表格
#define errCodeCmbcEuspPackageMDL_NotSupportedUnit			(errCodeOffsetOfCmbcEuspPackageMDL-16)	// 不支持的单元
#define errCodeCmbcEuspPackageMDL_ConnectCmbcEuspPackageMDL		(errCodeOffsetOfCmbcEuspPackageMDL-17)	// 联接CmbcEusp报文模块出错
#define errCodeCmbcEuspPackageMDL_CmbcEuspPackageNotConnected		(errCodeOffsetOfCmbcEuspPackageMDL-18)	// CmbcEusp报文模块未联连

// offset = -9000
// 密码机指令错误
#define errCodeHsmCmdMDL_ReturnLen			(errCodeOffsetOfHsmCmdMDL-1)	// 返回长度错	
#define errCodeHsmCmdMDL_NoMK				(errCodeOffsetOfHsmCmdMDL-2)	// 没有主密钥
#define errCodeHsmCmdMDL_WrongTerminalKey		(errCodeOffsetOfHsmCmdMDL-3)	// 错误的终端密钥
#define errCodeHsmCmdMDL_WKParity			(errCodeOffsetOfHsmCmdMDL-4)	// 密钥的奇偶校验错
#define errCodeHsmCmdMDL_InvalidTerminalKeyIndex	(errCodeOffsetOfHsmCmdMDL-5)	// 非法的终端密钥索引
#define errCodeHsmCmdMDL_InvalidBmkIndex		(errCodeOffsetOfHsmCmdMDL-6)	// 非法的BMK索引
#define errCodeHsmCmdMDL_MacOrPinCheckFailure		(errCodeOffsetOfHsmCmdMDL-7)	// MAC或者密钥校验错
#define errCodeHsmCmdMDL_FirstPikParity			(errCodeOffsetOfHsmCmdMDL-8)	// 第一个PIK奇偶校验错
#define errCodeHsmCmdMDL_SecondPikParity		(errCodeOffsetOfHsmCmdMDL-9)	// 第二个PIK奇偶校验错
#define errCodeHsmCmdMDL_InvalidPinType			(errCodeOffsetOfHsmCmdMDL-10)	// 非法的PIN类型
#define errCodeHsmCmdMDL_MacDataLength			(errCodeOffsetOfHsmCmdMDL-11)	// MAC数据的长度错
#define errCodeHsmCmdMDL_Mak1Parity			(errCodeOffsetOfHsmCmdMDL-13)	// 第一个ZAK奇偶校验错
#define errCodeHsmCmdMDL_Mak2Parity			(errCodeOffsetOfHsmCmdMDL-14)	// 第二个ZAK奇偶校验错
#define errCodeHsmCmdMDL_PinType			(errCodeOffsetOfHsmCmdMDL-15)	// PIN的类型错
#define errCodeHsmCmdMDL_InvalidCmd			(errCodeOffsetOfHsmCmdMDL-16)	// 非法的指令
#define errCodeHsmCmdMDL_CmdTooShort			(errCodeOffsetOfHsmCmdMDL-17)	// 指令太短
#define errCodeHsmCmdMDL_CmdTooLong			(errCodeOffsetOfHsmCmdMDL-18)	// 找令太长
#define errCodeHsmCmdMDL_CommError			(errCodeOffsetOfHsmCmdMDL-19)	// 通讯出错
#define errCodeHsmCmdMDL_InvalidChar			(errCodeOffsetOfHsmCmdMDL-20)	// 非法字符
#define errCodeHsmCmdMDL_Timeout			(errCodeOffsetOfHsmCmdMDL-21)	// 超时
#define errCodeHsmCmdMDL_NoBmkOrBmkParity		(errCodeOffsetOfHsmCmdMDL-22)	// 没有BMK或BMK奇偶校验错
#define errCodeHsmCmdMDL_Unknown			(errCodeOffsetOfHsmCmdMDL-23)	// 未知错误
#define errCodeHsmCmdMDL_ErrCodeNotSuccess		(errCodeOffsetOfHsmCmdMDL-24)	// 返回的错误码不是00
#define errCodeHsmCmdMDL_VerifyFailure			(errCodeOffsetOfHsmCmdMDL-25)	// 验证失败
#define errCodeHsmCmdMDL_KeyLen				(errCodeOffsetOfHsmCmdMDL-26)	// 密钥长度
#define errCodeHsmCmdMDL_InvalidKeyType			(errCodeOffsetOfHsmCmdMDL-27)	// 密钥类型
#define errCodeHsmCmdMDL_KeyLenFlag			(errCodeOffsetOfHsmCmdMDL-28)	// 密钥长度标识
#define errCodeHsmCmdMDL_HsmNotAvailable		(errCodeOffsetOfHsmCmdMDL-29)	// 密码机不可用
#define errCodeHsmCmdMDL_PinByLmk0203			(errCodeOffsetOfHsmCmdMDL-30)	// LMK02-03加密的PIN错
#define errCodeHsmCmdMDL_InvalidPinInputData		(errCodeOffsetOfHsmCmdMDL-31)	// PIN输入数据
#define errCodeHsmCmdMDL_PrinterNotReady		(errCodeOffsetOfHsmCmdMDL-32)	// 打印机没准备好
#define errCodeHsmCmdMDL_HSMNotAuthorized		(errCodeOffsetOfHsmCmdMDL-33)	// 加密机没有授权
#define errCodeHsmCmdMDL_FormatNotLoaded		(errCodeOffsetOfHsmCmdMDL-34)	// 格式未加载
#define errCodeHsmCmdMDL_DieboldTableInvalid		(errCodeOffsetOfHsmCmdMDL-35)	// DieboldTable
#define errCodeHsmCmdMDL_PinBlock			(errCodeOffsetOfHsmCmdMDL-36)	// PinBlock
#define errCodeHsmCmdMDL_InvalidIndex			(errCodeOffsetOfHsmCmdMDL-37)	// 非法索引
#define errCodeHsmCmdMDL_InvalidAcc			(errCodeOffsetOfHsmCmdMDL-38)	// 非法账号
#define errCodeHsmCmdMDL_PinBlockFormat			(errCodeOffsetOfHsmCmdMDL-39)	// PinBlock格式
#define errCodeHsmCmdMDL_PinData			(errCodeOffsetOfHsmCmdMDL-40)	// Pin数据
#define errCodeHsmCmdMDL_DecimalizationTable		(errCodeOffsetOfHsmCmdMDL-41)	// DecimalizationTable
#define errCodeHsmCmdMDL_KeyScheme			(errCodeOffsetOfHsmCmdMDL-42)	// 密钥类型
#define errCodeHsmCmdMDL_IncompitableKeyLen		(errCodeOffsetOfHsmCmdMDL-43)	// 不兼容密钥长度
#define errCodeHsmCmdMDL_KeyType			(errCodeOffsetOfHsmCmdMDL-44)	// 密钥类型
#define errCodeHsmCmdMDL_KeyNotPermitted		(errCodeOffsetOfHsmCmdMDL-45)	// 不允许使用密钥
#define errCodeHsmCmdMDL_ReferenceNumber		(errCodeOffsetOfHsmCmdMDL-46)	// ReferenceNumber
#define errCodeHsmCmdMDL_InsufficientSolicitation	(errCodeOffsetOfHsmCmdMDL-47)	// 不足的Solicitation
#define errCodeHsmCmdMDL_LMKKeyChangeCorrupted		(errCodeOffsetOfHsmCmdMDL-48)	// 
#define errCodeHsmCmdMDL_DesFailure			(errCodeOffsetOfHsmCmdMDL-49)	// DES出错
#define errCodeHsmCmdMDL_DataLength			(errCodeOffsetOfHsmCmdMDL-50)	// 数据长度
#define errCodeHsmCmdMDL_LRCError			(errCodeOffsetOfHsmCmdMDL-51)	// LRC
#define errCodeHsmCmdMDL_InternalCommand		(errCodeOffsetOfHsmCmdMDL-52)	// InternalCommand
#define errCodeHsmCmdMDL_CmdResTooShort			(errCodeOffsetOfHsmCmdMDL-53)	// 指令响应太短
#define errCodeHsmCmdMDL_InvalidStorageIndex		(errCodeOffsetOfHsmReturnCodeMDL-21)	// 非法存储索引	// 21
#define errCodeHsmCmdMDL_InvalidInputData		(errCodeOffsetOfHsmReturnCodeMDL-15)	// 输入数据错	//15
#define errCodeHsmCmdMDL_LmkError			(errCodeOffsetOfHsmReturnCodeMDL-13)	// LMK错误	// 13
#define errCodeHsmCmdMDL_NoKeyLoadsInStorage		(errCodeOffsetOfHsmReturnCodeMDL-12)	// 没有密钥加载到用户存储区	//12
#define errCodeHsmCmdMDL_ZmkParity			(errCodeOffsetOfHsmReturnCodeMDL-10)	// ZMK奇偶校验错	//10

// offset = -30000
// 银联前置机错误
#define errCodeYLQZMDL_InvalidMDLID					(errCodeOffsetOfYLQZMDL-1)	// 非法的模块标识
#define errCodeYLQZMDL_ProcNotExistsAnymore				(errCodeOffsetOfYLQZMDL-2)	// 进程已不存在
#define errCodeYLQZMDL_WaitResponse					(errCodeOffsetOfYLQZMDL-3)	// 等待响应超时
#define errCodeYLQZMDL_MsgForThisModuleTooShort				(errCodeOffsetOfYLQZMDL-4)	// 给本模块的消息太短
#define errCodeYLQZMDL_ConnectYLPackageMDL				(errCodeOffsetOfYLQZMDL-5)	// 连接银联报文模块出错
#define errCodeYLQZMDL_NoAtmpErrCodeDefinedForYLErrCode			(errCodeOffsetOfYLQZMDL-6)	// 未定义该银联响应码对应的ATMP响应码
#define errCodeYLQZMDL_NoYLErrCodeDefinedForHostErrCode			(errCodeOffsetOfYLQZMDL-7)	// 未定义该主机响应码对应的银联响应码
#define errCodeYLQZMDL_HostCommConfNotDefinedInCommConfMDL		(errCodeOffsetOfYLQZMDL-8)	// 通讯模块中未定义主机通讯定义
#define errCodeYLQZMDL_AtmpTHKTransDefTBLConnected			(errCodeOffsetOfYLQZMDL-9)	// 他行卡交易定义模块联连错
#define errCodeYLQZMDL_THKTransNotDefined				(errCodeOffsetOfYLQZMDL-10)	// 他行卡交易未定义
#define errCodeYLQZMDL_GetAtmpTransMapFile				(errCodeOffsetOfYLQZMDL-11)	// 获取ATMP交易的位图文件错
#define errCodeYLQZMDL_AtmpPackageFldPoolNotInitialized			(errCodeOffsetOfYLQZMDL-12)	// ATMP报文域池未初始化
#define errCodeYLQZMDL_MacFldTBLDefOfSpecifiedIDDefined			(errCodeOffsetOfYLQZMDL-13)	// 未定义MAC计算域表
#define errCodeYLQZMDL_InvalidIDOfMacFldTBL				(errCodeOffsetOfYLQZMDL-14)	// 非法的MAC计算域表标识
#define errCodeYLQZMDL_InvalidMacFldTBLIndex				(errCodeOffsetOfYLQZMDL-15)	// 非法的MAC计算域表索引
#define errCodeYLQZMDL_MacFldTBLOfSameID				(errCodeOffsetOfYLQZMDL-16)	// MAC计算域表的标识重复
#define errCodeYLQZMDL_InvalidMaxFldNum					(errCodeOffsetOfYLQZMDL-17)	// 非法的最大域数目
#define errCodeYLQZMDL_IDOfMacFldTBLNotDefined				(errCodeOffsetOfYLQZMDL-18)	// MAC计算域表的ID未定义
#define errCodeYLQZMDL_MacFldTBLDefNotDefined				(errCodeOffsetOfYLQZMDL-19)	// MAC计算域定义表未定义
#define errCodeYLQZMDL_InvalidMacFldIndex				(errCodeOffsetOfYLQZMDL-20)	// 非法的MAC域索引号
#define errCodeYLQZMDL_YLPackageMacNotSameAsThatLocalGenerated		(errCodeOffsetOfYLQZMDL-21)	// 本地生成的MAC和银联报文中的不同
#define errCodeYLQZMDL_GetYLPackage					(errCodeOffsetOfYLQZMDL-22)	// 获取银联报文出错
#define errCodeYLQZMDL_YLQZKeyTypeInvalid				(errCodeOffsetOfYLQZMDL-23)	// 非法的前置机密钥类型
#define errCodeYLQZMDL_YLQZKeyLengthInvalid				(errCodeOffsetOfYLQZMDL-24)	// 前置机密钥的长度不对
#define errCodeYLQZMDL_YLQZZmkNotPermittedUpdated			(errCodeOffsetOfYLQZMDL-25)	// 前置机的ZMK不允许更新
#define errCodeYLQZMDL_SSNConfFileNotExists				(errCodeOffsetOfYLQZMDL-26)	// SSN配置文件不存在
#define errCodeYLQZMDL_SSNConfFileUpdate				(errCodeOffsetOfYLQZMDL-27)	// 更新SSN配置文件出错
#define errCodeYLQZMDL_ApplyHostKey					(errCodeOffsetOfYLQZMDL-28)	// 申请主机密钥出错
#define errCodeYLQZMDL_YLResponseCodeNotSuccess				(errCodeOffsetOfYLQZMDL-29)	// 银联响应码不是00
#define errCodeYLQZMDL_LengthOf8583PackIsZero				(errCodeOffsetOfYLQZMDL-30)	// 8583包的长度是0
#define errCodeYLQZMDL_InvalidPinLength					(errCodeOffsetOfYLQZMDL-31)	// PIN长度非法
#define errCodeYLQZMDL_InvalidAccNoLength				(errCodeOffsetOfYLQZMDL-32)	// 非法的账号长度
#define errCodeYLQZMDL_NoYLErrCodeDefinedForYLQZErrCode			(errCodeOffsetOfYLQZMDL-33)	// 未定义该前置机错误码对应的银联响应码
#define errCodeYLQZMDL_NoAtmpErrCodeDefinedForYLQZErrCode		(errCodeOffsetOfYLQZMDL-34)	// 未定义该前置机错误码对应的ATMP响应码
#define errCodeYLQZMDL_SetYLQZSSNToHost					(errCodeOffsetOfYLQZMDL-35)	// 设置前置机流水号出错
#define errCodeYLQZMDL_YLRequestNotSameWithHostResponse			(errCodeOffsetOfYLQZMDL-37)	// 银联的请求报文和主机响应报文不匹配
#define errCodeYLQZMDL_GetDefaultPreAuthEffectiveDate			(errCodeOffsetOfYLQZMDL-38)	// 获取缺省的预授权有效期出错
#define errCodeYLQZMDL_ThisTransIsNotPermitted				(errCodeOffsetOfYLQZMDL-39)	// 该交易不允许
#define errCodeYLQZMDL_SetHostFlds					(errCodeOffsetOfYLQZMDL-40)	// 设置主机域出错
#define errCodeYLQZMDL_TransNotPermitted				(errCodeOffsetOfYLQZMDL-41)	// 交易不允许
#define errCodeYLQZMDL_FindAtmpErrCodeForYLQZErrCode			(errCodeOffsetOfYLQZMDL-42)	// 查找银联前置错误码对应的ATMP响应码出错
#define errCodeYLQZMDL_SetYLFlds					(errCodeOffsetOfYLQZMDL-43)	// 设置银联域出错
#define errCodeYLQZMDL_FindAtmpErrCodeForYLErrCode			(errCodeOffsetOfYLQZMDL-44)	// 查找银联响应码对应的ATMP响应码出错
#define errCodeYLQZMDL_YLReqAreNotSameWithYLRes				(errCodeOffsetOfYLQZMDL-45)	// 银联请求和响应不匹配
#define errCodeYLQZMDL_OriginTransNotSuccess				(errCodeOffsetOfYLQZMDL-46)	// 原始交易不成功
#define errCodeYLQZMDL_TransNotSurpported				(errCodeOffsetOfYLQZMDL-47)	// 交易不支持
#define errCodeYLQZMDL_Not8583RequestMTI				(errCodeOffsetOfYLQZMDL-48)	// 不是8583包的请求MTI
#define errCodeYLQZMDL_YLNotSupportedReversalForThisTrans		(errCodeOffsetOfYLQZMDL-49)	// 银联不支持该交易的冲正交易
#define errCodeYLQZMDL_TransConfFileNotSet				(errCodeOffsetOfYLQZMDL-50)	// 交易配置文件未配置
#define errCodeYLQZMDL_HostPackageNotConnected				(errCodeOffsetOfYLQZMDL-51)	// 主机报文模块未连接
#define errCodeYLQZMDL_NoSetValueMethodForHostFld			(errCodeOffsetOfYLQZMDL-52)	// 未为主机域定义赋值方法。
#define errCodeYLQZMDL_AtmpPackageNotConnected				(errCodeOffsetOfYLQZMDL-53)	// ATMP报文模块未定义
#define errCodeYLQZMDL_NoSetValueMethodForAtmpFld			(errCodeOffsetOfYLQZMDL-54)	// 未为ATMP域定义赋值方法
#define errCodeYLQZMDL_AtmpTransDefNotDefined				(errCodeOffsetOfYLQZMDL-55)	// ATMP交易定义未定义
#define errCodeYLQZMDL_YLTransDataDefNotConnected			(errCodeOffsetOfYLQZMDL-56)	// 银联交易数据定义未连接
#define errCodeYLQZMDL_YL8583PackageFldDefIsNull			(errCodeOffsetOfYLQZMDL-57)	// 银联报文域定义是空
#define errCodeYLQZMDL_WaitHostResponse					(errCodeOffsetOfYLQZMDL-58)	// 等待主机响应出错
#define errCodeYLQZMDL_FixedMDLIDHasNoPID				(errCodeOffsetOfYLQZMDL-59)	// 固定模块的标识不含进程号
#define errCodeYLQZMDL_IDOfYLNotDefined					(errCodeOffsetOfYLQZMDL-60)	// 未定义银联标识
#define errCodeYLQZMDL_NoBankErrCodeDefinedForYLErrCode			(errCodeOffsetOfYLQZMDL-61)	// 未定义银联响应码对应的银行响应码
#define errCodeYLQZMDL_NoYLErrCodeDefinedForBankErrCode			(errCodeOffsetOfYLQZMDL-62)	// 未定义银行响应码对应的银联响应码
#define errCodeYLQZMDL_NoBankErrCodeDefinedForYLQZErrCode		(errCodeOffsetOfYLQZMDL-63)	// 未定义前置机错误码对应的银行响应码
#define errCodeYLQZMDL_KeyGroupNotDefined				(errCodeOffsetOfYLQZMDL-64)	// 密钥组未定义
#define errCodeYLQZMDL_KeyGroupAlreadyExists				(errCodeOffsetOfYLQZMDL-65)	// 密钥组已存在
#define errCodeYLQZMDL_KeyGroupTBLIsFull				(errCodeOffsetOfYLQZMDL-66)	// 密钥组是空
#define errCodeYLQZMDL_ConnectHsmSvrCmdPackageMDL			(errCodeOffsetOfYLQZMDL-67)	// 连接密码机服务报文出错
#define errCodeYLQZMDL_InvalidHsmSvrCmd					(errCodeOffsetOfYLQZMDL-68)	// 非法的密码服务指令
#define errCodeYLQZMDL_ConnectToHsm					(errCodeOffsetOfYLQZMDL-69)	// 连接密码机出错
#define errCodeYLQZMDL_ResetKeyFld48					(errCodeOffsetOfYLQZMDL-70)	// 键值置换报文的48域错
#define errCodeYLQZMDL_ConnectHostPackageMDL				(errCodeOffsetOfYLQZMDL-71)	// 连接主机报文出错
#define errCodeYLQZMDL_ConnectAtmpPackageMDL				(errCodeOffsetOfYLQZMDL-72)	// 连接ATMP报文出错
#define errCodeYLQZMDL_NoStrategyDefinedWhenHostTimeout			(errCodeOffsetOfYLQZMDL-73)	// 未定义主机超时的处理策略
#define errCodeYLQZMDL_HostResponseCodeNotSuccess			(errCodeOffsetOfYLQZMDL-74)	// 主机响应码不是成功
#define errCodeYLQZMDL_HostNotSupportedThisKindOfTrans			(errCodeOffsetOfYLQZMDL-75)	// 主机不支持这种交易
#define errCodeYLQZMDL_NoAtmTeller					(errCodeOffsetOfYLQZMDL-76)	// 不是ATM柜员
#define errCodeYLQZMDL_NoPosTeller					(errCodeOffsetOfYLQZMDL-77)	// 不是POS柜员
#define errCodeYLQZMDL_CommWithDBSvr					(errCodeOffsetOfYLQZMDL-78)	// 与DBSvr通讯错
#define errCodeYLQZMDL_PinNotExists					(errCodeOffsetOfYLQZMDL-79)	// PIN不存在
#define errCodeYLQZMDL_AmountExceedPerTransLimit			(errCodeOffsetOfYLQZMDL-80)	// 交易额超过每笔的最大值
#define errCodeYLQZMDL_AmountExceedPerDayLimit				(errCodeOffsetOfYLQZMDL-81)	// 当日交易额超过累计
#define errCodeYLQZMDL_IsBitMapFld					(errCodeOffsetOfYLQZMDL-82)	// 是位图域
#define errCodeYLQZMDL_ThisKindCardNotPermitYLTrans			(errCodeOffsetOfYLQZMDL-83)	// 这种卡不允许做银联交易
#define errCodeYLQZMDL_CardHasNoCVV					(errCodeOffsetOfYLQZMDL-84)	// 卡中无CVV
#define errCodeYLQZMDL_YLQZAuthorizedTrans				(errCodeOffsetOfYLQZMDL-85)	// 前置机授权返回
#define errCodeYLQZMDL_ForbideTransFromThisOccurer			(errCodeOffsetOfYLQZMDL-86)	// 禁止交易从这个受理行发起
#define errCodeYLQZMDL_NoCreditTeller					(errCodeOffsetOfYLQZMDL-87)     // 不是贷记卡柜员
#define errCodeYLQZMDL_NoDebitTeller					(errCodeOffsetOfYLQZMDL-88)     // 不是借机卡柜员
#define errCodeYLQZMDL_InvalidIDCard					(errCodeOffsetOfYLQZMDL-89)     // 非法的身份证号
#define errCodeYLQZMDL_InvalidCustName					(errCodeOffsetOfYLQZMDL-90)     // 户名不一致
#define errCodeYLQZMDL_OriginTransSuccess				(errCodeOffsetOfYLQZMDL-91)	// 原始交易成功
#define errCodeYLQZMDL_SytInvalidAcctNo					(errCodeOffsetOfYLQZMDL-92) // 生意通转入卡不存在（在该行未登记）
#define errCodeYLQZMDL_SytAmountExceedPerTransLimit			(errCodeOffsetOfYLQZMDL-93) // 生意通单笔超过金额限制
#define errCodeYLQZMDL_SytAmountExceedPerDayLimit			(errCodeOffsetOfYLQZMDL-94) // 生意通当日合计超过金额限制

// offset = -26000
// 8583 模块错误
#define errCodeISO8583MDL_MTINotDefined				(errCodeOffsetOfISO8583MDL-1)	// MTI未定义
#define errCodeISO8583MDL_8583PackageClassTBLConnected		(errCodeOffsetOfISO8583MDL-2)	// 连接8583报文类定义出错
#define errCodeISO8583MDL_8583BHKTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-3)	// 连接8583本行卡交易定义出错
#define errCodeISO8583MDL_BHKTransNotDefined			(errCodeOffsetOfISO8583MDL-4)	// 本行卡交易未定义
#define errCodeISO8583MDL_8583YLMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-5)	// 连接银联管理类交易表错
#define errCodeISO8583MDL_YLMngTransNotDefined			(errCodeOffsetOfISO8583MDL-6)	// 银联管理交易未定义
#define errCodeISO8583MDL_8583Fld90Length			(errCodeOffsetOfISO8583MDL-7)	// 90域长度错误
#define errCodeISO8583MDL_FldNoValue				(errCodeOffsetOfISO8583MDL-8)	// 域无值

// offset = -25000
// 报文模块错误
#define errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined	(errCodeOffsetOfPackageDefMDL-1)	// 报文最大域数量未定义
#define errCodePackageDefMDL_PackageDefGroupIsFull		(errCodeOffsetOfPackageDefMDL-2)	// 报文定义组满了
#define errCodePackageDefMDL_PackageDefNotDefined		(errCodeOffsetOfPackageDefMDL-3)	// 报文定义未定义
#define errCodePackageDefMDL_IDOfPackageNotDefined		(errCodeOffsetOfPackageDefMDL-4)	// 报文的标识未定义
#define errCodePackageDefMDL_InvalidIDOfPackage			(errCodeOffsetOfPackageDefMDL-5)	// 非法的报文标识
#define errCodePackageDefMDL_PackageOfSameID			(errCodeOffsetOfPackageDefMDL-6)	// 报文标识重复
#define errCodePackageDefMDL_InvalidMaxFldNum			(errCodeOffsetOfPackageDefMDL-7)	// 非法的最大域数目
#define errCodePackageDefMDL_PackageTypeNotDefined		(errCodeOffsetOfPackageDefMDL-8)	// 报文类型未定义
#define errCodePackageDefMDL_InvalidPackageType			(errCodeOffsetOfPackageDefMDL-9)	// 非法的报文类型
#define errCodePackageDefMDL_InvalidPrimaryKey			(errCodeOffsetOfPackageDefMDL-10)	// 非法的主键
#define errCodePackageDefMDL_FldNotDefined			(errCodeOffsetOfPackageDefMDL-11)	// 域未定义
#define errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-12)	// 指定标识的报文定义错
#define errCodePackageDefMDL_PackFldIsNull			(errCodeOffsetOfPackageDefMDL-13)	// 报文域满了
#define errCodePackageDefMDL_PackFldLength			(errCodeOffsetOfPackageDefMDL-14)	// 报文域长度错
#define errCodePackageDefMDL_FldNotDefinedInBitMap		(errCodeOffsetOfPackageDefMDL-15)	// 位图中未定义域
#define errCodePackageDefMDL_BitMapHaveValueForNullValueFld	(errCodeOffsetOfPackageDefMDL-16)	// 位图中定义了值为空的域
#define errCodePackageDefMDL_PackageNotConnected		(errCodeOffsetOfPackageDefMDL-17)	// 报文未连接
#define errCodePackageDefMDL_InvalidFldIndex			(errCodeOffsetOfPackageDefMDL-18)	// 非法的域索引
#define errCodePackageDefMDL_BitMapFldCannotBeSet		(errCodeOffsetOfPackageDefMDL-19)	// 位图域不能设置
#define errCodePackageDefMDL_InvalidPackageIndex		(errCodeOffsetOfPackageDefMDL-20)	// 非法的报文索引
#define errCodePackageDefMDL_TwoPackageOfSamePackageIndex	(errCodeOffsetOfPackageDefMDL-21)	// 两个报文具有相同的报文索引
#define errCodePackageDefMDL_ConnectPackage			(errCodeOffsetOfPackageDefMDL-22)	// 连接报文出错
#define errCodePackageDefMDL_PackExchDefTBLOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-23)	// 指定标识的报文交换定义表定义错
#define errCodePackageDefMDL_IDOfPackExchNotDefined		(errCodeOffsetOfPackageDefMDL-24)	// 指定的报文交换定义
#define errCodePackageDefMDL_InvalidIDOfPackExch		(errCodeOffsetOfPackageDefMDL-25)	// 非法的报文交换定义标识
#define errCodePackageDefMDL_PackExchDefTBLNotDefined		(errCodeOffsetOfPackageDefMDL-26)	// 报文交换定义表未定义
#define errCodePackageDefMDL_PackExchDefOfSameID		(errCodeOffsetOfPackageDefMDL-27)	// 报文交换定义有相同的标识
#define errCodePackageDefMDL_NotBitMapFld			(errCodeOffsetOfPackageDefMDL-28)	// 不是位图域
#define errCodePackageDefMDL_TwoMuchBitMapLevel			(errCodeOffsetOfPackageDefMDL-29)	// ？

// 交易类型错误
// offset = -24000
#define errCodeTransClassDefMDL_MaxNumOfTransClassDefNotDefined		(errCodeOffsetOfTransClassDefMDL-1)	// 最大的交易类型数未定义
#define errCodeTransClassDefMDL_SpecifiedTransClassDefNotDefined	(errCodeOffsetOfTransClassDefMDL-2)	// 指定的交易类型未定义
#define errCodeTransClassDefMDL_TransDefNotDefined			(errCodeOffsetOfTransClassDefMDL-3)	// 交易未定义

// SJL06模块错误
// offset = -23000
#define errCodeSJL06MDL_ServiceFailure		(errCodeOffsetOfSJL06MDL-1)	// 服务失败
#define errCodeSJL06MDL_NoWorkingSJL06		(errCodeOffsetOfSJL06MDL-2)	// 没有工作的密码机
#define errCodeSJL06MDL_SJL06Def		(errCodeOffsetOfSJL06MDL-3)	// SJL06定义错
#define errCodeSJL06MDL_InvalidSJL06Type	(errCodeOffsetOfSJL06MDL-4)	// 非法的SJL06类型
#define errCodeSJL06MDL_NotConnected		(errCodeOffsetOfSJL06MDL-5)	// SJL06模块未连接
#define errCodeSJL06MDL_WorkingTableFull	(errCodeOffsetOfSJL06MDL-6)	// 工作组表满了
#define errCodeSJL06MDL_SJL06NotExists		(errCodeOffsetOfSJL06MDL-7)	// 指定的SJL06不存在
#define errCodeSJL06MDL_SJL06StillOnline	(errCodeOffsetOfSJL06MDL-8)	// 该SJL06仍在工作
#define errCodeSJL06MDL_GenerayKeyByStatus      (errCodeOffsetOfSJL06MDL-9)     // 根据状态随机生成并打印密钥
#define errCodeSJL06MDL_ModifyKeyPosStatus	(errCodeOffsetOfSJL06MDL-10)	// 修改密钥位置状态出错
#define errCodeSJL06MDL_InvalidLMKPaire		(errCodeOffsetOfSJL06MDL-11)	// 非法的LMK对
#define errCodeSJL06MDL_SharedKeySpaceConfFile	(errCodeOffsetOfSJL06MDL-12)	// 共享密钥空间配置文件错
#define errCodeSJL06MDL_SharedKeySpaceNotConnected	(errCodeOffsetOfSJL06MDL-13)	// 共享密钥空间连接出错
#define errCodeSJL06MDL_SharedKeySpaceFull	(errCodeOffsetOfSJL06MDL-14)	// 共享密钥空间已满
#define errCodeSJL06MDL_NoAvaliableSharedKeySpacePos	(errCodeOffsetOfSJL06MDL-15)	// 无可用的共享密钥位置
#define errCodeSJL06MDL_NotSharedKeySpacePos	(errCodeOffsetOfSJL06MDL-16)	// 不是共享密钥位置
#define errCodeSJL06MDL_SharedKeySpacePosNum	(errCodeOffsetOfSJL06MDL-17)	// 共享密钥空间位置数错
#define errCodeSJL06MDL_SJL06StillNotOnline	(errCodeOffsetOfSJL06MDL-18)	// 密码机状态不在线
#define errCodeSJL06MDL_InvalidHsmResponseCode	(errCodeOffsetOfSJL06MDL-19)	// 非法的密码机响应代码
#define errCodeSJL06MDL_InvalidKeyExchange	(errCodeOffsetOfSJL06MDL-20)	// 非法的密钥交换
#define errCodeSJL06MDL_InvalidHsmRetLen	(errCodeOffsetOfSJL06MDL-21)	// 非法的密码机返回长度
#define errCodeSJL06MDL_PanCannotBeNull		(errCodeOffsetOfSJL06MDL-22)	// PAN不能为空
#define errCodeSJL06MDL_ConnectSJL06		(errCodeOffsetOfSJL06MDL-23)	// 连接密码机
#define errCodeSJL06MDL_DefaultSJL06Conf	(errCodeOffsetOfSJL06MDL-24)	// SJL06缺省配置错误
#define errCodeSJL06MDL_SJL06Abnormal		(errCodeOffsetOfSJL06MDL-25)	// 密码机异常
#define errCodeSJL06MDL_ConnectHsmGrp		(errCodeOffsetOfSJL06MDL-26)	// 连接密码机组
#define errCodeSJL06MDL_SJL06AlreadyExists	(errCodeOffsetOfSJL06MDL-27)	// SJL06已存在
#define errCodeSJL06MDL_MustSpecifyIPAddrForSJL06	(errCodeOffsetOfSJL06MDL-28)	// SJL06的IP地址必须指定
#define errCodeSJL06MDL_SJL06AlreadyInGrpFile	(errCodeOffsetOfSJL06MDL-29)	// SJL06已经在密码机组文件中了
#define errCodeSJL06MDL_SJL06NotExistsInGrpFile	(errCodeOffsetOfSJL06MDL-30)	// SJL06不在密码机组文件中了
#define errCodeSJL06MDL_SJL06AlreadyAbnormal	(errCodeOffsetOfSJL06MDL-31)	// 密码机已是异常状态
#define errCodeSJL06MDL_SJL06AlreadyBackup	(errCodeOffsetOfSJL06MDL-32)	// 密码机已是备份状态
#define errCodeSJL06MDL_SJL06AlreadyOnline	(errCodeOffsetOfSJL06MDL-33)	// 密码机已是正常状态
//add by linxj 20130502
#define errCodeSJL06MDL_HsmGrpAlreadyBackup     (errCodeOffsetOfSJL06MDL-34)    // 密码机组设为备份状态
#define errCodeSJL06MDL_HsmIPNotExists		(errCodeOffsetOfSJL06MDL-35)	// 指定的密码机IP不存在


// SJL05模块错误
// offset = -70000
#define errCodeSJL05MDL_HsmCmdFormatError	(errCodeOffsetOfSJL05MDL-1)	// 指令格式错误

// 任务模块错误
// offset = -13000
#define errCodeTaskMDL_CreateTaskInstance	(errCodeOffsetOfTaskMDL-1)	// 创建任务实例错
#define errCodeTaskMDL_ConfFile			(errCodeOffsetOfTaskMDL-2)	// 任务配置文件错
#define errCodeTaskMDL_NotConnected		(errCodeOffsetOfTaskMDL-3)	// 任务模块未连接
#define errCodeTaskMDL_TaskNotExists		(errCodeOffsetOfTaskMDL-4)	// 任务不存在
#define errCodeTaskMDL_TaskClassAlreadyExists	(errCodeOffsetOfTaskMDL-5)	// 任务类型已存在
#define errCodeTaskMDL_TaskClassNameTooLong	(errCodeOffsetOfTaskMDL-6)	// 任务类型名称太长
#define errCodeTaskMDL_TaskClassStartCmdTooLong	(errCodeOffsetOfTaskMDL-7)	// 任务启动命令太长
#define errCodeTaskMDL_TaskClassLogFileNameIsNull	(errCodeOffsetOfTaskMDL-8)	// 任务日志名称为空
#define errCodeTaskMDL_TaskClassNameIsNull	(errCodeOffsetOfTaskMDL-9)	// 任务类型名称为空
#define errCodeTaskMDL_TaskClassLogFileNameTooLong	(errCodeOffsetOfTaskMDL-10)	// 任务日志名称太长
#define errCodeTaskMDL_TaskClassTableIsFull	(errCodeOffsetOfTaskMDL-11)	// 任务类型表已满
#define errCodeTaskMDL_Connect			(errCodeOffsetOfTaskMDL-12)	// 连接任务表出错
#define errCodeTaskMDL_TaskClassIsUsed		(errCodeOffsetOfTaskMDL-13)	// 任务类型正在被使用
#define errCodeTaskMDL_TaskClassNameNotDefined	(errCodeOffsetOfTaskMDL-14)	// 任务类型名称未定义
#define errCodeTaskMDL_TaskClassNotExists	(errCodeOffsetOfTaskMDL-15)	// 任务类型不存在
#define errCodeTaskMDL_TaskClassDefLineError	(errCodeOffsetOfTaskMDL-16)	// 任务类型定义行有错
#define errCodeTaskMDL_AnotherTaskOfNameExists	(errCodeOffsetOfTaskMDL-17)	// 同名的任务已经存在 2007/11/30，王纯军增加

// Socket模块错误
// offset = -22000
#define errCodeSocketMDL_Timeout		(errCodeOffsetOfSocketMDL-1)	// 超时
#define errCodeSocketMDL_ReceiveLen		(errCodeOffsetOfSocketMDL-2)	// 接收长度
#define errCodeSocketMDL_Error			(errCodeOffsetOfSocketMDL-3)	// 通讯失败
#define errCodeSocketMDL_DataLen		(errCodeOffsetOfSocketMDL-4)	// 数据长度错
#define errCodeSocketMDL_RecvDataTimeout	(errCodeOffsetOfSocketMDL-5)	// 接收数据超时
#define errCodeSocketMDL_ErrCodeNotSet		(errCodeOffsetOfSocketMDL-6)	// 未设置出错码

// 通讯模块错误
// offset = -21000
#define errCodeSckCommMDL_ConfFile		(errCodeOffsetOfSckCommMDL-1)	// 定义文件
#define errCodeSckCommMDL_InvalidWorkingMode	(errCodeOffsetOfSckCommMDL-2)	// 非法工作状态
#define errCodeSckCommMDL_ServerNotDefined	(errCodeOffsetOfSckCommMDL-3)	// 服务器未定义
#define errCodeSckCommMDL_ServiceFailure	(errCodeOffsetOfSckCommMDL-4)	// 服务失败
#define errCodeSckCommMDL_NoWorkingServer	(errCodeOffsetOfSckCommMDL-5)	// 不是工作着的服务器
#define errCodeSckCommMDL_InternalError		(errCodeOffsetOfSckCommMDL-6)	// 内部错误
#define errCodeSckCommMDL_Connect		(errCodeOffsetOfSckCommMDL-7)	// 连接通讯模块


// REC模块错误
// offset = -20000
#define errCodeRECMDL_ConfFile			(errCodeOffsetOfRECMDL-1)	// 配置文件出错
#define errCodeRECMDL_VarNotExists		(errCodeOffsetOfRECMDL-2)	// 变量不存在
#define errCodeRECMDL_VarType			(errCodeOffsetOfRECMDL-3)	// 变量类型错
#define errCodeRECMDL_VarValue			(errCodeOffsetOfRECMDL-4)	// 变量值有问题
#define errCodeRECMDL_Connect			(errCodeOffsetOfRECMDL-5)	// 连接变量配置模块
#define errCodeRECMDL_VarAlreadyExists		(errCodeOffsetOfRECMDL-6)	// 变量已存在
#define errCodeRECMDL_VarTBLIsFull		(errCodeOffsetOfRECMDL-7)	// 变量表已满
#define errCodeRECMDL_VarNameLength		(errCodeOffsetOfRECMDL-8)	// 变量名称长度错
#define errCodeRECMDL_VarValueTooLong		(errCodeOffsetOfRECMDL-9)	// 值太长了
#define errCodeRECMDL_RealNumError		(errCodeOffsetOfRECMDL-10)	// 变量的数目错

// 公钥模块错误
// offset = -12000
#define errCodePKDBMDL_KeyNonExists		(errCodeOffsetOfPKDBMDL-1)	// 密钥不存在
#define errCodePKDBMDL_KeyNum			(errCodeOffsetOfPKDBMDL-2)	// 密钥数量错
#define errCodePKDBMDL_KeyDBFull		(errCodeOffsetOfPKDBMDL-3)	// 密钥库满
#define errCodePKDBMDL_KeyDBNonConnected	(errCodeOffsetOfPKDBMDL-4)	// 密钥库未连接
#define errCodePKDBMDL_InvalidKeyLength		(errCodeOffsetOfPKDBMDL-5)	// 非法的密钥长度
#define errCodePKDBMDL_KeyAlreadyExists		(errCodeOffsetOfPKDBMDL-6)	// 密钥已经存在
#define errCodePKDBMDL_KeyOutdate		(errCodeOffsetOfPKDBMDL-7)	// 密钥过了有效期
#define errCodePKDBMDL_KeyValueWithoutEnd	(errCodeOffsetOfPKDBMDL-8)	// 密钥值无结束符
#define errCodePKDBMDL_KeyIndexFileAlreadyExists	(errCodeOffsetOfPKDBMDL-9)	// 密钥文件已存在
#define errCodePKDBMDL_KeyIndexFileNotExists	(errCodeOffsetOfPKDBMDL-10)	// 密钥文件不存在
#define errCodePKDBMDL_TBLLocked		(errCodeOffsetOfPKDBMDL-11)	// 密钥文件已被锁住
#define errCodePKDBMDL_SomeRecIsWritingLocked	(errCodeOffsetOfPKDBMDL-12)	// 有记录被写锁住了
#define errCodePKDBMDL_KeyNotEffective		(errCodeOffsetOfPKDBMDL-13)	// 密钥无效
#define errCodePKDBMDL_Connect			(errCodeOffsetOfPKDBMDL-14)	// 连接库
#define errCodePKDBMDL_KeyNotLocked		(errCodeOffsetOfPKDBMDL-15)	// 密钥未被锁住
#define errCodePKDBMDL_KeyAlreadyLocked		(errCodeOffsetOfPKDBMDL-16)	// 密钥已被锁住

// 操作员模块错误
// offset = -19000
#define errCodeOperatorMDL_OperatorNum		(errCodeOffsetOfOpertatorMDL-1)	// 操作员数目错
#define errCodeOperatorMDL_OperatorNotExists	(errCodeOffsetOfOpertatorMDL-2)	// 操作员不存在
#define errCodeOperatorMDL_NotConnected		(errCodeOffsetOfOpertatorMDL-3)	// 操作员模块未连接
#define errCodeOperatorMDL_TableFull		(errCodeOffsetOfOpertatorMDL-4)	// 操作员表满
#define errCodeOperatorMDL_ReadOperator		(errCodeOffsetOfOpertatorMDL-5)	// 读操作员
#define errCodeOperatorMDL_WrongPassword	(errCodeOffsetOfOpertatorMDL-6)	// 操作员密码错
#define errCodeOperatorMDL_PasswordLocked	(errCodeOffsetOfOpertatorMDL-7)	// 操作员密码被锁住了
#define errCodeOperatorMDL_AlreadyLogon		(errCodeOffsetOfOpertatorMDL-8)	// 操作员已经登录了
#define errCodeOperatorMDL_NotLogon		(errCodeOffsetOfOpertatorMDL-9)	// 操作员还没有登录
#define errCodeOperatorMDL_TooLongTimeWithoutOperation		(errCodeOffsetOfOpertatorMDL-10)	// 操作员太长时间没有操作
#define errCodeOperatorMDL_StillLogon		(errCodeOffsetOfOpertatorMDL-11)	// 操作员在登录状态
#define errCodeOperatorMDL_CannotDeleteSelf	(errCodeOffsetOfOpertatorMDL-12)	// 不能删除自己
#define errCodeOperatorMDL_OperatorAlreadyExists	(errCodeOffsetOfOpertatorMDL-13)	// 操作员已存在
#define errCodeOperatorMDL_OperatorPwdOverTime	(errCodeOffsetOfOpertatorMDL-14)	// 操作员密码已过期 
#define errCodeOperatorMDL_OperatorOverMaxFreeTime	(errCodeOffsetOfOpertatorMDL-15)	// 操作员超过最大空闲时间
#define errCodeOperatorMDL_NotLegitimateClient	(errCodeOffsetOfOpertatorMDL-16)	// 操作员不是在合法客户端登陆
#define errCodeOperatorMDL_NoPermissions	(errCodeOffsetOfOpertatorMDL-17)	// 操作员没有权限执行该操作
#define errCodeOperatorMDL_OperatorIsLocked	(errCodeOffsetOfOpertatorMDL-18)	// 操作员被锁住了
#define errCodeOperatorMDL_AuthOperatorIsNotValid   (errCodeOffsetOfOpertatorMDL-19)    // 授权操作员认证不通过
#define errCodeOperatorMDL_PasswordIsSame	(errCodeOffsetOfOpertatorMDL-20) // 新旧密码相同
#define errCodeOperatorMDL_IdentifyCodeNotExists	(errCodeOffsetOfOpertatorMDL-21)	// 认证码不存在
#define errCodeOperatorMDL_InvalidLogon		(errCodeOffsetOfOpertatorMDL-22)	// 非法登录
#define errCodeOperatorMDL_OperatorPwdOldEncType	(errCodeOffsetOfOpertatorMDL-23)	//密码为旧格式 


// 消息交换区错误
// offset = -18000
#define errCodeMsgBufMDL_NotConnected		(errCodeOffsetOfMsgBufMDL-1)	// 消息交换区未连接
#define errCodeMsgBufMDL_OutofRange		(errCodeOffsetOfMsgBufMDL-2)	// 超界
#define errCodeMsgBufMDL_InitDef		(errCodeOffsetOfMsgBufMDL-3)	// 初始定义错
#define errCodeMsgBufMDL_ConnectIndexTBL	(errCodeOffsetOfMsgBufMDL-4)	// 连接索引表错
#define errCodeMsgBufMDL_ProviderExit		(errCodeOffsetOfMsgBufMDL-5)	// 提供者已退出
#define errCodeMsgBufMDL_NotOriginMsg		(errCodeOffsetOfMsgBufMDL-6)	// 不是原始信息
#define errCodeMsgBufMDL_InvalidIndex		(errCodeOffsetOfMsgBufMDL-7)	// 非法的索引
#define errCodeMsgBufMDL_ReadIndex		(errCodeOffsetOfMsgBufMDL-8)	// 读取索引
#define errCodeMsgBufMDL_MsgLen			(errCodeOffsetOfMsgBufMDL-9)	// 消息长度错
#define errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfMsgBufMDL-10)	// 具有指定类型和索引的消息不存在
#define errCodeMsgBufMDL_MsgIndexAlreadyLosted	(errCodeOffsetOfMsgBufMDL-11)	// 消息索引已经丢失
#define errCodeMsgBufMDL_PointerAbnormal	(errCodeOffsetOfMsgBufMDL-12)	// 消息交换区指针异常

// 交易监控消息交换区错误
// offset = -4000
#define errCodeTransSpierBufMDL_NotConnected		(errCodeOffsetOfTransSpierBufMDL-1)	// 消息交换区未连接
#define errCodeTransSpierBufMDL_OutofRange		(errCodeOffsetOfTransSpierBufMDL-2)	// 超界
#define errCodeTransSpierBufMDL_InitDef			(errCodeOffsetOfTransSpierBufMDL-3)	// 初始定义错
#define errCodeTransSpierBufMDL_ConnectIndexTBL		(errCodeOffsetOfTransSpierBufMDL-4)	// 连接索引表错
#define errCodeTransSpierBufMDL_ProviderExit		(errCodeOffsetOfTransSpierBufMDL-5)	// 提供者已退出
#define errCodeTransSpierBufMDL_NotOriginMsg		(errCodeOffsetOfTransSpierBufMDL-6)	// 不是原始信息
#define errCodeTransSpierBufMDL_InvalidIndex		(errCodeOffsetOfTransSpierBufMDL-7)	// 非法的索引
#define errCodeTransSpierBufMDL_ReadIndex		(errCodeOffsetOfTransSpierBufMDL-8)	// 读取索引
#define errCodeTransSpierBufMDL_MsgLen			(errCodeOffsetOfTransSpierBufMDL-9)	// 消息长度错
#define errCodeTransSpierBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfTransSpierBufMDL-10)	// 具有指定类型和索引的消息不存在

// 共享内存模块错误
// offset = -17000
#define errCodeSharedMemoryMDL_MDLNotDefined	(errCodeOffsetOfSharedMemoryMDL-1)	// 共享内存模块未定义
#define errCodeSharedMemoryMDL_LockMDL		(errCodeOffsetOfSharedMemoryMDL-2)	// 锁共享内存错
#define errCodeSharedMemoryMDL_Connect		(errCodeOffsetOfSharedMemoryMDL-3)	// 连接共享内存模块

// 日志模块错误
// offset = -16000
#define errCodeLogMDL_ConfFile			(errCodeOffsetOfLogMDL-1)	// 配置文件错
#define errCodeLogMDL_LogFileAlreadyExists	(errCodeOffsetOfLogMDL-2)	// 日志已存在
#define errCodeLogMDL_LogFileTblFull		(errCodeOffsetOfLogMDL-3)	// 日志表满了
#define errCodeLogMDL_LogFileUsedByUsers	(errCodeOffsetOfLogMDL-4)	// 日志正被使用
#define errCodeLogMDL_LogFileNotExists		(errCodeOffsetOfLogMDL-5)	// 日志不存在
#define errCodeLogMDL_NotConnected		(errCodeOffsetOfLogMDL-6)	// 日志模块未连接
#define errCodeLogMDL_Connect			(errCodeOffsetOfLogMDL-7)	// 连接日志模块
#define errCodeLogMDL_LogFileNameMustSpecified	(errCodeOffsetOfLogMDL-8)	// 必须指明日志名称

// 环境变量模块错误
// offset = -14000
#define errCodeEnviMDL_VarNotExists		(errCodeOffsetOfEnviMDL-1)	// 变量不存在
#define errCodeEnviMDL_NullLine			(errCodeOffsetOfEnviMDL-2)	// 空行
#define errCodeEnviMDL_OccupiedByOther		(errCodeOffsetOfEnviMDL-3)	// 环境变量模块被别人占用了
#define errCodeEnviMDL_NoValueDefinedForVar	(errCodeOffsetOfEnviMDL-4)	// 变量值没定义
#define errCodeEnviMDL_VarAlreadyExists		(errCodeOffsetOfEnviMDL-5)	// 变量已存在
#define errCodeEnviMDL_NotRecFormatDefStr	(errCodeOffsetOfEnviMDL-6)	// 不是记录格式定义字符串
#define errCodeEnviMDL_NotRecStrTooLong		(errCodeOffsetOfEnviMDL-7)	// 记录字符串太长
#define errCodeEnviMDL_NotRecDefStr		(errCodeOffsetOfEnviMDL-8)	// 不是记录定义串

// 数据库备份模块错误
// offset = -15000
#define errCodeKeyDBBackuperMDL_ConfError	(errCodeOffsetOfKeyDBBackuperMDL-1)	// 配置出错
#define errCodeKeyDBBackuperMDL_IPAddrMustInput	(errCodeOffsetOfKeyDBBackuperMDL-2)	// IP地址必须输入
#define errCodeKeyDBBackuperMDL_PortMustInput	(errCodeOffsetOfKeyDBBackuperMDL-3)	// PORT必须输入
#define errCodeKeyDBBackuperMDL_BrotherNotDefined	(errCodeOffsetOfKeyDBBackuperMDL-4)	// 未定义的备份服务器
#define errCodeKeyDBBackuperMDL_BrotherReturnFailure	(errCodeOffsetOfKeyDBBackuperMDL-5)	// 服务器返回失败	// 2007/11/29增加
#define errCodeKeyDBBackuperMDL_BrotherNotActive	(errCodeOffsetOfKeyDBBackuperMDL-6)	// 备份服务器非活动状态	// 2007/12/25增加

// DES密钥库模块错误
// offset = -11000
#define errCodeDesKeyDBMDL_KeyNonExists		(errCodeOffsetOfDesKeyDBMDL-1)	// 密钥不存在
#define errCodeDesKeyDBMDL_KeyNum		(errCodeOffsetOfDesKeyDBMDL-2)	// 密钥数量错
#define errCodeDesKeyDBMDL_KeyDBFull		(errCodeOffsetOfDesKeyDBMDL-3)	// 库满
#define errCodeDesKeyDBMDL_KeyDBNonConnected	(errCodeOffsetOfDesKeyDBMDL-4)	// 库未连接
#define errCodeDesKeyDBMDL_InvalidKeyLength	(errCodeOffsetOfDesKeyDBMDL-5)	// 非法密钥长度
#define errCodeDesKeyDBMDL_KeyAlreadyExists	(errCodeOffsetOfDesKeyDBMDL-6)	// 密钥已存在
#define errCodeDesKeyDBMDL_KeyOutdate		(errCodeOffsetOfDesKeyDBMDL-7)	// 密钥过期
#define errCodeDesKeyDBMDL_KeyValueWithoutEnd	(errCodeOffsetOfDesKeyDBMDL-8)	// 密钥值无结束符
#define errCodeDesKeyDBMDL_KeyIndexFileAlreadyExists	(errCodeOffsetOfDesKeyDBMDL-9)	// 密钥文件已存在
#define errCodeDesKeyDBMDL_KeyIndexFileNotExists	(errCodeOffsetOfDesKeyDBMDL-10)	// 密钥文件不存在
#define errCodeDesKeyDBMDL_TBLLocked		(errCodeOffsetOfDesKeyDBMDL-11)	// 密钥文件已被锁住
#define errCodeDesKeyDBMDL_SomeRecIsWritingLocked	(errCodeOffsetOfDesKeyDBMDL-12)	// 有记录被写锁住了
#define errCodeDesKeyDBMDL_KeyNotEffective	(errCodeOffsetOfDesKeyDBMDL-13)	// 密钥无效
#define errCodeDesKeyDBMDL_Connect		(errCodeOffsetOfDesKeyDBMDL-14)	// 连接库
#define errCodeDesKeyDBMDL_KeyNotLocked		(errCodeOffsetOfDesKeyDBMDL-15)	// 密钥未被锁住
#define errCodeDesKeyDBMDL_InvalidKeyRec	(errCodeOffsetOfDesKeyDBMDL-16)	// 非法的密钥记录
#define errCodeDesKeyDBMDL_KeyValueNotSameAsExpected	(errCodeOffsetOfDesKeyDBMDL-17)	// 不是期待的密钥值，2007/11/28增加
#define errCodeDesKeyDBMDL_KeyAlreadyLocked	(errCodeOffsetOfDesKeyDBMDL-18)	// 密钥已被锁住
#define errCodeDesKeyDBMDL_KeyStrategyInactive	(errCodeOffsetOfDesKeyDBMDL-19)	// 密钥策略未激活

// 密码服务平台错误
// offset = -29000
#define errCodeEssc_InvalidUseKey		(errCodeOffsetOfEsscMDL-1)	// 非法使用密钥
#define errCodeEssc_CheckValue			(errCodeOffsetOfEsscMDL-2)	// 校验值错
#define errCodeEssc_KeyLength			(errCodeOffsetOfEsscMDL-3)	// 密钥长度错
#define errCodeEssc_AppNotDefined		(errCodeOffsetOfEsscMDL-4)	// 应用未定义
#define errCodeEssc_LmkNotDefined		(errCodeOffsetOfEsscMDL-5)	// LMK未定义
#define errCodeEsscMDLHsmKeyPosOccupied		(errCodeOffsetOfEsscMDL-6)	// 密码机密钥位置已被占
#define errCodeEsscMDLNoAvaliableHsmKeyPos	(errCodeOffsetOfEsscMDL-7)	// 不可用的密码机密钥位置
#define errCodeEsscMDLNoEnoughHsmKeyPos		(errCodeOffsetOfEsscMDL-8)	// 密码机密钥位置不足
#define errCodeEsscMDLInitHsmKeyPosForSK	(errCodeOffsetOfEsscMDL-9)	// 为SK初始化密码机密钥位置
#define errCodeEsscMDLWrongMaxAgentNum		(errCodeOffsetOfEsscMDL-10)	// 错误的最大网点数量
#define errCodeEsscMDLWrongAgentNo		(errCodeOffsetOfEsscMDL-11)	// 错误的网点号
#define errCodeEsscMDLAgentNoAlreadExists	(errCodeOffsetOfEsscMDL-12)	// 网点已存在
#define errCodeEsscMDLAgentTBLIsFull		(errCodeOffsetOfEsscMDL-13)	// 网点表已满
#define errCodeEsscMDLWrongAgentType		(errCodeOffsetOfEsscMDL-14)	// 错误的网点类型
#define errCodeEsscMDLInvalidAgentType		(errCodeOffsetOfEsscMDL-15)	// 非法的网点类型
#define errCodeEsscMDLWrongMaxAtmNum		(errCodeOffsetOfEsscMDL-16)	// 错误的最大ATM数量
#define errCodeEsscMDLWrongAtmNo		(errCodeOffsetOfEsscMDL-17)	// 错误的ATM编号
#define errCodeEsscMDLAtmNoAlreadExists		(errCodeOffsetOfEsscMDL-18)	// ATM已存在
#define errCodeEsscMDLAtmTBLIsFull		(errCodeOffsetOfEsscMDL-19)	// ATM表已满
#define errCodeEsscMDLAtmNotExists		(errCodeOffsetOfEsscMDL-20)	// ATM不存在
#define errCodeEsscMDLWrongBranchNo		(errCodeOffsetOfEsscMDL-21)	// 错误的分行号
#define errCodeEsscMerelyGeneralBranchAction	(errCodeOffsetOfEsscMDL-22)	// 只有总行才可执行本操作
#define errCodeEsscNotGeneralBranchAction	(errCodeOffsetOfEsscMDL-23)	// 总行不能执行该操作
#define errCodeEsscKeyCannotGeneratedByGeneralBranch	(errCodeOffsetOfEsscMDL-24)	// 总行不能生成该密钥
#define errCodeEsscConnectAgentTBL		(errCodeOffsetOfEsscMDL-25)	// 连接网点表出错
#define errCodeEsscConnectAtmKeyTBL		(errCodeOffsetOfEsscMDL-26)	// 连接ATM表出错
#define errCodeEsscAtmUseBranchTMK		(errCodeOffsetOfEsscMDL-27)	// ATM使用分行的TMK
#define errCodeEsscMDLReqStr			(errCodeOffsetOfEsscMDL-28)	// 请求串出错
#define errCodeEsscMDLNotKMSvrServer		(errCodeOffsetOfEsscMDL-29)	// 不是密钥管理服务器
#define errCodeEsscMDLNotKMSvrClient		(errCodeOffsetOfEsscMDL-30)	// 不是密钥管理客户端
#define errCodeEsscMDLKeyOfSKAgentCannotBeStored	(errCodeOffsetOfEsscMDL-31)	// SK网点的密钥不能存储
#define errCodeEsscMDLKeyOperationNotPermitted	(errCodeOffsetOfEsscMDL-32)	// 密钥操作不允许
#define errCodeEsscMDLInvalidKeyOperation	(errCodeOffsetOfEsscMDL-33)	// 非法的密钥操作
#define errCodeEsscMDLInvalidKeyTable		(errCodeOffsetOfEsscMDL-34)	// 非法的密钥表
#define errCodeEsscMDLConnectKMSvrCommConf	(errCodeOffsetOfEsscMDL-35)	// 连接密钥管理服务器通讯配置
#define errCodeEsscMDL_InvalidMDLID		(errCodeOffsetOfEsscMDL-36)	// 非法的模块号
#define errCodeEsscMDL_ProcNotExistsAnymore	(errCodeOffsetOfEsscMDL-37)	// 进程已不存在
#define errCodeEsscMDL_MsgForThisModuleTooShort	(errCodeOffsetOfEsscMDL-38)	// 提供给本模块的消息太短了
#define errCodeEsscMDL_WaitResponse		(errCodeOffsetOfEsscMDL-39)	// 等待响应出错
#define errCodeEsscMDL_PeerDataError		(errCodeOffsetOfEsscMDL-40)	// 对方的数据有误
#define errCodeEsscMDL_TimeoutForReceivingData	(errCodeOffsetOfEsscMDL-41)	// 接收数据超时
#define errCodeEsscMDL_InvalidService		(errCodeOffsetOfEsscMDL-42)	// 非法的服务
#define errCodeEsscMDL_InvalidUseOfKey		(errCodeOffsetOfEsscMDL-43)	// 非法使用密钥
#define errCodeEsscMDL_CVVVerifyFailure		(errCodeOffsetOfEsscMDL-44)	// CVV验证失败
#define errCodeEsscMDL_TwoMacNotSame		(errCodeOffsetOfEsscMDL-45)	// 两个MAC不同
#define errCodeEsscMDL_AnotherTaskOfSameFunExists	(errCodeOffsetOfEsscMDL-46)	// 另一个同功能的任务已存在
#define errCodeEsscMDL_NotValidKeyDownloadRequest	(errCodeOffsetOfEsscMDL-47)	// 非法的密钥下载请求
#define errCodeEsscMDL_NotValidKeyFinishRequest	(errCodeOffsetOfEsscMDL-48)	// 非法的密钥完成请求
#define errCodeEsscMDL_HsmStoreKeysCmdNotFound	(errCodeOffsetOfEsscMDL-49)	// 密码机存储密钥指令未发现
#define errCodeEsscMDL_NoAvailableTmpKeyPos	(errCodeOffsetOfEsscMDL-50)	// 不可用的密钥暂时位置
#define errCodeEsscMDL_TmpKeyPosNotExists	(errCodeOffsetOfEsscMDL-51)	// 暂时密钥位置不存在
#define errCodeEsscMDL_ShortRetLenOfTmpKeyPosSvr	(errCodeOffsetOfEsscMDL-52)	// 暂时密钥位置服务器的返回长度太短
#define errCodeEsscMDL_HsmTaskAbnormal		(errCodeOffsetOfEsscMDL-53)	// 密码机任务异常
#define errCodeEsscMDL_BranchAlreadyExists	(errCodeOffsetOfEsscMDL-54)	// 分行已经存在
#define errCodeEsscMDL_ClientReqPack		(errCodeOffsetOfEsscMDL-55)	// 客户端请求报文错
#define errCodeEsscMDL_DecryptPin		(errCodeOffsetOfEsscMDL-57)	// PIN解密失败
#define errCodeEsscMDL_TranslatePin		(errCodeOffsetOfEsscMDL-58)	// PIN转换失败
#define errCodeEsscMDL_TranslateKey		(errCodeOffsetOfEsscMDL-59)	// 密钥转换失败
#define errCodeEsscMDL_InvalidEsscIPAddr	(errCodeOffsetOfEsscMDL-60)	// ESSCIP非法
#define errCodeEsscMDL_NotDefineEsscIPAddr	(errCodeOffsetOfEsscMDL-61)	// ESSCIP未定义
#define errCodeEsscMDL_TCPIPTaskTooIdle		(errCodeOffsetOfEsscMDL-62)	// TCPIPSvr的任务实例太空闲了
#define errCodeEsscMDL_EsscSpierClientAlreadyExists		(errCodeOffsetOfEsscMDL-63)	// 监控客户端已经存在
#define errCodeEsscMDL_EsscSpierClientTBLFull	(errCodeOffsetOfEsscMDL-64)	// 监控客户端列表已经满了
#define errCodeEsscMDL_EsscSpierClientNotExists	(errCodeOffsetOfEsscMDL-65)	// 监控客户端不存在
#define errCodeEsscMDL_EsscDefaultSpierClientCannotBeDeleted	(errCodeOffsetOfEsscMDL-66)	// 缺省监控客户端不能被删除
#define errCodeEsscMDL_FindEsscRes		(errCodeOffsetOfEsscMDL-67)	// 找不到资源
#define errCodeEsscMDL_ClientDataLenIsZero	(errCodeOffsetOfEsscMDL-68)	// 客户端数据长度是0
#define errCodeEsscMDL_ClientDataLenTooLong	(errCodeOffsetOfEsscMDL-69)	// 客户端数据长度太长
#define errCodeEsscMDL_ClientDataLenTooShort	(errCodeOffsetOfEsscMDL-70)	// 客户端数据长度太短
#define errCodeEsscMDL_InvalidResID		(errCodeOffsetOfEsscMDL-71)	// 非法的资源ID
#define errCodeEsscMDL_WrongResInterprotorCalled	(errCodeOffsetOfEsscMDL-72)	// 调用了错误的资源管得程序
#define errCodeEsscMDL_WrongResMngSvrCmd	(errCodeOffsetOfEsscMDL-73)	// 非法的资源管理命令
#define errCodeEsscMDL_WrongPasswordLength	(errCodeOffsetOfEsscMDL-74)	// 密码长度错
#define errCodeEsscMDL_ConnectEsscSpierClientTBL	(errCodeOffsetOfEsscMDL-75)	// 连接监控客户端控制表
#define errCodeEsscMDL_ResCannotBeSpied		(errCodeOffsetOfEsscMDL-76)	// 不资源不可以被监控
#define errCodeEsscMDL_CommErrForSendingResSpierInfo	(errCodeOffsetOfEsscMDL-77)	// 因通讯原因发送资源监控信息失败
#define errCodeEsscMDL_TransSpierCommManagePoolIsFull	(errCodeOffsetOfEsscMDL-78)	// 交易监控通讯管理池已满
#define errCodeEsscMDL_NoMaxHsmGrpNumDefinedInLocalKMSvrDef	(errCodeOffsetOfEsscMDL-79)	// 在本地密钥管理配置文件中未定义密码机组最大数目
#define errCodeEsscMDL_ReqAndResNotIsIndentified	(errCodeOffsetOfEsscMDL-80)	// 请求与响应不匹配
#define errCodeEsscMDL_NoLocalKMSvrDefinedForHsmGrp	(errCodeOffsetOfEsscMDL-81)	// 没有为指定的密码机组定义本地密钥管理服务器
#define errCodeEsscMDL_NoRemoteKMSvrDefinedForContainer	(errCodeOffsetOfEsscMDL-82)	// 没有为指定的密钥容器定义远程密钥管理服务器
#define errCodeEsscMDL_TooMuchEsscPackageFld	(errCodeOffsetOfEsscMDL-83)	// 太多的ESSC报文域
#define errCodeEsscMDL_EsscPackageFldNotFound	(errCodeOffsetOfEsscMDL-84)	// ESSC报文域未在该报文中定义
#define errCodeEsscMDL_EsscPackageFldLength	(errCodeOffsetOfEsscMDL-85)	// ESSC报文域长度错
#define errCodeEsscMDL_EsscPackageLength	(errCodeOffsetOfEsscMDL-86)	// ESSC报文长度错
#define errCodeEsscMDL_NotEsscRequestPackage	(errCodeOffsetOfEsscMDL-87)	// 不是ESSC请求报文
#define errCodeEsscMDL_NotEsscResponsePackage	(errCodeOffsetOfEsscMDL-88)	// 不是ESSC响应报文
#define errCodeEsscMDL_RemoteKeyMngNotPermitted	(errCodeOffsetOfEsscMDL-89)	// 不允许远程管理该密钥
#define errCodeEsscMDL_NotPermitUseKey		(errCodeOffsetOfEsscMDL-90)	// 不允许使用密钥
#define errCodeEsscMDL_ZmkAndWKNotOwnedBySameOne	(errCodeOffsetOfEsscMDL-91)	// ZMK和工作密钥不属于同一属主
#define errCodeEsscMDL_WrongUsageOfKey		(errCodeOffsetOfEsscMDL-92)	// 非法使用密钥
#define errCodeEsscMDL_NotKMSvr			(errCodeOffsetOfEsscMDL-93)	// 不是KMSvr
#define errCodeEsscMDL_NoContainerSetForKey	(errCodeOffsetOfEsscMDL-94)	// 没有为密钥设置container
#define errCodeEsscMDL_InvalidOperation		(errCodeOffsetOfEsscMDL-95)	// 非法操作
#define errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile	(errCodeOffsetOfEsscMDL-96)	// 在mngSvr的客户端定义文件中没有定义ESSC的IP地址
#define errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile	(errCodeOffsetOfEsscMDL-97)	// 在mngSvr的客户端定义文件中没有定义ESSC的端口
#define errCodeEsscMDL_DataLenFromMngSvr	(errCodeOffsetOfEsscMDL-98)	// 在mngSvr送来的数据长度错
#define errCodeEsscMDL_NoResponsePackFromMngSvr	(errCodeOffsetOfEsscMDL-99)	// mngSvr未回送响应包
#define errCodeEsscMDL_AnotherStartingSendDataCmd	(errCodeOffset2OfEsscMDL-1)	// 又一个mngSvr数据开始发送包
#define errCodeEsscMDL_NotFinishSendingData	(errCodeOffset2OfEsscMDL-2)	// mngSvr数据传送未结束
#define errCodeEsscMDL_FailToGetTempFile	(errCodeOffset2OfEsscMDL-3)	// 不能获取暂时文件
#define errCodeEsscMDL_NoStartingSendDataCmdBefore	(errCodeOffset2OfEsscMDL-4)	// 没有收到过数据开始传输包
#define errCodeEsscMDL_FldNotExists		(errCodeOffset2OfEsscMDL-5)	// 域不存在
#define errCodeEsscMDL_PVVVerifyFailure		(errCodeOffset2OfEsscMDL-6)	// PVV验证失败
#define errCodeEsscMDL_NoOperationAuthority	(errCodeOffset2OfEsscMDL-7)	// 无操作权限
#define errCodeEsscMDL_NotOperationDefLine	(errCodeOffset2OfEsscMDL-8)	// 不是操作定义行
#define errCodeEsscMDL_NoSuchOperationDefined	(errCodeOffset2OfEsscMDL-9)	// 没有定义操作
#define errCodeEsscMDL_FldIndexOutOfPackageFldNum	(errCodeOffset2OfEsscMDL-10)	// 域索引号超出了域数目
#define errCodeEsscMDL_PackageFldValueLen	(errCodeOffset2OfEsscMDL-11)	// 域值长度错
#define errCodeEsscMDL_ResSpiedAlreadyOpened	(errCodeOffset2OfEsscMDL-12)	// 对该资源监控已被打开
#define errCodeEsscMDL_ResSpiedNotOpened	(errCodeOffset2OfEsscMDL-13)	// 对该资源监控未被打开
#define errCodeEsscMDL_EsscResAlreadyExists	(errCodeOffset2OfEsscMDL-14)	// 资源已经定义
#define errCodeEsscMDL_EsscResNotExists		(errCodeOffset2OfEsscMDL-15)	// 资源定义不存在
#define errCodeEsscMDL_EsscAppAlreadyExists	(errCodeOffset2OfEsscMDL-16)	// 应用已经定义
#define errCodeEsscMDL_EsscAppIDMustInput	(errCodeOffset2OfEsscMDL-17)	// 必须输入应用ID号
#define errCodeEsscMDL_LocalKMSvrDefAlreadyExists	(errCodeOffset2OfEsscMDL-18)	// 本地密钥管理服务器定义已存在
#define errCodeEsscMDL_EsscHsmGrpIDMustInput	(errCodeOffset2OfEsscMDL-19)	// 密码机组必须输入
#define errCodeEsscMDL_EsscHsmGrpIDNotDefined	(errCodeOffset2OfEsscMDL-20)	// 密码机组没有定义
#define errCodeEsscMDL_RemoteKMSvrDefAlreadyExists	(errCodeOffset2OfEsscMDL-21)	// 远程密钥管理服务器定义已存在
#define errCodeEsscMDL_NotEsscFldTagDefLine	(errCodeOffset2OfEsscMDL-22)	// 不是EsscFldTag定义行
#define errCodeEsscMDL_ThisOperationForbidden	(errCodeOffset2OfEsscMDL-23)	// 操作被禁止
#define errCodeEsscMDL_SendRequestTimeout	(errCodeOffset2OfEsscMDL-24)	// 发送请求超时
#define errCodeEsscMDL_SendResponseTimeout	(errCodeOffset2OfEsscMDL-25)	// 发送响应超时
#define errCodeEsscMDL_ResponseCodeNotSuccess	(errCodeOffset2OfEsscMDL-26)	// 响应码不是成功
#define errCodeEsscMDL_ResponseToShort		(errCodeOffset2OfEsscMDL-27)	// 响应数据太短
#define errCodeEsscMDL_ZmkIndexNotDefined	(errCodeOffset2OfEsscMDL-28)	// ZMK索引没有定义
#define errCodeEsscMDL_VerifyAutSign		(errCodeOffset2OfEsscMDL-29)	// 验证自动身份签名出错
#define errCodeEsscMDL_IDOfAppNotDefined	(errCodeOffset2OfEsscMDL-30)	// 未定义应用
#define errCodeEsscMDL_RsaPairNotDefinedForApp	(errCodeOffset2OfEsscMDL-31)	// 没有为应用定义私钥对
#define errCodeEsscMDL_NotMngSvrDefDefLine	(errCodeOffset2OfEsscMDL-32)	// 不是mngSvr的定义行。 2007/12/22
// Mary add begin, 2008-9-9
#define errCodeEsscMDL_BufferIsFull		(errCodeOffset2OfEsscMDL-33)	// 缓冲池已满
#define errCodeEsscMDL_ReachPreDefinedTime	(errCodeOffset2OfEsscMDL-34)	// 到达预定义时间
// Mary add end, 2008-9-9
#define errCodeEsscMDL_MngSvrNotDefined		(errCodeOffset2OfEsscMDL-35)	// 未定义mngSvr
#define errCodeEsscMDL_SecuControlInactive	(errCodeOffset2OfEsscMDL-36)	// 控件未激活
#define errCodeEsscMDL_SecuControlOutdate	(errCodeOffset2OfEsscMDL-37)	// 控件失效
#define errCodeEsscMDL_SecuControlExceedMaxDownloadTimes	(errCodeOffset2OfEsscMDL-38)	// 控件下载次数超过规定次数
#define errCodeEsscMDL_DBSvrNotDefined		(errCodeOffset2OfEsscMDL-39)	// DBServer没定义
#define errCodeEsscMDL_KeyObjectTypeNotSupported	(errCodeOffset2OfEsscMDL-40)	// 密钥对象类型不支持
#define errCodeEsscMDL_VKPosNotOccupiedByThisApp	(errCodeOffset2OfEsscMDL-41)	// 私钥位置不是被这个应用占用
#define errCodeEsscMDL_HsmVKSpaceSizeTooSmall	(errCodeOffset2OfEsscMDL-42)	// 私钥空间太小
#define errCodeEsscMDL_TBLDefGrpInitFailure	(errCodeOffset2OfEsscMDL-43)	// 表定义组初始化失败
#define errCodeEsscMDL_TooManyParentTBLs	(errCodeOffset2OfEsscMDL-44)	// 太多父表了
#define errCodeEsscMDL_TooManyChildTBLs		(errCodeOffset2OfEsscMDL-45)	// 太多子表了
#define errCodeEsscMDL_HsmDesKeySpaceSizeTooSmall	(errCodeOffset2OfEsscMDL-46)	// DES密钥存储空间太小
#define errCodeEsscMDL_InvalidAlgorithmID	(errCodeOffset2OfEsscMDL-47)	// 算法标识非法
#define errCodeEsscMDL_KeyStatusDisabled	(errCodeOffset2OfEsscMDL-48)	// 密钥未启用
#define errCodeEsscMDL_KeyOutputNotPermitted	(errCodeOffset2OfEsscMDL-49)	// 密钥不允许导出
#define errCodeEsscMDL_KeyStatusNotActive	(errCodeOffset2OfEsscMDL-50)	// 密钥未生效
#define errCodeEsscMDL_KeyInputNotPermitted	(errCodeOffset2OfEsscMDL-51)	// 密钥不允许导入
//add begin by lusj 20151116
#define errCodeEsscMDL_maxConnIsFull    (errCodeOffset2OfEsscMDL-52)    // sysID最大链接数被超过
//add end by lusj 20151116



// 密钥CACHE模块错误
// offset = -11000
#define errCodeKeyCacheMDL_KeyNonExists		(errCodeOffsetOfKeyCacheMDL-1)	// 密钥不存在
#define errCodeKeyCacheMDL_KeyNum		(errCodeOffsetOfKeyCacheMDL-2)	// 密钥数量错
#define errCodeKeyCacheMDL_KeyDBFull		(errCodeOffsetOfKeyCacheMDL-3)	// 库满
#define errCodeKeyCacheMDL_KeyDBNonConnected	(errCodeOffsetOfKeyCacheMDL-4)	// 库未连接
#define errCodeKeyCacheMDL_InvalidKeyLength	(errCodeOffsetOfKeyCacheMDL-5)	// 非法密钥长度
#define errCodeKeyCacheMDL_KeyAlreadyExists	(errCodeOffsetOfKeyCacheMDL-6)	// 密钥已存在
#define errCodeKeyCacheMDL_KeyOutdate		(errCodeOffsetOfKeyCacheMDL-7)	// 密钥过期
#define errCodeKeyCacheMDL_KeyValueWithoutEnd	(errCodeOffsetOfKeyCacheMDL-8)	// 密钥值无结束符
#define errCodeKeyCacheMDL_WrongKeyName		(errCodeOffsetOfKeyCacheMDL-9)	// 密钥名称不对

// offset = -33000
// 一般银行主机报文错误
#define errCodeBankBasicPackageMDL_InvalidDDU				(errCodeOffsetOfBankBasicPackageMDL-1)	// 非法的DDU
#define errCodeBankBasicPackageMDL_LenOfValueLargerThanMaxLenDefined	(errCodeOffsetOfBankBasicPackageMDL-2)	// 值的长度大于定义的值
#define errCodeBankBasicPackageMDL_DDULength				(errCodeOffsetOfBankBasicPackageMDL-3)	// DDU的长度错
#define errCodeBankBasicPackageMDL_DDUValueIsNull			(errCodeOffsetOfBankBasicPackageMDL-4)	// DDU的值是空
#define errCodeBankBasicPackageMDL_LenOfValueLargerThanStr		(errCodeOffsetOfBankBasicPackageMDL-5)	// 值的长度比串的长度大
#define errCodeBankBasicPackageMDL_FldOfNameNotDefinedInSpecForm	(errCodeOffsetOfBankBasicPackageMDL-6)	// 在表格中未定义该域
#define errCodeBankBasicPackageMDL_InvalidFormPointer			(errCodeOffsetOfBankBasicPackageMDL-7)	// 非法的表格指针
#define errCodeBankBasicPackageMDL_FormIsFull				(errCodeOffsetOfBankBasicPackageMDL-8)	// 表格为空
#define errCodeBankBasicPackageMDL_FormInvalid				(errCodeOffsetOfBankBasicPackageMDL-9)	// 表格非法
#define errCodeBankBasicPackageMDL_CreateFormRec			(errCodeOffsetOfBankBasicPackageMDL-10)	// 创建表格记录出错
#define errCodeBankBasicPackageMDL_FormCurrentRecNotInitialized		(errCodeOffsetOfBankBasicPackageMDL-11)	// 表格的当前记录未初始化
#define errCodeBankBasicPackageMDL_LocalAndRemoteFormNameNotSame	(errCodeOffsetOfBankBasicPackageMDL-12)	// 本地和远端的表格名不相同
#define errCodeBankBasicPackageMDL_LocalAndRemoteFldNameNotSame		(errCodeOffsetOfBankBasicPackageMDL-13)	// 本地和远端的域名不相同
#define errCodeBankBasicPackageMDL_DDUOfNameNotFoundInTransData		(errCodeOffsetOfBankBasicPackageMDL-14)	// 交易数据中没有该名称的DDU
#define errCodeBankBasicPackageMDL_FormOfNameNotFoundInTransData	(errCodeOffsetOfBankBasicPackageMDL-15)	// 交易数据中没有该名称的表格
#define errCodeBankBasicPackageMDL_NotSupportedUnit			(errCodeOffsetOfBankBasicPackageMDL-16)	// 不支持的单元
#define errCodeBankBasicPackageMDL_ConnectBankBasicPackageMDL		(errCodeOffsetOfBankBasicPackageMDL-17)	// 联接BankBasic报文模块出错
#define errCodeBankBasicPackageMDL_BankBasicPackageNotConnected		(errCodeOffsetOfBankBasicPackageMDL-18)	// BankBasic报文模块未联连

// Mary add begin, 20080717
// offset = -32000
// 数据库的错误码
#define errCodeDatabaseMDL_DatabaseAlreadyExist		(errCodeOffsetOfDatabase-1)	// 数据库已经存在
#define errCodeDatabaseMDL_DatabaseNotFound		(errCodeOffsetOfDatabase-2)	// 数据库没有找到
#define errCodeDatabaseMDL_TableAlreadyExist		(errCodeOffsetOfDatabase-3)	// 表已经存在
#define errCodeDatabaseMDL_TableNotFound		(errCodeOffsetOfDatabase-4)	// 表没有找到
#define errCodeDatabaseMDL_RecordNotFound		(errCodeOffsetOfDatabase-5)	// 记录没有找到
#define errCodeDatabaseMDL_RecordAlreadyExist		(errCodeOffsetOfDatabase-6)	// 记录已经存在
#define errCodeDatabaseMDL_NoData			(errCodeOffsetOfDatabase-7)	// 没有数据
// Mary add end, 20080717
#define errCodeDatabaseMDL_MoreRecordFound		(errCodeOffsetOfDatabase-8)     // 找到太多数据		20090609，陈家梅加，张永定要求
#define errCodeDatabaseMDL_RecordUpdateFailure		(errCodeOffsetOfDatabase-9)	// 更新数据失败
#define errCodeDatabaseMDL_MoreTable			(errCodeOffsetOfDatabase-10)	// 表太多
#define errCodeDatabaseMDL_ReconnectDataBase		(errCodeOffsetOfDatabase-11)	// 重连数据库

// xusj add begin, 20090323
// offset = -61000
// 二进制文件的错误码
#define errCodeBinaryFileMDL_ConditionErr		(errCodeOffsetOfBinaryFileDB-4)	// 查询条件格式错误
#define errCodeBinaryFileMDL_RecordNotFound		(errCodeOffsetOfBinaryFileDB-5)	// 记录没有找到
#define errCodeBinaryFileMDL_RecordAlreadyExist		(errCodeOffsetOfBinaryFileDB-6)	// 记录已经存在
#define errCodeBinaryFileMDL_TableDefErr		(errCodeOffsetOfBinaryFileDB-7)	// 表结构定义错误
// xusj add end, 20090323

// xusj add begin, 20090823
// offset = -62000
// 在线密钥管理的错误码
#define errCodeOnlineKeyMngMDL_authCodeErr		(errCodeOffsetOfOnlineKeyMng-1)	// 授权码错误
#define errCodeOnlineKeyMngMDL_authCodeOverTime		(errCodeOffsetOfOnlineKeyMng-2)	// 授权码已过期
#define errCodeOnlineKeyMngMDL_authCodeOverUseTimes	(errCodeOffsetOfOnlineKeyMng-3)	// 授权码超过使用次数
#define errCodeOnlineKeyMngMDL_secuControlInactive	(errCodeOffsetOfOnlineKeyMng-4)	// 安全控件未激活
#define errCodeOnlineKeyMngMDL_secuControlOverTime	(errCodeOffsetOfOnlineKeyMng-5)	// 安全控件已过期
#define errCodeOnlineKeyMngMDL_secuControlOverUseTimes	(errCodeOffsetOfOnlineKeyMng-6)	// 安全控件超过使用次数
#define errCodeOnlineKeyMngMDL_generateKey		(errCodeOffsetOfOnlineKeyMng-7)	// 产生密钥失败
#define errCodeOnlineKeyMngMDL_storeKey			(errCodeOffsetOfOnlineKeyMng-8)	// 保存密钥到密钥库失败
#define errCodeOnlineKeyMngMDL_overMaxNumOfKey		(errCodeOffsetOfOnlineKeyMng-9)	// 产生密钥超过最大值
#define errCodeOnlineKeyMngMDL_GetKeyModel		(errCodeOffsetOfOnlineKeyMng-10)	// 获取终端主密钥的密钥模板失败
#define errCodeOnlineKeyMngMDL_GetMyKmcID		(errCodeOffsetOfOnlineKeyMng-11)	// 获取本KMC的ID失败
#define errCodeOnlineKeyMngMDL_CallSecutrlOfNode	(errCodeOffsetOfOnlineKeyMng-12)	// 与节点上的安全控件通讯失败
#define errCodeOnlineKeyMngMDL_NotFoundPKFile           (errCodeOffsetOfOnlineKeyMng-13)        // 控件没有找到KMC的PK文件
#define errCodeOnlineKeyMngMDL_veryfySignFail           (errCodeOffsetOfOnlineKeyMng-14)        // 验证签名失败
#define errCodeOnlineKeyMngMDL_GetSignFail              (errCodeOffsetOfOnlineKeyMng-15)        // 获取签名失败
#define errCodeOnlineKeyMngMDL_keyInOthSysIsUsing       (errCodeOffsetOfOnlineKeyMng-16)        // 密钥副本还在使用
#define errCodeOnlineKeyMngMDL_hsmKeyPosIsOccupied         (errCodeOffsetOfOnlineKeyMng-17)        // 密码机密钥索引已被占用
// xusj add end, 20090823

// 证书模块错误码
#define errCodeOffsetOfCertificate_InvalidDate		(errCodeOffsetOfCertificate-1)	// 非法日期格式
#define errCodeOffsetOfCertificate_DateTooBig		(errCodeOffsetOfCertificate-2)	// 日期太大
#define errCodeOffsetOfCertificate_InvalidRID		(errCodeOffsetOfCertificate-3)	// 非法的RID
#define errCodeOffsetOfCertificate_InvalidServiceID	(errCodeOffsetOfCertificate-4)	// 非法的服务标识
#define errCodeOffsetOfCertificate_InvalidRecordHead	(errCodeOffsetOfCertificate-5)	// 非法的记录头
#define errCodeOffsetOfCertificate_HashValueErr		(errCodeOffsetOfCertificate-6)	// 哈希值错误
#define errCodeOffsetOfCertificate_InvalidRecordTail	(errCodeOffsetOfCertificate-7)	// 非法的记录尾

#define errCodeOffsetOfCertMDL_FileNotExist		(errCodeOffsetOfCertMDL-1)	//文件不存在
#define errCodeOffsetOfCertMDL_ReadFile			(errCodeOffsetOfCertMDL-2)	//读文件失败
#define errCodeOffsetOfCertMDL_WriteFile		(errCodeOffsetOfCertMDL-3)	//写文件失败
#define errCodeOffsetOfCertMDL_FileFormat		(errCodeOffsetOfCertMDL-4)	//文件格式不匹配
#define errCodeOffsetOfCertMDL_VerifyCertReq		(errCodeOffsetOfCertMDL-5)	//验证证书请求失败
#define errCodeOffsetOfCertMDL_VerifyCert		(errCodeOffsetOfCertMDL-6)	//验证证书失败
#define errCodeOffsetOfCertMDL_Passwd			(errCodeOffsetOfCertMDL-7)	//密码不正确
#define errCodeOffsetOfCertMDL_Arithmetic		(errCodeOffsetOfCertMDL-8)	//运算过程失败
#define errCodeOffsetOfCertMDL_SmallBuffer		(errCodeOffsetOfCertMDL-9)	//太小的缓冲区
#define errCodeOffsetOfCertMDL_CodeParameter		(errCodeOffsetOfCertMDL-10)	//参数错
#define errCodeOffsetOfCertMDL_Sign			(errCodeOffsetOfCertMDL-11)	//签名失败
#define errCodeOffsetOfCertMDL_VerifySign		(errCodeOffsetOfCertMDL-12)	//验证签名失败
#define errCodeOffsetOfCertMDL_OpenSSl			(errCodeOffsetOfCertMDL-13)	//openssl调用出错
#define errCodeOffsetOfCertMDL_PKNotMatchVK		(errCodeOffsetOfCertMDL-14)	//rsa公私钥不匹配
#define errCodeOffsetOfCertMDL_AlgorithmFlag		(errCodeOffsetOfCertMDL-15)	//算法标识不存在
#define errCodeOffsetOfCertMDL_CaNotExist		(errCodeOffsetOfCertMDL-16)	//根证书记录不存在
#define errCodeOffsetOfCertMDL_RootCertExpire		(errCodeOffsetOfCertMDL-17)     //根证书已过有效期
#define errCodeOffsetOfCertMDL_OutOfDate		(errCodeOffsetOfCertMDL-18)     //证书已过有效期


// 动态口令系统错误码 errCodeOffsetOfOTPS
#define errCodeOffsetOfOTPS_SNIsGEN			(errCodeOffsetOfOTPS-1)   //令牌数据已经产生
#define errCodeOffsetOfOTPS_SNLenERR			(errCodeOffsetOfOTPS-2)
#define errCodeOffsetOfOTPS_exportFileOpenErr		(errCodeOffsetOfOTPS-3)
#define errCodeOffsetOfOTPS_qcodeLenIsTooLong		(errCodeOffsetOfOTPS-4)//挑战值太长
#define errCodeOffsetOfOTPS_tokenStatusErr		(errCodeOffsetOfOTPS-5) //令牌状态错误
#define errCodeOffsetOfOTPS_keyCheckValueErr		(errCodeOffsetOfOTPS-6)//秘钥校验值错误
#define errCodeOffsetOfOTPS_keyBackupFileNotExist	(errCodeOffsetOfOTPS-7)//秘钥备份文件不存在
#define errCodeOffsetOfOTPS_snIsNotRegister		(errCodeOffsetOfOTPS-8)//号段数据未注册
#define errCodeOffsetOfOTPS_tokenTemplateIsNoExist	(errCodeOffsetOfOTPS-9)//令牌类不存在
#define errCodeOffsetOfOTPS_seedExportFileIsNoExist	(errCodeOffsetOfOTPS-10) //种子导入文件不存在
#define errCodeOffsetOfOTPS_tokenSeqHasNotGen		(errCodeOffsetOfOTPS-11)//令牌序号还为长生
#define errCodeOffsetOfOTPS_tokenSeedHasGen		(errCodeOffsetOfOTPS-12) //种子已经产生
#define errCodeOffsetOfOTPS_otpErr			(errCodeOffsetOfOTPS-13)//动态口令错误
#define errCodeOffsetOfOTPS_tokenERR			(errCodeOffsetOfOTPS-14) //令牌错误
#define errCodeOffsetOfOTPS_manuCodeIsNotExist		(errCodeOffsetOfOTPS-15) //没有厂商号
#define errCodeOffsetOfOTPS_tokenIDnotExist		(errCodeOffsetOfOTPS-16)//没有这个令牌
#define errCodeOffsetOfOTPS_tokenRecordError		(errCodeOffsetOfOTPS-17)//令牌记录错误
#define errCodeOffsetOfOTPS_tokenIsLock			(errCodeOffsetOfOTPS-18)//令牌锁定
#define errCodeOffsetOfOTPS_tokenIsHangUp		(errCodeOffsetOfOTPS-19)//令牌挂起
#define errCodeOffsetOfOTPS_tokenHasNoActive		(errCodeOffsetOfOTPS-20) //令牌未激活
#define errCodeOffsetOfOTPS_tokenIsCancel		(errCodeOffsetOfOTPS-21) //令牌已废止
#define errCodeOffsetOfOTPS_tokenIsOverDue		(errCodeOffsetOfOTPS-22) //令牌已过期
#define errCodeOffsetOfOTPS_tokenKeyDeriveType		(errCodeOffsetOfOTPS-23) //令牌变形算法未定义
#define errCodeOffsetOfOTPS_windowsTypeErr		(errCodeOffsetOfOTPS-24) //窗口类型值错
#define errCodeOffsetOfOTPS_seedFileNameFormatErr	(errCodeOffsetOfOTPS-25) //种子文件名格式错 otp_001001_20140122_2.data
#define errCodeOffsetOfOTPS_importSeedHaveErr		(errCodeOffsetOfOTPS-26) //导入种子时有错
#define errCodeOffsetOfOTPS_otpUsed			(errCodeOffsetOfOTPS-27) //导入种子时有错
#define errCodeOffsetOfOTPS_OTPIsOverDue              	(errCodeOffsetOfOTPS-28) //口令已过期
#define errCodeOffsetOfOTPS_OTPIsOverMaxErrTimes        (errCodeOffsetOfOTPS-29) //口令已超过最大错误次数


//DPS 数据准备系统
#define errCodeOffsetOfDPS_repeat                       (errCodeOffsetOfDPS-1)   //制卡数据文件记录有重复
#define errCodeOffsetOfDPS_notStandardized              (errCodeOffsetOfDPS-2)   //制卡数据文件记录不规范
#define errCodeOffsetOfDPS_connectionException          (errCodeOffsetOfDPS-3)	//DPC和DPS链接异常

//pboc

#define errCodeOffsetOfPBOC_iccTypeERR (errCodeOffsetOfPBOC-1) //iccType错误
#define errCodeOffsetOfPBOC_encryptModeErr (errCodeOffsetOfPBOC-2) //加密模式错 
#define errCodeOffsetOfPBOC_ivFormatErr (errCodeOffsetOfPBOC-3)//iv格式错
#define errCodeOffsetOfPBOC_arcFormatErr (errCodeOffsetOfPBOC-4)//arc格式错

#endif

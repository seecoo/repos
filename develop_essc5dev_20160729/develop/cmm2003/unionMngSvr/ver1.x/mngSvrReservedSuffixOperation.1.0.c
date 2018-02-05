#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "UnionLog.h"

#ifdef _WIN32
#include "unionRECVar.h"
#else
#include "unionREC.h"
#include "UnionTask.h"
#endif

int UnionExcuteSpecAppSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr);

int UnionExcuteSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr)
{
	switch (serviceID)
	{
		case	conResCmdQuerySpecRec:		// 读取记录
		case	conResCmdQueryAllRec:		// 查询所有记录
		case	conResCmdQueryWithCondition:	// 查询符合条件所有记录
			return(0);
		default:
			break;
	}
	switch (resID)
	{
		//日志注册表
		case	conResIDLogTBL:	
			return(UnionReloadLogFileTBL());
		//任务注册表
		case	conResIDTaskTBL:
			return UnionReloadTaskTBL();
		//客户化参数表
		case	conResIDREC:
			return UnionReloadREC();
		//密钥备份服务器
		case	conResIDKeyDBBackuper:
			return UnionLoadKeyDBBackupServerIntoMemory();
		//消息交换区
		case	conResIDMsgBuf:
			return UnionReloadMsgBufDef();
			/*
			UnionDisconnectMsgBufMDL();
			return UnionReconnectMsgBufMDLAnyway();
			*/
		//密钥服务器的缓冲
		case	conResIDKDBSvrBuf:
			return UnionReloadKDBSvrBufDef();
			
		//监控信息缓冲区
		case	conResIDTransSpierBuf:
			return UnionReloadTransSpierBufDef();
		/*
		//通讯客户端表
		case	conResIDCommConf:
		//操作员表
		case	conResIDOperatorTBL:
		//PK库
		case	conResIDPKDB:
		*/		
		// 锁表
		case	conResIDLockTBL:
			return UnionReloadLockTBL();
		//密码机错误
		case	conResIDHsmError:
			return UnionReloadSoftErrCodeTBL();
		//ESSC错误
		case	conResIDError:
			return UnionReloadErrCodeTranslater();
		/*
		// 不知道该包含哪个库
		//密码机工作组
		case	conResIDHsmGrp:
			return UnionAutoLoadSJL06IntoSJL06MDL();
		//监控客户端控制表
		case	conResIDSpierClientTBL:
			return UnionReloadEsscResSpierClientTBL();
		*/
		default:
			return(UnionExcuteSpecAppSuffixOperation(resID,serviceID,reqStr,lenOfReqStr));
			/*
			// essc
			//应用配置
			case	conResIDAppConf:
				return UnionReloadAppTable();
			//本地密钥管理服务器配置
			case	conResIDLocalKMSvr:
				return UnionReloadLocalKMSvr();
			//远程密钥管理服务器配置
			case	conResIDRemoteKMSvr:
				return UnionReloadRemoteKMSvr();
			*/
			//return 0;
	}
}

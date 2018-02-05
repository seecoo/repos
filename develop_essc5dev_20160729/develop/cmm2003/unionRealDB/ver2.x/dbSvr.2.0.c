//	Author:		zhang yong ding
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-04-23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#if ( defined __linux__ )
#include <zlib.h>
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionREC.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionCommand.h"
#include "unionRealDBCommon.h"
#include "unionMsgBufGroup.h"

PUnionTaskInstance	ptaskInstance = NULL;
jmp_buf 		gunionDBSvrJmpEnv;

void UnionDealCloseDatabase();

int UnionHelp()
{
	printf("Usage:: %s \n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	//UnionCloseDatabase();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

static int UnionTestDBByQuery(char *tableName)
{
	return(UnionSelectRealDBRecordCounts(tableName, "", ""));
}
int UnionSetDatabaseLastConnTime();
int UnionSetMsgTimeout(int t);
static int UnionIsDBOK()
{
	char		tableName[64], *ptr = NULL;
	
	UnionSetDatabaseLastConnTime(); // 更新访问时间，防止数据库重连
	if ((ptr = UnionReadStringTypeRECVar("testDBTable")) == NULL || strlen(ptr) == 0)
	{
		strcpy(tableName, "sysUser");
	}
	else
	{
		strcpy(tableName, ptr);
	}

	if(UnionTestDBByQuery(tableName) < 0)
	{
		return(0);
	}
	else
	{
		return(1);
	}
}

int main(int argc,char *argv[])
{
	int			ret;
	int			currentPage;
	int			numOfPerPage;
	int			maxIdleTimeOfDBConn;
	int			sizeOfBuf = 0;
	int			fieldType = 0;
	int			returnSql = 0;
	TUnionModuleID		applierMDLID;
	char			reqBuf[81920];
	char			tmpBuf[204800];
	char			resBuf[204800];
	unsigned long		len;
	char			command[32];
	char			tableName[512];
	char			condition[2048];
	char			joinCondition[2048];
	char			rootValue[256];
	char			parentFieldName[128];
	char			fieldName[128];
	char			*ptr = NULL;
	TUnionMessageHeader	msgHeader;
	int			timeoutOfDB;
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(0);
	}

	// 设置连接空闲时间控制
	if ((maxIdleTimeOfDBConn = UnionReadIntTypeRECVar("maxIdleTimeOfDBConn")) < 0)
		maxIdleTimeOfDBConn = 60;

	if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) < 0)
		timeoutOfDB = 20;
	UnionSetMsgTimeout(++timeoutOfDB); // 设置消息过期时间

	for (;;)
	{
		//UnionSetMultiLogBegin(); remarked 2015-06-18
		alarm(0);
#ifdef _WIN32
		// add codes here!
#else
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionDBSvrJmpEnv,1) != 0) // 超时退出
#elif ( defined _AIX )
		if (setjmp(gunionDBSvrJmpEnv) != 0)	// 超时退出
#endif
		{
			// modified 20160628 数据库正常不重连数据库
			/*
			UnionCloseDatabase();
			UnionConnectDatabase();
			*/
			if(!UnionIsDBOK())
			{
				UnionLog("in %s:: TEST DB failure!\n", UnionGetApplicationName());
				UnionCloseDatabase();
				UnionConnectDatabase();
			}
			else
			{
				UnionLog("in %s:: TEST DB OK!\n", UnionGetApplicationName());
			}
			// modified end 20160628
			UnionSetMultiLogEnd(); //added 2015-06-18
			//goto readMsg;//20150619
			continue;
		}
		alarm(maxIdleTimeOfDBConn);
		signal(SIGALRM,UnionDealCloseDatabase);
#endif

//readMsg:
		UnionSetMultiLogBegin(); //added 2015-06-18
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
		if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr,(unsigned char *)reqBuf,sizeof(reqBuf),&applierMDLID,&msgHeader)) <= 0)
		{
			UnionUserErrLog("in %s:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%d]!\n",UnionGetApplicationName(),conMDLTypeOffsetOfDbSvr + conMDLTypeUnionDBSvr);
			continue;
		}
		alarm(0); 
		if (len < defLenOfCommand)
		{
			UnionUserErrLog("in %s:: len = [%ld]\n",UnionGetApplicationName(),len);
			ret = errCodeParameter;
			goto sendToClient;
		}
		reqBuf[len] = 0;
		
		memcpy(command,reqBuf,defLenOfCommand);
		command[defLenOfCommand] = 0;
		UnionProgramerLog("in %s:: UnionReadRequestToSpecifiedModuleWithMsgHeader from [%ld] OK! len = [%ld] command = [%s]\n",UnionGetApplicationName(),applierMDLID,len,command);

		switch (atoi(command))
		{
			// 查询数据库
			case	conDBSvrCmdSelect:
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) != NULL)
				{
					numOfPerPage = atoi(ptr + 1);
					*ptr = 0;
				}
				else
					numOfPerPage = 0;
				
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) != NULL)
				{
					currentPage = atoi(ptr + 1);
					*ptr = 0;
				}
				else
					currentPage = 0;
				
				if ((ret = UnionSelectRealDBRecord(reqBuf + defLenOfCommand,currentPage,numOfPerPage)) <= 0)
				{
					UnionLog("in %s:: UnionSelectRealDBRecord!\n",UnionGetApplicationName());
					if (ret == 0)
						len = sprintf(resBuf,"%0*d",defLenOfCommand,ret);
					break;
				}
				sprintf(resBuf,"%0*d",defLenOfCommand,ret);
				if ((ret = UnionXMLPackageToBuf(tmpBuf,sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in %s:: UnionXMLPackageToBuf!\n",UnionGetApplicationName());
					break;
				}
				tmpBuf[ret] = 0;
				
				if (ret > (UnionGetMaxSizeOfMsg() - defLenOfCommand - 1))
				{
					// 压缩
					strcpy(resBuf + defLenOfCommand,"1");
					len = sizeof(resBuf) - defLenOfCommand - 1;
					if ((ret = compress((unsigned char *)resBuf + defLenOfCommand + 1, &len, (unsigned char *)tmpBuf,ret)) != 0)
					{
						UnionUserErrLog("in %s:: compress ret = [%d]!\n",UnionGetApplicationName(),ret);
						ret = 0 - abs(ret);
						break;
					}
				}
				else
				{
					strcpy(resBuf + defLenOfCommand,"0");
					len = ret;
					memcpy(resBuf + defLenOfCommand + 1,tmpBuf,len);
				}
				ret = 1;
				len += defLenOfCommand + 1;
				break;
			// 查询数据总数
			case	conDBSvrCmdSelectCounts:
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) != NULL)
				{
					strcpy(condition,ptr + 1);
					*ptr = 0;
				}
				else
					condition[0] = 0;
	
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) != NULL)
				{
					strcpy(joinCondition,ptr + 1);
					*ptr = 0;
				}
				else
					joinCondition[0] = 0;
	
				strcpy(tableName,reqBuf + defLenOfCommand);
				
				if ((ret =  UnionSelectRealDBRecordCounts(tableName,joinCondition,condition)) < 0)
				{
					UnionUserErrLog("in %s:: UnionSelectRealDBRecordCounts!\n",UnionGetApplicationName());
					break;
				}
				sprintf(resBuf,"%0*d",defLenOfCommand,ret);
				len = defLenOfCommand;
				break;
			// 获取序列号
			case	conDBSvrCmdGetSequence:
				if ((ret = UnionGetAddSelfNumBySequence(reqBuf + defLenOfCommand)) < 0)
				{
					UnionUserErrLog("in %s:: UnionGetAddSelfNumBySequence!\n",UnionGetApplicationName());
					break;
				}
				sprintf(resBuf,"%0*d",defLenOfCommand,ret);
				len = defLenOfCommand;
				break;
			// 执行SQL语句
			case	conDBSvrCmdExecSql:
				if ((ret = UnionExecRealDBSql(reqBuf + defLenOfCommand)) < 0)
				{
					UnionUserErrLog("in %s:: UnionGetAddSelfNumBySequence!\n",UnionGetApplicationName());
					break;
				}
				len = snprintf(resBuf,sizeof(resBuf),"%0*d",defLenOfCommand,ret);
				break;
			case	conDBSvrCmdSelectTree:
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				sizeOfBuf = atoi(ptr + 1);
				*ptr = 0;
				
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(rootValue,sizeof(rootValue),"%s",ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				fieldType = atoi(ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(parentFieldName,sizeof(parentFieldName),"%s",ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(fieldName,sizeof(fieldName),"%s",ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(tableName,sizeof(tableName),"%s",ptr + 1);
				*ptr = 0;
			
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				returnSql = atoi(ptr + 1);
				*ptr = 0;

				if (sizeOfBuf > sizeof(tmpBuf))
				{
					UnionUserErrLog("in %s:: sizeOfBuf[%d] too small!\n",UnionGetApplicationName(),sizeOfBuf);
					ret = errCodeSmallBuffer;
					break;
				}
				if ((ret = UnionSelectRealDBTree(returnSql,tableName,fieldName,parentFieldName,fieldType,rootValue,tmpBuf,sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in %s:: UnionSelectRealDBTree!\n",UnionGetApplicationName());
					break;
				}
				tmpBuf[ret] = 0;
				len = snprintf(resBuf,sizeof(resBuf),"%0*d%s",defLenOfCommand,ret,tmpBuf);
				break;
			case	conDBSvrCmdDeleteTree:
				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(rootValue,sizeof(rootValue),"%s",ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				fieldType = atoi(ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(parentFieldName,sizeof(parentFieldName),"%s",ptr + 1);
				*ptr = 0;

				if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}
				snprintf(fieldName,sizeof(fieldName),"%s",ptr + 1);
				*ptr = 0;

				/*if ((ptr = strrchr(reqBuf + defLenOfCommand,defDBSvrCmd_Separate)) == NULL)
				{
					UnionUserErrLog("in %s:: strrchr [%s]!\n",UnionGetApplicationName(),reqBuf + defLenOfCommand);
					ret = errCodeParameter;
					break;
				}*/
				//snprintf(tableName,sizeof(tableName),"%s",ptr + 1);
				snprintf(tableName,sizeof(tableName),"%s",reqBuf + defLenOfCommand);
				*ptr = 0;
				
				if ((ret = UnionDeleteRealDBTree(tableName,fieldName,parentFieldName,fieldType,rootValue)) < 0)
				{
					UnionUserErrLog("in %s:: UnionDeleteRealDBTree!\n",UnionGetApplicationName());
					break;
				}
				len = snprintf(resBuf,sizeof(resBuf),"%0*d",defLenOfCommand,ret);
				break;
			default:
				UnionUserErrLog("in %s:: command[%d] error!\n",UnionGetApplicationName(),atoi(command));
				ret = errCodeParameter;
				break;
		}
sendToClient:
		if (ret < 0)
		{
			sprintf(resBuf,"%0*d",defLenOfCommand,ret);
			len = defLenOfCommand;
			UnionProgramerLog("in %s:: UnionSendResponseToApplyModuleWithOriMsgHeader! len = [%ld] resBuf = [%s]\n",UnionGetApplicationName(),len,resBuf);
		}

		//UnionProgramerLog("in %s:: UnionSendResponseToApplyModuleWithOriMsgHeader! len = [%ld] resBuf = [%s]\n",UnionGetApplicationName(),len,resBuf);
		resBuf[len] = 0;
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDBSvr);
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(applierMDLID,(unsigned char *)resBuf,len,&msgHeader)) < 0)
		{
			UnionUserErrLog("in %s:: UnionSendResponseToApplyModuleWithOriMsgHeader [%ld]!\n",UnionGetApplicationName(),applierMDLID);
			continue;
		}
		UnionSetMultiLogEnd();
	}
	UnionSetMultiLogEnd();
	return(UnionTaskActionBeforeExit());
}	

void UnionDealCloseDatabase()
{
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionDBSvrJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gunionDBSvrJmpEnv,10);
#endif
}

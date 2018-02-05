//	Author:		chenwd
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-01-15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionRealDBCommon.h"
#include "UnionLog.h"
#include "unionCommand.h"
#include "unionRealBaseDB.h"
#include "unionHighCachedAPI.h"
#include "unionDataSync.h"
#include "UnionStr.h"

#define		maxProcNum	100

static int conProcNum = 5;

PUnionTaskInstance ptaskInstance = NULL;
int                     gunionBoundSckHDL = -1;
int                     gunionFatherProcID;
int                     gunionChildPidGrp[maxProcNum];
int                     gsynchTCPIPSvrSckHDL = -1;

int (*UnionFunDLKeyUpdate)(char *parameter);
int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int (*UnionTaskActionBeforeExit)());

// add by leipp begin 20160516
int UnionSeprateExecuteSql(char *buf)
{
	char	sql[8192];
	char	*ptr = NULL;
	int	ret = 0;

	if ((ptr = strchr(buf,';')) != NULL)
	{
		ret = ptr - buf;
		memcpy(sql,buf,ret);
		sql[ret] = 0;

		if ((ret = UnionExecRealDBSql2(0, sql)) < 0)
		{
			UnionUserErrLog("in UnionSeprateExecuteSql:: UnionExecRealDBSql2 0[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionSeprateExecuteSql:: UnionExecRealDBSql2 [%s] not found!\n",sql);
			return(errCodeDatabaseMDL_RecordNotFound);
		}

		if ((ret = UnionExecRealDBSql2(0, ptr+1)) < 0)
		{
			UnionUserErrLog("in UnionSeprateExecuteSql:: UnionExecRealDBSql2 1[%s]!\n",ptr+1);
			return(ret);
		}
	}
	else
	{
		ret = UnionExecRealDBSql2(0, buf);
	}

	return(ret);
}
// add by leipp end 20160516

int UnionGetActiveTCIPSvrSckHDL()
{
        return(gsynchTCPIPSvrSckHDL);
}

int gunionGetFreePos(int minTaskNum,int maxTaskNum)
{
        int     index = 0;

                                                                                                        
        for (index = minTaskNum; index < maxTaskNum; index++)                                           
        {
                if (gunionChildPidGrp[index] > 0)                                                       
                {                                                                                       
                        if (kill(gunionChildPidGrp[index],0) == 0)      // 进程存在                     
                                continue;                                                               
                        else                                                                            
                                gunionChildPidGrp[index] = -1;                                          
                }                                                                                       
                                                                                                        
                return index;                                                                           
        }                                                                                               
                                                                                                        
        return(errCodeParameter);                                                                       
}

int UnionHelp()
{
       printf("Usage:: %s \n",UnionGetApplicationName());
       return(0);
}
 
int UnionTaskActionBeforeExit()
{
	int     sckHDL;
        int     index;

        if (gunionFatherProcID == getpid())
        {
                for (index = 0; index < conProcNum; index++)
                {
                        if (gunionChildPidGrp[index] > 0)
                                kill(gunionChildPidGrp[index],9);
                }
        }
        if (gunionBoundSckHDL >= 0)
                UnionCloseSocket(gunionBoundSckHDL); 
        if ((sckHDL = UnionGetActiveTCIPSvrSckHDL()) >= 0)
                UnionCloseSocket(sckHDL);
        
        UnionCloseHighCachedService();
       	UnionDisconnectTaskTBL();
       	UnionDisconnectLogFileTBL();
       	exit(0);
}

int UnionTcpipSvrTask()
{               
        int                     ret;
        unsigned int            clilen;
        struct sockaddr_in      cli_addr;
        int                     sckinstance; 
        struct linger           Linger;
	//char			taskName[128];
         
	/*                                                                                               
        if ((ret = UnionConnectDatabase()) < 0)
        {                                                                                               
                UnionUserErrLog("in UnionTcpipSvrTask:: UnionConnectDatabase ret = [%d]\n",ret); 
                return(ret);
        }                                                                                               
        */
	if ((ptaskInstance = UnionCreateTaskInstanceForClient(UnionTaskActionBeforeExit, "%s", UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstanceForClient Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	                                                                                               
        clilen = sizeof(cli_addr);
        sckinstance = accept(gunionBoundSckHDL, (struct sockaddr *)&cli_addr,&clilen);                  
        if ( sckinstance < 0 )                                                                          
        {                                                                                               
                UnionSystemErrLog("in UnionTcpipSvrTask:: accept()!\n");                                
                return(errCodeUseOSErrCode);
        }                                                                                               
        Linger.l_onoff = 1;                                                                             
        Linger.l_linger = 0;                                                                            
        if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)           
        {                                                                                               
                UnionSystemErrLog("in UnionTcpipSvrTask:: setsockopt linger!");                         
                close(sckinstance);                                                                     
                return(errCodeUseOSErrCode);
        }
	
	ret = UnionSynchTCPIPTaskServer(sckinstance,&cli_addr,UnionTaskActionBeforeExit);          
        UnionCloseSocket(sckinstance);                                                                  
        return(ret);                                                                                    
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int (*UnionTaskActionBeforeExit)())
{
	int		ret;
	int		len;
	int		offset = 0;
	char		key[128];
	char		value[10240];
	int		lenOfValue;
	int		expiration;
	char		lenBuf[32];
	unsigned char	buf[20480];
	unsigned char	result[32];
	void		*dlHandle = NULL;
	char		dlName[256];
	
	
	
	result[0] = 0;
	result[1] = 2;
	
	gsynchTCPIPSvrSckHDL = handle;
	while(1)
	{
		// 获取接收数据长度
		if ((ret = UnionReceiveFromSocketUntilLen(handle, buf, 2)) != 2)
		{
			if (ret < 0)
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen ret = [%d] !\n",ret);
			return(ret);
		}
		len = buf[0] * 256 + buf[1];

		if (len == 0)
		{
			UnionDebugLog("in UnionSynchTCPIPTaskServer:: recv data len = [0] !\n");
			continue;
		}
		
		// 获取接收数据内容	
		if ((ret = UnionReceiveFromSocketUntilLen(handle, buf, len)) != len)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen ret = [%d] !\n",ret);
			return(ret);
		}
		buf[len] = 0;

		// 拼装返回结果
		if (buf[1] - '0' == conNotWaitResult)
		{
			// 把结果发送回客户端
			memcpy(result+2,"00",2);
			if ((ret = UnionSendToSocket(handle, result, 4)) < 0)
			{
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendToSocket ret = [%d] !\n", ret);
				return(ret);
			}
		}
		
		// 数据类型
		switch(buf[0] - '0')
		{
			case conSQL:
				// modify by leipp begin 20160516
				ret = UnionSeprateExecuteSql((char *)buf + 2);
				// modify by leipp end 20160516
				break;
			case conCache:
				offset = 2;
				if (memcmp(buf + 2,"set:",4) == 0)
				{
					offset += 4;
					// key
					memcpy(lenBuf,buf + offset,4);
					lenBuf[4] = 0;
					offset += 4;
					memcpy(key,buf + offset,atoi(lenBuf));
					key[atoi(lenBuf)] = 0;
					offset += atoi(lenBuf);
					
					// value
					memcpy(lenBuf,buf + offset,4);
					lenBuf[4] = 0;
					offset += 4;
					lenOfValue = atoi(lenBuf);
					memcpy(value,buf + offset,lenOfValue);
					value[lenOfValue] = 0;
					offset += lenOfValue;
					
					// expiration
					memcpy(lenBuf,buf + offset,4);
					lenBuf[4] = 0;
					offset += 4;
					expiration = atoi((char *)buf + offset);
					ret = UnionSetHighCachedValue2(0,key,value,lenOfValue,expiration);
				}
				else if (memcmp(buf + 2,"delete:",7) == 0)
				{
					offset += 7;
					ret = UnionDeleteHighCachedValue2(0,(char *)buf + offset);
				}
				else if (memcmp(buf + 2,"flush:",6) == 0)
				{
					offset += 6;
					ret = UnionFlushHighCachedService2(0);
				}
				break;
			case conSQLAndCache:
				break;
			//add by zhouxw 20150910
			case conSymmetricKeyMsg:
				if(getenv("UNIONDLDIR") != NULL)
					len = snprintf(dlName, sizeof(dlName), "%s/%s", getenv("UNIONDLDIR"), "libKeyUpdate.so");
				else
					len = snprintf(dlName, sizeof(dlName), "%s/bin/%s", getenv("UNIONREC"), "libKeyUpdate.so");
				dlName[len] = 0;
				if((dlHandle = dlopen(dlName, RTLD_NOW)) == NULL)
				{
					UnionUserErrLog("in UnionSynchTCPIPTaskServer:: dlopen[%s] error[%s]\n", dlName, dlerror());
					ret = errCodeParameter;
					break;
				}
				if((UnionFunDLKeyUpdate = dlsym(dlHandle, "UnionUpdateKey")) == NULL)
				{
					UnionUserErrLog("in UnionSynchTCPIPTaskServer:: dlsym[%s] error[%s]\n", "UnionUpdateKey", dlerror());
					dlclose(dlHandle);
					ret = errCodeParameter;
					break;
				}
				if((ret = (*UnionFunDLKeyUpdate)((char *)buf+2)) < 0)
				{
					UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionFunDLKeyUpdate ret=[%d]\n", ret);
					dlclose(dlHandle);
					break;
				}
				dlclose(dlHandle);
				break;
			//end
			default:
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: dataType[%c] error!\n",buf[0]);
				return(errCodeParameter);
		}
		
		// 拼装返回结果
		if (buf[1] - '0' == conWaitResult)
		{
			if (ret < 0 )
			{
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: buf[%s] error!\n",buf);
				memcpy(result+2,"01",2);
			}
			else
				memcpy(result+2,"00",2);
			
			// 把结果发送回客户端
			if ((ret = UnionSendToSocket(handle, result, 4)) < 0)
			{
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendToSocket ret = [%d] !\n", ret);
				return(ret);
			}
		}
	}
	return(0);
}

int main(int argc, char *argv[])
{
	int	ret;
	int	index = 0;
	int	childPid;
	char	parentLogName[256];

	UnionSetApplicationName(argv[0]);

	if (argc == 2)
	{
		conProcNum = atoi(argv[1]);
		if (conProcNum <= 0)
			conProcNum = 1;
		else if (conProcNum > maxProcNum)
			conProcNum = maxProcNum;
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()) > 0)                                                 
        {                                                                                               
                printf("Another task of name [%s] already exists!\n",UnionGetApplicationName());                         
                return(-1);                                                                             
        }   

	gunionFatherProcID = getpid();
        // 初始化子进程表
        for (index = 0; index < conProcNum; index++)
                gunionChildPidGrp[index] = -1;

	if (UnionCreateProcess() > 0)
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit, UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if((ret = UnionReloadDataSyncIni()) < 0 )
	{
		UnionUserErrLog("in %s:: UnionReloadDataSyncIni ret = [%d]!\n", UnionGetApplicationName(), ret);
		return(UnionTaskActionBeforeExit());
	}
	
	// 初始化一个侦听端口                                                                           
        if ((gunionBoundSckHDL = UnionInitializeTCPIPServer(UnionGetDataSyncLocalPort())) < 0)                        
        {                                                                                               
                UnionPrintf("in %s:: UnionInitializeTCPIPServer[%d] Error!\n",UnionGetApplicationName(),UnionGetDataSyncLocalPort());
                return(UnionTaskActionBeforeExit()); 
        }

        if(ptaskInstance->pclass->logFileName)
        {
                strcpy(parentLogName, ptaskInstance->pclass->logFileName);
        }
        else                                                                                            
        {                                                                                               
                strcpy(parentLogName, "unknown");                                                       
        }                                                                                               
loop:
	// 开启子进程
	for (index = 0; index < conProcNum; index++)                                                    
	{
		                                                                                       
	        if (gunionChildPidGrp[index] > 0)                                                       
	        {                                                                                       
	                if (kill(gunionChildPidGrp[index],0) == 0)      // 进程存在                     
	                        continue;                                                               
	        }
		                                                                                      
	        // 创建子进程                                                                           
	        if ((childPid = UnionCreateProcess()) == 0)     // 是子进程                             
	                goto callChildProcess;                                                          
	        UnionProgramerLog("in %s:: create child %d!\n",UnionGetApplicationName(),childPid);
	        gunionChildPidGrp[index] = childPid;                                                    
	}
	sleep(2);
	goto	loop;
                                                                                                        
        // 子进程执行以下代码                                                                           
callChildProcess:                                                                                       
        ret = UnionTcpipSvrTask();
        if(ret < 0)                                                                                     
                return(UnionTaskActionBeforeExit());                                                    
        else                                                                                            
                goto callChildProcess;                                                                  
}                                    

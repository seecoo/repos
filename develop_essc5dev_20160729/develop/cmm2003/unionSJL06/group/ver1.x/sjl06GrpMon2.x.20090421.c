//	Wolfgang Wang
//	2001/10/15
// 	Version	1.0

//	20040623 Wolfgang Wang, 对异常加密机采用反显

#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_	

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>
#include <time.h>

#include "sjl06.h"
#include "sjl06Grp.h"
#include "UnionTask.h"
#include "unionMonitor.h"
#include "UnionLog.h"

// Defined in HsmGrp.a
extern PUnionSJL06	pgunionSJL06Grp;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

long gunionLastTotalTimes = 0;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectWorkingSJL06MDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

// this is rewritten by Wolfgang Wang, 2004/06/23
int RefreshHsmTransWin(int interval)
{
	int		i;
	int		ret;
	int		len;
	char		Buf[128];
	PUnionSJL06	pSJL06;
	int		hsmNum;
	long		totalNum = 0;
	char            cmdReq[conMaxNumOfHsmCheckCmd][256+1],reqStr[256+1];
        char            cmdRes[conMaxNumOfHsmCheckCmd][256+1],resStr[256+1],ascResBuf[256+1];
	char		cmdRemark[conMaxNumOfHsmCheckCmd][256+1];
	char            currentHsmGrp[3+1];
	char            remark[256+1];
	char		status;
	int             realCmdNum = 0;
	int             cmdIndex;
	int             successCmdNum = 0;
			
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionSJL06Grp == NULL)
	{
		UnionUserErrLog("in RefreshHsmTransWin:: pgunionSJL06Grp is NULL!\n");
		return(-1);
	}
	
	//UnionLog("in RefreshHsmTransWin:: HsmNum = [%d]\n",pgunionSJL06Grp->HsmNum);
	memset(Buf,0,sizeof(Buf));
	//UnionLog("HsmNum = [%d]\n",pgunionSJL06Grp->HsmNum);
	sprintf(Buf,"%6s %15s %6s %5s %4s %10s %13s %13s",
		"工作组","  密码机IP地址 "," 端口"," 状态 ", "说明", "连接","   失败次数  ","   成功次数  ");
	wmove(pgContentWin,1,2);
	waddstr(pgContentWin,Buf);
	memset(currentHsmGrp,0,sizeof(currentHsmGrp));
	for (hsmNum = 0,i = 0; i < conMaxNumOfSJL06 && i < gLineOfWin; i++)
	{
		pSJL06 = pgunionSJL06Grp + i;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;

		if ((strcmp(currentHsmGrp,pSJL06->staticAttr.hsmGrpID) != 0) || (realCmdNum <= 0))       // 与上一台密码机同组，并且组指令已初始化
		{
			for (cmdIndex = 0; cmdIndex < conMaxNumOfHsmCheckCmd; cmdIndex++)
			{
				memset(cmdReq[cmdIndex],0,sizeof(cmdReq[cmdIndex]));
				memset(cmdRes[cmdIndex],0,sizeof(cmdRes[cmdIndex]));
			}
			if ((realCmdNum = UnionReadFullTestCmdGrpOfHsmGrp(pSJL06->staticAttr.hsmGrpID,cmdReq,cmdRes,cmdRemark,conMaxNumOfHsmCheckCmd)) <= 0)
			{
				strcpy(cmdReq[0],pSJL06->staticAttr.testCmdReq);
				strcpy(cmdRes[0],pSJL06->staticAttr.testCmdSuccessRes);
				strcpy(cmdRemark[0],"");
				realCmdNum = 1;
			}
			strcpy(currentHsmGrp, pSJL06->staticAttr.hsmGrpID);
		}
		// 逐一测试每条指令
                successCmdNum = 0;
                for (cmdIndex = 0; cmdIndex < realCmdNum; cmdIndex++)
                {

                        if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)      // 不是二进制密码机
                        {
                                strcpy(reqStr,cmdReq[cmdIndex]);
                                len = strlen(reqStr);
                        }
                        else    // 是二进制密码机
                        {
                                aschex_to_bcdhex(cmdReq[cmdIndex],strlen(cmdReq[cmdIndex]),reqStr);
                                len = strlen(cmdReq[cmdIndex]) / 2;
                        }
			if ((strcmp(reqStr,"null") == 0) || (strcmp(reqStr,"NULL") == 0))       // 不执行这条指令
                        {
                                // 与密码机建立连接
                                if ((ret = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
                                {
                                        break;
                                }
                                UnionCloseSocket(ret);
                                successCmdNum++;
                                continue;
                        }
                        // 执行这条指令
                        memset(resStr,0,sizeof(resStr));
                        if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,reqStr,len,resStr,sizeof(resStr))) < 0)  // 执行指令
                        {
                                break;
                        }
                        memset(ascResBuf,0,sizeof(ascResBuf));
                        // 比较响应
                        if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)
                                memcpy(ascResBuf,resStr,ret);
                        else
                                bcdhex_to_aschex(resStr,ret,ascResBuf);
                        if (strncmp(cmdRes[cmdIndex],ascResBuf,strlen(cmdRes[cmdIndex])) != 0)  // 响应不符合预期
                        {
                                UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: response of Hsm [%s] of cmd [%s] = [%s] != defined [%s]\n", pSJL06->staticAttr.ipAddr,cmdReq[cmdIndex],ascResBuf,cmdRes[cmdIndex]);
                                break;
                        }
                        UnionNullLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: check [%s] [%s]=[%s]OK!\n",cmdReq[cmdIndex],cmdRes[cmdIndex],ascResBuf);
                        successCmdNum++;
                        continue;
                }
		if (successCmdNum < realCmdNum) // 有指令的执行结果不符合预期
                {
                        //UnionSetWorkingSJL06Abnormal(pSJL06);
			strcpy(remark, cmdRemark[cmdIndex]);
			status = '0';
                }
                else    // 所有指令执行正确
                {
                        if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
                        {
                                UnionAuditLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
                                //UnionSetWorkingSJL06Online(pSJL06);
                        }
			status = '1';
			strcpy(remark, "加密机正常");
                }
		if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
		{
			status = '2';
			strcpy(remark, "");
		}

		memset(Buf,0,sizeof(Buf));
		if ((pSJL06->dynamicAttr.activeLongConn > 0) && (pSJL06->dynamicAttr.status != conOnlineSJL06))
			UnionKillTaskInstanceByName(pSJL06->staticAttr.ipAddr);
		sprintf(Buf,"%5s %15s %6d ",
			pSJL06->staticAttr.hsmGrpID,
			pSJL06->staticAttr.ipAddr,
			pSJL06->staticAttr.port);
		switch (status)
		{
			case	'0':
				sprintf(Buf+strlen(Buf),"  异常 ");
				break;
			case	'1':
				sprintf(Buf+strlen(Buf),"  在线 ");
				break;
			case	'2':
				sprintf(Buf+strlen(Buf),"  备份 ");
				break;
			default:
				sprintf(Buf+strlen(Buf),"  不明 ");
				break;
		}
		sprintf(Buf+strlen(Buf)," %10s ", remark);
		sprintf(Buf+strlen(Buf),"%4d %12ld %12ld",
			pSJL06->dynamicAttr.activeLongConn,
			pSJL06->dynamicAttr.timeoutTimes + pSJL06->dynamicAttr.connFailTimes,
			pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes);
		wmove(pgContentWin,hsmNum + 2,2);
		totalNum += pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes;
		//UnionNullLog("Buf = [%s]\n",Buf);
		switch (pSJL06->dynamicAttr.status)
		{
			case	'0':
#ifndef _AIX_
				wstandout(pgContentWin);
#endif
				waddstr(pgContentWin,Buf);
#ifndef _AIX_
				wstandend(pgContentWin);
#endif
				break;
			case	'1':
			case	'2':
				waddstr(pgContentWin,Buf);
				break;
			default:
#ifndef _AIX_
				wstandout(pgContentWin);
#endif
				waddstr(pgContentWin,Buf);
#ifndef _AIX_
				wstandend(pgContentWin);
#endif
				break;
		}
		hsmNum++;
	}
	memset(Buf,0,sizeof(Buf));
	memset(Buf,' ',gColOfWin - 4);
	for (i = hsmNum + 2; i < gLineOfWin - 2; i++)
	{
		wmove(pgContentWin,i, 2);
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);
	if (gunionLastTotalTimes != 0)
		UnionAuditNullLogWithTime("excutedHsmCmdNum = [%012ld]\n",totalNum - gunionLastTotalTimes);
	if ((gunionLastTotalTimes = totalNum) < 0)
		gunionLastTotalTimes = 0;
			
	return(0);
}

int main(int argc,char **argv)
{
	int	interval;
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		interval = 1;
	else
	{
		if ((interval = abs(atoi(argv[1]))) <= 0)
			interval = 1;
	}
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

#ifndef _nullTaskName_
	return(UnionMonitorMain(interval,argv[0],"密码机工作组监控程序",UnionConnectWorkingSJL06MDL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
#else
	return(UnionMonitorMain(interval,argv[0],"",UnionConnectWorkingSJL06MDL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
#endif
}
	
int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}

// 2009/10/19，xusj增加，从一个指令配置文件中读取定义的字令串
int UnionReadFullTestCmdGrpOfHsmGrp(char *hsmGrpID,char cmdReq[][256+1],char cmdRes[][256+1],char cmdRemark[][256+1],int maxExpectedNum)
{
	char	tmpBuf[512+1];
	int	realNum=0;
	FILE	*fp;
	int	ret;
	int	reqExists;
	int	resExists;
	char	*ptr;

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s/HsmCfg/checkCmdGrpOf%s.conf",getenv("UNIONETC"),hsmGrpID);
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionProgramerLog("in UnionReadFullTestCmdGrpOfHsmGrp:: fopen [%s] failure!\n",tmpBuf);
		return(0);
	}

	reqExists = 0;
	resExists = 0;
	while (!feof(fp))
	{
		if (realNum >= maxExpectedNum)	// 已读了最大数量的测试指令
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))
			continue;
		if (!reqExists) // 还没有定义请求
		{
			if ((ptr=strstr(tmpBuf,"hsmCmdReq=")) == NULL)
				continue;
			if (strlen(ptr+strlen("hsmCmdReq=")) >= sizeof(cmdReq[realNum]))
			{
				UnionUserErrLog("in UnionReadFullTestCmdGrpOfHsmGrp:: [%s] is too long!\n",tmpBuf);
				continue;
			}
			strcpy(cmdReq[realNum],ptr+strlen("hsmCmdReq="));
			reqExists = 1;
		}
		else	// 读请求的对应响应
		{
			if (!resExists)
			{
				if ((ptr=strstr(tmpBuf,"hsmCmdRes=")) == NULL)
				{
					reqExists = 0;
					continue;
				}
				if (strlen(ptr+strlen("hsmCmdRes=")) >= sizeof(cmdRes[realNum]))
				{
					UnionUserErrLog("in UnionReadFullTestCmdGrpOfHsmGrp:: [%s] is too long!\n",tmpBuf);
					reqExists = 0;
					continue;
				}
				strcpy(cmdRes[realNum],ptr+strlen("hsmCmdRes="));
				resExists = 1;
			}
			else
			{
				reqExists = 0;
				resExists = 0;
				if ((ptr=strstr(tmpBuf,"remark=")) == NULL)
					strcpy(cmdRemark[realNum],"错误未定义");
				else
				{
					if (strlen(ptr+strlen("remark=")) >= sizeof(cmdRemark[realNum]))
					{
						UnionUserErrLog("in UnionReadFullTestCmdGrpOfHsmGrp:: [%s] is too long!\n",tmpBuf);
						continue;
					}
					strcpy(cmdRemark[realNum],ptr+strlen("remark="));
				}
				realNum++;
			}
		}
	}
	fclose(fp);
	return(realNum);
}

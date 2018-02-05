#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionRecFile.h"
#include "unionMemcachedConf.h"

#include "unionREC.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmd.h"
#include "highCached.h"

#define  MAX_NUM_SERVICE  8
#define	defTableNameOfSymmetricKeyDB		"symmetricKeyDB"


PUnionSharedMemoryModule	pgunionMemConfMDL = NULL;
PUnionMemcachedConf		pgunionMemConf = NULL;
PUnionMemcachedSvr		pgunionMemcachedSvr = NULL;

int UnionIsMemcachedConfConnected()
{
	if ((pgunionMemConfMDL == NULL) || (pgunionMemConf == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfMemcachedConf)))
		return(0);
	else
		return(1);
}

int UnionReloadMemcachedConf()
{
	int			ret;
	int			realNum =0;
	int			i,j;
	memcached_st    	*st;			

	PUnionMemcachedSvr	pMemcachedSvr;
	PUnionHighCachedTBL	phighCachedTBL = NULL;

	if ((ret = UnionConnectMemcachedConf()) < 0)
	{
		UnionUserErrLog("in UnionReloadMemcachedConf:: UnionConnectREC!\n");
		return(ret);
	}
	
	if ((phighCachedTBL = UnionGetHighCachedTBL()) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcached:: UnionGetHighCachedTBL!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < phighCachedTBL->realNum; i++)
	{
		// 激活
		if (phighCachedTBL->rec[i].enabled==1)
		{	
		//	if (phighCachedTBL->rec[i].master==1)//统计所以的memcached，所以delete
				realNum++;
		}
	}
	pgunionMemConf->realNum = realNum;

	for (i = 0,j=0; i < phighCachedTBL->realNum; i++)
	{
		// 激活
		if (phighCachedTBL->rec[i].enabled==1)
		{	
			if (phighCachedTBL->rec[i].master==1)
			{
				pMemcachedSvr = pgunionMemConf->pmemGrp + j;
				strcpy(pMemcachedSvr->ipAddr, phighCachedTBL->rec[i].ipAddr);
				pMemcachedSvr->port = phighCachedTBL->rec[i].port;
				pMemcachedSvr->isBackSvr = 0;
				pMemcachedSvr->conStats =1 ;
		              
			 	if ((st = UnionConnectMemcachedWithDetail(pMemcachedSvr->ipAddr,pMemcachedSvr->port )) == NULL)
               				pMemcachedSvr->conStats = 0;
				else
                		{
						pMemcachedSvr->conStats =1 ;
                				memcached_free(st);
					 	st=NULL;
                		}
				j++;
			}
			else if (phighCachedTBL->rec[i].master == 0)
			{
				pMemcachedSvr = pgunionMemConf->pmemGrp + j;

				strcpy(pMemcachedSvr->ipAddr, phighCachedTBL->rec[i].ipAddr);
				pMemcachedSvr->port = phighCachedTBL->rec[i].port;
				pMemcachedSvr->isBackSvr = 1;
				pMemcachedSvr->conStats = 1;
				
				if ((st = UnionConnectMemcachedWithDetail(pMemcachedSvr->ipAddr,pMemcachedSvr->port )) == NULL)
                                        pMemcachedSvr->conStats = 0;
                                else
                                {
                                                pMemcachedSvr->conStats = 1;
                                                memcached_free(st);
						st=NULL;
                                }
				j++;
			}
		}
	}
	return(0);
}

int UnionConnectMemcachedConf()
{

	if (UnionIsMemcachedConfConnected())	// 已经连接
		return(0);
	if ((pgunionMemConfMDL = UnionConnectSharedMemoryModule(conMDLNameOfMemcachedConf, sizeof(TUnionMemcachedConf)+sizeof(TUnionMemcachedSvr)*MAXNUMOFMEMCACHED)) == NULL)
	{
		UnionLog("in UnionConnectMemcachedConf:: UnionConnectSharedMemoryModule:: can't get conMDLNameOfMemcachedConf of magent!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionMemConf = (PUnionMemcachedConf)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionMemConfMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcachedConf:: PUnionREC!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionMemConf->pmemGrp = (PUnionMemcachedSvr)((unsigned char *)pgunionMemConf+ sizeof(*pgunionMemConf))) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcachedConf:: PUnionMemcachedSvr!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionMemcachedSvr = pgunionMemConf->pmemGrp;

	if (UnionIsNewCreatedSharedMemoryModule(pgunionMemConfMDL))
	{
		return(UnionReloadMemcachedConf());
	}
	else
		return(0);
}


int UnionGetCurrentMemcachedServerNum()
{
	int	ret;

	if ((ret = UnionConnectMemcachedConf()) < 0)
		return(ret);
	else
		return(pgunionMemConf->realNum);
}

int UnionPrintMemcachedSvrToFile(PUnionMemcachedSvr pmemSvr,FILE *fp)
{
	if ((pmemSvr == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp,"[memcached server 地址] = %-15s ",pmemSvr->ipAddr);
	fprintf(fp,"[端口] = %-8d",pmemSvr->port);
	fprintf(fp,"[是否为备份] = %-5s ",pmemSvr->isBackSvr?"是":"否");
	if(pmemSvr->conStats == 0)
		fprintf(fp,"[状态] = %-5s", "异常");
	if(pmemSvr->conStats == 1)
		fprintf(fp,"[状态] = %-5s ", "正常");
	fprintf(fp,"\n");
	return(0);
}

int UnionPrintMemcachedConfToFile(FILE *fp)
{
	int	i;
	int	ret;

	if ((ret = UnionConnectMemcachedConf()) < 0)
	{
		UnionUserErrLog("in UnionPrintRECToFile:: UnionConnectREC!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);

	fprintf(fp,"[memcached server数量] = [%d]\n",pgunionMemConf->realNum);

	for (i = 0; i < pgunionMemConf->realNum; i++)
	{
		if ((i != 0) && (i % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		UnionPrintMemcachedSvrToFile((pgunionMemcachedSvr+i),fp);
	}
	return(0);
}
//add begin by lusj 20151026
memcached_st *UnionConnectMemcachedWithDetail(char *ip, int port)
{
	memcached_st *st;
	memcached_return        rc;

	memcached_stat_st *stats_return=NULL;//lusj 进行释放开源库没是否的申请内存

	if ((st = memcached_create(NULL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail::memcached_create!\n");
		return(NULL);
	}
	if ((rc = memcached_behavior_set(st, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail:: memcached_behavior_set rc = [%d] [%s]!\n",rc, memcached_last_error_message(st));
		memcached_free(st);
		return(NULL);
	}
	if ((rc = memcached_behavior_set(st, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail:: memcached_behavior_set rc = [%d] [%s]!\n",rc, memcached_last_error_message(st));
		memcached_free(st);
		return(NULL);
	}
	if ((rc = memcached_behavior_set(st, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail:: memcached_behavior_set rc = [%d] [%s]!\n",rc, memcached_last_error_message(st));
		memcached_free(st);
		return(NULL);
	}
	//add end
	// 加入服务端地址
	if ((rc = memcached_server_add(st, ip, port)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail:: memcached_server_add rc = [%d] [%s]!\n",rc, memcached_last_error_message(st));
		memcached_free(st);
		return(NULL);
	}

	stats_return=memcached_stat(st,NULL,&rc);
	if (rc != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionConnectMemcachedWithDetail:: connect failue memcached_stat[%s] [%d]  rc = [%d] [%s]!\n",ip, port,rc, memcached_last_error_message(st));
		memcached_free(st);
		return(NULL);
	}
	//add by lusj 20160618 释放申请的空间
	free(stats_return);
	stats_return=NULL;
	//add end 

	return(st);
}

//add by  lusj 20160614
int UnionCheckMemcachedConnectInit(PUnionMemcachedSvr pmemcached)
{

	if(pmemcached->init_flag==1)
		return 1;
//	UnionLog("UnionCheckMemcachedConnectInit:: memcached_create !\n");
	if ((pmemcached->st = memcached_create(NULL)) == NULL)
	{
		UnionUserErrLog("in UnionCheckMemcachedConnectInit::memcached_create!\n");
		return(-1);
	}
	if ((pmemcached->rc = memcached_behavior_set(pmemcached->st, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionCheckMemcachedConnectInit:: memcached_behavior_set rc = [%d] [%s]!\n",pmemcached->rc, memcached_last_error_message(pmemcached->st));
		free(pmemcached->st);
		pmemcached->st=NULL;
		pmemcached->init_flag=0;
		return(-1);
	}
	if ((pmemcached->rc = memcached_behavior_set(pmemcached->st, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionCheckMemcachedConnectInit:: memcached_behavior_set rc = [%d] [%s]!\n",pmemcached->rc, memcached_last_error_message(pmemcached->st));
		free(pmemcached->st);
		pmemcached->st=NULL;
		pmemcached->init_flag=0;
		return(-1);
	}
	if ((pmemcached->rc = memcached_behavior_set(pmemcached->st, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 500)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionCheckMemcachedConnectInit:: memcached_behavior_set rc = [%d] [%s]!\n",pmemcached->rc, memcached_last_error_message(pmemcached->st));
		free(pmemcached->st);
		pmemcached->st=NULL;
		pmemcached->init_flag=0;
		return(-1);
	}
	//add end
	// 加入服务端地址
//	UnionLog("UnionCheckMemcachedConnectInit::memcached_server_add!\n");
	if ((pmemcached->rc = memcached_server_add(pmemcached->st, pmemcached->ipAddr, pmemcached->port)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionCheckMemcachedConnectInit:: memcached_server_add rc = [%d] [%s]!\n",pmemcached->rc, memcached_last_error_message(pmemcached->st));
		free(pmemcached->st);
		pmemcached->st=NULL;
		pmemcached->init_flag=0;
		return(-1);
	}

	pmemcached->init_flag=1;
	return 1;
}

int UnionCheckMemcachedConnectOfIPandPort(PUnionMemcachedSvr	pmemcached)
{
	memcached_stat_st *stats_return=NULL;
//	UnionLog("UnionCheckMemcachedConnectOfIPandPort::memcached_stat\n");
	stats_return=memcached_stat(pmemcached->st,NULL,&(pmemcached->rc));//由于memcached_stat申请了指针，并返回给调用函没有释放，所以调用函数结束并释放
	free(stats_return);
	stats_return=NULL;
	if ((pmemcached->rc) != MEMCACHED_SUCCESS)
	{
		UnionLog("in UnionCheckMemcachedConnectOfIPandPort:: connect failue memcached_stat[%s] [%d]  rc = [%d] [%s]!\n",pmemcached->ipAddr, pmemcached->port,pmemcached->rc, memcached_last_error_message(pmemcached->st));
	//	pmemcached->st=NULL;
	//	pmemcached->init_flag=0;
		return(-1);
	}
	return 1;

}

//add end 


/*
   功能：检查工具的状态
 */	
	
int UnionCheckMemcachedStatus()
{
	int	ret, x;
	int	totalNum = -1;	//add by lusj 20151215	
	PUnionMemcachedSvr	pmemcached=NULL;

	if ((ret = UnionConnectMemcachedConf()) < 0)
	{
		UnionLog("in UnionCheckMemcachedStatus::UnionConnectMemcachedConf:: magent setup failed!\n");
		return(ret);
	}
	

	for (x=0;x<pgunionMemConf->realNum;x++)
	{
		pmemcached = pgunionMemcachedSvr + x;

		//add by lusj 20160614 
		if ((ret = UnionCheckMemcachedConnectInit(pmemcached)) < 0)
		{
			UnionLog("in UnionCheckMemcachedStatus::UnionCheckMemcachedConnectInit:: failed!\n");
			pmemcached->conStats = 0;
			return(ret);
		}
		//add end

		//modify by lusj 20160614
		if ((ret = UnionCheckMemcachedConnectOfIPandPort(pmemcached)) <0)
		{
			pmemcached->conStats = 0;
		}//modify end
		else
		{	
			//add by lusj
			if(pmemcached->conStats == 0)
			{
	        		//modify  by lusj 201151214 检查数据库，数据正常才清空
            			if ((totalNum = UnionSelectRealDBRecordCounts(defTableNameOfSymmetricKeyDB,NULL,NULL))>0)
            			{
					if((ret = UnionFlushMemcached_Of_IP(pmemcached))<0)
					{
						UnionUserErrLog("in UnionCheckMemcachedStatus :: UnionFlushMemcached_Of_IP::UnionFlushMemcachedNode!\n");
						return(ret);
					}
             			}
			}
			//add end by lusj 20151026
			pmemcached->conStats = 1;
		}
	}
	return 1;

}



// 清空指定IP地址和端口的Memcached
//add by lusj 20151026
	
int UnionFlushMemcached_Of_IP(PUnionMemcachedSvr pmemcached)
{
	int ret;

	if ((ret = UnionConnectMemcachedConf()) <0)
	{
		UnionUserErrLog("UnionFlushMemcached_Of_IP::UnionConnectMemcachedConf!\n");
		return(ret);
	}

	UnionLog("in UnionFlushMemcached_Of_IP ::FlushMemcached_Of[%s][%d]!!\n",pmemcached->ipAddr,pmemcached->port);

	if ((ret = UnionFlushMemcachedNode(pmemcached)) <0)
	{
		UnionUserErrLog("in UnionFlushMemcached_Of_IP::UnionFlushMemcachedNode!\n");
		return(ret);
	}

	UnionLog("in UnionFlushMemcached_Of_IP ::FlushMemcached_Of[%s][%d] OK!!\n",pmemcached->ipAddr,pmemcached->port);
	return(0);
}
//add end  by lusj 20151026	



int UnionFlushMemcachedNode(PUnionMemcachedSvr	pmemcached)
{
	memcached_return	rc;
	time_t		expiration = 0;

	if ((rc = memcached_flush(pmemcached->st,expiration)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionFlushMemcachedNode:: memcached_flush [%s] [%d] [%s]!\n",pmemcached->ipAddr, pmemcached->port, memcached_last_error_message(pmemcached->st));
		return(0 - abs(rc));
	}
	return(0);
}




#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionREC.h"
#include "unionRecFile.h"
#include "baseUIService.h"

/*********************************
服务代码:	0903
服务名:		添加缓存服务器停止脚本
功能描述:	添加缓存服务器停止脚本
**********************************/

int UnionDealServiceCode0903(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		len = 0;
	int		memcachedNum = 0;
	char		tmpBuf[512];
	int		port = 0;
//	int		operateType = 0;
	char		*ptrLine = NULL;
	char		fileDir[1024];
	char		tmpPort[32];
	FILE		*fp;

	// 读取操作类型
	/*memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/operateType",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: UnionReadRequestXMLPackageValue[%s]!\n","body/operateType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	operateType = atoi(tmpBuf);*/

	// 读取端口号
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: UnionReadRequestXMLPackageValue[%s]!\n","body/port");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	port = atoi(tmpBuf);


	// 在启动脚本中查找缓存服务器序号
	memset(fileDir,0,sizeof(fileDir));
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"$UNIONREC/bin.x/start_memcached.x");	
	UnionReadDirFromStr(tmpBuf,-1,fileDir);
	
	if ((fp = fopen(fileDir,"rb+")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: fopen[%s] failed!\n",fileDir);
		return(errCodeUseOSErrCode);
	}
	else
	{
		//  查询要添加的服务器的序号
		while (fgets(tmpBuf,sizeof(tmpBuf),fp))
		{
			if ((ptrLine = strstr(tmpBuf,"memcached")) == NULL)
				continue;

			memcachedNum ++;

			memset(tmpPort,0,sizeof(tmpPort));
			sprintf(tmpPort,"%d",port);
			if ((ptrLine = strstr(tmpBuf,tmpPort)) != NULL)
				break;
		}
	}
	fclose(fp);

	if (memcachedNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: fileDir[%s] not found port[%d]!\n",fileDir,port);
		UnionSetResponseRemark("请先创建启动命令");
		return(errCodeParameter);
	}

	// 在脚本中添加停止命令
	memset(fileDir,0,sizeof(fileDir));
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"$UNIONREC/bin.x/stop_memcached.x");	
	UnionReadDirFromStr(tmpBuf,-1,fileDir);

	if ((fp = fopen(fileDir,"rb+")) == NULL)
	{
		// 没有则创建
		if ((fp = fopen(fileDir,"ab")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode0903:: fopen[%s] create failed!\n",fileDir);
			return(errCodeUseOSErrCode);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		len += sprintf(tmpBuf,"%s\n\n","#!/bin/sh");
		len += sprintf(tmpBuf + len,"kill `cat $UNIONTEMP/memcached%d.pid`\n",memcachedNum);

		if ((ret = fprintf(fp,"%s",tmpBuf)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0903:: fprintf[%s] failed!\n",tmpBuf);
			fclose(fp);
			return(errCodeUseOSErrCode);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"chmod a+x %s",fileDir);
		system(tmpBuf);
	}
	else
	{
		// 检查是否存在
		while (fgets(tmpBuf,sizeof(tmpBuf),fp))
		{
			memset(tmpPort,0,sizeof(tmpPort));
			sprintf(tmpPort,"memcached%d",memcachedNum);
			if ((ptrLine = strstr(tmpBuf,tmpPort)) != NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode0903:: port[%s] stop command is already exist!\n",tmpBuf);
				UnionSetResponseRemark("端口[%d]停止脚本已经存在",port);
				fclose(fp);
				return(errCodeParameter);
			}
		}

		fseek(fp,0L,SEEK_END);

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"kill `cat $UNIONTEMP/memcached%d.pid`\n",memcachedNum);

		if ((ret = fprintf(fp,"%s",tmpBuf)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0903:: fprintf[%s] failed!\n",tmpBuf);
			//UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
			fclose(fp);
			return(errCodeUseOSErrCode);
		}
	}
	
	fclose(fp);
	return 0;
}

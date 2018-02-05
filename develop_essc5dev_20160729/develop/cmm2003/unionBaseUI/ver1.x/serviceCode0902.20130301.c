
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
�������:	0902
������:		��ӻ�������������ű�
��������:	��ӻ�������������ű�
**********************************/

int UnionDealServiceCode0902(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		len = 0;
	int		memcachedNum = 0;
	int		memSize;
	char		tmpBuf[512+1];
	int		port = 0;
//	int		operateType = 0;
	char		*ptrLine = NULL;
	char		fileDir[1024];
	char		tmp[32];
	FILE		*fp;

	// ��ȡ��������
	/*memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/operateType",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0902:: UnionReadRequestXMLPackageValue[%s]!\n","body/operateType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	operateType = atoi(tmpBuf);*/

	// ��ȡ�ڴ��С
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/memSize",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0902:: UnionReadRequestXMLPackageValue[%s]!\n","body/memSize");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	memSize = atoi(tmpBuf);

	// ��ȡ�˿ں�
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0902:: UnionReadRequestXMLPackageValue[%s]!\n","body/port");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	port = atoi(tmpBuf);

	memset(fileDir,0,sizeof(fileDir));
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"$UNIONREC/bin.x/start_memcached.x");	
	UnionReadDirFromStr(tmpBuf,-1,fileDir);
	
	if ((fp = fopen(fileDir,"rb+")) == NULL)
	{
		// û���򴴽�
		if ((fp = fopen(fileDir,"ab")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode0902:: fopen[%s] create failed!\n",fileDir);
			return(errCodeUseOSErrCode);
		}
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		len += sprintf(tmpBuf,"%s\n\n","#!/bin/sh");
		len += sprintf(tmpBuf + len,"memcached -d -m %d -p %d -P $UNIONTEMP/memcached1.pid\n",memSize,port);

		if ((ret = fprintf(fp,"%s",tmpBuf)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0902:: fprintf[%s] failed!\n",tmpBuf);
			fclose(fp);
			return(errCodeUseOSErrCode);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"chmod a+x %s",fileDir);
		system(tmpBuf);
	}
	else
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		// ��ȡ�������������
		while (fgets(tmpBuf,sizeof(tmpBuf),fp))
		{
			if ((ptrLine = strstr(tmpBuf,"memcached")) != NULL)
			{
				memset(tmp,0,sizeof(tmp));
				sprintf(tmp,"%d",port);
				if ((ptrLine = strstr(tmpBuf,tmp)) != NULL)
				{
					UnionUserErrLog("in UnionDealServiceCode0902:: port[%d] is already exist!\n",port);
					UnionSetResponseRemark("�˿�[%d]���������Ѿ�����",port);
					fclose(fp);
					return(errCodeParameter);
				}
			}
			else
				continue;

			memcachedNum ++;
		}
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"memcached -d -m %d -p %d -P $UNIONTEMP/memcached%d.pid\n",memSize,port,memcachedNum + 1);

		if ((ret = fprintf(fp,"%s",tmpBuf)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0902:: fprintf[%s] failed!\n",tmpBuf);
			fclose(fp);
			return(errCodeUseOSErrCode);
		}
	}
	
	fclose(fp);
	return 0;
}

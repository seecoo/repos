
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
�������:	0903
������:		��ӻ��������ֹͣ�ű�
��������:	��ӻ��������ֹͣ�ű�
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

	// ��ȡ��������
	/*memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/operateType",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: UnionReadRequestXMLPackageValue[%s]!\n","body/operateType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	operateType = atoi(tmpBuf);*/

	// ��ȡ�˿ں�
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0903:: UnionReadRequestXMLPackageValue[%s]!\n","body/port");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	port = atoi(tmpBuf);


	// �������ű��в��һ�����������
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
		//  ��ѯҪ��ӵķ����������
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
		UnionSetResponseRemark("���ȴ�����������");
		return(errCodeParameter);
	}

	// �ڽű������ֹͣ����
	memset(fileDir,0,sizeof(fileDir));
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"$UNIONREC/bin.x/stop_memcached.x");	
	UnionReadDirFromStr(tmpBuf,-1,fileDir);

	if ((fp = fopen(fileDir,"rb+")) == NULL)
	{
		// û���򴴽�
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
		// ����Ƿ����
		while (fgets(tmpBuf,sizeof(tmpBuf),fp))
		{
			memset(tmpPort,0,sizeof(tmpPort));
			sprintf(tmpPort,"memcached%d",memcachedNum);
			if ((ptrLine = strstr(tmpBuf,tmpPort)) != NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode0903:: port[%s] stop command is already exist!\n",tmpBuf);
				UnionSetResponseRemark("�˿�[%d]ֹͣ�ű��Ѿ�����",port);
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

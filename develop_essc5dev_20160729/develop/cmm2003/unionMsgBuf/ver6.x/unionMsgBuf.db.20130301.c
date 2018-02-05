//	Author:	Wolfgang Wang
//	Date:	2006/8/1

#define _realBaseDB_2_x_

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "unionMsgBuf6.x.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionEnv.h"
#include "UnionLog.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

extern PUnionSharedMemoryModule		pgpdfzMsgBufMDL;
extern PUnionMsgBufHDL			pgpdfzMsgBufHDL;
extern unsigned char			*pgpdfzMsgBuf;
extern int				gpdfzMsgBufMDLConnected;

extern char *UnionGetMDLNameOfMsgBuf();

int UnionGetFileNameOfMsgBufHDL(char *fileName)
{
	int	len;

	len = sprintf(fileName,"unionMsgBuf");
	fileName[len] = 0;
	return(len);
}

int UnionInitMsgBufHDL(PUnionMsgBufHDL pdef)
{
	int	i;
	int	ret = 0;
	int	iCnt;
	char	propertyName[50+1];
	char	propertyValue[128+1];
	char	sql[128+1];
	char	tmpBuf[128+1];

	int	default_value_maxNumOfMsg = 0;
	int	default_value_maxSizeOfMsg = 0;
	int	default_value_maxStayTime = 0;

	int	spec_value_maxNumOfMsg = 0;
	int	spec_value_maxSizeOfMsg = 0;
	int	spec_value_maxStayTime = 0;

	char	default_field_maxNumOfMsg[40+1] = "maxNumOfMsg";
	char	default_field_maxSizeOfMsg[40+1] = "maxSizeOfMsg";
	char	default_field_maxStayTime[40+1] = "maxStayTime";

	char	spec_field_maxNumOfMsg[40+1] = "";
	char	spec_field_maxSizeOfMsg[40+1] = "";
	char	spec_field_maxStayTime[40+1] = "";

	if (pdef == NULL)
		return(errCodeParameter);

	if(strcmp(UnionGetMDLNameOfMsgBuf(), "UnionMsgBufMDL") != 0)
	{
		sprintf(spec_field_maxNumOfMsg, "%s_%s", default_field_maxNumOfMsg, UnionGetMDLNameOfMsgBuf());
		sprintf(spec_field_maxSizeOfMsg, "%s_%s", default_field_maxSizeOfMsg, UnionGetMDLNameOfMsgBuf());
		sprintf(spec_field_maxStayTime, "%s_%s", default_field_maxStayTime, UnionGetMDLNameOfMsgBuf());
	}
	else
	{
		strcpy(spec_field_maxNumOfMsg, default_field_maxNumOfMsg);
		strcpy(spec_field_maxSizeOfMsg, default_field_maxSizeOfMsg);
		strcpy(spec_field_maxStayTime, default_field_maxStayTime);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select propertyName,propertyValue from unionMsgBuf");
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);
	
	for (i = 0; i < iCnt; i ++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionInitMsgBufHDL:: UnionLocateXMLPackage[detail]\n");
			continue;
		}
				
		memset(propertyName,0,sizeof(propertyName));
		UnionReadXMLPackageValue("propertyName", propertyName, sizeof(propertyName));
		memset(propertyValue,0,sizeof(propertyValue));
		UnionReadXMLPackageValue("propertyValue", propertyValue, sizeof(propertyValue));
		
		// modified 2013-12-05
		/*
		if (memcmp(propertyName,,11) == 0)
			pdef->maxNumOfMsg = atoi(propertyValue);
		
		if (memcmp(propertyName,"maxSizeOfMsg",12) == 0)
			pdef->maxSizeOfMsg = atol(propertyValue);

		if (memcmp(propertyName,"maxStayTime",11) == 0)
			pdef->maxStayTime = atoi(propertyValue);
		*/

		if (strcmp(propertyName, default_field_maxNumOfMsg) == 0)
			default_value_maxNumOfMsg = atoi(propertyValue);
		
		if (strcmp(propertyName, default_field_maxSizeOfMsg) == 0)
			default_value_maxSizeOfMsg = atol(propertyValue);

		if (strcmp(propertyName, default_field_maxStayTime) == 0)
			default_value_maxStayTime = atoi(propertyValue);


		if (strcmp(propertyName, spec_field_maxNumOfMsg) == 0)
			spec_value_maxNumOfMsg = atoi(propertyValue);
		
		if (strcmp(propertyName, spec_field_maxSizeOfMsg) == 0)
			spec_value_maxSizeOfMsg = atol(propertyValue);

		if (strcmp(propertyName, spec_field_maxStayTime) == 0)
			spec_value_maxStayTime = atoi(propertyValue);

		// end of modification 2013-12-05
	}

	// added 2013-12-05
	spec_value_maxNumOfMsg = spec_value_maxNumOfMsg ? spec_value_maxNumOfMsg : default_value_maxNumOfMsg;
	spec_value_maxSizeOfMsg = spec_value_maxSizeOfMsg ? spec_value_maxSizeOfMsg : default_value_maxSizeOfMsg;
	spec_value_maxStayTime = spec_value_maxStayTime ? spec_value_maxStayTime : default_value_maxStayTime;

	if(spec_value_maxNumOfMsg > 0 && spec_value_maxSizeOfMsg > 0 && spec_value_maxStayTime > 0)
	{
		pdef->maxNumOfMsg = spec_value_maxNumOfMsg;
		pdef->maxSizeOfMsg = spec_value_maxSizeOfMsg;
		pdef->maxStayTime = spec_value_maxStayTime;
	}

	UnionAuditLog("in UnionInitMsgBufHDL:: maxNumOfMsg = [%d], maxSizeOfMsg = [%d], maxStayTime = [%d].\n", pdef->maxNumOfMsg, pdef->maxSizeOfMsg, pdef->maxStayTime);
	// end of addition 2013-12-05

	pdef->userID = -1;
	pdef->queueIDOfFreePos = pdef->queueIDOfOccupiedPos = -1;
	return(0);
}


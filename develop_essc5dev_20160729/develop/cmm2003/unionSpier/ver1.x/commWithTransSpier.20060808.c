//	Author:		Wolfgang Wang
//	Date:		2006/8/8
//	Version:	5.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "transSpierBuf.h"
#include "unionErrCode.h"
#include "commWithTransSpier.h"
#include "unionResID.h"
#include "UnionLog.h"
#include "unionREC.h"
#include "unionVersion.h"

//extern char *UnionGetHsmIPAddrOfThisHsmTask();

// 2009/9/19，王纯军增加
char	gunionClientIPAddrToldTransSpier[15+1] = "";
// 2009/9/19，王纯军增加
int	gunionClientPortToldTransSpier=-1;

// 2009/9/19，王纯军增加
long gunionSpierSSN = 0;

int UnionIsTestingSystem()
{
	if (UnionReadIntTypeRECVar("isTestingSystem") > 0)
		return(1);
	else
		return(0);
}

// 2009/9/19，王纯军增加
void UnionIncreaseSpierSSN()
{
	if (++gunionSpierSSN <= 0)
		gunionSpierSSN = 1;
}

// 2009/9/19，王纯军增加
long UnionGetSpierSSN()
{
	return(gunionSpierSSN);
}

// 2009/9/19，王纯军增加
char *UnionGetClientIPAddrToldTransSpier()
{
	return(gunionClientIPAddrToldTransSpier);
}

// 2009/9/19，王纯军增加
int UnionGetClientPortToldTransSpier()
{
	return(gunionClientPortToldTransSpier);
}


// 2009/9/19，王纯军增加
void UnionSetClientAttrToldTransSpier(char *ipAddr,int port)
{
	strcpy(gunionClientIPAddrToldTransSpier,ipAddr);
	gunionClientPortToldTransSpier = port;
}

// 2009/9/19，王纯军增加
int UnionSendRequestInfoToTransSpier(int len,char *data)
{
	int	ret;
	int	resID;
	
	UnionIncreaseSpierSSN();
		
	if ((ret = UnionSendInfoToTransSpier(len,data)) < 0)
		return(ret);
	
	if (UnionIsTestingSystem())
		return(UnionBufferTransSpierMsg(data,len,resID = UnionGetMyTransInfoResID()));
	else
		return(0);
}

// 2009/9/19，王纯军增加
int UnionSendRequestInfoToTransSpierAlways(int len,char *data)
{
	int	ret;
	int	resID;
	
	UnionIncreaseSpierSSN();
		
	if ((ret = UnionSendInfoToTransSpier(len,data)) < 0)
		return(ret);
	
	return(UnionBufferTransSpierMsg(data,len,resID = UnionGetMyTransInfoResID()));
}

// 2009/9/19，王纯军增加
int UnionSendKDBServiceInfoToTransSpier(int len,char *data)
{
	return(UnionBufferTransSpierMsg(data,len,conResIDKDBService));
}

// 2009/9/19，王纯军增加
int UnionSendKeyDBSynchronizerInfoToTransSpier(int len,char *data)
{
	return(UnionBufferTransSpierMsg(data,len,conResIDKDBSynchronizerService));
}

// 2009/10/19，徐上钧增加
int UnionSendKeyDBBackuperInfoToTransSpier(int len,char *data)
{
	return(UnionBufferTransSpierMsg(data,len,conResIDKDBBackuper));
}

// 2009/9/19，王纯军增加
int UnionSendResponseInfoToTransSpier(int len,char *data)
{
	int	ret;
	int	resID;

	if ((ret = UnionSendInfoToTransSpier(len,data)) < 0)
		return(ret);
	
	if (UnionIsTestingSystem())
		return(UnionBufferTransSpierMsg(data,len,resID = UnionGetMyTransInfoResID()));
	else
		return(0);
}

int UnionSendHsmCmdInfoToTransSpier(int len,char *data)
{
	if ((data == NULL) || (len <= 0))
		return(errCodeParameter);
	
	data[len] = 0;
#ifdef _transMsgIsBinary_
	//UnionNullLogWithTime("%s::",UnionGetHsmIPAddrOfThisHsmTask());
	UnionNullLogWithTime("UnionSendHsmCmdInfoToTransSpier::");
	UnionMemNullLog((unsigned char *)data,len);
#else
	//UnionNullLogWithTime("%s::[%04d][%s]\n",UnionGetHsmIPAddrOfThisHsmTask(),len,data);
	UnionNullLogWithTime("UnionSendHsmCmdInfoToTransSpier::[%04d][%s]\n",len,data);
#endif
	return(0);
}

// 2009/9/19，王纯军增加
int UnionSendHsmCmdRequestInfoToTransSpier(int len,char *data)
{
	int	ret;
	
	if ((ret = UnionSendHsmCmdInfoToTransSpier(len,data)) < 0)
		return(ret);
	
	if (UnionIsTestingSystem())
		return(UnionBufferTransSpierMsg(data,len,conResIDHsmCmd));
	else
		return(0);
}


// 2009/9/19，王纯军增加
int UnionSendHsmCmdResponseInfoToTransSpier(int len,char *data)
{
	int	ret;
	
	if ((ret = UnionSendHsmCmdInfoToTransSpier(len,data)) < 0)
		return(ret);
	
	if (UnionIsTestingSystem())
		return(UnionBufferTransSpierMsg(data,len,conResIDHsmCmd));
	else
		return(0);
}

int UnionSendInfoToTransSpier(int len,char *data)
{
	if ((data == NULL) || (len <= 0))
		return(errCodeParameter);
	
	data[len] = 0;
#ifdef _transMsgIsBinary_
	UnionNullLogWithTime("[%s][%s,%d]\n",UnionGetApplicationName(),gunionClientIPAddrToldTransSpier,gunionClientPortToldTransSpier);
	UnionMemNullLog((unsigned char *)data,len);
#else
	UnionNullLogWithTime("[%s][%s,%d][%04d][%s]\n",UnionGetApplicationName(),gunionClientIPAddrToldTransSpier,gunionClientPortToldTransSpier,len,data);
#endif
	return(0);
}


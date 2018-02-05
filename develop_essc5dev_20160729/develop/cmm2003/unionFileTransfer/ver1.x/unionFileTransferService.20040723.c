//
#ifndef _WIN32
#if ( defined _AIX )
#define _LARGE_FILES
#include <unistd.h>
#else
#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#endif
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unionFileTransfer.h"
#include "UnionLog.h"
#include "UnionIO.h"
#include "unionRecFile.h"

//返回值：0，和客户端的会话结束，应该马上返回
//	1，和客户端的会话还没结束，应该接着返回响应包
int UnionFileTransferService(PUnionFileTransferHandle handle)
{
	char	fullFileName[1024];
	int	ret;

	if (handle->sizePerTransfer <= 0)
		handle->sizePerTransfer = 4000;

	switch (handle->cmdTag)
	{
		case	conFileReceiverCmdTagUpLoadFile://upload
			UnionSetAsFileReceiver();
			memset(fullFileName,0,sizeof(fullFileName));
			UnionFormFullFileName(handle->destinitionDir, handle->destinitionFileName, fullFileName);
			if (access(fullFileName, 0) == 0)
			{
				if ((handle->lengthTransferred = UnionGetFileSize(fullFileName)) < 0)
				{
					UnionUserErrLog("in UnionFileTransferSftService::UnionGetFileSize [%s]!\n", fullFileName);
					handle->totalFileLength = ret;
					return(1);
				}
			}
			else
				handle->lengthTransferred = 0;

			handle->numTransferred = handle->lengthTransferred / handle->sizePerTransfer;
			return(1);
		case	conFileReceiverCmdTagDownloadFile://download
			UnionSetAsNoneFileReceiver();
			if ((ret = UnionSetTransAttrForTransferHandle(handle)) < 0)
			{
				UnionUserErrLog("in UnionFileTransferSftService:: UnionSetTransAttrForTransferHandle!\n");
				handle->totalFileLength = ret;
			}

			handle->numTransferred = handle->lengthTransferred / handle->sizePerTransfer;
			return(1);
		default:
			UnionUserErrLog("in UnionFileTransferSftService::service code error!\n");
			handle->totalFileLength = -1;
			return(1);
	}
	return(0);
}


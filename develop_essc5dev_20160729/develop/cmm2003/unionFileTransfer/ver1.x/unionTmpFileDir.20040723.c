//	Wolfgang Wang, 2004/7/23

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"		// 使用3.x版本
#include "unionREC.h"

#include "unionFileTransfer.h"

int UnionAgreeToAccepteFileTransferred(PUnionFileTransferHandle phdl)
{
	char	*ptr = NULL;

	if (phdl == NULL)
		return(errCodeNullPointer);

	// 非文件接收者
	if (!UnionIsFileReceiver())
	{
		if ((strlen(phdl->originDir) == 0) && (phdl->originFileName[0] != '/') && (phdl->originFileName[0] != '$'))
		{
			if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
#ifdef _WIN32
				sprintf(phdl->originDir,".\\tmp");
#else
			sprintf(phdl->originDir ,"%s/tmp",getenv("HOME"));
#endif
			else
				sprintf(phdl->originDir, "%s",ptr);
		}
	}

	else if ((strlen(phdl->destinitionDir) == 0) && (phdl->destinitionFileName[0] != '/') && (phdl->destinitionFileName[0] != '$'))
	{
		if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
#ifdef _WIN32
			sprintf(phdl->destinitionDir,".\\tmp");
#else
		sprintf(phdl->destinitionDir,"%s/tmp",getenv("HOME"));
#endif
		else
			sprintf(phdl->destinitionDir,"%s",ptr);
	}
	if (strlen(phdl->destinitionFileName) == 0)
		strcpy(phdl->destinitionFileName,phdl->originFileName);

	return(0);
}

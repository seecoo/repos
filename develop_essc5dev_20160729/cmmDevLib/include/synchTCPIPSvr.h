// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2002/3/13

#ifndef _SynchTCPIPSvr_1_x_
#define _SynchTCPIPSvr_1_x_

#ifndef _WIN32_
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#ifdef _maxSizeOfClientMsg_8192_
#define conMaxSizeOfClientMsg	8192
#else
#define conMaxSizeOfClientMsg	81920
#endif

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)());
int UnionGetActiveTCIPSvrSckHDL();

int UnionFormTaskNameOfMngSvr(int port,char *taskName);

int UnionFreeUserSpecResource();

int UnionConnectRunningEnv();

int UnionGetFunSvr(char *funSvr);

#endif

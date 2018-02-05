// Wolfgang Wang
// 2005/03/30

#ifndef _UnionCommunicationCheck_
#define _UnionCommunicationCheck_

typedef struct
{
	char	ipAddr[15+1];
	int	port;
	short	checked;
	short	active;
} TUnionHost;
typedef TUnionHost	*PUnionHost;

#define conMaxHosts	20
typedef struct
{
	long		numOfChecker;
	short		terminateCheckIfAllHostsDown;
	long		intervalPerChecking;
	TUnionHost	hosts[conMaxHosts];
} TUnionHostGroup;
typedef TUnionHostGroup	*PUnionHostGroup;

int UnionConnectHostGroup();

int UnionDisconnectHostGroup();

int UnionRemoveHostGroup(int id);	

int UnionGetPortOfNullListener();

int UnionReloadHostGroup();

int UnionCheckAllHosts();

int UnionIsTerminateCheckIfAllHostsDown();

int UnionCheckAllHostsAlways();

int UnionCheckAllHostsUntilAllHostsDown();

int UnionOpenExitWhenAllHostsDown();

int UnionCloseExitWhenAllHostsDown();

int UnionPrintHostGroupToFile(FILE *fp);

int UnionPrintHostGroup();

int UnionStartNullHostListener();

#endif

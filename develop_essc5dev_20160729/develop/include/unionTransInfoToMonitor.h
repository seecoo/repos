#ifndef _unionTransInfoToMonitor_
#define _unionTransInfoToMonitor_

#include "transSpierBuf.h"

int UnionCloseCommWithMonitor();

int UnionGetDefFileNameOfMonitorSvrList(char *fileName);

int UnionReadAllMonitorConf(char ipAddrList[][15+1], int portList[], int maxCnt);

char *UnionGetMyMngSvrIDAtMonitor();

int UnionInitCommWithMonitor();

int UnionReconnectMonitorConn(PUnionConn pconn);

int UnionPackTransInfoResponsePackage(const char *data,int lenOfData,const char *errCode, const char *esscID, int resID,int resCmd, char *buf,int sizeOfBuf);

int UnionPackTransInfoRequestPackage(const char *data,int lenOfData,const char *esscID,int resID,int resCmd, char *buf,int sizeOfBuf);

int UnionSendResouceInfoToMonitor(int resID,const char *data,int len, int resCmd);

#endif // _unionTransInfoToMonitor_

#ifndef _unionMonitorEsscAvailable_
#define _unionMonitorEsscAvailable_


int UnionCheckEsscAvailable(char *ipAddr, int port, char *reqBuf, int sizeOfReqBuf, char *resBuf, int sizeOfResBuf, char *responseCode);

int UnionDealEsscAvailableRequest(int handle);

int UnionStartEsscAvailableSvr(int port, int (*UnionTaskActionBeforeExit)());

#endif // _unionMonitorEsscAvailable_

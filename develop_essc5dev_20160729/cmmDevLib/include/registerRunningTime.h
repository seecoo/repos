#ifndef _RegisterRunningTime_
#define _RegisterRunningTime_

#define defStageOfTransBefore	0
#define defStageOfTransing	1
#define defStageOfTransAfter	2

// 设置交易处理阶段
void UnionSetStageOfTransProcessing(int stage);

// 初始化登记节点ID
void UnionInitRegisterNodeID();

// 登记节点运行时间
void UnionRegisterRunningTime(char *nodeName);

// 根据请求报文，设置时间
void UnionSetTimeByRequestPackage();

// 设置时间到请求报文
void UnionSetTimeToRequestPackage();

#endif

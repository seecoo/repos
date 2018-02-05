// 2008/7/26
// Wolfang Wang

#ifndef _simuMngSvrLocally_
#define _simuMngSvrLocally_

// 2009/8/1,Wolfgang Wang added
char *UnionGetCurrentOperationTellerNo();

// 判断一个资源是否是保留资源
int UnionIsReservedRes(int resID);

// 获取当前资源号
int UnionGetCurrentResID();

// 2009/3/19增加
char *UnionGetCurrentRemoteMngSvrName();

// 获取当前请求号
int UnionGetCurrentServiceID();

// 设置当前请求号
int UnionSetCurrentServiceID(int serviceID);

// 判断是否是本地仿真mngSvr
int UnionIsReadMngSvrDataLocally();

// 设置本地仿真mngSvr
void UnionSetSimuMngSvrLocally();

// 与MngSvr通讯
// resName,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionExcuteDBSvrOperation(char *resName,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// 与MngSvr通讯
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionReadMngSvrDataLocally(int handle,char *tellerNo,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// 从记录"域1=域值|域2=域值|域3=域值|…"这种格式串中，拆分出关键域值"关键域1=域值|…"
int UnionGetPrimaryKeyFldFromObjectRecord(char *resName,char *record,char *priFld);

/*
功能	
	自动在记录后附加记录输入属性
输入参数
	recStr		记录串
	lenOfRecStr	记录长度
	sizeOfRecStr	记录串缓冲大小
输入出数
	recStr	记录串
返回值
	>=0	附加了新串的值长度
	<0	错误码
*/
int UnionAutoAppendDBRecInputAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr);

/*
功能	
	自动在记录后附加记录修改属性
输入参数
	recStr		记录串
	lenOfRecStr	记录长度
	sizeOfRecStr	记录串缓冲大小
输入出数
	recStr	记录串
返回值
	>=0	附加了新串的值长度
	<0	错误码
*/
int UnionAutoAppendDBRecUpdateAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr);

/*
function：
        获取允许修改的字段名称以及内容
param:
        [IN]:
        resName:        对象名
        record:         整条对象记录信息

        [OUT]:
        modFld:         保存所有允许修改的字段名以及字段名称
return:
        >=0:            所有允许修改的字段名以及字段名称的长度
        < 0:            解析出错
*/
int UnionGetAllowModifyFldFromObjectRecord(char *resName, char *record, char *modFld);

#endif


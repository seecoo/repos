#ifndef _PinPadDB_H_
#define _PinPadDB_H_

#include "unionDesKey.h"
#include "commWithHsmSvr.h"

#define	defTableNameOfPinPadDB		"pinPad"
#define	APPNOOFITMK			"ITMK"
#define	APPNOOFKMC			"HOST"

// 定义对称密钥状态标识
typedef enum 
{
	conPinPadStatusOfInitial,		// 初始状态
	conPinPadStatusOfEnabled,		// 启用状态
	conPinPadStatusOfSuspend,		// 挂起状态
} TUnionPinPadStatus;

// 定义密码键盘结构体
typedef struct
{
	char				pinPadID[32];			// 键盘ID
	char				regTime[16];			// 登记时间
	char				lastTime[16];			// 最后修改时间
	char				status[4];			// 状态
	char				branch[16];			// 分行号	
	char				teller[16];			// 操作员
	char				producter[48];			//生产厂商
	char				producterID[48];		//键盘序列号
	int				zmkLength;			//zmk密钥长度
	int				zpkLength;			//zpk密钥长度
	int				zakLength;			//zak密钥长度
	char				appNo[16];			//应用ID
	char				remark[128];			// 备注
} TUnionPinPadDB;
typedef TUnionPinPadDB		*PUnionPinPadDB;

/* 
功能：	增加密码键盘
参数：	ppinPadDB[in]	密码键盘结构
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionInsertPinPadDB(PUnionPinPadDB ppinPadDB);

/* 
功能：	删除一个密码键盘
参数：	ppinPadDB[in]	密码键盘结构
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionDropPinPadDB(PUnionPinPadDB ppinPadDB);

/* 
功能：	读取一个对称密钥容器
参数：	pinPadID[in]		密码键盘ID
	ppinPadDB[out]		密码键盘结构
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadPinPadDB(char *pinPadID,PUnionPinPadDB ppinPadDB);

/* 
功能：	更新一个密码键盘
参数：	ppinPadDB[in]	密码键盘结构
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdatePinPadDB(PUnionPinPadDB ppinPadDB);


/* 
功能：  读取一个密码键盘
参数：  pinPadID[in]            键盘ID
        ppinPadDB[out]
返回值：>=0                     成功
        <0                      失败，返回错误码
*/
int UnionReadPinPadDB(char *pinPadID,PUnionPinPadDB ppinPadDB);

/* 

功能：  读取行号
参数：  branch[out]     分行行号

*/
void UnionGetPinPadBranch(char *branchID);


/*

功能:   取总行分行标志 
返回值:  <0     失败，返回错误码
         >=0    成功，返回成功值
*/
int UnionGetPinPadBranchType();


/*

功能:   获取远程标志
返回值:         <0      失败,返回错误码
                >=0     成功

*/
int UnionGetPinPadRemoteDistributeID(char *distributeID);


// 获取分行与总行约定的密钥
int UnionGetPinPadZmkName(char *zmkName);

// 删除键盘密钥
int UnionDeleteKeyOfPinPad(char *appNo,char *pinPadID);

// 创建zmk\zpk\zak密钥
int UnionPinPadInsertDesKeyIntoKeyDB(char *appNo,char *pinPadID,int zmkLength,int zpkLength,int zakLength,char *branch);

//设置密码键盘密钥有效
int UnionActiveKeyOfPinPad(char *appNo, char *pinPadID);


//设置密钥失效
int UnionInActiveKeyOfPinPad(char *appNo, char *pinPadID);

// 读取所有地方联社到文件
int UnionReadQueryAllBranchToFile(char *fileName);

// 写所有厂商到文件
int UnionReadQueryAllProducterToFile(char *fileName);

// 写所有操作者到文件
int UnionReadQueryAllOperatorToFile(char *fileName);

// 读取厂商信息
int UnionReadPinPadProducterRec(char *producter,char *dllName,int sizeofDllName,char *remark,int sizeofRemark);
/*      
功能：  批量读取密码键盘
参数：  branch[in]            键盘ID
返回值：>=0                     成功
        <0                      失败，返回错误码
*/
int UnionReadPinPadDBbranch(char *branch);

// 获取总行与分行约定的密钥
int UnionGetPinPadZmkNameWithBranch(char *zmkName);

// 分行操作失败回滚
int UnionPinPadOperateRollback(char *branch,char *status);

/* 
功能：  获取地方联社中文名称
参数：  branchNo[in]            branchNo
        sizeofBranchName[in]    长度
        branchName[out]         地方联社中文名

返回值：>=0                     成功
        <0                      失败，返回错误码
*/
int UnionGetBranchNameByBranchNo(char *branchNo,char *branchName,int sizeofBranchName);
#endif


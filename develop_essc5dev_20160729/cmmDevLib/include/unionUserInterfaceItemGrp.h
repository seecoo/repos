//	Wolfgang Wang
//	2009/4/29

#ifndef _unionUserInterfaceItemGrp_
#define _unionUserInterfaceItemGrp_

#include "unionTree.h"

/*
功能	
	判断界面项数组是否已被初始化了
输入参数
	无
输出参数
	无
返回值
	1	已初始化
	0	未初始化
*/
int UnionIsInterfaceItemsGrpInited();

/*
功能	
	获取当前的界面项数组
输入参数
	无
输出参数
	无
返回值
	当前界面项数组指针
*/
PUnionTree UnionGetCurrentInterfaceItemGrp();

/*
功能	
	释放界面项数据结构
输入参数
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
void UnionDeleteInterfaceItemGrp();

/*
功能	
	判断一个界面项是否存在
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	存在
	0	不存在
	<0	出错代码
*/
int UnionIsInterfaceItemExists(char *itemTag,char *fmt,...);

/*
功能	
	判断一个包含了指定串的界面项是否存在
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	存在
	0	不存在
	<0	出错代码
*/
int UnionIsInterfaceItemContainSpecStrExists(char *itemTag,char *fmt,...);

/*
功能	
	寻找一个界面项
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	成功	界面项指针
	失败	空指针
*/
PUnionLeaf UnionFindInterfaceItem(char *itemTag,char *fmt,...);

/*
功能	
	初始化界面项数据结构
输入参数
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitInterfaceItemGrp(char *mainMenuName);

/*
功能	
	将界面项写入到指定文件中
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintInterfaceItemGrpToSpecFile(char *fileName);
	
/*
功能	
	将错误的界面项写入到指定文件中
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintErrorInterfaceItemGrpToSpecFile(char *fileName);

/*
功能	
	初始化界面项数据结构
输入参数
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitInterfaceItemGrp(char *mainMenuName);
	
/*
功能	
	判断一个界面项当前是否正在被处理
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	正在被处理
	0	未被处理
	<0	出错代码
*/
int UnionSetInterfaceItemDealCurrently(char *itemTag,char *fmt,...);

/*
功能	
	将一个界面项加入到界面项组中
输入参数
	remark	说明
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionAddInterfaceItemToItemGrp(char *remark,char *itemTag,char *fmt,...);

#endif

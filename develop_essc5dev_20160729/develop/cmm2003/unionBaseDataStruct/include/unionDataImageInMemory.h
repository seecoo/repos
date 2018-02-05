// Wolfgang Wang
// 2008/11/03

#ifndef _unionDataImageInMemory_
#define _unionDataImageInMemory_

#define conResNameOfDataImageInMemory	"unionDataImageInMemory"

// 判断是否将一个对象的映像加载到映像区
/*
输入参数
	objectName	表名		
	key		关键字
输出参数
	无
返回值
	1		加载
	0		不加载
*/
int UnionIsUseDataImageInMemory(char *objectName,char *key);

// 删除所有数据映像
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0，删除的数据映像的数量
	失败		<0,错误码
*/
int UnionDeleteAllDataImageInMemory();

// 判断一个数据映像是否存在
/*
输入参数
	key		关键字
输出参数
	无
返回值
	存在		1
	不存在		0
	出错		负值
*/
int UnionExistDataImageInMemory(char *key);

// 增加一个数据映像
/*
输入参数
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionInsertDataImageInMemory(char *key,unsigned char *data,int lenOfData);

// 修改一个数据映像
/*
输入参数
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionModifyDataImageInMemory(char *key,unsigned char *data,int lenOfData);

// 读一个数据映像
/*
输入参数
	key		关键字
	sizeOfBuf	数据缓冲的大小
输出参数
	data		数据
返回值
	成功		>=0，读到的数据长度
	失败		<0,错误码
*/
int UnionReadDataImageInMemory(char *key,unsigned char *data,int sizeOfBuf);

// 删除一个数据映像
/*
输入参数
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionDeleteDataImageInMemory(char *key);

// 判断一个记录数据映像是否存在
/*
输入参数
	objectName	表名		
	key		关键字
输出参数
	无
返回值
	存在		1
	不存在		0
	出错		负值
*/
int UnionExistRecDataImageInMemory(char *objectName,char *key);

// 增加一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionInsertRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData);

// 修改一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
	data		数据
	lenOfData	数据长度
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionModifyRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData);

// 读一个记录的数据映像
/*
输入参数
	key		关键字
	sizeOfBuf	数据缓冲的大小
输出参数
	data		数据
返回值
	成功		>=0，读到的数据长度
	失败		<0,错误码
*/
int UnionReadRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int sizeOfBuf);

// 删除一个记录的数据映像
/*
输入参数
	objectName	表名		
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionDeleteRecDataImageInMemory(char *objectName,char *key);

// 显示所有数据映像
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionPrintAllDataImageInMemoryToFile(FILE *fp);

	
// 显示所有数据
/*
输入参数
	resID		资源号
输出参数
	无
返回值
	>=0		数据数目
	<0		错误码
*/
int UnionSpierAllDataImageInMemory(int resID);

#endif

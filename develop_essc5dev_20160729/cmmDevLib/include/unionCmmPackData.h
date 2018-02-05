// Author:	Wolfgang Wang
// Date:	2008/9/26

// 这个文件定义了ESSC报文格式

#ifndef _unionCmmPackData_
#define _unionCmmPackData_

// 以下定义一个域的值的位置
typedef struct
{
	int		tag;		// 域的标识号
	int		len;		// 域的长度
	unsigned char	*value;		// 域值的位置
} TUnionCmmPackFldData;
typedef TUnionCmmPackFldData	*PUnionCmmPackFldData;

#define conMaxNumOfFldPerCmmPack	128
// 定义一个域组的位置表
typedef struct
{
	TUnionCmmPackFldData	fldGrp[conMaxNumOfFldPerCmmPack];	// 域组
	int			fldNum;					// 域组中域的数量
	char			data[8192+1];				// 数据
	int			len;					// 数据长度
	int			offset;					// 当前偏移
} TUnionCmmPackData;
typedef TUnionCmmPackData	*PUnionCmmPackData;

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutCharTypeCmmPackFldIntoStr(int tag,char value,int sizeOfBuf,char *buf);

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutDoubleTypeCmmPackFldIntoStr(int tag,double value,int sizeOfBuf,char *buf);

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutLongTypeCmmPackFldIntoStr(int tag,long value,int sizeOfBuf,char *buf);

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutIntTypeCmmPackFldIntoStr(int tag,int value,int sizeOfBuf,char *buf);

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	lenOfValue	域值长度
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutCmmPackFldIntoStr(int tag,char *value,int lenOfValue,int sizeOfBuf,char *buf);

// 从一个报文中读取一个域，返回域在包中占的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	sizeOfBuf	接收域值的缓冲大小
输出参数
	tag		域标识
	value		域值
	fldValue	域值长度
返回值
	>=0		域值在包中占的长度
	<0		出错代码
*/
int UnionReadCmmPackFldFromStr(char *data,int len,int sizeOfBuf,int *tag,int *fldLen,char *value);

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadSpecIntTypeCmmPackFldFromStr(char *data,int len,int tag,int *value);

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadSpecLongTypeCmmPackFldFromStr(char *data,int len,int tag,long *value);

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadSpecDoubleTypeCmmPackFldFromStr(char *data,int len,int tag,double *value);

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadSpecCharTypeCmmPackFldFromStr(char *data,int len,int tag,char *value);



// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
	sizeOfBuf	接收域值的缓冲大小
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadSpecCmmPackFldFromStr(char *data,int len,int tag,int sizeOfBuf,char *value);

// 初始化域值位置表
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
	sizeOfBuf	接收域值的缓冲大小
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionInitCmmPackFldDataList(PUnionCmmPackData pposList);
	
// 解包，将域值列解包到一个域值位置清单中
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionUnpackCmmPackIntoFldDataList(char *data,int len,PUnionCmmPackData pposList);

// 解包，将一个域值写到一个域值位置清单中
/*
输入参数
	len		数据串的长度
	tag		域标识
	value		数据串的值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutCmmPackFldIntoFldDataList(int tag,int len,char *value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutIntTypeCmmPackFldIntoFldDataList(int tag,int value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutLongTypeCmmPackFldIntoFldDataList(int tag,long value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutCharTypeCmmPackFldIntoFldDataList(int tag,char value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList(int tag,unsigned int value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList(int tag,unsigned long value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList(int tag,unsigned char value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutStringTypeCmmPackFldIntoFldDataList(int tag,char *value,PUnionCmmPackData pposList);

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutDoubleTypeCmmPackFldIntoFldDataList(int tag,double value,PUnionCmmPackData pposList);

// 解包，将域值列解包到一个域值位置清单中
/*
输入参数
	pposList	域值位置清单
	sizeOfBuf	缓冲的大小
输出参数
	data		域所在的数据串
返回值
	>=0		数据包的长度
	<0		出错代码
*/
int UnionPackFldDataListIntoCmmPack(PUnionCmmPackData pposList,int sizeOfBuf,char *data);

// 将报文数据写入到日志中
/*
输入参数
	title		标题
	pposList	域值位置清单
输出参数
	无
返回值
	无
*/
void UnionLogCmmPackData(char *title,PUnionCmmPackData pposList);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	sizeOfBuf	域值缓中大小
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int sizeOfBuf,char *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,long *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,char *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadUnsignedIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned int *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned long *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadUnsignedCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned char *value);

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionReadDoubleTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,double *value);


// 测试函数
int UnionTestCmmPackDataFun(char *data,int len);

// 复制报文
/*
输入参数
	poriList	源包
输出参数
	pdesList	目标包
返回值
	>=0		域数目
	<0		出错代码
*/
int UnionCopyPackFldDataList(PUnionCmmPackData poriList,PUnionCmmPackData pdesList);

#endif

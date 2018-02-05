// Author:	Wolfgang Wang
// Date:	2006/8/9

// 这个文件定义了报文格式

#ifndef _unionPackageDef_
#define _unionPackageDef_

#define conDataFormatIsAscii	1
#define conDataFormatIsBinary	2

#define conPackIsRequest	0
#define conPackIsResponse	1
#define conPackIsData		2

// 定义包的最大长度
#define conMaxPackSizeOfEngine	16384

// 以下定义一个域的结构
typedef struct
{
	int		tag;		// 域的标识号
	int		len;		// 域的长度
	int		dataFormat;	// 编码方式
	unsigned char	*value;		// 域的值
} TUnionPackageFld;
typedef TUnionPackageFld	*PUnionPackageFld;

#define conMaxNumOfPackageFld	32
// 以下定义报文
typedef struct
{
	int	direction;		// 方向标识，0请求，1响应
	int	serviceID;		// 服务代码
	int	responseCode;		// 响应码,只有当direcion=1时，长有该域
	int	fldNum;			// 域数目
	TUnionPackageFld	fldGrp[conMaxNumOfPackageFld];	// 域组
	int	offset;			// 当前可用缓冲的偏移
	unsigned char	dataBuf[conMaxPackSizeOfEngine+1024+1];	// 缓冲
} TUnionPackage;
typedef TUnionPackage	*PUnionPackage;

/*
功能
	将一个报文域打入到包中,返回打入到包中的数据的长度
输入参数
	pfld		域指针
	sizeOfBuf	缓冲区大小
输出参数
	buf		缓冲
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionPutPackageFldIntoStr(PUnionPackageFld pfld,char *buf,int sizeOfBuf);
 
/*
功能
	从一个报文中读取一个域，返回域在包中占的长度
输入参数
	data		数据包
	len		数据包长度
	sizeOfFldValue	pfld的value值的缓冲大小
输出参数
	pfld		域指针
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionReadPackageFldFromStr(char *data,int len,PUnionPackageFld pfld,int sizeOfFldValue);

/*
功能
	初始化报文
输入参数
	pfld		域指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitPackage(PUnionPackage ppackage);

/*
功能
	初始化报文成一个请求报文
输入参数
	pfld		域指针
	serviceID	请求代码
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitPackageAsRequest(PUnionPackage ppackage,int serviceID);

/*
功能
	初始化报文成一个数据报文
输入参数
	pfld		域指针
	serviceID	请求代码
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitPackageAsDataBlock(PUnionPackage ppackage,int serviceID);

/*
功能
	初始化报文成一个响应报文
输入参数
	pfld		域指针
	serviceID	请求代码
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitPackageAsResponse(PUnionPackage ppackage,int serviceID);
/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	dataFormat	域的编码方式
	len		域的长度
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutPackageFld(PUnionPackage ppackage,int fldTag,int dataFormat,int len,char *value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	dataFormat	域的编码方式
	len		域的长度
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutBitsTypePackageFld(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	value		域的值
	len		数据的长度
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutStrTypePackageFldWithLen(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutIntTypePackageFld(PUnionPackage ppackage,int fldTag,int value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutLongTypePackageFld(PUnionPackage ppackage,int fldTag,long value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	fldTag		域标识
	dataFormat	域的编码方式
	len		域的长度
	value		域的值
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionPutBitsTypePackageFld(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	dataFormat	域的编码方式
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadPackageFld(PUnionPackage ppackage,int fldTag,int *dataFormat,char *value,int sizeOfBuf);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadBitsTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadIntTypePackageFld(PUnionPackage ppackage,int fldTag,int *value);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadLongTypePackageFld(PUnionPackage ppackage,int fldTag,long *value);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	fldTag		域标识
	sizeOfBuf	域的值的缓冲大小
输出参数
	value		域的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadDoubleTypePackageFld(PUnionPackage ppackage,int fldTag,double *value);

/*
功能
	将包写入日志
输入参数
	ppackage	包指针
输出参数
	无
返回值
	无
*/
void UnionLogPackage(PUnionPackage ppackage);

/*
功能
	将包打入到串中
输入参数
	ppackage	包指针
	sizeOfBuf	缓冲大小
输出参数
	buf		包组成的串
返回值
	>=0	成功，包组成的串长度
	<0	错误码
*/
int UnionPackPackage(PUnionPackage ppackage,char *buf,int sizeOfBuf);

/*
功能
	将一个串解到包中
输入参数
	data		串
	lenOfData	串长度
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
*/
int UnionUnpackPackage(char *data,int lenOfData,PUnionPackage ppackage);

#endif

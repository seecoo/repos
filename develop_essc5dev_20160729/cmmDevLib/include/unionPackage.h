// Author:	zhangyd
// Date:	2015/1/8

// 这个文件定义了报文格式

#ifndef _unionPackage_
#define _unionPackage_

/*
#define conDataFormatIsAscii	1
#define conDataFormatIsBinary	2

#define conPackIsRequest	0
#define conPackIsResponse	1
#define conPackIsData		2
*/

#define PACKAGE_VERSION_001		"V001"
#define PACKAGE_MEMORY_BLOCK_SIZE	32

// 以下定义元素类型
typedef enum
{
	PACKAGE_ELEMENT_TAG = 2,
	PACKAGE_ELEMENT_VALUE = 4
} TUnionPackageElementType;

// 以下定义一个元素的结构
typedef struct
{
	unsigned char		*pos;		// 元素的位置
	int			len;		// 元素的长度
} TUnionPackageElement;
typedef TUnionPackageElement	*PUnionPackageElement;

// 以下定义一个域的结构
typedef struct
{
	TUnionPackageElement	tag;		// 域的标签
	TUnionPackageElement	value;		// 域的值
} TUnionPackageFld;
typedef TUnionPackageFld	*PUnionPackageFld;

// 以下定义一个组的结构
#define conMaxNumOfPackageFld	99
typedef struct
{
	int			num;				// 数量
	int			index;
	TUnionPackageElement	fldGrpID;			// 组ID
	TUnionPackageFld	fld[conMaxNumOfPackageFld];	// 域的值
} TUnionPackageGrp;
typedef TUnionPackageGrp	*PUnionPackageGrp;

// 以下定义报文
#define conMaxNumOfPackageGrp	9
#define PACKAGE_BUF_SIZE	8192

typedef struct
{
	char			version[PACKAGE_MEMORY_BLOCK_SIZE];
	int			num;				// 组数目
	int			index;
	TUnionPackageGrp	fldGrp[conMaxNumOfPackageGrp];	// 域组

	long			bufSize;			// 缓存的大小
	unsigned char		*bufbase;			// 缓冲
	unsigned char		*bufptr ;			
	long			buf_remain_len;			// 当前可用缓冲的偏移
} TUnionPackage;
typedef TUnionPackage		*PUnionPackage;

int UnionPackageConvertIntStringToInt(unsigned char *str,int lenOfStr);

/*
功能
	将一个元素打入到包中,返回打入到包中的数据的长度
输入参数
	ppackage	包指针
	buf		数据指针
	len		数据大小
输出参数
	buf		缓冲
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionPackageFormatBuffer(PUnionPackage ppackage,PUnionPackageElement pelement,unsigned char *buf,int len);

/*
功能
	将一个元素打入到包中,返回打入到包中的数据的长度
输入参数
	pelement	元素指针
	elementType	元素类型
	sizeOfBuf	缓冲区大小
输出参数
	buf		缓冲
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionPutPackageElementIntoStr(PUnionPackageElement pelement,TUnionPackageElementType elementType,char *buf,int sizeOfBuf);

/*
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	tag		TAG
	lenOfTag	TAG的长度
	value		VALUE
	lenOfValue	VALUE的长度
输出参数
	ppackage	包指针
返回值
	>=0	成功，偏移量
	<0	错误码
*/
int UnionPutPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int lenOfValue);
 
/*
功能
	从一个报文中读取一个元素，返回域在包中占的长度
输入参数
	ppackage	包指针
	data		数据包
	len		数据包长度
	elementType	元素类型
输出参数
	pelement	元素指针
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionReadPackageElementFromStr(PUnionPackage ppackage,unsigned char *data,int len,TUnionPackageElementType elementType,PUnionPackageElement pelement);

/*
功能
	从一个报文中读取一个域，返回域在包中占的长度
输入参数
	data		数据包
	len		数据包长度
输出参数
	pfld		域指针
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionReadPackageFldFromStr(PUnionPackage ppackage,unsigned char *data,int len,PUnionPackageFld pfld);

/*
功能
	初始化报文
输入参数
	ppackage		报文结构体指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitPackage(PUnionPackage ppackage);

/*
功能
	释放报文
输入参数
	ppackage		报文结构体指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionFreePackage(PUnionPackage ppackage);

/*
功能
	从包中读一个元素
输入参数
	pelement	元素指针
	element		元素
	sizeOfBuf	元素的值的缓冲大小
输出参数
	value		元素的值	
返回值
	>=0	成功，返回元素长度
	<0	错误码
*/
int UnionReadPackageElement(PUnionPackageElement pelement,char *value,int sizeOfBuf);

/*
功能
	从包中读一个域
输入参数
	ppackage	包指针
	tag		TAG标识
	lenOfTag	TAG的长度
	sizeOfBuf	VALUE的值的缓冲大小
输出参数
	value		VALUE的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
*/
int UnionReadPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int sizeOfBuf);

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
int UnionUnpackPackage(unsigned char *data,int lenOfData,PUnionPackage ppackage);

/*
功能
	设置报文版本
输入参数
	ppackage	包指针
	version		版本
	len		版本长度
输出参数

返回值
	>=0	成功
	<0	错误码
*/
int UnionPackageSetVersion(PUnionPackage ppackage,char *version,int len);
/*
功能    
        将一个报文域打入到包中,返回打入到包中的数据的长度
输入参数
        pfld            域指针
        sizeOfBuf       缓冲区大小
输出参数
        buf             缓冲
返回值  
        >=0     域在包中的字节数
        <0      错误码
*/      
int UnionPutPackageFldIntoStr(PUnionPackageFld pfld,char *buf,int sizeOfBuf);

/*  2015-08-26
功能
        清空包中某组的字段内容
输入参数
        ppackage        包指针
        groupName       组名
输出参数
        无
返回值
        >=0     成功，包组成的串长度
        <0      错误码
*/
int UnionClearPackageGrpByName(PUnionPackage ppackage, char *groupName);

#endif

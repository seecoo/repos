// wolfang wang
// 2008/10/3

#ifndef _unionDatagramDef_
#define _unionDatagramDef_

#define conDatagramFldDefTagFldID	"fldID"
#define conDatagramFldDefTagFldValueID	"valueID"
#define conDatagramFldDefTagAlais	"alais"
#define conDatagramFldDefTagLen		"len"
#define conDatagramFldDefTagRemark	"remark"
#define conDatagramFldDefTagIsRequest	"isRequest"
#define conDatagramFldDefTagOptional	"optional"
#define conDatagramTagAppID		"idOfApp"
#define conDatagramTagServiceID		"serviceID"
#define conDatagramTagHeader		"header="

/** 外部报文定义 **/
// 报文域定义
typedef struct
{
	int		isRequest;		// 是请求还是响应标识
	char		fldID[64+1];		// 报文域标识
	char		alais[64+1];		// 别名
	int		len;			// 最大长度
	char		valueID[64+1];		// 值域，对域进行赋值，进行检查，引用值域表中的标识
	char		remark[128+1];		// 说明
	int		optional;		// 是否是可选域，1表示可选，0表示必须域
} TUnionDatagramFldDef;
typedef TUnionDatagramFldDef	*PUnionDatagramFldDef;

#define conMaxNumOfFldPerDatagram	64
// 定义一个双向报文的定义
typedef struct
{
	char			appID[64+1];		// 报文的用途
	char			datagramID[64+1];	// 报文的标识
	char			serviceID[3+1];		// 服务代码
	int			fldNum;			// 域数目
	TUnionDatagramFldDef	fldGrp[conMaxNumOfFldPerDatagram];	// 报文中域数目
} TUnionDatagramDef;
typedef TUnionDatagramDef	*PUnionDatagramDef;

/*
功能	
	从一个数据串中读取一个报文的头定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	serviceID	报文头中的服务代码
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramHeaderFromStr(char *str,int lenOfStr,char *serviceID);

/*
功能	
	获得一个报文定义的文件名
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	fileName	获得的报文定义的文件名称
返回值
	无
*/
void UnionGetFileNameOfDatagramDef(char *appID,char *datagramID,char *fileName);

/*
功能	
	从一个数据串中读取一个报文域定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	pdef		读出的报文域定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramFldDefFromStr(char *str,int lenOfStr,PUnionDatagramFldDef pdef);

/*
功能	
	从表中读取一个报文的定义
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	pdef		读出的报文定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadSpecDatagramDefFromFile(char *appID,char *datagramID,PUnionDatagramDef pdef);

/*
功能	
	打印一个报文的域定义
输入参数
	pdef		要打印的报文域定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramFldDefToFp(PUnionDatagramFldDef pdef,FILE *fp);

/*
功能	
	打印一个报文的定义
输入参数
	pdef		要打印的报文定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramDefToFp(PUnionDatagramDef pdef,FILE *fp);

/*
功能	
	打印一个报文的定义
输入参数
	appID		报文用途标识
	datagramID	报文的类别
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramDefToFp(char *appID,char *datagramID,FILE *fp);

/*
功能	
	打印一个报文的定义到指定文件
输入参数
	appID		报文用途标识
	datagramID	报文的类别
	fileName	文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramDefToSpecFile(char *appID,char *datagramID,char *fileName);
#endif

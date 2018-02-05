// wolfang wang
// 2008/10/3

#ifndef _unionDatagrmClassDef_
#define _unionDatagrmClassDef_

#define conDatagramClassDefTagClassID		"classID"
#define conDatagramClassDefTagRemark		"remark"
#define conDatagramClassDefTagAppID		"idOfApp"
#define conDatagramClassDefTagNameOfModuleOfSvr	"nameOfModuleOfSvr"

// 定义报文的类别
#define conDatagramClassTagUnionCmmPack	"unionCmmPack"
#define conDatagramClassTagISO8583	"ISO-8583"

// 一种用途的报文采用的实现类型
typedef struct
{
	char		appID[64+1];			// 报文的用途标识
	char		classID[64+1];			// 采用的报文类型
	char		remark[128+1];			// 说明
	char		nameOfModuleOfSvr[64+1];	// Svr端模块
} TUnionDatagramClassDef;
typedef TUnionDatagramClassDef		*PUnionDatagramClassDef;

/*
功能	
	获得应用报文类型定义的文件名
输入参数
	无
输出参数
	fileName	获得的应用报文类型定义的文件名称
返回值
	无
*/
void UnionGetFileNameOfDatagramClassDef(char *fileName);

/*
功能	
	获得应用报文类型对应的服务端模块标识
输入参数
	appID			应用标识
输出参数
	nameOfModuleOfSvr	服务端模块标识
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetNameOfModuleSvrOfSpecDatagramClass(char *appID,char *nameOfModuleOfSvr);

/*
功能	
	从一个定义口串中读取一个报文类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadDatagramClassDefFromStr(char *str,int lenOfStr,PUnionDatagramClassDef pdef);

/* 指定名称的报文类型标识的定义
输入参数
	appID	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadDatagramClassDefFromFile(char *appID,PUnionDatagramClassDef pdef);


/*
功能	
	打印一个报文的类型定义
输入参数
	pdef		要打印的报文类型定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintDatagramClassDefToFp(PUnionDatagramClassDef pdef,FILE *fp);
/*
功能	
	打印一个报文类型的定义
输入参数
	appID		报文用途标识
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramClassDefToFp(char *appID,FILE *fp);

/*
功能	
	打印一个报文类型的定义
输入参数
	appID		报文用途标识
	fileName	输出到的文件
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecDatagramClassDef(char *appID,char *fileName);

#endif

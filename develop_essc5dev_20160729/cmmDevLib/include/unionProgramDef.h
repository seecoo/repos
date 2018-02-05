// wolfang wang
// 2008/10/3

#ifndef _unionProgramDef_
#define _unionProgramDef_

#define conProgramDefTagNameOfProgram	"nameOfProgram"
#define conProgramDefTagNameOfModule	"nameOfModule"
#define conProgramDefTagVersion		"version"
#define conProgramDefTagRemark		"remark"

typedef struct
{
	char			nameOfProgram[64+1];		// 程序名称
	char			nameOfModule[64+1];		// 模块名称
	char			version[128+1];			// 开发目录
	char			remark[128+1];
} TUnionProgramDef;
typedef TUnionProgramDef	*PUnionProgramDef;

/*
功能	
	获得一个程序所属的模块名称
输入参数
	nameOfProgram	程序名称
输出参数
	nameOfModule	模块名称
	version		版本
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetModuleNameAndVersionOfProgram(char *nameOfProgram,char *nameOfModule,char *version);

/*
功能	
	从一个定义口串中读取一个程序定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadProgramDefFromStr(char *str,int lenOfStr,PUnionProgramDef pdef);

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintProgramDefToFp(PUnionProgramDef pdef,FILE *fp);

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
nameOfPrograme	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintProgramDefInFileToFp(char *fileName,char *nameOfProgram,FILE *fp);
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
nameOfPrograme	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputProgramDefInFile(char *nameOfProgram,char *fileName);
#endif

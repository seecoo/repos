// wolfang wang
// 2008/10/3

#ifndef _unionModuleDef_
#define _unionModuleDef_

#define conModuleDefTagNameOfModule	"nameOfModule"
#define conModuleDefTagDevDir		"devDir"
#define conModuleDefTagRemark		"remark"
#define conModuleDefTagIncFileConf	"incFileConf"

typedef struct
{
	char			nameOfModule[64+1];		// 模块名称
	char			devDir[128+1];			// 开发目录
	char			incFileConf[64+1];		// 定义了模块包括的头文件的文件
	char			remark[128+1];
} TUnionModuleDef;
typedef TUnionModuleDef	*PUnionModuleDef;

/*
功能	
	获得一个模块的开发目录和头文件配置
输入参数
	nameOfModulde	模块名称
输出参数
	devDir		开发目录
	incFileConf	头文件配置
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetDevDirAndIncFileConfOfModule(char *nameOfModule,char *devDir,char *incFileConf);

/*
功能	
	从一个定义口串中读取一个模块定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadModuleDefFromStr(char *str,int lenOfStr,PUnionModuleDef pdef);

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintModuleDefToFp(PUnionModuleDef pdef,FILE *fp);

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
nameOfModulee	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintModuleDefInFileToFp(char *fileName,char *nameOfModule,FILE *fp);
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
nameOfModulee	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputModuleDefInFile(char *nameOfModule,char *fileName);
#endif

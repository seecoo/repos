//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionPointerDef_
#define _unionPointerDef_

#define conPointerDefTagVarName		"name"
#define conPointerDefTagNameOfType	"nameOfType"
#define conPointerDefTagRemark		"remark"

// 定义指针的定义
typedef struct
{
	char		name[64+1];		// 指针类型的名称
	char		nameOfType[64+1];	// 指针的类型
	char		remark[80+1];
} TUnionPointerDef;
typedef TUnionPointerDef	*PUnionPointerDef;

/* 从缺省文件读指定名称的指针的定义
输入参数
	nameOfType	指针类型的名称
输出参数
	typeNameOfPointer	指针类型的类型
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadTypeOfPointerDefFromDefaultDefFile(char *nameOfType,char *typeNameOfPointer);

/* 将指定指针的定义打印到文件中
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefToFp(PUnionPointerDef pdef,FILE *fp);

/* 将指定指针的定义打印到定义文件中
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefToFpInDefFormat(PUnionPointerDef pdef,FILE *fp);

/* 将指定文件中定义的指针打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintPointerDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的指针打印到屏幕上
输入参数
	pdef	指针定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputPointerDefInFile(char *fileName);

/* 将指定名称的指针类型定义输出到文件中
输入参数
	nameOfType	指针类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecPointerDefToFp(char *nameOfType,FILE *fp);
	
/* 将指定文件中定义的指针类型打印到屏幕上
输入参数
	nameOfType	指针类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecPointerDef(char *nameOfType);

#endif

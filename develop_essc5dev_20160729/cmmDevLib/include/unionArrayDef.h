//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionArrayDef_
#define _unionArrayDef_

#define conArrayDefTagVarName		"name"
#define conArrayDefTagNameOfType	"nameOfType"
#define conArrayDefTagDimisionNum	"dimisionNum"
#define conArrayDefTagSizeOfDimision	"sizeOfDimision"
#define conArrayDefTagRemark		"remark"


#ifndef conMaxNumOfArrayDimision
#define conMaxNumOfArrayDimision	4
#endif

// 定义数组的定义
typedef struct
{
	char		name[64+1];		// 数组类型的名称
	char		nameOfType[64+1];	// 数组的类型
	int		dimisionNum;		// 数组的维度，0表示不是数组
	char		sizeOfDimision[conMaxNumOfArrayDimision][64+1];	// 每一维度的大小
	char		remark[80+1];
} TUnionArrayDef;
typedef TUnionArrayDef	*PUnionArrayDef;

/* 将指定数组的定义打印到文件中
输入参数
	pdef	数组定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintArrayDefToFp(PUnionArrayDef pdef,FILE *fp);

/* 将指定文件中定义的数组打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintArrayDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的数组打印到屏幕上
输入参数
	pdef	数组定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputArrayDefInFile(char *fileName);

/* 将指定名称的数组定义输出到文件中
输入参数
	nameOfType	数组名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecArrayDefToFp(char *nameOfType,FILE *fp);
	
/* 将指定文件中定义的数组打印到屏幕上
输入参数
	nameOfType	数组名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecArrayDef(char *nameOfType);

#endif

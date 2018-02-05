//---------------------------------------------------------------------------

#ifndef unionFldGrpH
#define unionFldGrpH
//---------------------------------------------------------------------------

#define 	conMaxNumOfQueryFld		64
#define 	conMaxNumOfPrimaryKeyFld	8
typedef char	TUnionFldName[40+1];

// 定义一个查询域组
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfQueryFld];
	TUnionFldName	fldAlais[conMaxNumOfQueryFld];
} TUnionQueryFldGrp;
typedef TUnionQueryFldGrp *PUnionQueryFldGrp;

// 定义一个域组
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfQueryFld];
} TUnionFldGrp;
typedef TUnionFldGrp *PUnionFldGrp;

/*
功能	从文件中读取查询域定义
输入参数
	fileName	文件名
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadQueryFldGrpDef(char *fileName,PUnionQueryFldGrp prec);

/*
功能	向文件中写入查询域定义
输入参数
	prec		查询域定义
	fp		文件句柄
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintQueryFldGrpDefToFp(PUnionQueryFldGrp prec,FILE *fp);

/*
功能	从文件中读取域定义
输入参数
	fileName	文件名
	fldFlag		域标识
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadFldGrpDef(char *fileName,char *fldFlag,PUnionFldGrp prec);

//---------------------------------------------------------------------------
/*
功能	向文件中写入域定义
输入参数
	prec		查询域定义
	fldFlag		域标识
	fp		文件句柄
输出参数
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintFldGrpDefToFp(char *fldFlag,PUnionFldGrp prec,FILE *fp);

/*
功能	从文件中读取关键字域定义
输入参数
	fileName	文件名
输出参数
	prec		查询域定义
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionReadPrimaryKeyFldGrpDef(char *fileName,PUnionFldGrp prec);

//---------------------------------------------------------------------------
/*
功能	向文件中写入域定义
输入参数
	prec		查询域定义
	fp		文件句柄
输出参数
返回值
	>=0		查询域的数目
	<0		出错代码
*/
int UnionPrintPrimaryKeyFldGrpDefToFp(PUnionFldGrp prec,FILE *fp);

#endif

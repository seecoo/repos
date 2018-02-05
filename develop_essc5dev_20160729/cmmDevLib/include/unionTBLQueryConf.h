//---------------------------------------------------------------------------

#ifndef unionTBLQueryConfH
#define unionTBLQueryConfH
//---------------------------------------------------------------------------
#include "unionWorkingDir.h"
#include "unionFldGrp.h"

// 定义一个表客户端配置记录
typedef struct
{
	char			name[40+1];			// 表名称，表名称是唯一值，
	char			alais[40+1];			// 表别名，这个值用于显示在表界面的标题中
	int			id;				// 表标识，是表的唯一标识，这个值与平台定义的表的标识对应

	char			formIconFileName[40+1];		// 界面图标文件的名称
	char			formPopMenuDefFileName[40+1];	// 在界面空白处点击右键时，显示出的弹出菜单定义文件

	char			recIconFileName[40+1];		// 记录图标文件的名称
	char			recPopMenuDefFileName[40+1];	// 右键点击记录时，显示出的弹出菜单定义文件
	char			recDoubleClickMenuFileName[40+1];	// 双击记录菜单定义
	int			maxNumOfRecPerPage;		// 每页可以显示的记录的最大数量
	int			useSeparateTBL;			// 是否分页显示
	TUnionQueryFldGrp	queryFldGrp;			// 查询的记录域
	TUnionFldGrp		primaryKeyFldGrp;		// 关键字域组
} TUnionTBLQueryConf;
typedef TUnionTBLQueryConf	*PUnionTBLQueryConf;

/*
功能	获得表配置记录
输入参数
	tblName		表名
输出参数
	prec		表配置记录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionReadTBLQueryConf(char *tblName,PUnionTBLQueryConf prec);

/*
功能	将主表的配置写入到文件中
输入参数
	prec		配置记录
	fp		文件句柄
输出参数
	
返回值
	>=0		正确
	<0		出错代码
*/
int UnionPrintTBLQueryConfToFp(PUnionTBLQueryConf prec,FILE *fp);

#endif
 


#ifndef _UNIONTBLDATA_H_
#define _UNIONTBLDATA_H_

/* 
功能：
	从complexDB中导出数据
参数:
	[IN]:
		resName			: 对象名[表名]
	[OUT]:
		NULL
返回值:
	[0, +d)				: 成功, 导出数据记录的条数
	(-d, 0)				: 失败
*/
int UnionOutputTBLData(char *resName);

/* 
功能：
	从 tblFileName 文件中,将数据导入 resName 中
参数:
	[IN]:
		tblFileName		: 需要被导入数据的文件名,要求为全路径文件名
		resName			: 对象名[表名]
		isEnvData		: 是否为 env 数据
	[OUT]:
		NULL
返回值:
	[0, +d)				: 成功, 导入数据记录的条数
	(-d, 0)				: 失败
*/
int UnionInputTBLData(char *tblFileName, char *resName, int isEnvData);

#endif


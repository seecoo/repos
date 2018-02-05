//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionCProgramPhase_
#define _unionCProgramPhase_

/* 从指定文件读一行c语言的有效程序行
输入参数
	fp		文件句柄
	maxNumOfLine	可读出的最大行数
输出参数
	cprogramLine	程序行
	lineNum		读出的是文件的第几行
返回值：
	>=0 	读出的行数目，0表示文件结束了
	<0	出错代码	
	
*/
int UnionReadCProgramLineFromFile(FILE *fp,char cprogramLine[][1024+1],int maxNumOfLine,int *lineNum);

/* 去掉一个程序中的注释行
输入参数
	oriFileName	源文件名
	desFileName	目标文件名
输出参数
	无
返回值：
	>=0 	处理后的总行数
	<0	出错代码	
	
*/
int UnionFilterRemarkFromCProgramFile(char *oriFileName,char *desFileName);

#endif

#ifndef _unionFileDeal_
#define _unionFileDeal_

/*
 * 使用tar命令打包一个文件
 * 输入：packName - 包名称，fileName - 文件名称(带路径)
 * 输出：无
 * 返回：>=0,成功；　<0,失败
 */
int  UnionPackOneFileWithTarCmd(char *packName, char *fileName);

/*
 * 使用tar命令打包一个目录
 * 输入：packName - 包名称，dir - 目录名称
 * 输出：无
 * 返回：>=0,成功；　<0,失败
 */
int  UnionPackOneDirectoryWithTarCmd(char *packName, char *dir);

/*
 * 使用tar命令解包
 * 输入：packName - 包名称; dirName - 解包目录; maxNum - fileList数组的最大个数
 * 输出：fileList - 文件清单
 * 返回：>=0,成功；　<0,失败
 */
int  UnionUnPackWithTarCmd(char *packName, char *dirName, int maxNum, char fileList[][128+1]);

#endif


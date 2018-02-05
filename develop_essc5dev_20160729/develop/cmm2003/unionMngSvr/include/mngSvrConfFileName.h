// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#ifndef _mngSvrConfFileName_
#define _mngSvrConfFileName_

// 获得mngSvr客户端配置文件的目录
char *UnionGetMngSvrClientConfFileDir();

// 获得mngSvr客户端枚举类型定义文件名
char *UnionGetEnumConfFileNameByTableAlais(char *tableAlais);

// 获得mngSvr客户端视图定义文件名
char *UnionGetTBLQueryConfFileNameByTableAlais(char *tableAlais);

// 获得mngSvr客户端记录双击菜单文件名
char *UnionGetDoubleClickRecMenuFileNameByTableAlais(char *tableAlais);

// 获得mngSvr客户端记录右键菜单文件名
char *UnionGetRecPopupMenuFileNameByTableAlais(char *tableAlais);

// 获得mngSvr客户端界面右键菜单文件名
char *UnionGetFormPopupMenuFileNameByTableAlais(char *tableAlais);

// 获得mngSvr服务端表定义文件名
char *UnionGetComplexDBTBLDefFileNameByTableName(char *tableName);

// 获得mngSvr服务端数据库建表文件名
char *UnionGetDBTBLCreateSQLFileNameByTableName(char *tableName);

// 获得主菜单名称
char *UnionGetMngSvrClientMainMenuFileName();

// 获得二级菜单名称
char *UnionGetMngSvrClient2LevelMenuFileName(char *nameOf2LevelMenu);

#endif

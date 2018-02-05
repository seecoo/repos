//	Wolfgang Wang
//	2003/09/09

#ifndef _UnionCommand_
#define _UnionCommand_

int UnionConnectCommandMDL(char *fileName);
char *UnionInput(char *fmt,...);
int UnionConfirm(char *fmt,...);
int UnionCommandInterprotor(char *fileName);
int UnionIsQuit(char *p);
int UnionExcutePreDefinedCommand(int argc,char *argv[]);
// 第一个参数不转换为大写
int UnionExcutePreDefinedCommand2(int argc,char *argv[]);

char *UnionPressAnyKey(char *fmt,...);
char *UnionPressAny(char *fmt,...);

#endif

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
// ��һ��������ת��Ϊ��д
int UnionExcutePreDefinedCommand2(int argc,char *argv[]);

char *UnionPressAnyKey(char *fmt,...);
char *UnionPressAny(char *fmt,...);

#endif

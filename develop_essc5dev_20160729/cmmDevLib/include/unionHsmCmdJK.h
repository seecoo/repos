//      Author:         linxj
//      Copyright:      Union Tech. Guangzhou
//      Date:           2014-04-10

#ifndef _UnionHsmCmdJK_
#define _UnionHsmCmdJK_

#include "unionDesKey.h"

int UnionHsmCmdJKEE(char *alg, char *ikindex,char *flag, char *version,char *Goup_Index,char *M_number,char *Ls_Data1, char *Ls_Data2, char *Ls_Data3,char *yon,char *processkey,char *Key_Header_Len,char *Key_Header,char *Key_Tailer_Len,char *Key_Tailer,char *Mac_Header_Len,char *Mac_Header,char *date,char *mac);

int UnionHsmCmdJKF6(char *alg, char *ikindex,char *flag, char *version,char *Goup_Index,char *M_number,char *Ls_Data1, char *Ls_Data2, char *Key_Header_Len,char *Key_Header,char *Key_Tailer_Len,char *Key_Tailer,char *Mac_Header_Len,char *Mac_Header,char *date,char *mac);

int UnionHsmCmdJKF2(char *keyIndex, char *algFlag);


#endif

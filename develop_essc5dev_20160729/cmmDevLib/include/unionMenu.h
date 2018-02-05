//	Wolfgang Wang
//	2003/09/09

#ifndef _UnionMenu_
#define _UnionMenu_

typedef struct
{
	int	index;
	char	name[40+1];
	char	command[128+1];
} TUnionMenuItem;
typedef TUnionMenuItem	*PUnionMenuItem;
typedef struct
{
	char	title[80+1];
	TUnionMenuItem	item[20];
	int	num;
} TUnionMenu;
typedef TUnionMenu	*PUnionMenu;

int UnionDisconnectMenuMDL(PUnionMenu pmenu);
int UnionDisconnectMenuMDL(PUnionMenu pmenu);
int UnionDisplayMenu(PUnionMenu pmenu);
int UnionMenu(char *fileName);

int UnionGetNameOfUnionMenu(char *partName,char *fileName);


#endif

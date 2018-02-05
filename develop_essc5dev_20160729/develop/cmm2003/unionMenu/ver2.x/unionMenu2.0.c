//	Wolfgang Wang
//	2003/09/09

#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "UnionStr.h"
#include "unionMenu.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionLog.h"

int UnionGetNameOfUnionMenu(char *partName,char *fileName)
{
	sprintf(fileName,"%s/unionCmd/%s.MNU",getenv("UNIONETC"),partName);
	return(0);
}
	
PUnionMenu UnionConnectMenuMDL(char *fileName)
{
	int	ret;
	char	tmpBuf[256];
	char	*p;
	int	varNum;
	PUnionMenu	punionMenu=NULL;
	int	i;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetNameOfUnionMenu(fileName,tmpBuf);	
	
	if ((ret = UnionInitEnvi(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionConnectMenuMDL:: UnionInitEnvi [%s]!\n",tmpBuf);
		return(NULL);
	}
	
	if ((punionMenu = (PUnionMenu)malloc(sizeof(TUnionMenu))) == NULL)
	{
		UnionSystemErrLog("in UnionConnectMenuMDL:: malloc!\n");
		return(NULL);
	}
	
	memset(punionMenu->title,0,sizeof(punionMenu->title));
	if ((p = UnionGetEnviVarByName("Title")) == NULL)
		strcpy(punionMenu->title,"Main Menu::");
	else
	{
		if (strlen(p) > sizeof(punionMenu->title) - 1)
			memcpy(punionMenu->title,p,sizeof(punionMenu->title)-1);
		else
			strcpy(punionMenu->title,p);
	}
	
	for (punionMenu->num = 0,varNum = UnionGetEnviVarNum(),i = 0; (i < varNum) && (punionMenu->num < sizeof(punionMenu->item)); i++)
	{
		if ((p = UnionGetEnviVarNameByIndex(i)) == NULL)
		{
			UnionUserErrLog("in UnionConnectMenuMDL:: i = [%d] nullname!\n",i);
			continue;
		}
		if (strcmp(p,"Title") == 0)
			continue;
		memset(&(punionMenu->item[punionMenu->num]),0,sizeof(TUnionMenuItem));
		punionMenu->item[punionMenu->num].index = punionMenu->num;
		if (strlen(p) > sizeof(punionMenu->item[punionMenu->num].name) - 1)
			memcpy(punionMenu->item[punionMenu->num].name,p,sizeof(punionMenu->item[punionMenu->num])-1);
		else
			strcpy(punionMenu->item[punionMenu->num].name,p);
		if ((p = UnionGetEnviVarByIndex(i)) == NULL)
		{
			UnionUserErrLog("in UnionConnectMenuMDL:: i = [%d] nullcommand!\n",i);
			continue;
		}
		if (strlen(p) > sizeof(punionMenu->item[punionMenu->num].command) - 1)
			memcpy(punionMenu->item[punionMenu->num].command,p,sizeof(punionMenu->item[punionMenu->num])-1);
		else
			strcpy(punionMenu->item[punionMenu->num].command,p);
		//UnionNullLog("%03d %40s %s\n",punionMenu->num,punionMenu->item[punionMenu->num].name,punionMenu->item[punionMenu->num].command);
		punionMenu->num++;
	}
	
	UnionClearEnvi();
	return(punionMenu);
}

int UnionDisconnectMenuMDL(PUnionMenu pmenu)
{
	if (pmenu == NULL)
	{
		UnionUserErrLog("in UnionDisconnectMenuMDL:: null pointer!\n");
		return(errCodeParameter);
	}
	free(pmenu);
	return(0);
}

int UnionDisplayItemBlank(int len)
{
	int	i;
	for (i = 0; i < len; i++)
		printf("%c",' ');
	return(0);
}

int UnionDisplayDelimeterLine(int itemBlankLen,int len)
{
	int	i;
	
	/*UnionDisplayItemBlank(itemBlankLen);
	for (i = 0; i < len; i++)
		printf("-");
	*/
	printf("\n");
}

int UnionDisplayMenu(PUnionMenu pmenu)
{
	int	itemBlankLen = 0;
	int	i;
	int	j;
	
	if (pmenu == NULL)
	{
		UnionUserErrLog("in UnionDisplayMenu:: null pointer!\n");
		return(errCodeParameter);
	}
	if (strlen(pmenu->title) > sizeof(pmenu->item[0].name)-1)
		itemBlankLen = (80 - strlen(pmenu->title)) / 2;
	else
		itemBlankLen = (80 - sizeof(pmenu->item[0].name)-1)/2;
	
	system("clear");
	for (i = 0; i < (24 - pmenu->num - 1 - 1 - 1 - 1)/2; i++)
	{
		switch (i)
		{
			case	0:
				printf("广州科友科技股份有限公司  公共运行支撑平台 2003\n");
				break;
			default:
				printf("\n");
				break;
		}
	}
		
	UnionDisplayItemBlank(itemBlankLen);
	printf("%s\n",pmenu->title);
	UnionDisplayDelimeterLine(itemBlankLen,sizeof(pmenu->item[0].name));
	
	for (i = 0; i < pmenu->num; i++)
	{
		UnionDisplayItemBlank(itemBlankLen);
		if (strcmp(pmenu->item[i].name,"null") == 0)
			printf("\n");
		else
			printf("%03d  %s\n",i,pmenu->item[i].name);
	}
	UnionDisplayDelimeterLine(itemBlankLen,sizeof(pmenu->item[0].name));
	UnionDisplayItemBlank(itemBlankLen);
	printf("Choice(Exit/Quit to Exit)::");
	
	return(0);
}
	
int UnionMenu(char *menuFileName)
{
	int		ret;
	PUnionMenu	pmenu;
	char		choice[100];
	int		index;
	
	if ((pmenu = UnionConnectMenuMDL(menuFileName)) == NULL)
	{
		UnionUserErrLog("in UnionMenu:: UnionConnectMenuMDL [%s]\n",menuFileName);
		return(errCodeParameter);
	}
	
	for (;;)
	{
		UnionDisplayMenu(pmenu);
		memset(choice,0,sizeof(choice));
		scanf("%s",choice);
		system("clear");
		UnionToUpperCase(choice);
		if ((choice[0] == 'Q') || (choice[0] == 'E'))
			return(0);
		if ((strcmp(choice,"QUIT") == 0) || (strcmp(choice,"EXIT") == 0))
			return(0);
		if (!UnionIsDigitStr(choice))
		{
			system("clear");
			continue;
		}
		index = atoi(choice);
		if ((index < 0) || (index >= pmenu->num))
			continue;
		if (strcmp(pmenu->item[index].name,"null") != 0)
		{
			system(pmenu->item[index].command);
			printf("Press any key to continue...");
			getchar();
			getchar();
		}
	}
}	

//	Wolfgang Wang
//	2003/09/09

/*
	2003/09/20,Wolfgang Wang, ��unionCommand1.0�����ϣ���ԭ�����ຯ�����Ƶ��˱��ļ��У�ͬʱ��unionCommand1.0����Ϊ2.0
*/

// 2008/01/21,���������޸���UnionInput�������������ڹر���������ʱ�������봮��Ϊexit�Ĺ��ܡ�

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "UnionStr.h"
#include "unionCommand.h"

char	pgUnionInputStr[1024+1];

int UnionConfirm(char *fmt,...)
{
	va_list args;

	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
	printf("(Y/N)");
	memset(pgUnionInputStr,0,sizeof(pgUnionInputStr));
	scanf("%s",pgUnionInputStr);
	UnionToUpperCase(pgUnionInputStr);
	if (strncmp(pgUnionInputStr,"Y",1) == 0)
		return(1);
	else
		return(0);
}

int UnionIsQuit(char *p)
{
	if (p == NULL)
		return(0);
	if ((strcasecmp(p,"QUIT") == 0) || (strcasecmp(p,"EXIT") == 0))
		return(1);
	else
		return(0);
}

char *UnionInput(char *fmt,...)
{
	va_list args;
	int	i;
	
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
	for (i = 0; i < sizeof(pgUnionInputStr)-1;)
	{
		pgUnionInputStr[i] = getchar();
		// 2008/01/21������
		//if ((pgUnionInputStr[i] == 0xff) || (pgUnionInputStr[i] == 0xffffffff))
		if (pgUnionInputStr[i] == 0xffffffff)
		{
			strcpy(pgUnionInputStr,"exit");
			return(pgUnionInputStr);
		}
		// 2008/01/21������
		if ((pgUnionInputStr[i] == 10) || (pgUnionInputStr[i] == 13) || (pgUnionInputStr[i] == '\n'))
		{
			if (i == 0)
				continue;
			else
				break;
		}
		else
			i++;
	}
	//scanf("%s",pgUnionInputStr);
	pgUnionInputStr[i] = 0;
	return(pgUnionInputStr);
}

char *UnionPressAnyKey(char *fmt,...)
{
	va_list args;

	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
	memset(pgUnionInputStr,0,sizeof(pgUnionInputStr));
	
	pgUnionInputStr[0] = toupper(getchar());
	if ((pgUnionInputStr[0] != 'E') && (pgUnionInputStr[0] != 'Q'))
		return(pgUnionInputStr);
	
	if (pgUnionInputStr[0] == 'E')
	{
		if ((pgUnionInputStr[1] = toupper(getchar()))!= 'X')
			return(pgUnionInputStr);
		if ((pgUnionInputStr[2] = toupper(getchar()))!= 'I')
			return(pgUnionInputStr);
		if ((pgUnionInputStr[3] = toupper(getchar()))!= 'T')
			return(pgUnionInputStr);
		return(pgUnionInputStr);
	}
	else
	{
		if ((pgUnionInputStr[1] = toupper(getchar()))!= 'U')
			return(pgUnionInputStr);
		if ((pgUnionInputStr[2] = toupper(getchar()))!= 'I')
			return(pgUnionInputStr);
		if ((pgUnionInputStr[3] = toupper(getchar()))!= 'T')
			return(pgUnionInputStr);
		return(pgUnionInputStr);
	}
}

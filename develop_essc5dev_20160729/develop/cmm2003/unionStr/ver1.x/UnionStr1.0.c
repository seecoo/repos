// Wolfgang Wang
// 2002/08/26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <iostream>
#endif

#include "UnionStr.h"

int UnionToUpperCase(char *str)
{
	int	i = 0;
	
	while (str[i])
	{
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] = toupper(str[i]);
		i++;
	}
	return(0);
}

int UnionFilterRightBlank(char *Str)
{
	int	i;

	for (i = strlen(Str); i > 0; i--)
	{
		if (Str[i-1] != ' ')
			return(i);
		else
			Str[i-1] = 0;
	}
	return(0);
}

int UnionFilterLeftBlank(char *Str)
{
	int	i;
	int	Len;
	int	j;

	for (i = 0,Len = strlen(Str); i < Len; i++)
	{
		if (Str[i] != ' ')
			break;
	}
	if (i == 0)
		return(Len);

	for (j = 0; j < Len - i; j++)
	{
		Str[j] = Str[i + j];
	}

	Str[Len - i] = 0;

	return(Len - i);
}

int UnionVerifyDateStr(char *DateStr)
{
	int	Year;
	int	Month;
	int	Day;

	char	TmpBuf[5];

	if (DateStr == NULL)
		return(0);

	if (strlen(DateStr) != 8)
		return(0);

	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr,4);
	Year = atoi(TmpBuf);
	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr+4,2);
	Month = atoi(TmpBuf);
	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr+6,2);
	Day = atoi(TmpBuf);
	
	switch (Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if ((Day >= 1) && (Day <= 31))
				return(1);
			else
				return(0);
		case 4:
		case 6:
		case 9:
		case 11:
			if ((Day >= 1) && (Day <= 30))
				return(1);
			else
				return(0);
		case 2:
			if ((Year % 4 == 0) && (Year % 100 != 0))
			{
				if ((Day >= 1) && (Day <= 29))
					return(1);
				else
					return(0);
			}
			else
			{
				if ((Day >= 1) && (Day <= 28))
					return(1);
				else
					return(0);
			}
		default:
			return(0);
	}
}

int UnionCopyFilterHeadAndTailBlank(char *str1,char *str2)
{
	strcpy(str2,str1);
	UnionFilterLeftBlank(str2);
	UnionFilterRightBlank(str2);
	return(0);
}

int UnionIsValidIPAddrStr(char *ipAddr)
{
	int 	i,j,k;
	char	tmpBuf[4];

	if (strlen(ipAddr) > 15)
		return(0);

	for (i = -1,k = 0; k < 4; k++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		for (++i,j = 0; (j < 3) && (i < strlen(ipAddr)); i++,j++)
		{
			if (ipAddr[i] == '.') // 遇到一个.,一节结束
				break;
			else
			{
				if (!isdigit(ipAddr[i])) // 包含非数字
					return(0);
				else
					tmpBuf[j] = ipAddr[i];
			}
		}
		if (j == 0) // 连续2个.间无数字
			return(0);
		if ((atoi(tmpBuf) > 255) || (atoi(tmpBuf) < 0))
			return(0);	// 大于255或小于0
		if (k == 3) // 最后一节结束
			break;
		if (ipAddr[i] != '.') // 一节的结束符不是.
			return(0);
	}
	if (strlen(ipAddr) != i)
		return(0);	// 四节结束后，还有字符
	else
		return(1);
}

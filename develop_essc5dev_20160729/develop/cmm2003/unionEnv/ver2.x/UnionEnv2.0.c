// 2003/09/12, Wolfgang Wang
// 由1.2升级为2.0，2.0与以前版本区别在于，支持汉字做为变量名

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "UnionEnv.h"

PUnionEnviVariable	pUnionEnvi[MAXUNIONENVIVARIABLES];
int			UnionEnviVarNum = 0;

int UnionGetEnviVarNum()
{
	return(UnionEnviVarNum);
}

char *UnionGetEnviVarByIndex(int index)
{
	if (index >= UnionEnviVarNum)
		return(NULL);
		
	if (index > MAXUNIONENVIVARIABLES)
		return(NULL);
	
	if (pUnionEnvi[index] == NULL)
		return(NULL);
		
	return(pUnionEnvi[index]->pVariableValue);
}
	
char *UnionGetEnviVarNameByIndex(int index)
{
	if (index >= UnionEnviVarNum)
		return(NULL);
		
	if (index > MAXUNIONENVIVARIABLES)
		return(NULL);
	
	if (pUnionEnvi[index] == NULL)
		return(NULL);
		
	return(pUnionEnvi[index]->pVariableName);
}
	
char *UnionGetEnviVarByName(char *varname)
{
	int i;
	int j;
	char buf1[128],buf2[128];
	
	if (strlen(varname) >= 128)
		return(NULL);
	
	for (i=0;i<strlen(varname);i++)
	{
		buf1[i] = toupper(varname[i]);
	}
	buf1[i] = 0;
	
	for (i=0;i<UnionEnviVarNum;i++)
	{
		if (pUnionEnvi[i] == NULL)
			continue;
		
		if (pUnionEnvi[i]->pVariableName == NULL)
			continue;
	
		if (strlen(varname) != strlen(pUnionEnvi[i]->pVariableName))
			continue;
		
		for (j = 0;j<strlen(pUnionEnvi[i]->pVariableName);j++)
			buf2[j] = toupper(pUnionEnvi[i]->pVariableName[j]);
		buf2[j] = 0;
			
		if (strcmp(buf1,buf2) == 0)
			return(pUnionEnvi[i]->pVariableValue);
	}
	
	return(NULL);
}
		
// Modified by Wolfgang Wang, 2003/09/12
PUnionEnviVariable ReadUnionEnviVarFromStr(char *str)
{
	int 	i;
	int	j;
	char	varname		[512];
	char	varvalue	[512];
	PUnionEnviVariable pvar;
	int	len;
	
	if (str == NULL)
		return(NULL);
		
	len = strlen(str);
	
	for (i=0;i<len;i++)
	{
		if (str[i] == '[')
			break;
		if (str[i] == '\0')
			return(NULL);
	}
	if (i == len)
		return(NULL);		
		
	for (++i,j=0;i < len;i++,j++)
	{
		if (str[i] == '\0')
			return(NULL);
			
		if (str[i] == ']')
			break;
		
		varname[j] = str[i];
	}
	if (i == len)
		return(NULL);
	varname[j] = '\0';

	for (++i;i<len;i++)
	{
		if (str[i] == '[')
			break;	
		if (str[i] == '\0')
			return(NULL);
	}
	if (i == len)
		return(NULL);
	
	for (++i,j=0;i<len;i++,j++)
	{
		if (str[i] == '\0')
			return(NULL);
		if (str[i] == ']')
			break;
		varvalue[j] = str[i];
	}
	if (i == len)
		return(NULL);
	varvalue[j] = '\0';
	
	pvar = (PUnionEnviVariable)malloc(sizeof(TUnionEnviVariable));
	if (pvar == NULL)
		return(NULL);
	
	if ((pvar->pVariableName = (char *)malloc(strlen(varname)+1)) == NULL)
	{
		free(pvar);
		return(NULL);
	}
	
	if ((pvar->pVariableValue = (char *)malloc(strlen(varvalue)+1)) == NULL)
	{
		free(pvar->pVariableName);
		free(pvar);
		return(NULL);
	}
	
	strcpy(pvar->pVariableName,varname);
	strcpy(pvar->pVariableValue,varvalue);
	
	return(pvar);
}

int UnionReadOneFileLine(FILE *fp,char *buf)
{
	char *p;
	char c;
	int  num;
	
	p = buf;
	num = 0;
	while (!feof(fp))
	{
		fscanf(fp,"%c",&c);
		if (c == '\n')
		{
			*p = 0;
			return(num);
		}
		*p = c;
		p++;
		num++;
	}
	
	*p = 0;
	return(num);
}

int UnionInitEnvi(char *filename)
{
	int 	i;
	char	buf[512];
	FILE	*fp;
	
	for (i=0;i<MAXUNIONENVIVARIABLES;i++)
	{
		//if (pUnionEnvi[i] != NULL)
		//	free(pUnionEnvi[i]);
		pUnionEnvi[i] = NULL;
	}
	
	if ( (fp = fopen(filename,"r")) == NULL)
	{
		return(-1);
	}
	
	UnionEnviVarNum = 0;	
	for (;;)
	{
		memset(buf,0,512);
		if (feof(fp))
			break;
		if (UnionReadOneFileLine(fp,buf) < 0)
			break;
		pUnionEnvi[UnionEnviVarNum] = ReadUnionEnviVarFromStr(buf);
		if (pUnionEnvi[UnionEnviVarNum] == NULL)
			continue;
		
		UnionEnviVarNum++;
	}
		
	fclose(fp);
	
	return(0);
}

int UnionClearEnvi()
{
        int i;

        for (i=0;i<UnionEnviVarNum;i++)
        {
                if (pUnionEnvi[i] == NULL)
                        continue;
                if (pUnionEnvi[i]->pVariableName)
                {
                        free(pUnionEnvi[i]->pVariableName);
                }
                if (pUnionEnvi[i]->pVariableValue)
                {
                        free(pUnionEnvi[i]->pVariableValue);
                }
                free(pUnionEnvi[i]);
        }
        return(0);
}

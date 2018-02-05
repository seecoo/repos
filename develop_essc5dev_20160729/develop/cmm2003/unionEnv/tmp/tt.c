#include <stdio.h>
#include <string.h>

#define conMaxFldNumOfEnviRec	10
typedef int	TUnionEnviRecFldLength[conMaxFldNumOfEnviRec];

int UnionIsRecFormatDefLine(char *str)
{
	char	*ptr;
	
	//if (UnionIsRemarkFileLine(str))
	//	return(0);
	
	if ((ptr = strstr(str,"RECFORMAT=")) == NULL)
		return(0);
	else
		return(1);
}

int UnionReadEnviRecFldLengthFromStr(char *str,TUnionEnviRecFldLength fldLenDef)
{
	int	index;
	char	*varStart;
	char	*varEnd;
	int	strLen;
	
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
		fldLenDef[index] = -1;
		
	if (!UnionIsRecFormatDefLine(str))
		return(-1);

	if ((varStart = strstr(str,"RECFORMAT=")) == NULL)
		return(-1);
	varStart += 10;
	strLen = strlen(str);
	for (index = 0;index < conMaxFldNumOfEnviRec;index++)
	{
		if ((varEnd = strstr(varStart,";")) == NULL)
			break;
		*varEnd = 0;		
		fldLenDef[index] = atoi(varStart);
		*varEnd = ';';
		if (strLen + str <= varEnd)
			break;
		varStart = varEnd + 1;
	}
	return(0);
}

int UnionConvertEnviRecStrIntoFormat(TUnionEnviRecFldLength fldLenDef,char *recStr,int sizeOfRecStr)
{
	int	index;
	char	tmpBuf[512+1];
	int	strLen;
	char	*varStart;
	char	*varEnd;
	int	fldLen;
	int	offset = 0;
	char	tmpChar;
	
	if ((strLen = strlen(recStr)) >= sizeof(tmpBuf))
	{
		printf("Error::in UnionConvertEnviRecStrIntoFormat:: recStr [%s] too long!\n",recStr);
		return(-1);
	}
	strcpy(tmpBuf,recStr);
	varEnd = tmpBuf;
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
	{
		if ((varStart = strstr(varEnd,"[")) == NULL)
		{
			strcpy(recStr+offset,varEnd);
			offset += strlen(varEnd);
			printf("%04d %04d %s\n",index,offset,recStr);
			break;
		}
		if ((varEnd = strstr(varStart,"]")) == NULL)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			printf("%04d %04d %s\n",index,offset,recStr);
			break;
		}
		varEnd = varEnd+1;
		if (tmpBuf+strLen <= varEnd)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			printf("%04d %04d %s\n",index,offset,recStr);
			break;
		}
		tmpChar = *varEnd;
		*varEnd = 0;
		fldLen = strlen(varStart);
		if (offset + fldLen >= sizeOfRecStr)
		{
			printf("Error::in UnionConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLen);
			return(-1);
		}
		strcpy(recStr+offset,varStart);
		if (fldLen < fldLenDef[index])
		{
			if (offset + fldLenDef[index] >= sizeOfRecStr)
			{
				printf("Error::in UnionConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLenDef[index]);
				return(-1);
			}
			memset(recStr+offset+fldLen,' ',fldLenDef[index]-fldLen);
			fldLen = fldLenDef[index];
			recStr[offset+fldLen] = 0;
		}
		offset += fldLen;
		printf("%04d %04d %s\n",index,offset,recStr);
		*varEnd = tmpChar;
	}
	return(offset);
}	

void main()
{
	char			recStr[512+1] = "[hello]	[baby]	[44]	[loveyou]";
	char			format[] = "RECFORMAT=20;30;8;";
	TUnionEnviRecFldLength	fldLen;
	int			len;
	int			index;
	
	if (!UnionIsRecFormatDefLine(format))
	{
		printf("Not format line!\n");
		return;
	}
	if (UnionReadEnviRecFldLengthFromStr(format,fldLen) < 0)
		return;
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
	{
		printf("%02d %04d\n",index,fldLen[index]);
	}
	if ((len = UnionConvertEnviRecStrIntoFormat(fldLen,recStr,sizeof(recStr))) < 0)
	{
		printf("UnionConvertEnviRecStrIntoFormat failure!\n");
		return;
	}
	printf("[%04d] [%04d] [%s]\n",len,strlen(recStr),recStr);
	return;
}

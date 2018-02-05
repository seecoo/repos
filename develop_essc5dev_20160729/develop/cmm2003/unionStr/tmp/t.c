#include <stdio.h>
#include <string.h>

main(int argc,char *argv[])
{
	char	tmpBuf[100];
	int	len;
	int	i;

	for (i = 1; i < argc; i++)
	{
		strcpy(tmpBuf,argv[i]);
		len = UnionConvertLongStrIntoDoubleOf2TailStr(tmpBuf);
		printf("ori = [%s] len = [%d] des = [%s]\n",argv[i],len,tmpBuf);
	}
}

int UnionConvertLongStrIntoDoubleOf2TailStr(char *str)
{
	char	tmpBuf[100];
	char	tmpBuf2[100];
	int	len;
	int	offset = 0;
	int	retOffset = 0;
	int	i;
	
	//sprintf(tmpBuf,"%ld",atol(str));
	strcpy(tmpBuf2,str);
	if ((len = strlen(tmpBuf2)) > 0)
	{
		switch (tmpBuf2[0])
		{
			case	'-':
				offset = 1;
				retOffset = 1;
				str[0] = '-';
				break;
			case	'+':
				offset = 1;
				break;
			default:
				break;
		}
	}
	for (i = offset; i < len; i++)
	{
		if (tmpBuf2[i] == '0')
			continue;
		else
			break;
	}
	memcpy(tmpBuf,tmpBuf2+i,len-i);	
	tmpBuf[len-i] = 0;
	switch (len = strlen(tmpBuf))
	{
		case	0:
			strcpy(str+retOffset,"0.00");
			break;
		case	1:
			strcpy(str+retOffset,"0.0");
			retOffset += 3;
			strcpy(str+retOffset,tmpBuf);
			break;
		case	2:
			strcpy(str+retOffset,"0.");
			retOffset += 2;
			strcpy(str+retOffset,tmpBuf);
			break;
		default:
			memcpy(str+retOffset,tmpBuf,len-2);
			retOffset += len - 2;
			memcpy(str+retOffset,".",1);
			retOffset += 1;
			memcpy(str+retOffset,tmpBuf+len-2,2);
			retOffset += 2;
			str[retOffset] = 0;
			break;
	}
	return(strlen(str));
}


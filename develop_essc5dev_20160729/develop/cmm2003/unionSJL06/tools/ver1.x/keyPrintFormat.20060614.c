// Author:	Wolfgang Wang
// Date:	2006/06/14

#include <stdio.h>
#include <string.h>
#include "UnionLog.h"

char	pgPrintFormat[1024+1];
//	= ">L>L>L>L>070^3>L>L>001^4>L>L>001^5>L>L>001^6>060密钥分量明文：>L>L>001密钥名称：>011^0>060^P>L>L>001分量序号：>011^1>L>L>001分量数量：>011^2>L>L>001^7>L>001^8>L>001^9>L>L";
//	= ">L>001密钥名称：>011^0>L>001分量序号：>011^1>L>001分量数量：>011^2>L>001生成日期：>011^3>L>001密钥明文：>011^P>L>L>L>003^4>L>003^5>L>003^6>L>003^7>L>003^8>L>003^9>L>L";

char	pgPrintFormatForCheckValue[1024+1];
//	= ">L>L>L>L>L>L>001^0>L>L>001^1>L>L>001^2>L>L>001^3>L>L>001^4>L>L>001^5>L>L>001^6>L>L>001^7>L>L>001^8>L>L>001^9>L>L>L"; 
	//003^10>L>003^11>L>003^12>L>003^13>L>L";

int UnionSetKeyPrintFormat()
{
	char	tmpBuf[1024+1];
	FILE	*fp;

	strcpy(pgPrintFormat,">L>L>L>L>070^3>L>L>001^4>L>L>001^5>L>L>001^6>060密钥分量明文：>L>L>001密钥名称：>011^0>060^P>L>L>001分量序号：>011^1>L>L>001分量数量：>011^2>L>L>001^7>L>001^8>L>001^9>L>L");
	strcpy(pgPrintFormatForCheckValue,">L>L>L>L>L>L>001^0>L>L>001^1>L>L>001^2>L>L>001^3>L>L>001^4>L>L>001^5>L>L>001^6>L>L>001^7>L>L>001^8>L>L>001^9>L>L>L");

	sprintf(tmpBuf,"%s/keyPrintFormat.conf",getenv("UNIONETC"));
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionLog("in UnionSetKeyPrintFormat:: no conf file [%s]! default format used!\n",tmpBuf);
		goto exitNow;
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp,tmpBuf) <= 0)
			continue;
		if (strncmp(tmpBuf,"keyPrintFormat=",15) == 0)
		{
			strcpy(pgPrintFormat,tmpBuf+15);
		}
		if (strncmp(tmpBuf,"checkValuePrintFormat=",22) == 0)
		{
			strcpy(pgPrintFormatForCheckValue,tmpBuf+22);
		}
	}
	fclose(fp);
exitNow:
	UnionLog("in UnionSetKeyPrintFormat:: pgPrintFormat = [%s]\n",pgPrintFormat);
	UnionLog("in UnionSetKeyPrintFormat:: pgPrintFormatForCheckValue = [%s]\n",pgPrintFormatForCheckValue);
	return(0);
}

#include <stdio.h>
#include <string.h>
#include "UnionSHA1.h"

int main(int argc, char **argv)
{
	char	unit[1024] = "<?xml version=\"1.0\" encoding = \"utf-8\"?><SignData><AlertInfo name=\"交易进行中，下面是本次交易信息\">Trading</AlertInfo><TradeType name=\"银企对账签约\">corBankBalCheckSign</TradeType><Fields><PayerAcNo name=\"转出账户：\">6222030200000384</PayerAcNo><PayeeAcNo name=\"转入账户：\">6225 8855 1784 3413</PayeeAcNo><Amount name=\"金额：\">888.50</Amount><TranDate name=\"时间：\">2015-11-17</TranDate><JnlNo>1447743723840</JnlNo></Fields><ActionInfo name=\"请按OK键确认，或按C键取消\">Action</ActionInfo></SignData>";
	
	//char	unit[1024] = "<?xml version=\"1.0\" encoding = \"utf-8\"?><SignData><AlertInfo name=\"交易进行中，下面是本次交易信息\">Trading</AlertInfo><TradeType name=\"银企对账签约\">corBankBalCheckSign</TradeType><Fields><opinion name=\"意见：\">同意</opinion><TranDate name=\"时间：\">2015-11-18</TranDate><JnlNo>1447850346397</JnlNo></Fields><ActionInfo name=\"请按OK键确认，或按C键取消\">Action</ActionInfo></SignData>";
	
	char	result[100];
	//int	len = 0;
	char	tmpBuf[1024];
	
	char	ascData[8192];
	char	bcdData[8192];
	int	len = 0;
	
	len = tmp_bcdhex_to_aschex(unit, strlen(unit), ascData);
	
	printf("ascData: %s\n", ascData);
	
	tmp_aschex_to_bcdhex(ascData, len, bcdData);
	
	printf("bcdData: %s\n", bcdData);
	
	UnionSHA1((unsigned char *)unit,strlen(unit),(unsigned char *)result);
	
	tmp_bcdhex_to_aschex(result, 20, tmpBuf);
	printf("SHA1 result:[%s]\n",tmpBuf);
	return 0;
}

char hexlowtoasc1(int xxc)
{
        xxc&=0x0f;
        if (xxc<0x0a) xxc+='0';
        else xxc+=0x37;
        return (char)xxc;
}

char hexhightoasc1(int xxc)
{
        xxc&=0xf0;
        xxc = xxc>>4;
        if (xxc<0x0a) xxc+='0';
        else xxc+=0x37;
        return (char)xxc;
}

char asctohex1(char ch1,char ch2)
{
        char ch;
        if (ch1>='A') ch=(char )((ch1-0x37)<<4);
        else ch=(char)((ch1-'0')<<4);
        if (ch2>='A') ch|=ch2-0x37;
        else ch|=ch2-'0';
        return ch;
}

int tmp_aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
        int i,j;

        if (len % 2 == 0)
                j = len / 2;
        else
                j = len / 2 + 1;

        for (i = 0; i < j; i++)
                bcdhex[i] = asctohex1(aschex[2*i],aschex[2*i+1]);

        return(j);
}

int tmp_bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
        int i;

        for (i=0;i<len;i++)
        {
                aschex[2*i]   = hexhightoasc1(bcdhex[i]);
                aschex[2*i+1] = hexlowtoasc1(bcdhex[i]);
        }

        return(len*2);
}

#include <stdio.h>
#include <string.h>
#include "UnionSHA256.h"
#include "UnionStr.h"
int main(int argc, char **argv)
{
	//char	unit[1024] = "<?xml version=\"1.0\" encoding = \"utf-8\"?><SignData><AlertInfo name=\"���׽����У������Ǳ��ν�����Ϣ\">Trading</AlertInfo><TradeType name=\"�������ǩԼ\">corBankBalCheckSign</TradeType><Fields><PayerAcNo name=\"ת���˻���\">6222030200000384</PayerAcNo><PayeeAcNo name=\"ת���˻���\">6225 8855 1784 3413</PayeeAcNo><Amount name=\"��\">888.50</Amount><TranDate name=\"ʱ�䣺\">2015-11-17</TranDate><JnlNo>1447743723840</JnlNo></Fields><ActionInfo name=\"�밴OK��ȷ�ϣ���C��ȡ��\">Action</ActionInfo></SignData>";
	
	char	unit[1024] = "<?xml version=\"1.0\" encoding = \"utf-8\"?><SignData><AlertInfo name=\"���׽����У������Ǳ��ν�����Ϣ\">Trading</AlertInfo><TradeType name=\"�������ǩԼ\">corBankBalCheckSign</TradeType><Fields><opinion name=\"�����\">ͬ��</opinion><TranDate name=\"ʱ�䣺\">2015-11-18</TranDate><JnlNo>1447850346397</JnlNo></Fields><ActionInfo name=\"�밴OK��ȷ�ϣ���C��ȡ��\">Action</ActionInfo></SignData>";
	
	char	result[100] = {0};
	//int	len = 0;
	char	tmpBuf[1024];
	
	//UnionSHA1((unsigned char *)unit,strlen(unit),(unsigned char *)result);
	UnionSHA256(unit, strlen(unit), (unsigned char*)result);	

	//bcdhex_to_aschex(result, 64, tmpBuf);
	printf("SHA256 result:[%s]\n",result);
	return 0;
}

char hexlowtoasc(int xxc)
{
        xxc&=0x0f;
        if (xxc<0x0a) xxc+='0';
        else xxc+=0x37;
        return (char)xxc;
}

char hexhightoasc(int xxc)
{
        xxc&=0xf0;
        xxc = xxc>>4;
        if (xxc<0x0a) xxc+='0';
        else xxc+=0x37;
        return (char)xxc;
}

char asctohex(char ch1,char ch2)
{
        char ch;
        if (ch1>='A') ch=(char )((ch1-0x37)<<4);
        else ch=(char)((ch1-'0')<<4);
        if (ch2>='A') ch|=ch2-0x37;
        else ch|=ch2-'0';
        return ch;
}

int aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
        int i,j;

        if (len % 2 == 0)
                j = len / 2;
        else
                j = len / 2 + 1;

        for (i = 0; i < j; i++)
                bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);

        return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
        int i;

        for (i=0;i<len;i++)
        {
                aschex[2*i]   = hexhightoasc(bcdhex[i]);
                aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
        }

        return(len*2);
}

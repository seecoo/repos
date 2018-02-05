#include <stdio.h>
#include <string.h>
#include "UnionSHA1.h"
#include "UnionStr.h"
int main()
{
	char	unit[1024]="74826847len2315482uyfgw37r432rgy74826847len2315482uyfgw37r432rgy328783568734593532457834543344553222";
	char	data[1001];
	char	result[100];
	int	i,len;
	char	tmpBuf[1024];

	memset(data,0,sizeof(data));
	memset(tmpBuf, 0, sizeof(tmpBuf));
	len=0;
	for (i=0;i<3;i++)
	{
		memcpy(&data[len],unit,100);
		len += 100;
	}

	memset(unit, 0, sizeof unit);
	//strcpy(unit, "03016217BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB5F24032011305A0862175501900003645F3401009F0702FF008E0C000000000000000002031F009F0D05D86004A8009F0E0500109800009F0F05D86804F8005F280201567C00");
	strcpy(unit,"123456");
	aschex_to_bcdhex(unit, strlen(unit), tmpBuf);
	memset(result,0,sizeof(result));
	//UnionSHA1((unsigned char *)tmpBuf,strlen(unit)/2,(unsigned char *)result);
	UnionSHA1((unsigned char *)unit,strlen(unit),(unsigned char *)result);
	//printf("data = [%s] length = [%d]\n",data,len);
	printf("data length = [%d]\n",strlen(unit));
	memset(tmpBuf, 0, sizeof tmpBuf);
	bcdhex_to_aschex(result, 20, tmpBuf);
	printf("SHA1 result:[%s]\n",tmpBuf);
	/*
	for (i=0;i<30;i++)
	{
		printf("%02X ",result[i]);
		if ((i+1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
	*/
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

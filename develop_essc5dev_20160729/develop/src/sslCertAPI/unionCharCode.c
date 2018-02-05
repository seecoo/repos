#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32 
#include <windows.h>

// 将UTF8格式字串，转为ANSI
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen,unsigned char *ansiBuf,int sizeofAnsiBuf)
{
	int wcsLen;
	wchar_t *wszString;
	int ansiLen;
	char* szAnsi;

	wcsLen = MultiByteToWideChar(CP_UTF8, NULL, (char *)utf8Buf, utf8BufLen, NULL, 0);
    //分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
    wszString = (wchar_t *)malloc((wcsLen + 1) * sizeof(wchar_t));
    //转换
    MultiByteToWideChar(CP_UTF8, NULL, (char *)utf8Buf, utf8BufLen, wszString, wcsLen);
    //最后加上'\0'
    wszString[wcsLen] = '\0';
	ansiLen = WideCharToMultiByte(CP_ACP, NULL, wszString, wcsLen, NULL, 0, NULL, NULL);
    //同上，分配空间要给'\0'留个空间
    szAnsi =  (char *)malloc(ansiLen + 1);
    //转换
    //unicode版对应的strlen是wcslen
    WideCharToMultiByte(CP_ACP, NULL, wszString, wcsLen, szAnsi, ansiLen, NULL, NULL);
	free(wszString);
	if (ansiLen > sizeofAnsiBuf-1)
	{
		printf("sizeofAnsiBuf too small.\n");
		free(szAnsi);
		return -1;
	}
	memcpy(ansiBuf,szAnsi,ansiLen);
	ansiBuf[ansiLen] = '\0';
	free(szAnsi);
	return ansiLen;
}

//将ANSI字符串，转为UTF-8格式
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf)
{
    // ansi to unicode
    int wcsLen;
	wchar_t *wszString;
	char *szU8;
	int u8Len;
    //预转换，得到所需空间的大小
    wcsLen = MultiByteToWideChar(CP_ACP, NULL, (char *)ansiBuf, ansiBufLen, NULL, 0);
    //分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
    wszString = (wchar_t *)malloc((wcsLen+1)*sizeof(wchar_t));
	memset(wszString,0,(wcsLen+1)*sizeof(wchar_t));
    //转换
    MultiByteToWideChar(CP_ACP, NULL, (char *)ansiBuf, ansiBufLen, wszString, wcsLen);
    //最后加上'\0'
    wszString[wcsLen] = '\0';

	u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, wcsLen, NULL, 0, NULL, NULL);
    //同上，分配空间要给'\0'留个空间
    //UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存
    szU8 = (char *)malloc(u8Len + 1);
    //转换
    //unicode版对应的strlen是wcslen
    WideCharToMultiByte(CP_UTF8, NULL, wszString, wcsLen, szU8, u8Len, NULL, NULL);
    //最后加上'\0'
    szU8[u8Len] = '\0';

	free(wszString);
	if (u8Len > sizeofutf8Buf-1)
	{
		printf("sizeofutf8Buf too small.\n");
		free(szU8);
		return -1;
	}
	memcpy(utf8Buf,szU8,u8Len);
	utf8Buf[u8Len] = '\0';
	free(szU8);  
	return u8Len;
}
#else
/* 在linux下和AIX下通过 */
/* AIX编译要带库-liconv */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <stdlib.h>

int UnionCodeConvert(char *from_charset,char *to_charset,char *inbuf,int len_of_inbuf,char *outbuf,int len_of_outbuf)
{
        iconv_t cd;            /* iconv型（其实就是个整型变量） */
        char **pin=&inbuf;     /* 从*inbuf开始读取，最多读取*inbufleft字节-这句将形参的值传给实参 */
        char **pout=&outbuf;   /* 转化后从*outbuf开始写入，最多*outbufleft字节--这句将形参的值传给实参 */
 
	size_t lenin = len_of_inbuf;	
	size_t lenout = len_of_outbuf;
 				
        cd=iconv_open(to_charset,from_charset);   /* gb18030在嵌入式linux系统上是否被支持有待验证，保险期间可换为"gbk" or "gb2312" */
 
        if( cd == 0 )
	{
                printf("iconv_open() error!\n");
                return -1;
	}
 
        memset(outbuf,'\0',len_of_outbuf);          /* 这里清零是否会影响到其他线程在outbuf的数据呢 */
 
        if(iconv(cd,pin,(size_t *)&lenin,pout,(size_t *)&lenout) == -1)   /* 转换 */
	{
                printf("iconv() error!\n");
                iconv_close(cd);
                return  -1;
	}
 
        iconv_close(cd);
        return len_of_outbuf-lenout;
} 


// 将UTF-8转为ANSI
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen,unsigned char *ansiBuf,int sizeofAnsiBuf)
{
	return(UnionCodeConvert("UTF-8","GB18030",(char *)utf8Buf,utf8BufLen,(char *)ansiBuf,sizeofAnsiBuf));
}

//将ANSI转为UTF-8
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf)
{
	return UnionCodeConvert("GB18030","UTF-8",(char *)ansiBuf,ansiBufLen,(char *)utf8Buf,sizeofutf8Buf);
}

#endif


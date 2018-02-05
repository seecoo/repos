#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32 
#include <windows.h>

// ��UTF8��ʽ�ִ���תΪANSI
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen,unsigned char *ansiBuf,int sizeofAnsiBuf)
{
	int wcsLen;
	wchar_t *wszString;
	int ansiLen;
	char* szAnsi;

	wcsLen = MultiByteToWideChar(CP_UTF8, NULL, (char *)utf8Buf, utf8BufLen, NULL, 0);
    //����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ�
    wszString = (wchar_t *)malloc((wcsLen + 1) * sizeof(wchar_t));
    //ת��
    MultiByteToWideChar(CP_UTF8, NULL, (char *)utf8Buf, utf8BufLen, wszString, wcsLen);
    //������'\0'
    wszString[wcsLen] = '\0';
	ansiLen = WideCharToMultiByte(CP_ACP, NULL, wszString, wcsLen, NULL, 0, NULL, NULL);
    //ͬ�ϣ�����ռ�Ҫ��'\0'�����ռ�
    szAnsi =  (char *)malloc(ansiLen + 1);
    //ת��
    //unicode���Ӧ��strlen��wcslen
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

//��ANSI�ַ�����תΪUTF-8��ʽ
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf)
{
    // ansi to unicode
    int wcsLen;
	wchar_t *wszString;
	char *szU8;
	int u8Len;
    //Ԥת�����õ�����ռ�Ĵ�С
    wcsLen = MultiByteToWideChar(CP_ACP, NULL, (char *)ansiBuf, ansiBufLen, NULL, 0);
    //����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ�
    wszString = (wchar_t *)malloc((wcsLen+1)*sizeof(wchar_t));
	memset(wszString,0,(wcsLen+1)*sizeof(wchar_t));
    //ת��
    MultiByteToWideChar(CP_ACP, NULL, (char *)ansiBuf, ansiBufLen, wszString, wcsLen);
    //������'\0'
    wszString[wcsLen] = '\0';

	u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, wcsLen, NULL, 0, NULL, NULL);
    //ͬ�ϣ�����ռ�Ҫ��'\0'�����ռ�
    //UTF8��Ȼ��Unicode��ѹ����ʽ����Ҳ�Ƕ��ֽ��ַ��������Կ�����char����ʽ����
    szU8 = (char *)malloc(u8Len + 1);
    //ת��
    //unicode���Ӧ��strlen��wcslen
    WideCharToMultiByte(CP_UTF8, NULL, wszString, wcsLen, szU8, u8Len, NULL, NULL);
    //������'\0'
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
/* ��linux�º�AIX��ͨ�� */
/* AIX����Ҫ����-liconv */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <stdlib.h>

int UnionCodeConvert(char *from_charset,char *to_charset,char *inbuf,int len_of_inbuf,char *outbuf,int len_of_outbuf)
{
        iconv_t cd;            /* iconv�ͣ���ʵ���Ǹ����ͱ����� */
        char **pin=&inbuf;     /* ��*inbuf��ʼ��ȡ������ȡ*inbufleft�ֽ�-��佫�βε�ֵ����ʵ�� */
        char **pout=&outbuf;   /* ת�����*outbuf��ʼд�룬���*outbufleft�ֽ�--��佫�βε�ֵ����ʵ�� */
 
	size_t lenin = len_of_inbuf;	
	size_t lenout = len_of_outbuf;
 				
        cd=iconv_open(to_charset,from_charset);   /* gb18030��Ƕ��ʽlinuxϵͳ���Ƿ�֧���д���֤�������ڼ�ɻ�Ϊ"gbk" or "gb2312" */
 
        if( cd == 0 )
	{
                printf("iconv_open() error!\n");
                return -1;
	}
 
        memset(outbuf,'\0',len_of_outbuf);          /* ���������Ƿ��Ӱ�쵽�����߳���outbuf�������� */
 
        if(iconv(cd,pin,(size_t *)&lenin,pout,(size_t *)&lenout) == -1)   /* ת�� */
	{
                printf("iconv() error!\n");
                iconv_close(cd);
                return  -1;
	}
 
        iconv_close(cd);
        return len_of_outbuf-lenout;
} 


// ��UTF-8תΪANSI
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen,unsigned char *ansiBuf,int sizeofAnsiBuf)
{
	return(UnionCodeConvert("UTF-8","GB18030",(char *)utf8Buf,utf8BufLen,(char *)ansiBuf,sizeofAnsiBuf));
}

//��ANSIתΪUTF-8
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf)
{
	return UnionCodeConvert("GB18030","UTF-8",(char *)ansiBuf,ansiBufLen,(char *)utf8Buf,sizeofutf8Buf);
}

#endif


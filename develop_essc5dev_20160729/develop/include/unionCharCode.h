#ifndef _UNION_CHAR_CODE_
#define _UNION_CHAR_CODE_

// asniת��Ϊutf8
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf);

// utf8ת��Ϊasni
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen, unsigned char *ansiBuf,int sizeofAnsiBuf);

#endif

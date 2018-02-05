#ifndef _UNION_CHAR_CODE_
#define _UNION_CHAR_CODE_

// asni转化为utf8
int UnionAnsiToUtf8(unsigned char *ansiBuf,int ansiBufLen,unsigned char *utf8Buf,int sizeofutf8Buf);

// utf8转化为asni
int UnionUtf8ToAnsi(unsigned char *utf8Buf, int utf8BufLen, unsigned char *ansiBuf,int sizeofAnsiBuf);

#endif

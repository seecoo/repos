#ifndef _BASE64_H
#define _BASE64_H
int from64tobits(char *out, const char *in);
void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
char *Base64Enc(char *inout);  //字符串转base64,当inout为可见字符有效
char *Base64Dec(char *inout); //base64转字符串,当被加密的字符串为可见字符有效
void to64frombitsWithAscTable(unsigned char *out, const unsigned char *in, int inlen, char *ascTable);
int from64tobitsWithAscTable(char *out, const char *in, char *ascTable);

#endif


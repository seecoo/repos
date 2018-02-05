#ifndef _BASE64_H
#define _BASE64_H
int from64tobits(char *out, const char *in);
void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
char *Base64Enc(char *inout);  //�ַ���תbase64,��inoutΪ�ɼ��ַ���Ч
char *Base64Dec(char *inout); //base64ת�ַ���,�������ܵ��ַ���Ϊ�ɼ��ַ���Ч
void to64frombitsWithAscTable(unsigned char *out, const unsigned char *in, int inlen, char *ascTable);
int from64tobitsWithAscTable(char *out, const char *in, char *ascTable);

#endif


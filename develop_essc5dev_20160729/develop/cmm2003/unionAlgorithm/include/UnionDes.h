//	Date:	2002/6/20
/* Modified by Mary, 2000-7-23 */

#ifndef _UnionDES


// The pPlainKey must be 64bits, i.e. BCD 16chars
// pPlainText must be 64bits,i.e. BCD 16chars
int Union1DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram);

// The pPlainKey must be 64bits, i.e. BCD 16chars
// The pCryptogram must be 64bits, i.e. BCD 16chars.
int Union1DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText);

// The pPlainKey must be 128bits, i.e. BCD 32chars
// The plaintext must be 64bits, i.e. BCD 16 chars
int Union3DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram);

// The pPlainKey must be 128bits, i.e. BCD 32chars
// the cryptogram must be 64bits, i.e. BCD 16chars
int Union3DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText);

int Union192BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram);
int Union128BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram);
int Union64BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram);
int Union192BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText);
int Union128BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText);
int Union64BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText);

// The pPlainKey must be 128bits, i.e. ASCII 16chars
// The plaintext must be 64bits, i.e. ASCII 8 chars
int Union3DesEncryptText(char *pPlainKey,char *pPlainText,char *pCryptogram);

// The pPlainKey must be 128bits, i.e. ASCII 16chars
// the cryptogram must be 64bits, i.e. ASCII 8 chars
int Union3DesDecryptText(char *pPlainKey,char *pCryptogram,char *pPlainText);

#endif

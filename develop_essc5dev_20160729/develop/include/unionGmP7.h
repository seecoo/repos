#ifndef _UNION_CERT_GM_P7_FUN_
#define _UNION_CERT_GM_P7_FUN_
#include <openssl/asn1.h>

#ifdef __cplusplus
extern "C" {
#endif

int i2d_PKCS7_Ex(PKCS7 *a, unsigned char **out);
PKCS7 *d2i_PKCS7_Ex(PKCS7 **a, const unsigned char **in, long len);

#ifdef __cplusplus
extern "C" }
#endif

#endif

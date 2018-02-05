#include <openssl/ec.h>
#include <openssl/bn.h>

//SM2_sign_setup
int SM2_sign_setup(EC_KEY *eckey, BN_CTX *ctx_in, BIGNUM **kinvp, BIGNUM **rp);

//SM2_sign_ex
int	SM2_sign_ex(int type, const unsigned char *dgst, int dlen, unsigned char 
	*sig, unsigned int *siglen, const BIGNUM *kinv, const BIGNUM *r, EC_KEY *eckey);

//SM2_sign
int	SM2_sign(int type, const unsigned char *dgst, int dlen, unsigned char 
		*sig, unsigned int *siglen, EC_KEY *eckey);

//SM2_verify
int SM2_verify(int type, const unsigned char *dgst, int dgst_len,
		const unsigned char *sigbuf, int sig_len, EC_KEY *eckey);

//SM2 DH, comupting shared point
int SM2_DH_key(const EC_GROUP * group,const EC_POINT *b_pub_key_r, const EC_POINT *b_pub_key, const BIGNUM *a_r,EC_KEY *a_eckey,
			   unsigned char *outkey,size_t keylen);

int SM2_verify_signature(const unsigned char *digest,
	int digest_len,
	const unsigned char *pubkey_x_coordinate,
	int pubkey_x_coordinate_len,
	const unsigned char *pubkey_y_coordinate,
	int pubkey_y_coordinate_len,
	const unsigned char *sig_r_coordinate,
	int sig_r_coordinate_len,
	const unsigned char *sig_s_coordinate,
	int sig_s_coordinate_len);

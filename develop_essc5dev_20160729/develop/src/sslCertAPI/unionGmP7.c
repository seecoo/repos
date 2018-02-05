//create by hzh for sm2P7 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/pkcs12.h>
#include "unionGmP7.h"
#include "UnionStr.h"
 
#define SM2_PKCS7_SIGNED_DATA_TYPE_DER "060A2A811CCF550601040202"  //1.2.156.10197.6.1.4.2.2 pkcs7国密签名数据类型
#define SM2_PKCS7_DATA_TYPE_DER "060A2A811CCF550601040201"   //1.2.156.10197.6.1.4.2.1 sm2 pkcs7数据

int ASN1_item_ex_d2i_m(ASN1_VALUE **pval, const unsigned char **in, long len,
			const ASN1_ITEM *it,
			int tag, int aclass, char opt, ASN1_TLC *ctx);
ASN1_VALUE *ASN1_item_d2i_m(ASN1_VALUE **pval,
		const unsigned char **in, long len, const ASN1_ITEM *it);

int ASN1_item_ex_i2d_m(ASN1_VALUE **pval, unsigned char **out,
			const ASN1_ITEM *it, int tag, int aclass);

#define asn1_tlc_clear(c)	if (c) (c)->valid = 0



static int asn1_check_tlen(long *olen, int *otag, unsigned char *oclass,
				char *inf, char *cst,
				const unsigned char **in, long len,
				int exptag, int expclass, char opt,
				ASN1_TLC *ctx)
	{
	int i;
	int ptag, pclass;
	long plen;
	const unsigned char *p, *q;
	p = *in;
	q = p;
	if (ctx && ctx->valid)
		{
		i = ctx->ret;
		plen = ctx->plen;
		pclass = ctx->pclass;
		ptag = ctx->ptag;
		p += ctx->hdrlen;
		}
	else
		{
		i = ASN1_get_object(&p, &plen, &ptag, &pclass, len);
		if (ctx)
			{
			ctx->ret = i;
			ctx->plen = plen;
			ctx->pclass = pclass;
			ctx->ptag = ptag;
			ctx->hdrlen = p - q;
			ctx->valid = 1;
			/* If definite length, and no error, length +
			 * header can't exceed total amount of data available. 
			 */
			if (!(i & 0x81) && ((plen + ctx->hdrlen) > len))
				{
				ASN1err(ASN1_F_ASN1_CHECK_TLEN,
							ASN1_R_TOO_LONG);
				asn1_tlc_clear(ctx);
				return 0;
				}
			}
		}

	if (i & 0x80)
		{
		ASN1err(ASN1_F_ASN1_CHECK_TLEN, ASN1_R_BAD_OBJECT_HEADER);
		asn1_tlc_clear(ctx);
		return 0;
		}
	if (exptag >= 0)
		{
		if ((exptag != ptag) || (expclass != pclass))
			{
			/* If type is OPTIONAL, not an error:
			 * indicate missing type.
			 */
			if (opt) return -1;
			asn1_tlc_clear(ctx);
			ASN1err(ASN1_F_ASN1_CHECK_TLEN, ASN1_R_WRONG_TAG);
			return 0;
			}
		/* We have a tag and class match:
		 * assume we are going to do something with it */
		asn1_tlc_clear(ctx);
		}

	if (i & 1)
		plen = len - (p - q);

	if (inf)
		*inf = i & 1;

	if (cst)
		*cst = i & V_ASN1_CONSTRUCTED;

	if (olen)
		*olen = plen;

	if (oclass)
		*oclass = pclass;

	if (otag)
		*otag = ptag;

	*in = p;
	return 1;
	}


static int asn1_check_eoc(const unsigned char **in, long len)
	{
	const unsigned char *p;
	if (len < 2) return 0;
	p = *in;
	if (!p[0] && !p[1])
		{
		*in += 2;
		return 1;
		}
	return 0;
	}

static int asn1_template_noexp_d2i(ASN1_VALUE **val,
				const unsigned char **in, long len,
				const ASN1_TEMPLATE *tt, char opt,
				ASN1_TLC *ctx)
	{
	int flags, aclass;
	int ret;
	const unsigned char *p, *q;
	if (!val)
		return 0;
	flags = tt->flags;
	aclass = flags & ASN1_TFLG_TAG_CLASS;

	p = *in;
	q = p;

	if (flags & ASN1_TFLG_SK_MASK)
		{
		/* SET OF, SEQUENCE OF */
		int sktag, skaclass;
		char sk_eoc;
		/* First work out expected inner tag value */
		if (flags & ASN1_TFLG_IMPTAG)
			{
			sktag = tt->tag;
			skaclass = aclass;
			}
		else
			{
			skaclass = V_ASN1_UNIVERSAL;
			if (flags & ASN1_TFLG_SET_OF)
				sktag = V_ASN1_SET;
			else
				sktag = V_ASN1_SEQUENCE;
			}
		/* Get the tag */
		ret = asn1_check_tlen(&len, NULL, NULL, &sk_eoc, NULL,
					&p, len, sktag, skaclass, opt, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
						ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}
		else if (ret == -1)
			return -1;
		if (!*val)
			*val = (ASN1_VALUE *)sk_new_null();
		else
			{
			/* We've got a valid STACK: free up any items present */
			_STACK *sktmp = (_STACK *)*val;
			ASN1_VALUE *vtmp;
			while(sk_num(sktmp) > 0)
				{
				vtmp = (ASN1_VALUE *)sk_pop(sktmp);
				ASN1_item_ex_free(&vtmp,
						ASN1_ITEM_ptr(tt->item));
				}
			}
				
		if (!*val)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
						ERR_R_MALLOC_FAILURE);
			goto err;
			}

		/* Read as many items as we can */
		while(len > 0)
			{
			ASN1_VALUE *skfield;
			q = p;
			/* See if EOC found */
			if (asn1_check_eoc(&p, len))
				{
				if (!sk_eoc)
					{
					ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
							ASN1_R_UNEXPECTED_EOC);
					goto err;
					}
				len -= p - q;
				sk_eoc = 0;
				break;
				}
			skfield = NULL;
			if (!ASN1_item_ex_d2i_m(&skfield, &p, len,
						ASN1_ITEM_ptr(tt->item),
						-1, 0, 0, ctx))
				{
				ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
					ERR_R_NESTED_ASN1_ERROR);
				goto err;
				}
			len -= p - q;
			if (!sk_push((_STACK *)*val, (char *)skfield))
				{
				ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
						ERR_R_MALLOC_FAILURE);
				goto err;
				}
			}
		if (sk_eoc)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I, ASN1_R_MISSING_EOC);
			goto err;
			}
		}
	else if (flags & ASN1_TFLG_IMPTAG)
		{
		/* IMPLICIT tagging */
		ret = ASN1_item_ex_d2i_m(val, &p, len,
			ASN1_ITEM_ptr(tt->item), tt->tag, aclass, opt, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
						ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}
		else if (ret == -1)
			return -1;
		}
	else
		{
		/* Nothing special */
		ret = ASN1_item_ex_d2i_m(val, &p, len, ASN1_ITEM_ptr(tt->item),
							-1, 0, opt, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I,
					ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}
		else if (ret == -1)
			return -1;
		}

	*in = p;
	return 1;

	err:
	ASN1_template_free(val, tt);
	*val = NULL;
	return 0;
	}


static int asn1_template_ex_d2i(ASN1_VALUE **val,
				const unsigned char **in, long inlen,
				const ASN1_TEMPLATE *tt, char opt,
							ASN1_TLC *ctx)
	{
	int flags, aclass;
	int ret;
	long len;
	const unsigned char *p, *q;
	char exp_eoc;

	if (!val)
		return 0;
	flags = tt->flags;
	aclass = flags & ASN1_TFLG_TAG_CLASS;

	p = *in;
	/* Check if EXPLICIT tag expected */
	if (flags & ASN1_TFLG_EXPTAG)
		{
		char cst;
		/* Need to work out amount of data available to the inner
		 * content and where it starts: so read in EXPLICIT header to
		 * get the info.
		 */
		ret = asn1_check_tlen(&len, NULL, NULL, &exp_eoc, &cst,
					&p, inlen, tt->tag, aclass, opt, ctx);
		q = p;
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_EX_D2I,
					ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}
		else if (ret == -1)
			return -1;
		if (!cst)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_EX_D2I,
					ASN1_R_EXPLICIT_TAG_NOT_CONSTRUCTED);
			return 0;
			}
		/* We've found the field so it can't be OPTIONAL now */
		ret = asn1_template_noexp_d2i(val, &p, len, tt, 0, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_TEMPLATE_EX_D2I,
					ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}
		/* We read the field in OK so update length */
		len -= p - q;
		if (exp_eoc)
			{
			/* If NDEF we must have an EOC here */
			if (!asn1_check_eoc(&p, len))
				{
				ASN1err(ASN1_F_ASN1_TEMPLATE_EX_D2I,
						ASN1_R_MISSING_EOC);
				goto err;
				}
			}
		else
			{
			/* Otherwise we must hit the EXPLICIT tag end or its
			 * an error */
			if (len)
				{
				ASN1err(ASN1_F_ASN1_TEMPLATE_EX_D2I,
					ASN1_R_EXPLICIT_LENGTH_MISMATCH);
				goto err;
				}
			}
		}
		else 
			return asn1_template_noexp_d2i(val, in, inlen,
								tt, opt, ctx);

	*in = p;
	return 1;

	err:
	ASN1_template_free(val, tt);
	*val = NULL;
	return 0;
	}


static int asn1_find_end(const unsigned char **in, long len, char inf)
	{
	int expected_eoc;
	long plen;
	const unsigned char *p = *in, *q;
	/* If not indefinite length constructed just add length */
	if (inf == 0)
		{
		*in += len;
		return 1;
		}
	expected_eoc = 1;
	/* Indefinite length constructed form. Find the end when enough EOCs
	 * are found. If more indefinite length constructed headers
	 * are encountered increment the expected eoc count otherwise just
	 * skip to the end of the data.
	 */
	while (len > 0)
		{
		if(asn1_check_eoc(&p, len))
			{
			expected_eoc--;
			if (expected_eoc == 0)
				break;
			len -= 2;
			continue;
			}
		q = p;
		/* Just read in a header: only care about the length */
		if(!asn1_check_tlen(&plen, NULL, NULL, &inf, NULL, &p, len,
				-1, 0, 0, NULL))
			{
			ASN1err(ASN1_F_ASN1_FIND_END, ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}
		if (inf)
			expected_eoc++;
		else
			p += plen;
		len -= p - q;
		}
	if (expected_eoc)
		{
		ASN1err(ASN1_F_ASN1_FIND_END, ASN1_R_MISSING_EOC);
		return 0;
		}
	*in = p;
	return 1;
	}


static int collect_data(BUF_MEM *buf, const unsigned char **p, long plen)
	{
	int len;
	if (buf)
		{
		len = buf->length;
		if (!BUF_MEM_grow_clean(buf, len + plen))
			{
			ASN1err(ASN1_F_COLLECT_DATA, ERR_R_MALLOC_FAILURE);
			return 0;
			}
		memcpy(buf->data + len, *p, plen);
		}
	*p += plen;
	return 1;
	}


static int asn1_collect(BUF_MEM *buf, const unsigned char **in, long len,
				char inf, int tag, int aclass)
	{
	const unsigned char *p, *q;
	long plen;
	char cst, ininf;
	p = *in;
	inf &= 1;
	/* If no buffer and not indefinite length constructed just pass over
	 * the encoded data */
	if (!buf && !inf)
		{
		*in += len;
		return 1;
		}
	while(len > 0)
		{
		q = p;
		/* Check for EOC */
		if (asn1_check_eoc(&p, len))
			{
			/* EOC is illegal outside indefinite length
			 * constructed form */
			if (!inf)
				{
				ASN1err(ASN1_F_ASN1_COLLECT,
					ASN1_R_UNEXPECTED_EOC);
				return 0;
				}
			inf = 0;
			break;
			}

		if (!asn1_check_tlen(&plen, NULL, NULL, &ininf, &cst, &p,
					len, tag, aclass, 0, NULL))
			{
			ASN1err(ASN1_F_ASN1_COLLECT, ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}

		/* If indefinite length constructed update max length */
		if (cst)
			{
#ifdef OPENSSL_ALLOW_NESTED_ASN1_STRINGS
			if (!asn1_collect(buf, &p, plen, ininf, tag, aclass))
				return 0;
#else
			ASN1err(ASN1_F_ASN1_COLLECT, ASN1_R_NESTED_ASN1_STRING);
			return 0;
#endif
			}
		else if (!collect_data(buf, &p, plen))
			return 0;
		len -= p - q;
		}
	if (inf)
		{
		ASN1err(ASN1_F_ASN1_COLLECT, ASN1_R_MISSING_EOC);
		return 0;
		}
	*in = p;
	return 1;
	}

static int asn1_d2i_ex_primitive(ASN1_VALUE **pval,
				const unsigned char **in, long inlen, 
				const ASN1_ITEM *it,
				int tag, int aclass, char opt, ASN1_TLC *ctx)
	{
	int ret = 0, utype;
	long plen;
	char cst, inf, free_cont = 0;
	const unsigned char *p;
	BUF_MEM buf;
	const unsigned char *cont = NULL;
	long len; 
	if (!pval)
		{
		ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE, ASN1_R_ILLEGAL_NULL);
		return 0; /* Should never happen */
		}

	if (it->itype == ASN1_ITYPE_MSTRING)
		{
		utype = tag;
		tag = -1;
		}
	else
		utype = it->utype;

	if (utype == V_ASN1_ANY)
		{
		/* If type is ANY need to figure out type from tag */
		unsigned char oclass;
		if (tag >= 0)
			{
			ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE,
					ASN1_R_ILLEGAL_TAGGED_ANY);
			return 0;
			}
		if (opt)
			{
			ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE,
					ASN1_R_ILLEGAL_OPTIONAL_ANY);
			return 0;
			}
		p = *in;
		ret = asn1_check_tlen(NULL, &utype, &oclass, NULL, NULL,
					&p, inlen, -1, 0, 0, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE,
					ERR_R_NESTED_ASN1_ERROR);
			return 0;
			}
		if (oclass != V_ASN1_UNIVERSAL)
			utype = V_ASN1_OTHER;
		}
	if (tag == -1)
		{
		tag = utype;
		aclass = V_ASN1_UNIVERSAL;
		}
	p = *in;
	/* Check header */
	ret = asn1_check_tlen(&plen, NULL, NULL, &inf, &cst,
				&p, inlen, tag, aclass, opt, ctx);
	if (!ret)
		{
		ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE, ERR_R_NESTED_ASN1_ERROR);
		return 0;
		}
	else if (ret == -1)
		return -1;
	/* SEQUENCE, SET and "OTHER" are left in encoded form */
	if ((utype == V_ASN1_SEQUENCE)
		|| (utype == V_ASN1_SET) || (utype == V_ASN1_OTHER))
		{
		/* Clear context cache for type OTHER because the auto clear
		 * when we have a exact match wont work
		 */
		if (utype == V_ASN1_OTHER)
			{
			asn1_tlc_clear(ctx);
			}
		/* SEQUENCE and SET must be constructed */
		else if (!cst)
			{
			ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE,
				ASN1_R_TYPE_NOT_CONSTRUCTED);
			return 0;
			}

		cont = *in;
		/* If indefinite length constructed find the real end */
		if (inf)
			{
			if (!asn1_find_end(&p, plen, inf))
				 goto err;
			len = p - cont;
			}
		else
			{
			len = p - cont + plen;
			p += plen;
			buf.data = NULL;
			}
		}
	else if (cst)
		{
		buf.length = 0;
		buf.max = 0;
		buf.data = NULL;
		/* Should really check the internal tags are correct but
		 * some things may get this wrong. The relevant specs
		 * say that constructed string types should be OCTET STRINGs
		 * internally irrespective of the type. So instead just check
		 * for UNIVERSAL class and ignore the tag.
		 */
		if (!asn1_collect(&buf, &p, plen, inf, -1, V_ASN1_UNIVERSAL))
			goto err;
		len = buf.length;
		/* Append a final null to string */
		if (!BUF_MEM_grow_clean(&buf, len + 1))
			{
			ASN1err(ASN1_F_ASN1_D2I_EX_PRIMITIVE,
						ERR_R_MALLOC_FAILURE);
			return 0;
			}
		buf.data[len] = 0;
		cont = (const unsigned char *)buf.data;
		free_cont = 1;
		}
	else
		{
		cont = p;
		len = plen;
		p += plen;
		}

	/* We now have content length and type: translate into a structure */
	if (!asn1_ex_c2i(pval, cont, len, utype, &free_cont, it))
		goto err;

	*in = p;
	ret = 1;
	err:
	if (free_cont && buf.data) OPENSSL_free(buf.data);
	return ret;
	}

#define offset2ptr(addr, offset) (void *)(((char *) addr) + offset)

static const ASN1_TEMPLATE SM2_P7_Data_TMPL =  {(ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL|ASN1_TFLG_NDEF), (0), offsetof(PKCS7, d.data),"d.data", ASN1_ITEM_ref(ASN1_OCTET_STRING_NDEF) };
static const ASN1_TEMPLATE SM2_P7_SIGN_TMPL = {(ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL|ASN1_TFLG_NDEF),0,offsetof(PKCS7,d.sign),"d.sign",ASN1_ITEM_ref(PKCS7_SIGNED)};
const ASN1_TEMPLATE *getSM2P7DataTemplate(unsigned char *derSM2P7Data,int len)
{
	char buf[1024] = {0};
	bcdhex_to_aschex((char *)derSM2P7Data,len,buf);
	if(strcmp(buf,SM2_PKCS7_DATA_TYPE_DER) == 0)
		return &SM2_P7_Data_TMPL;
	if(strcmp(buf,SM2_PKCS7_SIGNED_DATA_TYPE_DER) == 0)
		return &SM2_P7_SIGN_TMPL;
	else
		return NULL;
}

const ASN1_TEMPLATE *asn1_do_adb_m(ASN1_VALUE **pval, const ASN1_TEMPLATE *tt,
								int nullerr)
	{
	const ASN1_ADB *adb;
	const ASN1_ADB_TABLE *atbl;
	long selector;
	ASN1_VALUE **sfld;
	int i;
	int len = 0;
	unsigned char derOBJBuf[4096] = {0};
	unsigned char *pBuf = NULL;
	const ASN1_TEMPLATE *tTmp = NULL;

	if (!(tt->flags & ASN1_TFLG_ADB_MASK))
	{
		return tt;
	}
	 
	/* Else ANY DEFINED BY ... get the table */
	adb = ASN1_ADB_ptr(tt->item);

	/* Get the selector field */
	sfld = (ASN1_VALUE **)offset2ptr(*pval, adb->offset);

	/* Check if NULL */
	if (!sfld)
		{
		if (!adb->null_tt)
			goto err;
		return adb->null_tt;
		}

	/* Convert type to a long:
	 * NB: don't check for NID_undef here because it
	 * might be a legitimate value in the table
	 */
	if (tt->flags & ASN1_TFLG_ADB_OID) 
	{
		selector = OBJ_obj2nid((ASN1_OBJECT *)*sfld);
	}
	else {
		selector = ASN1_INTEGER_get((ASN1_INTEGER *)*sfld);
	}
	 
	/* Try to find matching entry in table
	 * Maybe should check application types first to
	 * allow application override? Might also be useful
	 * to have a flag which indicates table is sorted and
	 * we can do a binary search. For now stick to a
	 * linear search.
	 */
	for (atbl = adb->tbl, i = 0; i < adb->tblcount; i++, atbl++)
	{
		if (atbl->value == selector)
		{
			return &atbl->tt;
		}
	}
	 
	/* FIXME: need to search application table too */
	pBuf = derOBJBuf;
	if((len = i2d_ASN1_OBJECT((ASN1_OBJECT *)*sfld,&pBuf)) <= 0)
	{
		goto err;
	}
	 
	if((tTmp = getSM2P7DataTemplate(derOBJBuf,len)) != NULL)
	{
		return tTmp;
	}

	/* No match, return default type */
	if (!adb->default_tt)
		goto err;		
	return adb->default_tt;
	
	err:
	/* FIXME: should log the value or OID of unsupported type */
	if (nullerr)
		ASN1err(ASN1_F_ASN1_DO_ADB,
			ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE);
	return NULL;
	}


/* Decode an item, taking care of IMPLICIT tagging, if any.
 * If 'opt' set and tag mismatch return -1 to handle OPTIONAL
 */

int ASN1_item_ex_d2i_m(ASN1_VALUE **pval, const unsigned char **in, long len,
			const ASN1_ITEM *it,
			int tag, int aclass, char opt, ASN1_TLC *ctx)
	{
	const ASN1_TEMPLATE *tt, *errtt = NULL;
	const ASN1_COMPAT_FUNCS *cf;
	const ASN1_EXTERN_FUNCS *ef;
	const ASN1_AUX *aux = (const ASN1_AUX *)it->funcs;
	ASN1_aux_cb *asn1_cb;
	const unsigned char *p, *q;
	unsigned char *wp=NULL;	/* BIG FAT WARNING!  BREAKS CONST WHERE USED */
	unsigned char imphack = 0, oclass;
	char seq_eoc, seq_nolen, cst, isopt;
	long tmplen;
	int i;
	int otag;
	int ret = 0;
	ASN1_VALUE *pchval, **pchptr, *ptmpval;
 
	if (!pval)
		return 0;
	
	if (aux && aux->asn1_cb)
	{
		asn1_cb = aux->asn1_cb;
	}
	else  {
		asn1_cb = 0;
	}
	 
	switch(it->itype)
		{
		case ASN1_ITYPE_PRIMITIVE:
		if (it->templates)
			{
			/* tagging or OPTIONAL is currently illegal on an item
			 * template because the flags can't get passed down.
			 * In practice this isn't a problem: we include the
			 * relevant flags from the item template in the
			 * template itself.
			 */
			if ((tag != -1) || opt)
				{
				ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
				ASN1_R_ILLEGAL_OPTIONS_ON_ITEM_TEMPLATE);
				goto err;
				}
			return asn1_template_ex_d2i(pval, in, len,
					it->templates, opt, ctx);
		}
		return asn1_d2i_ex_primitive(pval, in, len, it,
						tag, aclass, opt, ctx);
		break;

		case ASN1_ITYPE_MSTRING:
		p = *in;
		/* Just read in tag and class */
		ret = asn1_check_tlen(NULL, &otag, &oclass, NULL, NULL,
						&p, len, -1, 0, 1, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}

		/* Must be UNIVERSAL class */
		if (oclass != V_ASN1_UNIVERSAL)
			{
			/* If OPTIONAL, assume this is OK */
			if (opt) return -1;
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ASN1_R_MSTRING_NOT_UNIVERSAL);
			goto err;
			}
		/* Check tag matches bit map */
		if (!(ASN1_tag2bit(otag) & it->utype))
			{
			/* If OPTIONAL, assume this is OK */
			if (opt)
				return -1;
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ASN1_R_MSTRING_WRONG_TAG);
			goto err;
			}
		return asn1_d2i_ex_primitive(pval, in, len,
						it, otag, 0, 0, ctx);

		case ASN1_ITYPE_EXTERN:
		/* Use new style d2i */
		ef = (const ASN1_EXTERN_FUNCS *)it->funcs;
		return ef->asn1_ex_d2i(pval, in, len,
						it, tag, aclass, opt, ctx);

		case ASN1_ITYPE_COMPAT:
		/* we must resort to old style evil hackery */
		cf = (const ASN1_COMPAT_FUNCS *)it->funcs;

		/* If OPTIONAL see if it is there */
		if (opt)
			{
			int exptag;
			p = *in;
			if (tag == -1)
				exptag = it->utype;
			else exptag = tag;
			/* Don't care about anything other than presence
			 * of expected tag */

			ret = asn1_check_tlen(NULL, NULL, NULL, NULL, NULL,
					&p, len, exptag, aclass, 1, ctx);
			if (!ret)
				{
				ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ERR_R_NESTED_ASN1_ERROR);
				goto err;
				}
			if (ret == -1)
				return -1;
			}

		/* This is the old style evil hack IMPLICIT handling:
		 * since the underlying code is expecting a tag and
		 * class other than the one present we change the
		 * buffer temporarily then change it back afterwards.
		 * This doesn't and never did work for tags > 30.
		 *
		 * Yes this is *horrible* but it is only needed for
		 * old style d2i which will hopefully not be around
		 * for much longer.
		 * FIXME: should copy the buffer then modify it so
		 * the input buffer can be const: we should *always*
		 * copy because the old style d2i might modify the
		 * buffer.
		 */

		if (tag != -1)
			{
			wp = *(unsigned char **)in;
			imphack = *wp;
			*wp = (unsigned char)((*p & V_ASN1_CONSTRUCTED)
								| it->utype);
			}

		ptmpval = cf->asn1_d2i(pval, in, len);

		if (tag != -1)
			*wp = imphack;

		if (ptmpval)
			return 1;

		ASN1err(ASN1_F_ASN1_ITEM_EX_D2I, ERR_R_NESTED_ASN1_ERROR);
		goto err;


		case ASN1_ITYPE_CHOICE:
		if (asn1_cb && !asn1_cb(ASN1_OP_D2I_PRE, pval, it, NULL))
				goto auxerr;

		/* Allocate structure */
		if (!*pval && !ASN1_item_ex_new(pval, it))
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
						ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}
		/* CHOICE type, try each possibility in turn */
		pchval = NULL;
		p = *in;
		for (i = 0, tt=it->templates; i < it->tcount; i++, tt++)
			{
			pchptr = asn1_get_field_ptr(pval, tt);
			/* We mark field as OPTIONAL so its absence
			 * can be recognised.
			 */
			ret = asn1_template_ex_d2i(pchptr, &p, len, tt, 1, ctx);
			/* If field not present, try the next one */
			if (ret == -1)
				continue;
			/* If positive return, read OK, break loop */
			if (ret > 0)
				break;
			/* Otherwise must be an ASN1 parsing error */
			errtt = tt;
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
						ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}

		/* Did we fall off the end without reading anything? */
		if (i == it->tcount)
			{
			/* If OPTIONAL, this is OK */
			if (opt)
				{
				/* Free and zero it */
				ASN1_item_ex_free(pval, it);
				return -1;
				}
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ASN1_R_NO_MATCHING_CHOICE_TYPE);
			goto err;
			}

		asn1_set_choice_selector(pval, i, it);
		*in = p;
		if (asn1_cb && !asn1_cb(ASN1_OP_D2I_POST, pval, it, NULL))
				goto auxerr;
		return 1;

		case ASN1_ITYPE_NDEF_SEQUENCE:
		case ASN1_ITYPE_SEQUENCE:

		p = *in;
		tmplen = len;
		 

		/* If no IMPLICIT tagging set to SEQUENCE, UNIVERSAL */
		if (tag == -1)
			{
			tag = V_ASN1_SEQUENCE;
			aclass = V_ASN1_UNIVERSAL;
			}
		/* Get SEQUENCE length and update len, p */
		ret = asn1_check_tlen(&len, NULL, NULL, &seq_eoc, &cst,
					&p, len, tag, aclass, opt, ctx);
		if (!ret)
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}
		else if (ret == -1)
			return -1;
		if (aux && (aux->flags & ASN1_AFLG_BROKEN))
			{
			len = tmplen - (p - *in);
			seq_nolen = 1;
			}
		/* If indefinite we don't do a length check */
		else seq_nolen = seq_eoc;
		if (!cst)
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
				ASN1_R_SEQUENCE_NOT_CONSTRUCTED);
			goto err;
			}

		if (!*pval && !ASN1_item_ex_new(pval, it))
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
				ERR_R_NESTED_ASN1_ERROR);
			goto err;
			}

		if (asn1_cb && !asn1_cb(ASN1_OP_D2I_PRE, pval, it, NULL))
				goto auxerr;

		/* Get each field entry */
		for (i = 0, tt = it->templates; i < it->tcount; i++, tt++)
			{
			const ASN1_TEMPLATE *seqtt;
			ASN1_VALUE **pseqval;
			seqtt = asn1_do_adb_m(pval, tt, 1);
			if (!seqtt)
				goto err;
			pseqval = asn1_get_field_ptr(pval, seqtt);
			/* Have we ran out of data? */
			if (!len)
				break;
			q = p;
			if (asn1_check_eoc(&p, len))
				{
				if (!seq_eoc)
					{
					ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
							ASN1_R_UNEXPECTED_EOC);
					goto err;
					}
				len -= p - q;
				seq_eoc = 0;
				q = p;
				break;
				}
			/* This determines the OPTIONAL flag value. The field
			 * cannot be omitted if it is the last of a SEQUENCE
			 * and there is still data to be read. This isn't
			 * strictly necessary but it increases efficiency in
			 * some cases.
			 */
			if (i == (it->tcount - 1))
				isopt = 0;
			else isopt = (char)(seqtt->flags & ASN1_TFLG_OPTIONAL);
			/* attempt to read in field, allowing each to be
			 * OPTIONAL */

			ret = asn1_template_ex_d2i(pseqval, &p, len,
							seqtt, isopt, ctx);
			if (!ret)
				{
				errtt = seqtt;
				goto err;
				}
			else if (ret == -1)
				{
				/* OPTIONAL component absent.
				 * Free and zero the field.
				 */
				ASN1_template_free(pseqval, seqtt);
				continue;
				}
			/* Update length */
			len -= p - q;
			}

		/* Check for EOC if expecting one */
		if (seq_eoc && !asn1_check_eoc(&p, len))
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I, ASN1_R_MISSING_EOC);
			goto err;
			}
		/* Check all data read */
		if (!seq_nolen && len)
			{
			ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
					ASN1_R_SEQUENCE_LENGTH_MISMATCH);
			goto err;
			}

		/* If we get here we've got no more data in the SEQUENCE,
		 * however we may not have read all fields so check all
		 * remaining are OPTIONAL and clear any that are.
		 */
		 
		for (; i < it->tcount; tt++, i++)
			{
			const ASN1_TEMPLATE *seqtt;
			seqtt = asn1_do_adb_m(pval, tt, 1);
			if (!seqtt)
				goto err;
			 
			if (seqtt->flags & ASN1_TFLG_OPTIONAL)
				{
				ASN1_VALUE **pseqval;
				pseqval = asn1_get_field_ptr(pval, seqtt);
				ASN1_template_free(pseqval, seqtt);
				}
			else
				{
				errtt = seqtt;
				ASN1err(ASN1_F_ASN1_ITEM_EX_D2I,
							ASN1_R_FIELD_MISSING);
				goto err;
				}
			}
		/* Save encoding */
		if (!asn1_enc_save(pval, *in, p - *in, it))
			goto auxerr;
		*in = p;
		if (asn1_cb && !asn1_cb(ASN1_OP_D2I_POST, pval, it, NULL))
				goto auxerr;
		return 1;

		default:
		return 0;
		}
	auxerr:
	ASN1err(ASN1_F_ASN1_ITEM_EX_D2I, ASN1_R_AUX_ERROR);
	err:
	ASN1_item_ex_free(pval, it);
	if (errtt)
		ERR_add_error_data(4, "Field=", errtt->field_name,
					", Type=", it->sname);
	else
		ERR_add_error_data(2, "Type=", it->sname);
	return 0;
	}


typedef	struct {
	unsigned char *data;
	int length;
	ASN1_VALUE *field;
} DER_ENC;


static int der_cmp(const void *a, const void *b)
	{
	const DER_ENC *d1 = (const DER_ENC *)a, *d2 = (const DER_ENC *)b;
	int cmplen, i;
	cmplen = (d1->length < d2->length) ? d1->length : d2->length;
	i = memcmp(d1->data, d2->data, cmplen);
	if (i)
		return i;
	return d1->length - d2->length;
	}

static int asn1_set_seq_out(STACK_OF(ASN1_VALUE) *sk, unsigned char **out,
					int skcontlen, const ASN1_ITEM *item,
					int do_sort, int iclass)
	{
	int i;
	ASN1_VALUE *skitem;
	unsigned char *tmpdat = NULL, *p = NULL;
	DER_ENC *derlst = NULL, *tder;
	if (do_sort)
		 {
		/* Don't need to sort less than 2 items */
		if (sk_ASN1_VALUE_num(sk) < 2)
			do_sort = 0;
		else
			{
			derlst = (DER_ENC *)OPENSSL_malloc(sk_ASN1_VALUE_num(sk)
						* sizeof(*derlst));
			tmpdat = (unsigned char *)OPENSSL_malloc(skcontlen);
			if (!derlst || !tmpdat)
				return 0;
			}
		}
	/* If not sorting just output each item */
	if (!do_sort)
		{
		for (i = 0; i < sk_ASN1_VALUE_num(sk); i++)
			{
			skitem = sk_ASN1_VALUE_value(sk, i);
			ASN1_item_ex_i2d_m(&skitem, out, item, -1, iclass);
			}
		return 1;
		}
	p = tmpdat;

	/* Doing sort: build up a list of each member's DER encoding */
	for (i = 0, tder = derlst; i < sk_ASN1_VALUE_num(sk); i++, tder++)
		{
		skitem = sk_ASN1_VALUE_value(sk, i);
		tder->data = p;
		tder->length = ASN1_item_ex_i2d_m(&skitem, &p, item, -1, iclass);
		tder->field = skitem;
		}

	/* Now sort them */
	qsort(derlst, sk_ASN1_VALUE_num(sk), sizeof(*derlst), der_cmp);
	/* Output sorted DER encoding */	
	p = *out;
	for (i = 0, tder = derlst; i < sk_ASN1_VALUE_num(sk); i++, tder++)
		{
		memcpy(p, tder->data, tder->length);
		p += tder->length;
		}
	*out = p;
	/* If do_sort is 2 then reorder the STACK */
	if (do_sort == 2)
		{
		for (i = 0, tder = derlst; i < sk_ASN1_VALUE_num(sk);
							i++, tder++)
			sk_ASN1_VALUE_set(sk, i, tder->field);
		}
	OPENSSL_free(derlst);
	OPENSSL_free(tmpdat);
	return 1;
	}

static int asn1_template_ex_i2d(ASN1_VALUE **pval, unsigned char **out,
				const ASN1_TEMPLATE *tt, int tag, int iclass)
	{
	int i, ret, flags, ttag, tclass, ndef;
	flags = tt->flags;
	/* Work out tag and class to use: tagging may come
	 * either from the template or the arguments, not both
	 * because this would create ambiguity. Additionally
	 * the iclass argument may contain some additional flags
	 * which should be noted and passed down to other levels.
	 */
	 
	if (flags & ASN1_TFLG_TAG_MASK)
		{
		/* Error if argument and template tagging */
		if (tag != -1)
			/* FIXME: error code here */
			return -1;
		/* Get tagging from template */
		ttag = tt->tag;
		tclass = flags & ASN1_TFLG_TAG_CLASS;
		}
	else if (tag != -1)
		{
		/* No template tagging, get from arguments */
		ttag = tag;
		tclass = iclass & ASN1_TFLG_TAG_CLASS;
		}
	else
		{
		ttag = -1;
		tclass = 0;
		}
	/* 
	 * Remove any class mask from iflag.
	 */
	iclass &= ~ASN1_TFLG_TAG_CLASS;
	//UnionLog("in asn1_template_ex_i2d::iclass=[%d].\n",iclass);
	/* At this point 'ttag' contains the outer tag to use,Log
	 * 'tclass' is the class and iclass is any flags passed
	 * to this function.
	 */

	/* if template and arguments require ndef, use it */
	if ((flags & ASN1_TFLG_NDEF) && (iclass & ASN1_TFLG_NDEF))
		ndef = 2;
	else ndef = 1;

	if (flags & ASN1_TFLG_SK_MASK)
		{
		/* SET OF, SEQUENCE OF */
		STACK_OF(ASN1_VALUE) *sk = (STACK_OF(ASN1_VALUE) *)*pval;
		int isset, sktag, skaclass;
		int skcontlen, sklen;
		ASN1_VALUE *skitem;

		if (!*pval)
			return 0;

		if (flags & ASN1_TFLG_SET_OF)
			{
			isset = 1;
			/* 2 means we reorder */
			if (flags & ASN1_TFLG_SEQUENCE_OF)
				isset = 2;
			}
		else isset = 0;

		/* Work out inner tag value: if EXPLICIT
		 * or no tagging use underlying type.
		 */
		if ((ttag != -1) && !(flags & ASN1_TFLG_EXPTAG))
			{
			sktag = ttag;
			skaclass = tclass;
			}
		else
			{
			skaclass = V_ASN1_UNIVERSAL;
			if (isset)
				sktag = V_ASN1_SET;
			else sktag = V_ASN1_SEQUENCE;
			}

		/* Determine total length of items */
		skcontlen = 0;
		for (i = 0; i < sk_ASN1_VALUE_num(sk); i++)
			{
			skitem = sk_ASN1_VALUE_value(sk, i);
			skcontlen += ASN1_item_ex_i2d_m(&skitem, NULL,
						ASN1_ITEM_ptr(tt->item),
							-1, iclass);
			}
		sklen = ASN1_object_size(ndef, skcontlen, sktag);
		/* If EXPLICIT need length of surrounding tag */
		if (flags & ASN1_TFLG_EXPTAG)
			ret = ASN1_object_size(ndef, sklen, ttag);
		else ret = sklen;

		if (!out)
			return ret;

		/* Now encode this lot... */
		/* EXPLICIT tag */
		if (flags & ASN1_TFLG_EXPTAG)
			ASN1_put_object(out, ndef, sklen, ttag, tclass);
		/* SET or SEQUENCE and IMPLICIT tag */
		ASN1_put_object(out, ndef, skcontlen, sktag, skaclass);
		/* And the stuff itself */
		asn1_set_seq_out(sk, out, skcontlen, ASN1_ITEM_ptr(tt->item),
								isset, iclass);
		if (ndef == 2)
			{
			ASN1_put_eoc(out);
			if (flags & ASN1_TFLG_EXPTAG)
				ASN1_put_eoc(out);
			}

		return ret;
		}

	if (flags & ASN1_TFLG_EXPTAG)
		{
		/* EXPLICIT tagging */
		/* Find length of tagged item */
		i = ASN1_item_ex_i2d_m(pval, NULL, ASN1_ITEM_ptr(tt->item),
								-1, iclass);
		if (!i)
			return 0;
		/* Find length of EXPLICIT tag */
		ret = ASN1_object_size(ndef, i, ttag);
		if (out)
			{
			/* Output tag and item */
			ASN1_put_object(out, ndef, i, ttag, tclass);
			ASN1_item_ex_i2d_m(pval, out, ASN1_ITEM_ptr(tt->item),
								-1, iclass);
			if (ndef == 2)
				ASN1_put_eoc(out);
			}
		return ret;
		}

	/* Either normal or IMPLICIT tagging: combine class and flags */
	return ASN1_item_ex_i2d_m(pval, out, ASN1_ITEM_ptr(tt->item),
						ttag, tclass | iclass);

}


static int asn1_i2d_ex_primitive(ASN1_VALUE **pval, unsigned char **out,
				const ASN1_ITEM *it, int tag, int aclass)
	{
	int len;
	int utype;
	int usetag;
	int ndef = 0;

	utype = it->utype;

	/* Get length of content octets and maybe find
	 * out the underlying type.
	 */

	len = asn1_ex_i2c(pval, NULL, &utype, it);
	 

	/* If SEQUENCE, SET or OTHER then header is
	 * included in pseudo content octets so don't
	 * include tag+length. We need to check here
	 * because the call to asn1_ex_i2c() could change
	 * utype.
	 */
	if ((utype == V_ASN1_SEQUENCE) || (utype == V_ASN1_SET) ||
	   (utype == V_ASN1_OTHER))
		usetag = 0;
	else usetag = 1;

	/* -1 means omit type */

	if (len == -1)
		return 0;

	/* -2 return is special meaning use ndef */
	if (len == -2)
		{
		ndef = 2;
		len = 0;
		}

	/* If not implicitly tagged get tag from underlying type */
	if (tag == -1) tag = utype;

	/* Output tag+length followed by content octets */
	if (out)
		{
		if (usetag)
			ASN1_put_object(out, ndef, len, tag, aclass);
		asn1_ex_i2c(pval, *out, &utype, it);
		if (ndef)
			ASN1_put_eoc(out);
		else
			*out += len;
		}

	if (usetag)
		return ASN1_object_size(ndef, len, tag);
	return len;
	}

int ASN1_item_ex_i2d_m(ASN1_VALUE **pval, unsigned char **out,
			const ASN1_ITEM *it, int tag, int aclass)
	{
	const ASN1_TEMPLATE *tt = NULL;
	unsigned char *p = NULL;
	int i, seqcontlen, seqlen, ndef = 1;
	const ASN1_COMPAT_FUNCS *cf;
	const ASN1_EXTERN_FUNCS *ef;
	const ASN1_AUX *aux = (const ASN1_AUX *)it->funcs;
	ASN1_aux_cb *asn1_cb = 0;

	if ((it->itype != ASN1_ITYPE_PRIMITIVE) && !*pval)
		return 0;

	if (aux && aux->asn1_cb)
	{
		//UnionLog("in ASN1_item_ex_i2d_m:aux is not null.\n");
		 asn1_cb = aux->asn1_cb;
	}

	switch(it->itype)
		{
		case ASN1_ITYPE_PRIMITIVE:
		if (it->templates)
			return asn1_template_ex_i2d(pval, out, it->templates,
								tag, aclass);
		return asn1_i2d_ex_primitive(pval, out, it, tag, aclass);
		break;

		case ASN1_ITYPE_MSTRING:
		return asn1_i2d_ex_primitive(pval, out, it, -1, aclass);

		case ASN1_ITYPE_CHOICE:
		if (asn1_cb && !asn1_cb(ASN1_OP_I2D_PRE, pval, it, NULL))
				return 0;
		i = asn1_get_choice_selector(pval, it);
		if ((i >= 0) && (i < it->tcount))
			{
			ASN1_VALUE **pchval;
			const ASN1_TEMPLATE *chtt;
			chtt = it->templates + i;
			pchval = asn1_get_field_ptr(pval, chtt);
			return asn1_template_ex_i2d(pchval, out, chtt,
								-1, aclass);
			}
		/* Fixme: error condition if selector out of range */
		if (asn1_cb && !asn1_cb(ASN1_OP_I2D_POST, pval, it, NULL))
				return 0;
		break;

		case ASN1_ITYPE_EXTERN:
			//UnionLog("in ASN1_item_ex_i2d_m::it->itype = ASN1_ITYPE_EXTERN.\n");
		/* If new style i2d it does all the work */
		ef = (const ASN1_EXTERN_FUNCS *)it->funcs;
		return ef->asn1_ex_i2d(pval, out, it, tag, aclass);

		case ASN1_ITYPE_COMPAT:
		/* old style hackery... */
		cf = (const ASN1_COMPAT_FUNCS *)it->funcs;
		if (out)
			p = *out;
		i = cf->asn1_i2d(*pval, out);
		/* Fixup for IMPLICIT tag: note this messes up for tags > 30,
		 * but so did the old code. Tags > 30 are very rare anyway.
		 */
		if (out && (tag != -1))
			*p = aclass | tag | (*p & V_ASN1_CONSTRUCTED);
		return i;
		
		case ASN1_ITYPE_NDEF_SEQUENCE:

		/* Use indefinite length constructed if requested */
		if (aclass & ASN1_TFLG_NDEF) ndef = 2;
		/* fall through */

		case ASN1_ITYPE_SEQUENCE:
			
		i = asn1_enc_restore(&seqcontlen, out, pval, it);
		
		/* An error occurred */
		if (i < 0)
			return 0;
		/* We have a valid cached encoding... */
		if (i > 0)
			return seqcontlen;
		/* Otherwise carry on */
		seqcontlen = 0;
		/* If no IMPLICIT tagging set to SEQUENCE, UNIVERSAL */
		if (tag == -1)
			{
			tag = V_ASN1_SEQUENCE;
			/* Retain any other flags in aclass */
			aclass = (aclass & ~ASN1_TFLG_TAG_CLASS)
					| V_ASN1_UNIVERSAL;
			}
		if (asn1_cb && !asn1_cb(ASN1_OP_I2D_PRE, pval, it, NULL))
				return 0;
		/* First work out sequence content length */
		 
		for (i = 0, tt = it->templates; i < it->tcount; tt++, i++)
			{
			const ASN1_TEMPLATE *seqtt;
			ASN1_VALUE **pseqval;
			seqtt = asn1_do_adb_m(pval, tt, 1);
			
			if (!seqtt) {
				return 0;
			}
			 
			pseqval = asn1_get_field_ptr(pval, seqtt);
			/* FIXME: check for errors in enhanced version */
			seqcontlen += asn1_template_ex_i2d(pseqval, NULL, seqtt,
								-1, aclass);
			 
			}

		seqlen = ASN1_object_size(ndef, seqcontlen, tag);
		if (!out) {
			return seqlen;
		}
		/* Output SEQUENCE header */
		ASN1_put_object(out, ndef, seqcontlen, tag, aclass);
		for (i = 0, tt = it->templates; i < it->tcount; tt++, i++)
			{
			const ASN1_TEMPLATE *seqtt;
			ASN1_VALUE **pseqval;
			seqtt = asn1_do_adb_m(pval, tt, 1);
			if (!seqtt)
				return 0;
			pseqval = asn1_get_field_ptr(pval, seqtt);
			/* FIXME: check for errors in enhanced version */
			asn1_template_ex_i2d(pseqval, out, seqtt, -1, aclass);
			}
		if (ndef == 2) {
			ASN1_put_eoc(out);
		}
		if (asn1_cb  && !asn1_cb(ASN1_OP_I2D_POST, pval, it, NULL))
		{
				return 0;
		}
		return seqlen;

		default:
		return 0;

		}
	return 0;
	}


static int asn1_item_flags_i2d(ASN1_VALUE *val, unsigned char **out,
					const ASN1_ITEM *it, int flags)
	{
	if (out && !*out)
		{
		unsigned char *p, *buf;
		int len;
		len = ASN1_item_ex_i2d_m(&val, NULL, it, -1, flags);
		if (len <= 0)
			return len;
		buf = (unsigned char *)OPENSSL_malloc(len);
		if (!buf)
			return -1;
		p = buf;
		ASN1_item_ex_i2d_m(&val, &p, it, -1, flags);
		*out = buf;
		return len;
		}
	return ASN1_item_ex_i2d_m(&val, out, it, -1, flags);
	}

int ASN1_item_i2d_m(ASN1_VALUE *val, unsigned char **out, const ASN1_ITEM *it)
	{
	return asn1_item_flags_i2d(val, out, it, 0);
	}

ASN1_VALUE *ASN1_item_d2i_m(ASN1_VALUE **pval,
		const unsigned char **in, long len, const ASN1_ITEM *it)
	{
	ASN1_TLC c;
	ASN1_VALUE *ptmpval = NULL;
	 
	if (!pval)
		pval = &ptmpval;
	asn1_tlc_clear(&c);
	if (ASN1_item_ex_d2i_m(pval, in, len, it, -1, 0, 0, &c) > 0) 
		return *pval;
	return NULL;
	}
	

int i2d_PKCS7_Ex(PKCS7 *a, unsigned char **out) 
{ 
	return ASN1_item_i2d_m((ASN1_VALUE *)a, out, ASN1_ITEM_rptr(PKCS7));
} 

PKCS7 *d2i_PKCS7_Ex(PKCS7 **a, const unsigned char **in, long len) 
{  
	return (PKCS7 *)ASN1_item_d2i_m((ASN1_VALUE **)a, in, len, ASN1_ITEM_rptr(PKCS7));    
} 


 

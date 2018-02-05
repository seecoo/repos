// Date:	2002/06/20

#ifndef	__UNION_MD5__
#define	__UNION_MD5__

int UnionMD5(unsigned char *pData,	/*	待压缩串(字节数组)  */
	unsigned long cbData,		/*	待压缩串长度  */
	unsigned char *pDigest);	/*	压缩结果串(字节数组)  */

int UnionMD5File(char *fileName,unsigned char *pDigest);

#endif

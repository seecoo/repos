// Date:	2002/06/20

#ifndef	__UNION_MD5__
#define	__UNION_MD5__

int UnionMD5(unsigned char *pData,	/*	��ѹ����(�ֽ�����)  */
	unsigned long cbData,		/*	��ѹ��������  */
	unsigned char *pDigest);	/*	ѹ�������(�ֽ�����)  */

int UnionMD5File(char *fileName,unsigned char *pDigest);

#endif

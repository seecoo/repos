// Author:	zhangyd
// Date:	2015/1/8

// ����ļ������˱��ĸ�ʽ

#ifndef _unionPackage_
#define _unionPackage_

/*
#define conDataFormatIsAscii	1
#define conDataFormatIsBinary	2

#define conPackIsRequest	0
#define conPackIsResponse	1
#define conPackIsData		2
*/

#define PACKAGE_VERSION_001		"V001"
#define PACKAGE_MEMORY_BLOCK_SIZE	32

// ���¶���Ԫ������
typedef enum
{
	PACKAGE_ELEMENT_TAG = 2,
	PACKAGE_ELEMENT_VALUE = 4
} TUnionPackageElementType;

// ���¶���һ��Ԫ�صĽṹ
typedef struct
{
	unsigned char		*pos;		// Ԫ�ص�λ��
	int			len;		// Ԫ�صĳ���
} TUnionPackageElement;
typedef TUnionPackageElement	*PUnionPackageElement;

// ���¶���һ����Ľṹ
typedef struct
{
	TUnionPackageElement	tag;		// ��ı�ǩ
	TUnionPackageElement	value;		// ���ֵ
} TUnionPackageFld;
typedef TUnionPackageFld	*PUnionPackageFld;

// ���¶���һ����Ľṹ
#define conMaxNumOfPackageFld	99
typedef struct
{
	int			num;				// ����
	int			index;
	TUnionPackageElement	fldGrpID;			// ��ID
	TUnionPackageFld	fld[conMaxNumOfPackageFld];	// ���ֵ
} TUnionPackageGrp;
typedef TUnionPackageGrp	*PUnionPackageGrp;

// ���¶��屨��
#define conMaxNumOfPackageGrp	9
#define PACKAGE_BUF_SIZE	8192

typedef struct
{
	char			version[PACKAGE_MEMORY_BLOCK_SIZE];
	int			num;				// ����Ŀ
	int			index;
	TUnionPackageGrp	fldGrp[conMaxNumOfPackageGrp];	// ����

	long			bufSize;			// ����Ĵ�С
	unsigned char		*bufbase;			// ����
	unsigned char		*bufptr ;			
	long			buf_remain_len;			// ��ǰ���û����ƫ��
} TUnionPackage;
typedef TUnionPackage		*PUnionPackage;

int UnionPackageConvertIntStringToInt(unsigned char *str,int lenOfStr);

/*
����
	��һ��Ԫ�ش��뵽����,���ش��뵽���е����ݵĳ���
�������
	ppackage	��ָ��
	buf		����ָ��
	len		���ݴ�С
�������
	buf		����
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionPackageFormatBuffer(PUnionPackage ppackage,PUnionPackageElement pelement,unsigned char *buf,int len);

/*
����
	��һ��Ԫ�ش��뵽����,���ش��뵽���е����ݵĳ���
�������
	pelement	Ԫ��ָ��
	elementType	Ԫ������
	sizeOfBuf	��������С
�������
	buf		����
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionPutPackageElementIntoStr(PUnionPackageElement pelement,TUnionPackageElementType elementType,char *buf,int sizeOfBuf);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	tag		TAG
	lenOfTag	TAG�ĳ���
	value		VALUE
	lenOfValue	VALUE�ĳ���
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ���ƫ����
	<0	������
*/
int UnionPutPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int lenOfValue);
 
/*
����
	��һ�������ж�ȡһ��Ԫ�أ��������ڰ���ռ�ĳ���
�������
	ppackage	��ָ��
	data		���ݰ�
	len		���ݰ�����
	elementType	Ԫ������
�������
	pelement	Ԫ��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageElementFromStr(PUnionPackage ppackage,unsigned char *data,int len,TUnionPackageElementType elementType,PUnionPackageElement pelement);

/*
����
	��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
�������
	data		���ݰ�
	len		���ݰ�����
�������
	pfld		��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageFldFromStr(PUnionPackage ppackage,unsigned char *data,int len,PUnionPackageFld pfld);

/*
����
	��ʼ������
�������
	ppackage		���Ľṹ��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackage(PUnionPackage ppackage);

/*
����
	�ͷű���
�������
	ppackage		���Ľṹ��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionFreePackage(PUnionPackage ppackage);

/*
����
	�Ӱ��ж�һ��Ԫ��
�������
	pelement	Ԫ��ָ��
	element		Ԫ��
	sizeOfBuf	Ԫ�ص�ֵ�Ļ����С
�������
	value		Ԫ�ص�ֵ	
����ֵ
	>=0	�ɹ�������Ԫ�س���
	<0	������
*/
int UnionReadPackageElement(PUnionPackageElement pelement,char *value,int sizeOfBuf);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	tag		TAG��ʶ
	lenOfTag	TAG�ĳ���
	sizeOfBuf	VALUE��ֵ�Ļ����С
�������
	value		VALUE��ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int sizeOfBuf);

/*
����
	����д����־
�������
	ppackage	��ָ��
�������
	��
����ֵ
	��
*/
void UnionLogPackage(PUnionPackage ppackage);

/*
����
	�������뵽����
�������
	ppackage	��ָ��
	sizeOfBuf	�����С
�������
	buf		����ɵĴ�
����ֵ
	>=0	�ɹ�������ɵĴ�����
	<0	������
*/
int UnionPackPackage(PUnionPackage ppackage,char *buf,int sizeOfBuf);

/*
����
	��һ�����⵽����
�������
	data		��
	lenOfData	������
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionUnpackPackage(unsigned char *data,int lenOfData,PUnionPackage ppackage);

/*
����
	���ñ��İ汾
�������
	ppackage	��ָ��
	version		�汾
	len		�汾����
�������

����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPackageSetVersion(PUnionPackage ppackage,char *version,int len);
/*
����    
        ��һ����������뵽����,���ش��뵽���е����ݵĳ���
�������
        pfld            ��ָ��
        sizeOfBuf       ��������С
�������
        buf             ����
����ֵ  
        >=0     ���ڰ��е��ֽ���
        <0      ������
*/      
int UnionPutPackageFldIntoStr(PUnionPackageFld pfld,char *buf,int sizeOfBuf);

/*  2015-08-26
����
        ��հ���ĳ����ֶ�����
�������
        ppackage        ��ָ��
        groupName       ����
�������
        ��
����ֵ
        >=0     �ɹ�������ɵĴ�����
        <0      ������
*/
int UnionClearPackageGrpByName(PUnionPackage ppackage, char *groupName);

#endif

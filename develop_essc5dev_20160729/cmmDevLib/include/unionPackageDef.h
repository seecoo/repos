// Author:	Wolfgang Wang
// Date:	2006/8/9

// ����ļ������˱��ĸ�ʽ

#ifndef _unionPackageDef_
#define _unionPackageDef_

#define conDataFormatIsAscii	1
#define conDataFormatIsBinary	2

#define conPackIsRequest	0
#define conPackIsResponse	1
#define conPackIsData		2

// ���������󳤶�
#define conMaxPackSizeOfEngine	16384

// ���¶���һ����Ľṹ
typedef struct
{
	int		tag;		// ��ı�ʶ��
	int		len;		// ��ĳ���
	int		dataFormat;	// ���뷽ʽ
	unsigned char	*value;		// ���ֵ
} TUnionPackageFld;
typedef TUnionPackageFld	*PUnionPackageFld;

#define conMaxNumOfPackageFld	32
// ���¶��屨��
typedef struct
{
	int	direction;		// �����ʶ��0����1��Ӧ
	int	serviceID;		// �������
	int	responseCode;		// ��Ӧ��,ֻ�е�direcion=1ʱ�����и���
	int	fldNum;			// ����Ŀ
	TUnionPackageFld	fldGrp[conMaxNumOfPackageFld];	// ����
	int	offset;			// ��ǰ���û����ƫ��
	unsigned char	dataBuf[conMaxPackSizeOfEngine+1024+1];	// ����
} TUnionPackage;
typedef TUnionPackage	*PUnionPackage;

/*
����
	��һ����������뵽����,���ش��뵽���е����ݵĳ���
�������
	pfld		��ָ��
	sizeOfBuf	��������С
�������
	buf		����
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionPutPackageFldIntoStr(PUnionPackageFld pfld,char *buf,int sizeOfBuf);
 
/*
����
	��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
�������
	data		���ݰ�
	len		���ݰ�����
	sizeOfFldValue	pfld��valueֵ�Ļ����С
�������
	pfld		��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageFldFromStr(char *data,int len,PUnionPackageFld pfld,int sizeOfFldValue);

/*
����
	��ʼ������
�������
	pfld		��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackage(PUnionPackage ppackage);

/*
����
	��ʼ�����ĳ�һ��������
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsRequest(PUnionPackage ppackage,int serviceID);

/*
����
	��ʼ�����ĳ�һ�����ݱ���
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsDataBlock(PUnionPackage ppackage,int serviceID);

/*
����
	��ʼ�����ĳ�һ����Ӧ����
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsResponse(PUnionPackage ppackage,int serviceID);
/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	dataFormat	��ı��뷽ʽ
	len		��ĳ���
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutPackageFld(PUnionPackage ppackage,int fldTag,int dataFormat,int len,char *value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	dataFormat	��ı��뷽ʽ
	len		��ĳ���
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutBitsTypePackageFld(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
	len		���ݵĳ���
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutStrTypePackageFldWithLen(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutIntTypePackageFld(PUnionPackage ppackage,int fldTag,int value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutLongTypePackageFld(PUnionPackage ppackage,int fldTag,long value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value);

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	dataFormat	��ı��뷽ʽ
	len		��ĳ���
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutBitsTypePackageFld(PUnionPackage ppackage,int fldTag,int len,char *value);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	dataFormat	��ı��뷽ʽ
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadPackageFld(PUnionPackage ppackage,int fldTag,int *dataFormat,char *value,int sizeOfBuf);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadBitsTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadIntTypePackageFld(PUnionPackage ppackage,int fldTag,int *value);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadLongTypePackageFld(PUnionPackage ppackage,int fldTag,long *value);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value);

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadDoubleTypePackageFld(PUnionPackage ppackage,int fldTag,double *value);

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
int UnionUnpackPackage(char *data,int lenOfData,PUnionPackage ppackage);

#endif

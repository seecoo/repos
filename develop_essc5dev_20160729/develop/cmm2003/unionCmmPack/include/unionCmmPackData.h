// Author:	Wolfgang Wang
// Date:	2008/9/26

// ����ļ�������ESSC���ĸ�ʽ

#ifndef _unionCmmPackData_
#define _unionCmmPackData_

// ���¶���һ�����ֵ��λ��
typedef struct
{
	int		tag;		// ��ı�ʶ��
	int		len;		// ��ĳ���
	unsigned char	*value;		// ��ֵ��λ��
} TUnionCmmPackFldData;
typedef TUnionCmmPackFldData	*PUnionCmmPackFldData;

#define conMaxNumOfFldPerCmmPack	128
// ����һ�������λ�ñ�
typedef struct
{
	TUnionCmmPackFldData	fldGrp[conMaxNumOfFldPerCmmPack];	// ����
	int			fldNum;					// �������������
	char			data[8192+1];				// ����
	int			len;					// ���ݳ���
	int			offset;					// ��ǰƫ��
} TUnionCmmPackData;
typedef TUnionCmmPackData	*PUnionCmmPackData;

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutCharTypeCmmPackFldIntoStr(int tag,char value,int sizeOfBuf,char *buf);

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutDoubleTypeCmmPackFldIntoStr(int tag,double value,int sizeOfBuf,char *buf);

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutLongTypeCmmPackFldIntoStr(int tag,long value,int sizeOfBuf,char *buf);

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutIntTypeCmmPackFldIntoStr(int tag,int value,int sizeOfBuf,char *buf);

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	lenOfValue	��ֵ����
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutCmmPackFldIntoStr(int tag,char *value,int lenOfValue,int sizeOfBuf,char *buf);

// ��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	sizeOfBuf	������ֵ�Ļ����С
�������
	tag		���ʶ
	value		��ֵ
	fldValue	��ֵ����
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionReadCmmPackFldFromStr(char *data,int len,int sizeOfBuf,int *tag,int *fldLen,char *value);

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecIntTypeCmmPackFldFromStr(char *data,int len,int tag,int *value);

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecLongTypeCmmPackFldFromStr(char *data,int len,int tag,long *value);

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecDoubleTypeCmmPackFldFromStr(char *data,int len,int tag,double *value);

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecCharTypeCmmPackFldFromStr(char *data,int len,int tag,char *value);



// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
	sizeOfBuf	������ֵ�Ļ����С
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecCmmPackFldFromStr(char *data,int len,int tag,int sizeOfBuf,char *value);

// ��ʼ����ֵλ�ñ�
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
	sizeOfBuf	������ֵ�Ļ����С
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionInitCmmPackFldDataList(PUnionCmmPackData pposList);
	
// ���������ֵ�н����һ����ֵλ���嵥��
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionUnpackCmmPackIntoFldDataList(char *data,int len,PUnionCmmPackData pposList);

// �������һ����ֵд��һ����ֵλ���嵥��
/*
�������
	len		���ݴ��ĳ���
	tag		���ʶ
	value		���ݴ���ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutCmmPackFldIntoFldDataList(int tag,int len,char *value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutIntTypeCmmPackFldIntoFldDataList(int tag,int value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutLongTypeCmmPackFldIntoFldDataList(int tag,long value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutCharTypeCmmPackFldIntoFldDataList(int tag,char value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList(int tag,unsigned int value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList(int tag,unsigned long value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList(int tag,unsigned char value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutStringTypeCmmPackFldIntoFldDataList(int tag,char *value,PUnionCmmPackData pposList);

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutDoubleTypeCmmPackFldIntoFldDataList(int tag,double value,PUnionCmmPackData pposList);

// ���������ֵ�н����һ����ֵλ���嵥��
/*
�������
	pposList	��ֵλ���嵥
	sizeOfBuf	����Ĵ�С
�������
	data		�����ڵ����ݴ�
����ֵ
	>=0		���ݰ��ĳ���
	<0		�������
*/
int UnionPackFldDataListIntoCmmPack(PUnionCmmPackData pposList,int sizeOfBuf,char *data);

// ����������д�뵽��־��
/*
�������
	title		����
	pposList	��ֵλ���嵥
�������
	��
����ֵ
	��
*/
void UnionLogCmmPackData(char *title,PUnionCmmPackData pposList);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	sizeOfBuf	��ֵ���д�С
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int sizeOfBuf,char *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,long *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,char *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned int *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned long *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned char *value);

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadDoubleTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,double *value);


// ���Ժ���
int UnionTestCmmPackDataFun(char *data,int len);

// ���Ʊ���
/*
�������
	poriList	Դ��
�������
	pdesList	Ŀ���
����ֵ
	>=0		����Ŀ
	<0		�������
*/
int UnionCopyPackFldDataList(PUnionCmmPackData poriList,PUnionCmmPackData pdesList);

#endif

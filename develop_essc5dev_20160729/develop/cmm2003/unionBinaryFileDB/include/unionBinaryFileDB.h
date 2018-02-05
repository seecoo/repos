#ifndef  _BINARYFILEDB_
#define  _BINARYFILEDB_

typedef struct
{
	char	fldName[20+1];
	char	fldType[20+1];
	char	fldLen[4+1];
}TUnionTableDef;
typedef TUnionTableDef *PUnionTableDef; 

/******************************
 * ����һ��¼
 * input:  tbl - ����
 *         primaryKey - Ψһ��������ʽ��"�ؼ���1=ֵ|...|�ؼ���n=ֵ|"
 *         data - Ҫ����Ľṹ��ָ��
 *         sizeOfData - �ṹ���С
 * output:
 * return: =0;�ɹ�
 *         <0;ʧ��
******************************/
int UnionInsertIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * ɾ��һ����¼
 * input:  tbl - ����
 *         primaryKey - Ψһ��������ʽ��"�ؼ���1=ֵ|...|�ؼ���n=ֵ|"
 *         sizeOfData - �ṹ���С
 * output:
 * return: =0;�ɹ�
 *         <0;ʧ��
******************************/
int UnionDeleteByPrimaryKey(char *tbl, char *primaryKey, size_t sizeOfData);


/******************************
 * �޸�һ����¼
 * input:  tbl - ����
 *         primaryKey - Ψһ��������ʽ��"�ؼ���1=ֵ|...|�ؼ���n=ֵ|"
 *         data - �ṹ��ָ��
 *         sizeOfData - �ṹ���С
 * output:
 * return: =0;�ɹ�
 *         <0;ʧ��
******************************/
int UnionUpdateIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * ��ѯһ����¼
 * input:  tbl - ����
 *         primaryKey - Ψһ��������ʽ��"�ؼ���1=ֵ|...|�ؼ���n=ֵ|"
 *         sizeOfData - �ṹ���С
 * output: data - �ṹ��ָ��
 * return: =0;�ɹ�
 *         <0;ʧ��
******************************/
int UnionSelectSpecFromDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * ������ѯ��¼
 * input:  tbl - ����
 *         condition - ��������ʽ��"�ֶ�1=ֵ|...|�ֶ�n=ֵ|"
 *         sizeOfData - �ṹ���С
 *         outFile - ����ļ���
 * output:
 * return: >=0;�ɹ��������ܲ�ѯ������
 *         <0;ʧ��
******************************/
int UnionSelectBatchFromDB(char *tbl, char *condition, size_t sizeOfData, char *outFile);

/******************************
 * ��һ���ṹ��ת��Ϊһ���ַ���
 * input:  tbl - ����
 *         data - �ṹ��
 * output: resStr - �ַ�������ʽΪ: "�ֶ�1=ֵ|...|�ֶ�n=ֵ|"
 * return: >=0;�ɹ�������resStr�ĳ���
 *         <0;ʧ��
******************************/
int UnionTransStructToString(char *tbl, unsigned char *data, char *resStr);


#endif


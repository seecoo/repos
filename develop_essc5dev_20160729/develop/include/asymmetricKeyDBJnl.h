#ifndef _AsymmetricKeyDBJnl_H_
#define _AsymmetricKeyDBJnl_H_

// ����Գ���Կ�����
typedef enum 
{
	conAsymmetricKeyDBOperationInsert,
	conAsymmetricKeyDBOperationDelete,
	conAsymmetricKeyDBOperationUpdate
} TUnionAsymmetricKeyDBOperation;

/* 
���ܣ�	�ǼǷǶԳ���Կ������־
������	operation[in]		������ʶ
	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInsertAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	��ȡ�ǼǷǶԳ���Կ������־��SQL���
������	operation[in]		������ʶ
	pasymmetricKeyDB[in]	��Կ������Ϣ
	sql[out]		sql������
	sizeOfSql		sql��������С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetSqlForAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB,char *sql,int sizeOfSql);

#endif


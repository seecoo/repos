#ifndef _SymmetricKeyDBJnl_H_
#define _SymmetricKeyDBJnl_H_

// ����Գ���Կ�����
typedef enum 
{
	conSymmetricKeyDBOperationInsert,
	conSymmetricKeyDBOperationDelete,
	conSymmetricKeyDBOperationUpdate
} TUnionSymmetricKeyDBOperation;

/* 
���ܣ�	�ǼǶԳ���Կ������־
������	operation[in]		������ʶ
	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInsertSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	��ȡ�ǼǶԳ���Կ������־��SQL���
������	operation[in]		������ʶ
	psymmetricKeyDB[in]	��Կ������Ϣ
	sql[out]		sql������
	sizeOfSql		sql��������С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetSqlForSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB,char *sql,int sizeOfSql);

#endif


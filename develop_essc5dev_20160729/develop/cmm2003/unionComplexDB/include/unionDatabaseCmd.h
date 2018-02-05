#ifndef _unionDatabaseCmd_
#define _unionDatabaseCmd_

#define conSpecTBLIDOfOperator		7	//����Ա��

// ���¶�����Դ��������
#define conDatabaseCmdInsert		1	// �����¼
#define conDatabaseCmdDelete		2	// ɾ����¼
#define conDatabaseCmdUpdate		3	// ����������¼
#define conDatabaseCmdLock		10	// ��ס��¼
#define conDatabaseCmdUnLock		11	// ������¼
#define conDatabaseCmdUnLockAllRec	12	// �������м�¼
#define conEsscRecCmdUpdate		20	// ����������¼
#define conEsscRecCmdUpdateAttrFld	21	// ����������
#define conEsscRecCmdUpdateValueFld	22	// ����ֵ��
#define conDatabaseCmdRead		50	// ��ȡ��¼
#define conDatabaseCmdQuerySpecRec	conDatabaseCmdRead	// ��ȡ��¼
#define conDatabaseCmdQueryAllRec	51	// ��ѯ���м�¼
#define conDatabaseCmdQueryWithCondition	52	// ��ѯ�����������м�¼
#define conDatabaseCmdQuerySpecRecWithUnion	53	// ��ѯ���м�¼
#define conDatabaseCmdQueryAllRecWithUnion	54	// ��ѯ�����������м�¼
#define conDatabaseCmdQueryByRealSQL		55	// ������ʵSQL��ѯ
#define conDatabaseCmdQueryBySpecFieldList	56	// ��ѯָ���嵥��Ӧ������
#define conDatabaseCmdConditionDelete	62	// ɾ�����������ļ�¼
#define conDatabaseCmdConditionUpdate	63	// ���·��������ļ�¼

// ������֪ͨ����
// ������ϵͳ�������
#define conDatabaseCmdApplicationStoped		999	// Ӧ�ó���ֹͣ����

// �����Ǽ������
#define conDatabaseCmdSendingSpiedRec		100	// ���ͼ�ؼ�¼
#define conDatabaseCmdStartSendingSpiedRec	98	// ��ʼ���ͼ�ؼ�¼
#define conDatabaseCmdFinishSendingSpiedRec	99	// �������ͼ�ؼ�¼

// �������ļ���������
#define conDatabaseCmdStartSendingData		200	// ��ʼ�����ļ�����
#define conDatabaseCmdSendingData		201	// �������ݼ�¼
#define conDatabaseCmdFinishSendingData		202	// ���������ļ�����
#define conDatabaseCmdDownloadFile		203	// ���������ļ�
#define conDatabaseCmdUploadFile			204	// ���������ļ�
#define conDatabaseCmdReloadResFile		205	// �ڷ����������¼�����Դ�ļ�
#define conDatabaseCmdQueryResStatus		206	// ��ѯ��Դ��״̬
#define conDatabaseCmdUnlockRes			207	// ������Դ

// ����ĳ�������������ɲ�ͬ����Դ���и������������塣
#define conDatabaseCmdSpec001		101	
#define conDatabaseCmdSpec002		102	
#define conDatabaseCmdSpec003		103	
#define conDatabaseCmdSpec004		104	
#define conDatabaseCmdSpec005		105	
#define conDatabaseCmdSpec006		106	
#define conDatabaseCmdSpec007		107
#define conDatabaseCmdSpec008		108
#define conDatabaseCmdSpec009		109
#define conDatabaseCmdSpec010		110
#define conDatabaseCmdSpec011		111

#define conDatabaseCmdReserved01	9999	// ��������
#define conDatabaseCmdReserved02	9998	// ��������
#define conDatabaseCmdReserved03	9997	// ��������

// ��ؿͻ��˿��Ʊ����������
// ����Ա��¼
#define conDatabaseSpecCmdOfOperatorTBL_logon		conDatabaseCmdSpec001
// ����Ա�˳���¼
#define conDatabaseSpecCmdOfOperatorTBL_logoff		conDatabaseCmdSpec002
// ǿ�Ʋ���Ա�˳���¼
#define conDatabaseSpecCmdOfOperatorTBL_ForceLogoff	conDatabaseCmdSpec003

#endif

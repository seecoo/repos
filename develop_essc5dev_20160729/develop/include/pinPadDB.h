#ifndef _PinPadDB_H_
#define _PinPadDB_H_

#include "unionDesKey.h"
#include "commWithHsmSvr.h"

#define	defTableNameOfPinPadDB		"pinPad"
#define	APPNOOFITMK			"ITMK"
#define	APPNOOFKMC			"HOST"

// ����Գ���Կ״̬��ʶ
typedef enum 
{
	conPinPadStatusOfInitial,		// ��ʼ״̬
	conPinPadStatusOfEnabled,		// ����״̬
	conPinPadStatusOfSuspend,		// ����״̬
} TUnionPinPadStatus;

// ����������̽ṹ��
typedef struct
{
	char				pinPadID[32];			// ����ID
	char				regTime[16];			// �Ǽ�ʱ��
	char				lastTime[16];			// ����޸�ʱ��
	char				status[4];			// ״̬
	char				branch[16];			// ���к�	
	char				teller[16];			// ����Ա
	char				producter[48];			//��������
	char				producterID[48];		//�������к�
	int				zmkLength;			//zmk��Կ����
	int				zpkLength;			//zpk��Կ����
	int				zakLength;			//zak��Կ����
	char				appNo[16];			//Ӧ��ID
	char				remark[128];			// ��ע
} TUnionPinPadDB;
typedef TUnionPinPadDB		*PUnionPinPadDB;

/* 
���ܣ�	�����������
������	ppinPadDB[in]	������̽ṹ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInsertPinPadDB(PUnionPinPadDB ppinPadDB);

/* 
���ܣ�	ɾ��һ���������
������	ppinPadDB[in]	������̽ṹ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionDropPinPadDB(PUnionPinPadDB ppinPadDB);

/* 
���ܣ�	��ȡһ���Գ���Կ����
������	pinPadID[in]		�������ID
	ppinPadDB[out]		������̽ṹ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionReadPinPadDB(char *pinPadID,PUnionPinPadDB ppinPadDB);

/* 
���ܣ�	����һ���������
������	ppinPadDB[in]	������̽ṹ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdatePinPadDB(PUnionPinPadDB ppinPadDB);


/* 
���ܣ�  ��ȡһ���������
������  pinPadID[in]            ����ID
        ppinPadDB[out]
����ֵ��>=0                     �ɹ�
        <0                      ʧ�ܣ����ش�����
*/
int UnionReadPinPadDB(char *pinPadID,PUnionPinPadDB ppinPadDB);

/* 

���ܣ�  ��ȡ�к�
������  branch[out]     �����к�

*/
void UnionGetPinPadBranch(char *branchID);


/*

����:   ȡ���з��б�־ 
����ֵ:  <0     ʧ�ܣ����ش�����
         >=0    �ɹ������سɹ�ֵ
*/
int UnionGetPinPadBranchType();


/*

����:   ��ȡԶ�̱�־
����ֵ:         <0      ʧ��,���ش�����
                >=0     �ɹ�

*/
int UnionGetPinPadRemoteDistributeID(char *distributeID);


// ��ȡ����������Լ������Կ
int UnionGetPinPadZmkName(char *zmkName);

// ɾ��������Կ
int UnionDeleteKeyOfPinPad(char *appNo,char *pinPadID);

// ����zmk\zpk\zak��Կ
int UnionPinPadInsertDesKeyIntoKeyDB(char *appNo,char *pinPadID,int zmkLength,int zpkLength,int zakLength,char *branch);

//�������������Կ��Ч
int UnionActiveKeyOfPinPad(char *appNo, char *pinPadID);


//������ԿʧЧ
int UnionInActiveKeyOfPinPad(char *appNo, char *pinPadID);

// ��ȡ���еط����絽�ļ�
int UnionReadQueryAllBranchToFile(char *fileName);

// д���г��̵��ļ�
int UnionReadQueryAllProducterToFile(char *fileName);

// д���в����ߵ��ļ�
int UnionReadQueryAllOperatorToFile(char *fileName);

// ��ȡ������Ϣ
int UnionReadPinPadProducterRec(char *producter,char *dllName,int sizeofDllName,char *remark,int sizeofRemark);
/*      
���ܣ�  ������ȡ�������
������  branch[in]            ����ID
����ֵ��>=0                     �ɹ�
        <0                      ʧ�ܣ����ش�����
*/
int UnionReadPinPadDBbranch(char *branch);

// ��ȡ���������Լ������Կ
int UnionGetPinPadZmkNameWithBranch(char *zmkName);

// ���в���ʧ�ܻع�
int UnionPinPadOperateRollback(char *branch,char *status);

/* 
���ܣ�  ��ȡ�ط�������������
������  branchNo[in]            branchNo
        sizeofBranchName[in]    ����
        branchName[out]         �ط�����������

����ֵ��>=0                     �ɹ�
        <0                      ʧ�ܣ����ش�����
*/
int UnionGetBranchNameByBranchNo(char *branchNo,char *branchName,int sizeofBranchName);
#endif


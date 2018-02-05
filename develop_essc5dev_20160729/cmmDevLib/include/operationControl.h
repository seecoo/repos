// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#ifndef _operationControl_
#define _operationControl_

typedef struct
{
	int	resID;		// ��ԴID
	int	resCmd;		// ��Դ����
	long	roles;		// ���Բ����Ľ�ɫ��ÿһλ�����ִ���һ����ɫ��
				// ֵΪ0ʱ����ʾ�����Ʋ�����Ϊ����ʱ����ʾ��ֹ�κν�ɫ������
				// 321����ʾ��ɫ3��2��1���Բ�����149����ʾ��ɫ1��4��9�ɲ���
	char	remark[40+1];	// ����˵��
} TUnionOperationControlRec;
typedef TUnionOperationControlRec	*PUnionOperationControlRec;

// ������������
int UnionGetMngSvrOperationType(int resID,int resCmd);

int UnionGetDefFileNameOfOperationController(char *fileName);

// ��ȡ��Դ������˵��
char *UnionGetOperationRemark(int resID,int operationID);

// ���ַ����ж�����������
int UnionReadOperationFromDefStr(char *str,PUnionOperationControlRec prec);
// ���ļ��ж�ȡ����
int UnionReadSpecOperationFromDefFile(int resID,int resCmd,PUnionOperationControlRec prec);
// ���ļ��ж�ȡ����
int UnionReadSpecOperationFromSpecDefFile(char *fileName,int resID,int resCmd,PUnionOperationControlRec prec);

// �жϽ�ɫ�����ǲ�����Ȩ��
int UnionIsAuthorizedRoleLevel(long roles,char roleLevel);

// ��֤��ɫ�����ܷ�����������
int UnionVerifyOperation(int resID,int resCmd,char roleLevel);

// ��֤��ɫ�����ܷ�����������
int UnionVerifyOperationInSpecFile(char *fileName,int resID,int resCmd,char roleLevel);

int UnionPrintOperationToFile(PUnionOperationControlRec prec,FILE *fp);

int UnionPrintAllSpecOperationToFile(FILE *outputFP,int resID,int resCmd,char roleLevel);

int UnionPrintAllSpecOperationToSpecFile(char *fileName,int resID,int resCmd,char roleLevel);

#endif

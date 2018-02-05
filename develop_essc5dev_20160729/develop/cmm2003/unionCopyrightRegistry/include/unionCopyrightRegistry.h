//	Wolfgang Wang
//	2003/1/19

#ifndef _UnionCopyrightRegistry_
#define _UnionCopyrightRegistry_

#include <stdio.h>
#include <string.h>

typedef struct
{
	char	abbrName[20+1];		// ��Ʒ���
	char	primaryName[80+1];	// ��Ʒ������
	char	minorName[80+1];	// ��Ʒ������
	char	version[20+1];		// ��Ʒ�汾��
	char	userName[80+1];		// �û����ƣ�����Ʒʹ��������
	int	userNumber;		// ֧�ֵ�����û���
	char	serialNumber[16+1];	// ��Ʒ�����к�
	char	verificationCode[16+1];	// ��Ʒ����֤��
	char	lisenceData[16+1];	// ��Ʒ����֤����
	char	fixedLisenceData[64+1];	// ��Ʒ�Ĺ̻���֤����
	char	registerDateTime[14+1];	// ��������
} TUnionCopyrightRegistry;
typedef TUnionCopyrightRegistry	*PUnionCopyrightRegistry;

int UnionVerifyCopyrightRegistryUserNumber(int userNumber);
int UnionInitCopyrightRegistryStaticData(PUnionCopyrightRegistry pCopyrightRegistry,char *abbrProductName);
int UnionOrganizeCopyrightRegistryDynamicData(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionStoreCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry,char *nameOfProduct);
int UnionGenerateCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);
int UnionVerifyCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionReadUserCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionVerifyUserCopyrightRegistry(char *abbrProductName);
int UnionGenerateUserCopyrightRegistry(char *abbrProductName);

#ifdef _UnionCopyrightRegistry_2_x_
int UnionPrintCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);
#endif


#endif

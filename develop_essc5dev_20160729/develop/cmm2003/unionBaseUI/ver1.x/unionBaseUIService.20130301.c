//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "commWithHsmSvr.h"
#include "baseUIService.h"
#include "registerAuditAndApproval.h"
#include "UnionStr.h"

#define	defMaxDlHandleNum	2048	

typedef int funServiceCode(PUnionHsmGroupRec);

typedef struct
{
	unsigned long	id;
	char		serviceCode[32];
	funServiceCode	*pfunServiceCode;
} TUnionServiceCodeDlHandle;

TUnionServiceCodeDlHandle	gunionServiceCodeDlHandle[defMaxDlHandleNum];
//int				gunionCurrentDlHandleNum = 0;
int				gunionIsInitServiceCodeDlHandle = 0;

typedef struct
{
	unsigned long	id;
	char		dlName[32];
	void		*handle;
}TUnionSOHandle;

#define	defMaxSONum	1024
TUnionSOHandle			gunionSOHandle[defMaxSONum];
//int				gunionCurrentSONum = 0;
int				gunionIsInitSOHandle = 0;

int UnionDealSpecServiceCodeTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	unsigned long	i,j,k;
	int	ret;
	char	dlName[128];
	char	fullDLName[128];
	char	funName[128];

	if (gunionIsInitServiceCodeDlHandle == 0)
	{
		for (i = 0; i < defMaxDlHandleNum; i++)
		{
			gunionServiceCodeDlHandle[i].id = -1;
			memset(gunionServiceCodeDlHandle[i].serviceCode,0,sizeof(gunionServiceCodeDlHandle[i].serviceCode));
			gunionServiceCodeDlHandle[i].pfunServiceCode = NULL;
		}
		gunionIsInitServiceCodeDlHandle = 1;
	}

	if (gunionIsInitSOHandle == 0)
	{
		for (j = 0; j < defMaxSONum; j++)
		{
			gunionSOHandle[j].id = -1;
			memset(gunionSOHandle[j].dlName,0,sizeof(gunionSOHandle[j].dlName));
			gunionSOHandle[j].handle = NULL;
		}
		gunionIsInitSOHandle = 1;
	}

	i = UnionHashTableHashString(serviceCode,0);
	i %= defMaxDlHandleNum;
	//id = UnionHashTableHashString(serviceCode, 1);
	for (k = 0;; k++)
	{
		if (k == defMaxDlHandleNum)
		{
			UnionUserErrLog("in UnionDealSpecServiceCodeTask:: fun number[%d] is too big!\n",defMaxDlHandleNum);
			return(errCodeFileTransSvrInvalidServiceCode);
		}
		
		/*
		if (gunionServiceCodeDlHandle[i].id == id)
			goto dlFunTag;
		else if (gunionServiceCodeDlHandle[i].id == -1)
		{
			gunionServiceCodeDlHandle[i].id = id;
			break;
		}
		*/
		if (strcmp(gunionServiceCodeDlHandle[i].serviceCode,serviceCode) == 0)
			goto dlFunTag;
		else if (gunionServiceCodeDlHandle[i].id == -1)
		{
			gunionServiceCodeDlHandle[i].id = i;
			break;
		}
		else
			i = (i + 1) % defMaxDlHandleNum;
	}

	/*
	for (i = 0; i < gunionCurrentDlHandleNum; i++)
	{
		if (strcmp(serviceCode,gunionServiceCodeDlHandle[i].serviceCode) == 0)
			goto dlFunTag;
	}
	*/
	
	if ((serviceCode[0] == '0')&&(strncmp(serviceCode,"0004",4)!=0))
		snprintf(dlName,sizeof(dlName),"libBaseUI.so");
	else
		snprintf(dlName,sizeof(dlName),"lib%s.so",serviceCode);
	
	j = UnionHashTableHashString(dlName,0);
	j %= defMaxDlHandleNum;
	//id = UnionHashTableHashString(dlName, 1 );
	for (k = 0;; k++)
	{
		if (k == defMaxSONum)
		{
			UnionUserErrLog("in UnionDealSpecServiceCodeTask:: handle number[%d] is too big!\n",defMaxSONum);
			return(errCodeFileTransSvrInvalidServiceCode);
		}
		
		/*
		if (gunionSOHandle[j].id == id)
			goto dlSymTag;
		else if (gunionSOHandle[j].id == -1)
		{
			gunionSOHandle[j].id = id;
			break;
		}
		*/
		if (strcmp(gunionSOHandle[j].dlName,dlName) == 0)
		{
			if (gunionSOHandle[j].handle == NULL)
			{
				UnionUserErrLog("in UnionDealSpecServiceCodeTask:: dlName[%s] handle[%ld] is null!\n",dlName,j);
				return(errCodeFileTransSvrInvalidServiceCode);
			}
			goto dlSymTag;
		}
		else if (gunionSOHandle[j].id == -1)
		{
			gunionSOHandle[j].id = j;
			break;
		}
		else
			j = (j + 1) % defMaxSONum;
	}

	/*
	for (j = 0; j < gunionCurrentSONum; j++)
	{
		if (strcmp(dlName,gunionSOHandle[j].dlName) == 0)
			goto dlSymTag;
	}
	*/
		
	//newDl = 1;

	if (getenv("UNIONDLDIR") != NULL)
		snprintf(fullDLName,sizeof(fullDLName),"%s/%s",getenv("UNIONDLDIR"),dlName);
	else
		snprintf(fullDLName,sizeof(fullDLName),"%s/bin/%s",getenv("UNIONREC"),dlName);

	if ((gunionSOHandle[j].handle = dlopen(fullDLName, RTLD_NOW)) == NULL)
	{
		UnionUserErrLog("in UnionDealSpecServiceCodeTask:: dlopen[%s] error[%s]!\n",fullDLName,dlerror());
		gunionServiceCodeDlHandle[i].id = -1;
		gunionSOHandle[j].id = -1;
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	strcpy(gunionSOHandle[j].dlName,dlName);
	/*
	if (++gunionCurrentSONum > defMaxSONum)
	{
		UnionUserErrLog("in UnionDealSpecServiceCodeTask:: handle number[%d] is too big!\n",gunionCurrentSONum);
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	*/
dlSymTag:
	/*
	if (++gunionCurrentDlHandleNum > defMaxDlHandleNum)
	{
		UnionUserErrLog("in UnionDealSpecServiceCodeTask:: fun number[%d] is too big!\n",gunionCurrentDlHandleNum);
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	*/
	snprintf(funName,sizeof(funName),"UnionDealServiceCode%s",serviceCode);
	if ((gunionServiceCodeDlHandle[i].pfunServiceCode = (funServiceCode *)dlsym(gunionSOHandle[j].handle,funName)) == NULL)
	{
		gunionServiceCodeDlHandle[i].id = -1;
		UnionUserErrLog("in UnionDealSpecServiceCodeTask:: dlsym error funName[%s]!\n",funName);
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	strcpy(gunionServiceCodeDlHandle[i].serviceCode,serviceCode);
	return(ret = (*gunionServiceCodeDlHandle[i].pfunServiceCode)(phsmGroupRec));
dlFunTag:
	return(ret = (*gunionServiceCodeDlHandle[i].pfunServiceCode)(phsmGroupRec));
}

int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	// ����
	if (serviceCode[0] >= '0' && serviceCode[0] <= '9')
	{
		if ((ret = UnionRegisterApproval()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceTask:: UnionRegisterApproval!\n");
			return (ret);
		}
		else if (ret > 0)
			return(ret);
	}	

	/*
	// ���Է���
	if (strncmp(serviceCode,"0000",4) == 0)
		ret = UnionDealServiceCode0000(phsmGroupRec);
	// ��ȡ��֤��
	else if (strncmp(serviceCode,"0001",4) == 0)
		ret = UnionDealServiceCode0001(phsmGroupRec);
	// ��ȡϵͳ��Ϣ
	else if (strncmp(serviceCode,"0002",4) == 0)
		ret = UnionDealServiceCode0002(phsmGroupRec);
	// ��ȡϵͳ������Ϣ
	else if (strncmp(serviceCode,"0003",4) == 0)
		ret = UnionDealServiceCode0003(phsmGroupRec);
	// �û���¼
	else if (strncmp(serviceCode,"0101",4) == 0)
		ret = UnionDealServiceCode0101(phsmGroupRec);
	// �û��˳�
	else if (strncmp(serviceCode,"0102",4) == 0)
		ret = UnionDealServiceCode0102(phsmGroupRec);
	// ��ȡ�û���¼״̬ 
	else if (strncmp(serviceCode,"0103",4) == 0)
		ret = UnionDealServiceCode0103(phsmGroupRec);
	// ��ȡ�û���¼��ʽ 
	else if (strncmp(serviceCode,"0104",4) == 0)
		ret = UnionDealServiceCode0104(phsmGroupRec);
	// �޸�����
	else if (strncmp(serviceCode,"0105",4) == 0)
		ret = UnionDealServiceCode0105(phsmGroupRec);
	// �����û�
	else if (strncmp(serviceCode,"0201",4) == 0)
		ret = UnionDealServiceCode0201(phsmGroupRec);
	// �޸����� 
	else if (strncmp(serviceCode,"0202",4) == 0)
		ret = UnionDealServiceCode0202(phsmGroupRec);
	// �������� 
	else if (strncmp(serviceCode,"0203",4) == 0)
		ret = UnionDealServiceCode0203(phsmGroupRec);
	// ɾ���û�
	else if (strncmp(serviceCode,"0204",4) == 0)
		ret = UnionDealServiceCode0204(phsmGroupRec);
	// ɾ����ɫ
	else if (strncmp(serviceCode,"0205",4) == 0)
		ret = UnionDealServiceCode0205(phsmGroupRec);
	// �޸��û�״̬
	else if (strncmp(serviceCode,"0206",4) == 0)
		ret = UnionDealServiceCode0206(phsmGroupRec);
	// �޸���֯
	else if (strncmp(serviceCode,"0207",4) == 0)
		ret = UnionDealServiceCode0207(phsmGroupRec);
	// ɾ����֯
	else if (strncmp(serviceCode,"0208",4) == 0)
		ret = UnionDealServiceCode0208(phsmGroupRec);
	// ����Ա�����֤
	else if (strncmp(serviceCode,"0209",4) == 0)
		ret = UnionDealServiceCode0209(phsmGroupRec);
	// ���Ӽ�¼ 
	else if (strncmp(serviceCode,"0301",4) == 0)
		ret = UnionDealServiceCode0301(phsmGroupRec);
	// �޸ļ�¼ 
	else if (strncmp(serviceCode,"0302",4) == 0)
		ret = UnionDealServiceCode0302(phsmGroupRec);
	// ɾ����¼ 
	else if (strncmp(serviceCode,"0303",4) == 0)
		ret = UnionDealServiceCode0303(phsmGroupRec);
	// ��ѯ��¼
	else if (strncmp(serviceCode,"0304",4) == 0)
		ret = UnionDealServiceCode0304(phsmGroupRec);
	// �ϴ��ļ� 
	else if (strncmp(serviceCode,"0305",4) == 0)
		ret = UnionDealServiceCode0305(phsmGroupRec);
	// �����ļ� 
	else if (strncmp(serviceCode,"0306",4) == 0)
		ret = UnionDealServiceCode0306(phsmGroupRec);
	// ��ȡ�ļ��嵥
	else if (strncmp(serviceCode,"0307",4) == 0)
		ret = UnionDealServiceCode0307(phsmGroupRec);
	// �ļ����
	else if (strncmp(serviceCode,"0308",4) == 0)
		ret = UnionDealServiceCode0308(phsmGroupRec);
	// �޸���Ȩ��
	else if (strncmp(serviceCode,"0309",4) == 0)
		ret = UnionDealServiceCode0309(phsmGroupRec);
	// ����
	else if (strncmp(serviceCode,"0310",4) == 0)
		ret = UnionDealServiceCode0310(phsmGroupRec);
	// ֹͣ�ػ������ӽ��� 
	else if (strncmp(serviceCode,"0311",4) == 0)
		ret = UnionDealServiceCode0311(phsmGroupRec);
	// ���������
	else if (strncmp(serviceCode,"0312",4) == 0)
		ret = UnionDealServiceCode0312(phsmGroupRec);
	// ���ӱ�
	else if (strncmp(serviceCode,"0401",4) == 0)
		ret = UnionDealServiceCode0401(phsmGroupRec);
	// �޸ı�
	else if (strncmp(serviceCode,"0402",4) == 0)
		ret = UnionDealServiceCode0402(phsmGroupRec);
	// ɾ����
        else if (strncmp(serviceCode,"0403",4) == 0)
		ret = UnionDealServiceCode0403(phsmGroupRec);
	// ��ѯ�� 
        else if (strncmp(serviceCode,"0404",4) == 0)
		ret = UnionDealServiceCode0404(phsmGroupRec);
	// ���ɽ���SQL 
        else if (strncmp(serviceCode,"0405",4) == 0)
		ret = UnionDealServiceCode0405(phsmGroupRec);
	// �ȶ������ֵ�
        else if (strncmp(serviceCode,"0406",4) == 0)
		ret = UnionDealServiceCode0406(phsmGroupRec);
	// �����ݿ����½���
        else if (strncmp(serviceCode,"0407",4) == 0)
		ret = UnionDealServiceCode0407(phsmGroupRec);
	// �����ݿ���ɾ����
        else if (strncmp(serviceCode,"0408",4) == 0)
		ret = UnionDealServiceCode0408(phsmGroupRec);
	// �����ݿ����ؽ��� 
        else if (strncmp(serviceCode,"0409",4) == 0)
		ret = UnionDealServiceCode0409(phsmGroupRec);
	// ��ѯ���嵥 
        else if (strncmp(serviceCode,"0410",4) == 0)
		ret = UnionDealServiceCode0410(phsmGroupRec);
	// ���ӱ��ֶ�
	else if (strncmp(serviceCode,"0411",4) == 0)
		ret = UnionDealServiceCode0411(phsmGroupRec);
	// �޸ı��ֶ�
	else if (strncmp(serviceCode,"0412",4) == 0)
		ret = UnionDealServiceCode0412(phsmGroupRec);
	// ɾ�����ֶ�
	else if (strncmp(serviceCode,"0413",4) == 0)
		ret = UnionDealServiceCode0413(phsmGroupRec);
	// ��ѯ���ֶ�
	else if (strncmp(serviceCode,"0414",4) == 0)
		ret = UnionDealServiceCode0414(phsmGroupRec);   
	// �޸Ľ����ֶζ���
	else if (strncmp(serviceCode,"0415",4) == 0)
		ret = UnionDealServiceCode0415(phsmGroupRec);   
	// �ƶ����ֶ� 
	else if (strncmp(serviceCode,"0416",4) == 0)
		ret = UnionDealServiceCode0416(phsmGroupRec);   
	// ɾ�������� 
	else if (strncmp(serviceCode,"0417",4) == 0)
		ret = UnionDealServiceCode0417(phsmGroupRec);   
	// ���ز˵�
        else if (strncmp(serviceCode,"0501",4) == 0)
		ret = UnionDealServiceCode0501(phsmGroupRec);   
	// ���ذ�ť
        else if (strncmp(serviceCode,"0502",4) == 0)
		ret = UnionDealServiceCode0502(phsmGroupRec);   
	// ������ͼ
        else if (strncmp(serviceCode,"0503",4) == 0)
		ret = UnionDealServiceCode0503(phsmGroupRec);   
	// Ȩ�޹���
        else if (strncmp(serviceCode,"0504",4) == 0)
		ret = UnionDealServiceCode0504(phsmGroupRec);
	// �������˵�����
        else if (strncmp(serviceCode,"0505",4) == 0)
		ret = UnionDealServiceCode0505(phsmGroupRec);
	// �޸Ĳ˵�
        else if (strncmp(serviceCode,"0506",4) == 0)
		ret = UnionDealServiceCode0506(phsmGroupRec);
	// ɾ���˵�
        else if (strncmp(serviceCode,"0507",4) == 0)
		ret = UnionDealServiceCode0507(phsmGroupRec);
	// �ƶ��˵�
        else if (strncmp(serviceCode,"0508",4) == 0)
		ret = UnionDealServiceCode0508(phsmGroupRec);
	// ��������
        else if (strncmp(serviceCode,"0901",4) == 0)
		ret = UnionDealServiceCode0901(phsmGroupRec);
	// ��������������ű�
        else if (strncmp(serviceCode,"0902",4) == 0)
		ret = UnionDealServiceCode0902(phsmGroupRec);
	// ���������ֹͣ�ű�
        else if (strncmp(serviceCode,"0903",4) == 0)
		ret = UnionDealServiceCode0903(phsmGroupRec);
	// ���Գ�ʱ
        else if (strncmp(serviceCode,"0904",4) == 0)
		ret = UnionDealServiceCode0904(phsmGroupRec);
	// �޸��������
	else if (strncmp(serviceCode,"0H01",4) == 0)
		ret = UnionDealServiceCode0H01(phsmGroupRec);
	// ɾ��������� 
	else if (strncmp(serviceCode,"0H02",4) == 0)
		ret = UnionDealServiceCode0H02(phsmGroupRec);
	// ���������
	else if (strncmp(serviceCode,"0H03",4) == 0)
		ret = UnionDealServiceCode0H03(phsmGroupRec);
	// �޸������
	else if (strncmp(serviceCode,"0H04",4) == 0)
		ret = UnionDealServiceCode0H04(phsmGroupRec);
	// ɾ�������
	else if (strncmp(serviceCode,"0H05",4) == 0)
		ret = UnionDealServiceCode0H05(phsmGroupRec);
	// ��ʼ��˽Կ�ռ�
	else if (strncmp(serviceCode,"0H06",4) == 0)
		ret = UnionDealServiceCode0H06(phsmGroupRec);
	// ����LMK������ʽ
	else if (strncmp(serviceCode,"0H07",4) == 0)
		ret = UnionDealServiceCode0H07(phsmGroupRec);
	// �޸�LMK������ʽ
	else if (strncmp(serviceCode,"0H08",4) == 0)
		ret = UnionDealServiceCode0H08(phsmGroupRec);
	// ����˽Կ
	else if (strncmp(serviceCode,"0H09",4) == 0)
		ret = UnionDealServiceCode0H09(phsmGroupRec);
	// �ָ�˽Կ
	else if (strncmp(serviceCode,"0H10",4) == 0)
		ret = UnionDealServiceCode0H10(phsmGroupRec);
	else
	{
		ret = UnionDealSpecServiceCodeTask(serviceCode,phsmGroupRec);
	}
	*/

	ret = UnionDealSpecServiceCodeTask(serviceCode,phsmGroupRec);

	if ((ret >= 0) && (serviceCode[0] >= '0' && serviceCode[0] <= '9'))
	{
		// ��¼�������
		UnionRegisterOperatorAudit();
	}
		
	return(ret);
	
}

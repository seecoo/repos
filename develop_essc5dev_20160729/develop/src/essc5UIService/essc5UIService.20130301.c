//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "essc5UIService.h"
#include "registerAuditAndApproval.h"

// �������
int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	// ����
	if ((ret = UnionRegisterApproval()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceTask:: UnionRegisterApproval!\n");
		return (ret);
	}
	else if (ret > 0)
		return(ret);
	
	// ���ӶԳ���Կ
	if (memcmp(serviceCode,"8E00",4) == 0)
		ret = UnionDealServiceCode8E00(phsmGroupRec);
	// �޸ĶԳ���Կ
	else if (memcmp(serviceCode,"8E01",4) == 0)
		ret = UnionDealServiceCode8E01(phsmGroupRec);
	// ɾ���Գ���Կ
	else if (memcmp(serviceCode,"8E02",4) == 0)
		ret = UnionDealServiceCode8E02(phsmGroupRec);
	// ���¶Գ���Կ
	else if (memcmp(serviceCode,"8E03",4) == 0)
		ret = UnionDealServiceCode8E03(phsmGroupRec);
	// ��ӡ�Գ���Կ
	else if (memcmp(serviceCode,"8E04",4) == 0)
		ret = UnionDealServiceCode8E04(phsmGroupRec);
	// ����Գ���Կ
	else if (memcmp(serviceCode,"8E05",4) == 0)
		ret = UnionDealServiceCode8E05(phsmGroupRec);
	// ����Գ���Կ
	else if (memcmp(serviceCode,"8E06",4) == 0)
		ret = UnionDealServiceCode8E06(phsmGroupRec);
	// �ַ��Գ���Կ
	else if (memcmp(serviceCode,"8E07",4) == 0)
		ret = UnionDealServiceCode8E07(phsmGroupRec);
	// �ָ��Գ���Կ
	else if (memcmp(serviceCode,"8E08",4) == 0)
		ret = UnionDealServiceCode8E08(phsmGroupRec);
	// ����������Կ�ڵ�
	else if (memcmp(serviceCode,"8E09",4) == 0)
		ret = UnionDealServiceCode8E09(phsmGroupRec);
	// ���ӷǶԳ���Կ
	else if (memcmp(serviceCode,"8E10",4) == 0)
		ret = UnionDealServiceCode8E10(phsmGroupRec);
	// �޸ķǶԳ���Կ
	else if (memcmp(serviceCode,"8E11",4) == 0)
		ret = UnionDealServiceCode8E11(phsmGroupRec);
	// ɾ���ǶԳ���Կ
	else if (memcmp(serviceCode,"8E12",4) == 0)
		ret = UnionDealServiceCode8E12(phsmGroupRec);
	// ���·ǶԳ���Կ
	else if (memcmp(serviceCode,"8E13",4) == 0)
		ret = UnionDealServiceCode8E13(phsmGroupRec);
	// ���빫Կ
	else if (memcmp(serviceCode,"8E14",4) == 0)
		ret = UnionDealServiceCode8E14(phsmGroupRec);
	// ����ǶԳ���Կ
	else if (memcmp(serviceCode,"8E15",4) == 0)
		ret = UnionDealServiceCode8E15(phsmGroupRec);
	// �ַ��ǶԳ���Կ
	else if (memcmp(serviceCode,"8E16",4) == 0)
		ret = UnionDealServiceCode8E16(phsmGroupRec);
	// import pfx 
	else if (memcmp(serviceCode,"8E17",4) == 0)
		ret = UnionDealServiceCode8E17(phsmGroupRec);
	// ��������֤��
	else if (memcmp(serviceCode,"8E18",4) == 0)
		ret = UnionDealServiceCode8E18(phsmGroupRec);
	//
	else if (memcmp(serviceCode,"8E1A",4) == 0)
		ret = UnionDealServiceCode8E1A(phsmGroupRec);
	else if (memcmp(serviceCode,"8E1B",4) == 0)
		ret = UnionDealServiceCode8E1B(phsmGroupRec);
	// ����ڵ� 
	else if (memcmp(serviceCode,"8E20",4) == 0)
		ret = UnionDealServiceCode8E20(phsmGroupRec);
	// ������ܻ���Կ��Ϣ�� 
	else if (memcmp(serviceCode,"8EC1",4) == 0)
		ret = UnionDealServiceCode8EC1(phsmGroupRec);
	// ���ö��� 
	else if (memcmp(serviceCode,"8EB1",4) == 0)
		ret = UnionDealServiceCode8EB1(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	if (ret >= 0)
	{
		// ��¼�������
		UnionRegisterOperatorAudit();
	}

	return(ret);
	
}

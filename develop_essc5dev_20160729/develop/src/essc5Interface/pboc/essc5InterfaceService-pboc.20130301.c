//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "essc5Interface.h"

// ִ�������ָ��
int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	// E301У��ARQC(525)
        if (memcmp(serviceCode,"E301",4) == 0)
		ret = UnionDealServiceCodeE301(phsmGroupRec);
	// E302 ����ARPC(526/529)
        else if (memcmp(serviceCode,"E302",4) == 0)
		ret = UnionDealServiceCodeE302(phsmGroupRec);
	// E303 ���������ARPC(527)
        else if (memcmp(serviceCode,"E303",4) == 0)
		ret = UnionDealServiceCodeE303(phsmGroupRec);
	//E304 �ű����ݼ���mac(528)
        else if (memcmp(serviceCode,"E304",4) == 0)
		ret = UnionDealServiceCodeE304(phsmGroupRec);
	// E300 ���������״̬
        else if (memcmp(serviceCode,"E300",4) == 0)
		ret = UnionDealServiceCodeE300(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	return(ret);
	
}

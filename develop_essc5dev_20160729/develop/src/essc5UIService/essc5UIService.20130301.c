//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "essc5UIService.h"
#include "registerAuditAndApproval.h"

// 处理服务
int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	// 审批
	if ((ret = UnionRegisterApproval()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceTask:: UnionRegisterApproval!\n");
		return (ret);
	}
	else if (ret > 0)
		return(ret);
	
	// 增加对称密钥
	if (memcmp(serviceCode,"8E00",4) == 0)
		ret = UnionDealServiceCode8E00(phsmGroupRec);
	// 修改对称密钥
	else if (memcmp(serviceCode,"8E01",4) == 0)
		ret = UnionDealServiceCode8E01(phsmGroupRec);
	// 删除对称密钥
	else if (memcmp(serviceCode,"8E02",4) == 0)
		ret = UnionDealServiceCode8E02(phsmGroupRec);
	// 更新对称密钥
	else if (memcmp(serviceCode,"8E03",4) == 0)
		ret = UnionDealServiceCode8E03(phsmGroupRec);
	// 打印对称密钥
	else if (memcmp(serviceCode,"8E04",4) == 0)
		ret = UnionDealServiceCode8E04(phsmGroupRec);
	// 导入对称密钥
	else if (memcmp(serviceCode,"8E05",4) == 0)
		ret = UnionDealServiceCode8E05(phsmGroupRec);
	// 申请对称密钥
	else if (memcmp(serviceCode,"8E06",4) == 0)
		ret = UnionDealServiceCode8E06(phsmGroupRec);
	// 分发对称密钥
	else if (memcmp(serviceCode,"8E07",4) == 0)
		ret = UnionDealServiceCode8E07(phsmGroupRec);
	// 恢复对称密钥
	else if (memcmp(serviceCode,"8E08",4) == 0)
		ret = UnionDealServiceCode8E08(phsmGroupRec);
	// 批量增加密钥节点
	else if (memcmp(serviceCode,"8E09",4) == 0)
		ret = UnionDealServiceCode8E09(phsmGroupRec);
	// 增加非对称密钥
	else if (memcmp(serviceCode,"8E10",4) == 0)
		ret = UnionDealServiceCode8E10(phsmGroupRec);
	// 修改非对称密钥
	else if (memcmp(serviceCode,"8E11",4) == 0)
		ret = UnionDealServiceCode8E11(phsmGroupRec);
	// 删除非对称密钥
	else if (memcmp(serviceCode,"8E12",4) == 0)
		ret = UnionDealServiceCode8E12(phsmGroupRec);
	// 更新非对称密钥
	else if (memcmp(serviceCode,"8E13",4) == 0)
		ret = UnionDealServiceCode8E13(phsmGroupRec);
	// 导入公钥
	else if (memcmp(serviceCode,"8E14",4) == 0)
		ret = UnionDealServiceCode8E14(phsmGroupRec);
	// 申请非对称密钥
	else if (memcmp(serviceCode,"8E15",4) == 0)
		ret = UnionDealServiceCode8E15(phsmGroupRec);
	// 分发非对称密钥
	else if (memcmp(serviceCode,"8E16",4) == 0)
		ret = UnionDealServiceCode8E16(phsmGroupRec);
	// import pfx 
	else if (memcmp(serviceCode,"8E17",4) == 0)
		ret = UnionDealServiceCode8E17(phsmGroupRec);
	// 导入人行证书
	else if (memcmp(serviceCode,"8E18",4) == 0)
		ret = UnionDealServiceCode8E18(phsmGroupRec);
	//
	else if (memcmp(serviceCode,"8E1A",4) == 0)
		ret = UnionDealServiceCode8E1A(phsmGroupRec);
	else if (memcmp(serviceCode,"8E1B",4) == 0)
		ret = UnionDealServiceCode8E1B(phsmGroupRec);
	// 导入节点 
	else if (memcmp(serviceCode,"8E20",4) == 0)
		ret = UnionDealServiceCode8E20(phsmGroupRec);
	// 导入加密机密钥信息表 
	else if (memcmp(serviceCode,"8EC1",4) == 0)
		ret = UnionDealServiceCode8EC1(phsmGroupRec);
	// 启用订单 
	else if (memcmp(serviceCode,"8EB1",4) == 0)
		ret = UnionDealServiceCode8EB1(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	if (ret >= 0)
	{
		// 记录操作审计
		UnionRegisterOperatorAudit();
	}

	return(ret);
	
}

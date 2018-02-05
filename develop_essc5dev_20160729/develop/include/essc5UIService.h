#ifndef _Essc5UIService_H_
#define _Essc5UIService_H_

#include "commWithHsmSvr.h"

// 增加对称密钥
int UnionDealServiceCode8E00(PUnionHsmGroupRec phsmGroupRec);
// 修改对称密钥
int UnionDealServiceCode8E01(PUnionHsmGroupRec phsmGroupRec);
// 删除对称密钥
int UnionDealServiceCode8E02(PUnionHsmGroupRec phsmGroupRec);
// 更新对称密钥
int UnionDealServiceCode8E03(PUnionHsmGroupRec phsmGroupRec);
// 打印对称密钥
int UnionDealServiceCode8E04(PUnionHsmGroupRec phsmGroupRec);
// 导入对称密钥
int UnionDealServiceCode8E05(PUnionHsmGroupRec phsmGroupRec);
// 申请对称密钥
int UnionDealServiceCode8E06(PUnionHsmGroupRec phsmGroupRec);
// 分发对称密钥
int UnionDealServiceCode8E07(PUnionHsmGroupRec phsmGroupRec);
// 恢复对称密钥
int UnionDealServiceCode8E08(PUnionHsmGroupRec phsmGroupRec);
// 批量增加密钥节点
int UnionDealServiceCode8E09(PUnionHsmGroupRec phsmGroupRec);
// 增加非对称密钥
int UnionDealServiceCode8E10(PUnionHsmGroupRec phsmGroupRec);
// 修改非对称密钥
int UnionDealServiceCode8E11(PUnionHsmGroupRec phsmGroupRec);
// 删除非对称密钥
int UnionDealServiceCode8E12(PUnionHsmGroupRec phsmGroupRec);
// 更新非对称密钥
int UnionDealServiceCode8E13(PUnionHsmGroupRec phsmGroupRec);
// 导入公钥
int UnionDealServiceCode8E14(PUnionHsmGroupRec phsmGroupRec);
// 申请非对称密钥
int UnionDealServiceCode8E15(PUnionHsmGroupRec phsmGroupRec);
// 分发非对称密钥
int UnionDealServiceCode8E16(PUnionHsmGroupRec phsmGroupRec);
// import pfx 
int UnionDealServiceCode8E17(PUnionHsmGroupRec phsmGroupRec);
// 导入人行二代支付证书
int UnionDealServiceCode8E18(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode8E1A(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode8E1B(PUnionHsmGroupRec phsmGroupRec);
// 导入节点 
int UnionDealServiceCode8E20(PUnionHsmGroupRec phsmGroupRec);
// 启用订单 
int UnionDealServiceCode8EB1(PUnionHsmGroupRec phsmGroupRec);
// 导入加密机密钥信息表 
int UnionDealServiceCode8EC1(PUnionHsmGroupRec phsmGroupRec);

#endif


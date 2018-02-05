#ifndef _BaseUIService_H_
#define _BaseUIService_H_

#include "commWithHsmSvr.h"

typedef struct 
{
	char btnName[16];                        
 	char btnDisplayName[32];                   
    	char viewName[16];  
    	char operViewName[16];//²Ù×÷¶ÔÏó                        
    	char btnOrder[8];  
    	char isEdit[8];   
    	char selectedRow[8];  
    	char fileTransfer[8];  
    	char promptBox[8];   
    	char confirmBox[8];   
    	char isVisible[8];
    	char isApproval[8];   
    	char seqNo[16];                    
    	char btnIcon[128];                      
    	char btnCondition[1024];                      
    	char remark[128]; 
}TUnionSysButton;

typedef  TUnionSysButton	*PUnionSysButton;

int UnionDealServiceCode0000(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0001(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0002(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0003(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0004(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0101(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0102(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0103(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0104(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0105(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0201(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0202(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0203(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0204(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0205(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0206(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0207(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0208(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0209(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0301(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0302(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0303(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0304(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0305(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0306(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0307(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0308(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0309(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0310(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0311(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0312(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0401(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0402(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0403(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0404(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0405(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0406(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0407(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0408(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0409(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0410(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0411(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0412(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0413(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0414(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0415(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0416(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0417(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0501(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0502(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0503(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0504(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0505(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0506(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0507(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0508(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0901(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0902(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0903(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0904(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H01(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H02(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H03(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H04(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H05(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H06(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H07(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode0H08(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCode8EB1(PUnionHsmGroupRec phsmGroupRec);

#endif

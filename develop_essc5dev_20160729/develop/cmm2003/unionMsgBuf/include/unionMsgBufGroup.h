#ifndef _UnionMsgBufGroup_        
#define _UnionMsgBufGroup_ 

// ��Ϣ����
// # ��0��:UIͨѶ
// # ��1��:APPͨѶ
// # ��2��:dbSvr
// # ��3��:hsmSvr
// # ��4��:�����Ϣ
// # ��5��:ͬ��
#define defMsgBufGroupOfUI		0
#define defMsgBufGroupOfAPP		1
#define defMsgBufGroupOfDBSvr		2
#define defMsgBufGroupOfHsmSvr		3
#define defMsgBufGroupOfMon		4
#define defMsgBufGroupOfDataSync	5
//#define defMsgBufGroupOfHsmThrough      6
//#define defMsgBufGroupOfHsmCmd          7

// ������ID,��0��ʼ����
void UnionSetGroupIDOfMsgBuf(int groupID);

#endif

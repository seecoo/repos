//	2002/3/7 Wolfgang Wang, rename all the functions of the name XXXXMdl to
//	new name XXXMDL.
//	2002/3/7 Wolfgang Wang, change the parameter of the function 
//		UnionConnectMsgBufMDL, which is formerly named UnionConnectMsgBufMdl.

// 2002/8/17，Wolfgang Wang
//	由2.x版升级为3.x版
//	3.x版与2.x版的差异在于，对于消息不再区分请求与响应，而只有一组消息函数。

// 2003/02/27，Wolfgang Wang
/*
	在3.0基础上升级为3.1
	升级的原因为：
		由于不明原因,msgrcv读取的消息长度会为8，而sizeof(int)长度为4
		接收消息的缓冲的大小为4，从而导致程序异常中止。
	修改：
		经陈家梅查，msgrcv的第3个参数的长度为实际存放消息的最大长度，
		3.0以前版本，该长度为sizeof(PosOfMsgBuf)，现改为sizeof(int)
	
*/

// 2003/05/29，Wolfgang Wang
	
	在3.1基础上升级为3.2
	3.2源码和3.1相同，但增加了一个编译选项：_UnionUseMaxMsgBufModel_
	该宏在UnionMsgBuf3.x.h中定义。
	3.2与3.1及之前版本相比，差异在：
					3.1		3.2
	缓冲区最大长度(以K计)		10,000		256,000
	消息最大长度(以字节计)		1,024		1,280,000
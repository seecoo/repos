//	2002/3/7 Wolfgang Wang, rename all the functions of the name XXXXMdl to
//	new name XXXMDL.
//	2002/3/7 Wolfgang Wang, change the parameter of the function 
//		UnionConnectMsgBufMDL, which is formerly named UnionConnectMsgBufMdl.

// 2002/8/17��Wolfgang Wang
//	��2.x������Ϊ3.x��
//	3.x����2.x��Ĳ������ڣ�������Ϣ����������������Ӧ����ֻ��һ����Ϣ������

// 2003/02/27��Wolfgang Wang
/*
	��3.0����������Ϊ3.1
	������ԭ��Ϊ��
		���ڲ���ԭ��,msgrcv��ȡ����Ϣ���Ȼ�Ϊ8����sizeof(int)����Ϊ4
		������Ϣ�Ļ���Ĵ�СΪ4���Ӷ����³����쳣��ֹ��
	�޸ģ�
		���¼�÷�飬msgrcv�ĵ�3�������ĳ���Ϊʵ�ʴ����Ϣ����󳤶ȣ�
		3.0��ǰ�汾���ó���Ϊsizeof(PosOfMsgBuf)���ָ�Ϊsizeof(int)
	
*/

// 2003/05/29��Wolfgang Wang
	
	��3.1����������Ϊ3.2
	3.2Դ���3.1��ͬ����������һ������ѡ�_UnionUseMaxMsgBufModel_
	�ú���UnionMsgBuf3.x.h�ж��塣
	3.2��3.1��֮ǰ�汾��ȣ������ڣ�
					3.1		3.2
	��������󳤶�(��K��)		10,000		256,000
	��Ϣ��󳤶�(���ֽڼ�)		1,024		1,280,000
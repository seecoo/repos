//	Wolfgang Wang
//	2009/4/29

#ifndef _unionUserInterfaceItemGrp_
#define _unionUserInterfaceItemGrp_

#include "unionTree.h"

/*
����	
	�жϽ����������Ƿ��ѱ���ʼ����
�������
	��
�������
	��
����ֵ
	1	�ѳ�ʼ��
	0	δ��ʼ��
*/
int UnionIsInterfaceItemsGrpInited();

/*
����	
	��ȡ��ǰ�Ľ���������
�������
	��
�������
	��
����ֵ
	��ǰ����������ָ��
*/
PUnionTree UnionGetCurrentInterfaceItemGrp();

/*
����	
	�ͷŽ��������ݽṹ
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionDeleteInterfaceItemGrp();

/*
����	
	�ж�һ���������Ƿ����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	����
	0	������
	<0	�������
*/
int UnionIsInterfaceItemExists(char *itemTag,char *fmt,...);

/*
����	
	�ж�һ��������ָ�����Ľ������Ƿ����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	����
	0	������
	<0	�������
*/
int UnionIsInterfaceItemContainSpecStrExists(char *itemTag,char *fmt,...);

/*
����	
	Ѱ��һ��������
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	�ɹ�	������ָ��
	ʧ��	��ָ��
*/
PUnionLeaf UnionFindInterfaceItem(char *itemTag,char *fmt,...);

/*
����	
	��ʼ�����������ݽṹ
�������
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitInterfaceItemGrp(char *mainMenuName);

/*
����	
	��������д�뵽ָ���ļ���
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintInterfaceItemGrpToSpecFile(char *fileName);
	
/*
����	
	������Ľ�����д�뵽ָ���ļ���
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintErrorInterfaceItemGrpToSpecFile(char *fileName);

/*
����	
	��ʼ�����������ݽṹ
�������
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitInterfaceItemGrp(char *mainMenuName);
	
/*
����	
	�ж�һ�������ǰ�Ƿ����ڱ�����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	���ڱ�����
	0	δ������
	<0	�������
*/
int UnionSetInterfaceItemDealCurrently(char *itemTag,char *fmt,...);

/*
����	
	��һ����������뵽����������
�������
	remark	˵��
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionAddInterfaceItemToItemGrp(char *remark,char *itemTag,char *fmt,...);

#endif

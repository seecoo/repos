// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionListTree_
#define _unionListTree_

typedef struct	TUnionLeaf
{
	char			*leafName;	// ��������
	int			referenceNum;	// ���ô���
	char			*remark;	// �쳣˵��
	struct	TUnionLeaf	*pnextLeaf;	// ��һƬ��Ҷ
} TUnionLeaf;
typedef TUnionLeaf	*PUnionLeaf;

typedef struct	TUnionBranch
{
	char			*branchName;	// ��֦����
	int			leafNum;	// ��Ҷ����
	PUnionLeaf		plastLeaf;	// ����֦�����һƬ��Ҷ
	struct TUnionBranch	*pnextBranch;	// ��һ����֦
} TUnionBranch;
typedef TUnionBranch	*PUnionBranch;

typedef struct
{
	char			*treeName;	// ������
	int			branchNum;	// ��֦����
	PUnionBranch		plastBranch;	// ���һ����֦
} TUnionTree;
typedef TUnionTree	*PUnionTree;

/*
����
	��ʼ��һƬҶ��
���������
	leafName	Ҷ������
	remark		Ҷ��˵��
�������
	��
����ֵ
	�ɹ�	Ҷ��ָ��
	ʧ��	NULL
*/
PUnionLeaf UnionNewLeaf(char *leafName,char *remark);

/*
����
	�滻Ҷ�ӵ�˵��
���������
	pleaf		Ҷ��ָ��
	remark		Ҷ��˵��
�������
	��
����ֵ
	>=	�ɹ�
	<0	������
*/
int UnionReplaceLeafRemark(PUnionLeaf pleaf,char *remark);
	
/*
����
	�ͷ�һ��Ҷ��
���������
	ptr		Ҷ��ָ��
�������
	��
����ֵ
	��
*/
void UnionDeleteLeaf(PUnionLeaf ptr);

/*
����
	��һ��Ҷ��д�뵽��־��
���������
	leafIndex	Ҷ�����
	ptr		Ҷ��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogLeaf(int *leafIndex,PUnionLeaf ptr,FILE *fp);

/*
����
	��һ��Ҷ��д�뵽��־��
���������
	leafIndex	Ҷ�����
	ptr		Ҷ��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogLeafWithRemark(int *leafIndex,PUnionLeaf ptr,FILE *fp);

/*
����
	��ʼ��һ����֦
���������
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	��ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionNewBranch(char *branchName);
	
/*
����
	�ͷ�һ����֦
���������
	ptr		��ָ֦��
�������
	��
����ֵ
	��
*/
void UnionDeleteBranch(PUnionBranch ptr);

/*
����
	��һ����֦д�뵽��־��
���������
	branchIndex	��֦���
	ptr		��ָ֦��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogBranch(int *branchIndex,PUnionBranch ptr,FILE *fp);

/*
����
	��һ����֦д�뵽��־��
���������
	branchIndex	��֦���
	ptr		��ָ֦��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogBranchWithRemark(int *branchIndex,PUnionBranch ptr,FILE *fp);

/*
����
	��ʼ��һ����
���������
	treeName	������
�������
	��
����ֵ
	�ɹ�	��ָ��
	ʧ��	NULL
*/
PUnionTree UnionNewTree(char *treeName);
	
/*
����
	�ͷ�һ����
���������
	ptr		��ָ��
�������
	��
����ֵ
	��
*/
void UnionDeleteTree(PUnionTree ptr);


/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogTree(int *treeIndex,PUnionTree ptr,FILE *fp);

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogTreeWithRemark(int *treeIndex,PUnionTree ptr,FILE *fp);

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	0	ʧ�ܣ�������
*/
int UnionLogTreeWithRemarkToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName);

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	0	ʧ�ܣ�������
*/
int UnionLogTreeToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName);

/*
����
	��һ�����ϲ�һ����֦
���������
	ptree		��
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	������ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionFindBranchInTree(PUnionTree ptree,char *branchName);

/*
����
	��һ����֦�б��в�һ����֦
���������
	pbranchList	��֦�б�
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	������ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionFindBranchInBranchList(PUnionBranch pbranchList,char *branchName);

/*
����
	��һ����֦�ϲ�һ����Ҷ
���������
	pbranch		��֦
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInBranch(PUnionBranch pbranch,char *leafName);

/*
����
	��һ����Ҷ�б��в�һ����Ҷ
���������
	pleafList	��Ҷ�б�
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInLeafList(PUnionLeaf pleafList,char *leafName);

/*
����
	��һ����Ҷ�б��в��һ��������ָ����ʶ����Ҷ
���������
	pleafList	��Ҷ�б�
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindFirstLeafContainSpecStrInLeafList(PUnionLeaf pleafList,char *leafName);

/*
����
	��һ�����ϲ�һ����Ҷ
���������
	ptree		��
	branchName	��֦����
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInTree(PUnionTree ptree,char *branchName,char *leafName);

/*
����
	����������һƬҶ��
���������
	ptree		��
	branchName	��֦����
	leafName	Ҷ������
	remark		Ҷ��˵��
�������
	��
����ֵ
	>=0		�ɹ�����Ҷ����������
	<0		�������
*/
int UnionAddOneLeafToTree(PUnionTree ptree,char *branchName,char *leafName,char *remark);

#endif

// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionListTree_
#define _unionListTree_

typedef struct	TUnionLeaf
{
	char			*leafName;	// 变量名称
	int			referenceNum;	// 引用次数
	char			*remark;	// 异常说明
	struct	TUnionLeaf	*pnextLeaf;	// 下一片树叶
} TUnionLeaf;
typedef TUnionLeaf	*PUnionLeaf;

typedef struct	TUnionBranch
{
	char			*branchName;	// 树枝名称
	int			leafNum;	// 树叶数量
	PUnionLeaf		plastLeaf;	// 本树枝的最后一片树叶
	struct TUnionBranch	*pnextBranch;	// 下一个树枝
} TUnionBranch;
typedef TUnionBranch	*PUnionBranch;

typedef struct
{
	char			*treeName;	// 树名称
	int			branchNum;	// 树枝数量
	PUnionBranch		plastBranch;	// 最后一个树枝
} TUnionTree;
typedef TUnionTree	*PUnionTree;

/*
功能
	初始化一片叶子
输入参数：
	leafName	叶子名称
	remark		叶子说明
输出参数
	无
返回值
	成功	叶子指针
	失败	NULL
*/
PUnionLeaf UnionNewLeaf(char *leafName,char *remark);

/*
功能
	替换叶子的说明
输入参数：
	pleaf		叶子指针
	remark		叶子说明
输出参数
	无
返回值
	>=	成功
	<0	错误码
*/
int UnionReplaceLeafRemark(PUnionLeaf pleaf,char *remark);
	
/*
功能
	释放一个叶子
输入参数：
	ptr		叶子指针
输出参数
	无
返回值
	无
*/
void UnionDeleteLeaf(PUnionLeaf ptr);

/*
功能
	将一个叶子写入到日志中
输入参数：
	leafIndex	叶子序号
	ptr		叶子指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogLeaf(int *leafIndex,PUnionLeaf ptr,FILE *fp);

/*
功能
	将一个叶子写入到日志中
输入参数：
	leafIndex	叶子序号
	ptr		叶子指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogLeafWithRemark(int *leafIndex,PUnionLeaf ptr,FILE *fp);

/*
功能
	初始化一个树枝
输入参数：
	branchName	树枝名称
输出参数
	无
返回值
	成功	树枝指针
	失败	NULL
*/
PUnionBranch UnionNewBranch(char *branchName);
	
/*
功能
	释放一个树枝
输入参数：
	ptr		树枝指针
输出参数
	无
返回值
	无
*/
void UnionDeleteBranch(PUnionBranch ptr);

/*
功能
	将一个树枝写入到日志中
输入参数：
	branchIndex	树枝序号
	ptr		树枝指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogBranch(int *branchIndex,PUnionBranch ptr,FILE *fp);

/*
功能
	将一个树枝写入到日志中
输入参数：
	branchIndex	树枝序号
	ptr		树枝指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogBranchWithRemark(int *branchIndex,PUnionBranch ptr,FILE *fp);

/*
功能
	初始化一个树
输入参数：
	treeName	树名称
输出参数
	无
返回值
	成功	树指针
	失败	NULL
*/
PUnionTree UnionNewTree(char *treeName);
	
/*
功能
	释放一个树
输入参数：
	ptr		树指针
输出参数
	无
返回值
	无
*/
void UnionDeleteTree(PUnionTree ptr);


/*
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogTree(int *treeIndex,PUnionTree ptr,FILE *fp);

/*
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fp		文件指针
输出参数
	无
返回值
	无
*/
void UnionLogTreeWithRemark(int *treeIndex,PUnionTree ptr,FILE *fp);

/*
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	0	失败，错误码
*/
int UnionLogTreeWithRemarkToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName);

/*
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	0	失败，错误码
*/
int UnionLogTreeToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName);

/*
功能
	在一棵树上查一个树枝
输入参数：
	ptree		树
	branchName	树枝名称
输出参数
	无
返回值
	成功	返回树枝指针
	失败	NULL
*/
PUnionBranch UnionFindBranchInTree(PUnionTree ptree,char *branchName);

/*
功能
	在一棵树枝列表中查一个树枝
输入参数：
	pbranchList	树枝列表
	branchName	树枝名称
输出参数
	无
返回值
	成功	返回树枝指针
	失败	NULL
*/
PUnionBranch UnionFindBranchInBranchList(PUnionBranch pbranchList,char *branchName);

/*
功能
	在一个树枝上查一个树叶
输入参数：
	pbranch		树枝
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindLeafInBranch(PUnionBranch pbranch,char *leafName);

/*
功能
	在一个树叶列表中查一个树叶
输入参数：
	pleafList	树叶列表
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindLeafInLeafList(PUnionLeaf pleafList,char *leafName);

/*
功能
	在一个树叶列表中查第一个包含了指定标识的树叶
输入参数：
	pleafList	树叶列表
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindFirstLeafContainSpecStrInLeafList(PUnionLeaf pleafList,char *leafName);

/*
功能
	在一棵树上查一个树叶
输入参数：
	ptree		树
	branchName	树枝名称
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindLeafInTree(PUnionTree ptree,char *branchName,char *leafName);

/*
功能
	在树上增加一片叶子
输入参数：
	ptree		树
	branchName	树枝名称
	leafName	叶子名称
	remark		叶子说明
输出参数
	无
返回值
	>=0		成功，树叶的生长次数
	<0		出错代码
*/
int UnionAddOneLeafToTree(PUnionTree ptree,char *branchName,char *leafName,char *remark);

#endif

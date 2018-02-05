// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionReportModule_
#define _unionReportModule_

// 定义一个报表域的显示属性
typedef struct
{
	int		startRowOffset;		// 开始行的偏移
	int		startCol;		// 开始列的偏移
	int		endRowOffset;		// 结束行的偏移
	int		endCol;			// 结束列的偏移
	char		foregroundColor[16+1];	// 前景显示颜色
	char		backgroundColor[16+1];	// 背景显示颜色
	int		rowHeight;		// 行高
	int		colWidth;		// 列宽
	int		horizontal;
	int		vertical;
	int		isBold;
	int		font;
	char		type[16+1];
} TUnionReportCellDisplayAttr;
typedef TUnionReportCellDisplayAttr	*PUnionReportCellDisplayAttr;

// 定义一个域定义
typedef struct
{
	char				tag[40+1];				// 域段的标识
	TUnionReportCellDisplayAttr	attr;					// 域段的显示属性
} TUnionReportCellDef;
typedef TUnionReportCellDef		*PUnionReportCellDef;

// 定义一个报表段
#define conMaxNumOfCellPerReportLine	32					// 一个报表一行可以包括的最大域数目
typedef struct
{
	char				id[40+1];				// 段标识
	TUnionReportCellDisplayAttr	defaultDisplayAttr;			// 段缺省采用的显示属性
	int				cellNum;				// 本段包括的域数目
	int				displayCellTag;				// 本段是否显示域的标识,1:显示;0:不显示
	PUnionReportCellDef		pcellDef[conMaxNumOfCellPerReportLine];	//每个域的定义
} TUnionReportPhaseDef;
typedef TUnionReportPhaseDef		*PUnionReportPhaseDef;

// 定义一个报表模板
#define conMaxNumOfPhasePerReport	16					// 一个报表模板可以包括的最大段数量
typedef struct
{
	char				id[128+1];				// 报表标识
	int				type;					// 报表类型
	TUnionReportCellDisplayAttr	defaultDisplayAttr;			// 报表缺省采用的显示属性
	int				phaseNum;				// 段的数量
	PUnionReportPhaseDef		pphaseDef[conMaxNumOfPhasePerReport];	// 段的定义
} TUnionReportModule;
typedef TUnionReportModule		*PUnionReportModule;


#endif

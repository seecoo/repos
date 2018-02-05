// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionReportModule_
#define _unionReportModule_

// ����һ�����������ʾ����
typedef struct
{
	int		startRowOffset;		// ��ʼ�е�ƫ��
	int		startCol;		// ��ʼ�е�ƫ��
	int		endRowOffset;		// �����е�ƫ��
	int		endCol;			// �����е�ƫ��
	char		foregroundColor[16+1];	// ǰ����ʾ��ɫ
	char		backgroundColor[16+1];	// ������ʾ��ɫ
	int		rowHeight;		// �и�
	int		colWidth;		// �п�
	int		horizontal;
	int		vertical;
	int		isBold;
	int		font;
	char		type[16+1];
} TUnionReportCellDisplayAttr;
typedef TUnionReportCellDisplayAttr	*PUnionReportCellDisplayAttr;

// ����һ������
typedef struct
{
	char				tag[40+1];				// ��εı�ʶ
	TUnionReportCellDisplayAttr	attr;					// ��ε���ʾ����
} TUnionReportCellDef;
typedef TUnionReportCellDef		*PUnionReportCellDef;

// ����һ�������
#define conMaxNumOfCellPerReportLine	32					// һ������һ�п��԰������������Ŀ
typedef struct
{
	char				id[40+1];				// �α�ʶ
	TUnionReportCellDisplayAttr	defaultDisplayAttr;			// ��ȱʡ���õ���ʾ����
	int				cellNum;				// ���ΰ���������Ŀ
	int				displayCellTag;				// �����Ƿ���ʾ��ı�ʶ,1:��ʾ;0:����ʾ
	PUnionReportCellDef		pcellDef[conMaxNumOfCellPerReportLine];	//ÿ����Ķ���
} TUnionReportPhaseDef;
typedef TUnionReportPhaseDef		*PUnionReportPhaseDef;

// ����һ������ģ��
#define conMaxNumOfPhasePerReport	16					// һ������ģ����԰�������������
typedef struct
{
	char				id[128+1];				// �����ʶ
	int				type;					// ��������
	TUnionReportCellDisplayAttr	defaultDisplayAttr;			// ����ȱʡ���õ���ʾ����
	int				phaseNum;				// �ε�����
	PUnionReportPhaseDef		pphaseDef[conMaxNumOfPhasePerReport];	// �εĶ���
} TUnionReportModule;
typedef TUnionReportModule		*PUnionReportModule;


#endif

//---------------------------------------------------------------------------

#ifndef unionEnumValueDefH
#define unionEnumValueDefH
//---------------------------------------------------------------------------
typedef struct
{
        char    remark[40+1];   // ֵ˵��
        char    value[48+1];    // ֵ
} TUnionEnumValueDef;
typedef TUnionEnumValueDef      *PUnionEnumValueDef;


#define conMaxNumOfEnumValuePerGrp      64
typedef struct
{
        int                     valueNum;
        TUnionEnumValueDef      valueDef[conMaxNumOfEnumValuePerGrp];
} TUnionEnumValueGrp;
typedef TUnionEnumValueGrp      *PUnionEnumValueGrp;

//---------------------------------------------------------------------------
/*
����	���ļ��ж�ȡö�����Ͷ���
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�ֵ��Ŀ
	<0		�������
*/
int UnionReadEnumValueGrpDef(char *enumType,PUnionEnumValueGrp prec);

//---------------------------------------------------------------------------
/*
����	��ָ��ö�����͵�ָ��˵����ֵ��Ӧ��ֵ������
�������
	enumType         ö��������
        isReadValueByRemark  ����˵������ֵ����  1����˵����0����ֵ
        byTag           ˵����ֵ
�������
	readTag		������ֵ��˵��
����ֵ
	>=0		�����ĳ���
	<0		�������
*/
int UnionReadEnumValueOtherAttr(char *enumType,int isReadValueByRemark,char *byTag,char *readTag,int sizeOfBuf);

//---------------------------------------------------------------------------
/*
����	��ȡö�����͵�����ֵ
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�ֵ��Ŀ
	<0		�������
*/
int UnionReadAllValueOfEnumType(char *enumType,char value[][128+1],int maxNum);

//---------------------------------------------------------------------------
/*
����	��ȡö�����͵�����˵��
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�˵����Ŀ
	<0		�������
*/
int UnionReadAllRemarkOfEnumType(char *enumType,char remark[][128+1],int maxNum);
#endif

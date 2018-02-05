//	Author:	Wolfgang Wang
//	Date:	2002/7/31

#ifndef _Union8583_3_x
#define _Union8583_3_x


// 8583�����
typedef struct
{
	int		iVarLen;	// �䳤�ֶα䳤����λ��
	int		iLen;		// �䳤�ֶγ��ȣ���䳤�ֶ���󳤶ȡ�
	int		iRealLen;	// ���ʵ�ʳ���
	unsigned char	*pItem;		// ���ֵ���������Ŀռ�Ϊ��󳤶ȵĴ洢�ռ�
} TUnion8583_3_x;
typedef TUnion8583_3_x	*PUnion8583_3_x;

// Functions definition for emu8583 module

// Internal functions
int UnionInitEmu8583Attribute(PUnion8583_3_x p8583_3_x,char *p8583DefFileName);

// External functions
PUnion8583_3_x UnionConnectEmu8583_3_x_MDL(char *p8583DefFileName);
int UnionDisconnectEmu8583_3_x_MDL(PUnion8583_3_x p8583_3_x);

int UnionSetEmu8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionReadEmu8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);

int UnionInitEmu8583_3_x(PUnion8583_3_x p8583_3_x);
int UnionPackEmu8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pPackBuf,int iPackBufSize);
int UnionUnpackEmu8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pUnpackBuf,int iUnpackBufLen);

// Internal functions
int UnionInit8583Attribute(PUnion8583_3_x p8583_3_x,char *p8583DefFileName);

// External functions
PUnion8583_3_x UnionConnect8583_3_x_MDL(char *p8583DefFileName);
int UnionDisconnect8583_3_x_MDL(PUnion8583_3_x p8583_3_x);

int UnionSet8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionRead8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);

int UnionInit8583_3_x(PUnion8583_3_x p8583_3_x);
int UnionPack8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pPackBuf,int iPackBufSize);
int UnionUnpack8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pUnpackBuf,int iUnpackBufLen);


#endif

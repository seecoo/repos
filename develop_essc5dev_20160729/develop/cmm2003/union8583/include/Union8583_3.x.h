//	Author:	Wolfgang Wang
//	Date:	2002/7/31

#ifndef _Union8583_3_x
#define _Union8583_3_x


// 8583域定义表
typedef struct
{
	int		iVarLen;	// 变长字段变长长度位数
	int		iLen;		// 变长字段长度，或变长字段最大长度。
	int		iRealLen;	// 域的实际长度
	unsigned char	*pItem;		// 域的值，分配给其的空间为最大长度的存储空间
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

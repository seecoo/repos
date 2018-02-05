//	Author:	Wolfgang Wang
//	Date:	2001/8/29

#ifndef _Union8583
#define _Union8583

#define MAX_8583FIELD_NUM		129
#define	MAX_8583FIELD_LEN		32000
#define	MAX_8583_VARLEN			4

// 错误代码定义
#define UnionErrNoOffsetFor8583		-1400
// System functions error
#define ERR_FILE_OPEN			(UnionErrNoOffsetFor8583-1)
#define ERR_FSCANF			(UnionErrNoOffsetFor8583-2)
#define ERR_MALLOC			(UnionErrNoOffsetFor8583-3)
// Common error
#define ERR_NULL_POINT			(UnionErrNoOffsetFor8583-20)
#define ERR_INVALID_LENGTH		(UnionErrNoOffsetFor8583-21)
#define ERR_BUFFER_SIZE_SMALL		(UnionErrNoOffsetFor8583-22)
// Module special error
#define ERR_FIELD_INDEX			(UnionErrNoOffsetFor8583-50)
#define	ERR_COMMAND_TYPE_CODE		(UnionErrNoOffsetFor8583-51)
#define	ERR_VARLEN_DEFINE		(UnionErrNoOffsetFor8583-52)
#define	ERR_FIELD_LENGTH		(UnionErrNoOffsetFor8583-53)
#define	ERR_FIELD_TYPE			(UnionErrNoOffsetFor8583-54)
#define	ERR_MESSAGE_LEN_NOT_ENOUGH	(UnionErrNoOffsetFor8583-55)
#define	ERR_MESSAGE_LEN_TOO_LONG	(UnionErrNoOffsetFor8583-56)
#define ERR_VARIABLE_LEN		(UnionErrNoOffsetFor8583-57)
#define ERR_CONSTANT_LEN		(UnionErrNoOffsetFor8583-58)



// 8583域定义表
struct TABLE8583
{
	int		iLen;		// 域的长度
	unsigned char	*pItem;		// 域的值
};

// 8583协议属性表
struct ATTR8583
{
	int		iVarLen;	// 变长字段变长长度位数
	int		iLen;	       	// 定长字段长度，或变长字段最大长度
};


// Functions definition for 8583 module

// Internal functions
int UnionInit8583Attribute(char *pFileName);
int FieldLenFormat(int iFieldVarLen,int iFieldLen,unsigned char *pFieldLenStr);
int UnionSet8583Field_map(unsigned char *pFieldMap,int iIndex);
int UnionRead8583Field_map(unsigned char *pFieldMap,int iIndex);
unsigned char *UnionMallocMem(int iSize);

// External functions
int UnionConnect8583MDL(char *p8583DefFile);
int UnionDisconnect8583MDL();
int UnionInit8583RecvBuf();
int UnionInit8583SendBuf();
int UnionPack8583(unsigned char *pPackBuf,int iPackBufSize);
int UnionUnpack8583(unsigned char *pUnpackBuf,int iUnpackBufLen);
int UnionSet8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionRead8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);
int UnionSet8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionRead8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);



// Functions definition for emu8583 module

// Internal functions
int UnionInitEmu8583Attribute(char *pFileName);
int EmuFieldLenFormat(int iFieldVarLen,int iFieldLen,unsigned char *pFieldLenStr);
unsigned char *UnionEmuMallocMem(int iSize);

// External functions
int UnionConnectEmu8583MDL(char *p8583DefFile);
int UnionDisconnectEmu8583MDL();
int UnionInitEmu8583RecvBuf();
int UnionInitEmu8583SendBuf();
int UnionPackEmu8583(unsigned char *pPackBuf,int iPackBufSize);
int UnionUnpackEmu8583(unsigned char *pUnpackBuf,int iUnpackBufLen);
int UnionSetEmu8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionReadEmu8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);
int UnionSetEmu8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen);
int UnionReadEmu8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize);

#endif

// Author: Wolfgang Wang
// Copyright: Union Tech. Guangzhou.
// Date: 2003/09/10
// Version1.0

#ifndef _SJL06KeySpace_
#define _SJL06KeySpace_

#define	conSJL06JKKeySpaceName	"JK"
#define conSJL06ICKeySpaceName	"IC"
#define conSJL06RSAKeySpaceName	"RSA"

typedef struct
{
	char	hsmGrpID[3+1];	// 组号
	char	name[3+1];	// 密钥空间名称
	long	totalPos;	// 密钥存储位置的数量
	long	availablePos;	// 可用位置的数量
	long	occupiedPos;	// 占用的位置
	long	invalidPos;	// 无效的位置
} TUnionSJL06KeyPosSpace;
typedef TUnionSJL06KeyPosSpace		*PUnionSJL06KeyPosSpace;

#define	conAvailableKeyPos	0	// 可用的位置
#define	conOccupiedKeyPos	1	// 被占用的位置
#define	conInvalidKeyPos	-1	// 非法位置，对于索引号不连续的空间，该值有用
typedef struct
{
	short	status;
	char	date[8+1];		// the date to use.
	char	user[40+1];		// who occupy this pos.
} TUnionKeyPosStatus;
typedef TUnionKeyPosStatus		*PUnionKeyPosStatus;

int UnionExistSJL06KeySpaceFile(char *hsmGrp,char *name);
int UnionIsValidSJL06KeySpaceName(char *hsmGrp,char *name);
int UnionGetNameOfKeyPosSpace(char *hsmGrp,char *name,char *fileName);
int UnionCreateKeyPosSpace(char *hsmGrp,char *name,long totalKeyPosNum);
int UnionDeleteKeyPosSpace(char *hsmgGrp,char *name);

PUnionSJL06KeyPosSpace UnionConnectKeyPosSpace(char *hsmGrp,char *name);
int UnionDisconnectKeyPosSpace(PUnionSJL06KeyPosSpace pkeyPosSpace);

int UnionModifyKeyPosStatus(PUnionSJL06KeyPosSpace pkeyPosSpace,PUnionKeyPosStatus pRec,long keyPosIndex);
int UnionPrintKeyPosStatus(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex);
int UnionPrintAllAvailableKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace);
int UnionPrintAllInvalidKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace);
int UnionPrintAllOccupiedKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace);
int UnionIsKeyPosAvailable(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex);
int UnionUseKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex,char *userName);
int UnionUnuseKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex);
int UnionSetKeyPosInvalid(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex);
long UnionSelectAvailableKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace);
int UnionPrintStatusOfKeyPosSpaceToFile(PUnionSJL06KeyPosSpace pkeyPosSpace,FILE *fp);

int UnionUnuseAllKeyPosUsedByUser(PUnionSJL06KeyPosSpace pkeyPosSpace,char *userName);
int UnionPrintAllOccupiedKeyPosByUser(PUnionSJL06KeyPosSpace pkeyPosSpace,char *userName);

long UnionGetTotalKeyPosNum(PUnionSJL06KeyPosSpace pkeyPosSpace);

int UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace(char *hsmGrp,char *name);

#define	conSJL06ICKeyPosVersionFormat		"%01X"
#define conSJL06ICKeyPosVersionLen		1
#define	conSJL06ICKeyPosGroupFormat		"%02X"
#define conSJL06ICKeyPosGroupLen		2
#define	conSJL06ICKeyPosIndexFormat		"%02X"
#define conSJL06ICKeyPosIndexLen		2
typedef struct
{
	// the following fields uniquely refer to an  ic-key-stored-position
	// of a SJL06 Hsm.
	int	version;
	int	group;
	int	index;
} TUnionICKeyPosIndex;
typedef TUnionICKeyPosIndex	*PUnionICKeyPosIndex;

#define	conSJL06JKKeyPosFormat			"%03X"
#define conSJL06JKKeyPosLen			3
typedef struct
{
	int	bmk;
} TUnionJKKeyPosIndex;
typedef TUnionJKKeyPosIndex	*PUnionJKKeyPosIndex;

#define	conSJL06TerminalKeyPosFormat		"%03X"
#define conSJL06TerminalKeyPosLen		3
typedef struct
{
	int	tmk;
} TUnionTerminalKeyPosIndex;
typedef TUnionTerminalKeyPosIndex	*PUnionTerminalKeyPosIndex;

#define	conSJL06VKKeyPosFormat			"%02X"
#define conSJL06VKKeyPosLen			2
typedef struct
{
	int	vk;
} TUnionVKIndex;
typedef TUnionVKIndex		*PUnionVKIndex;

#endif

#ifndef  OTPINTERFACE_H_
#define  OTPINTERFACE_H_

#define SM4_KEY_LEN 16
#define SM4_DATA_LEN 32
#define SEED_LEN 32

typedef short				bool;
#define false				0
#define true				1
typedef unsigned char			uint8;
typedef unsigned int			uint32;
typedef unsigned long long		uint64;
typedef unsigned int			sm_word;
typedef unsigned char			byte;

byte *__S2M(char *str1, int len);
bool IsBigEndian();
bool isLittleEndian();
uint32 Reverse32(uint32 x);
uint64 Reverse64(uint64 x);
sm_word ML(byte X, uint8 j);
sm_word SUM(sm_word X, sm_word Y);

#define SM_DPWD_KEY_LEN_MIN		(128/8)
#define SM_DPWD_CHALLENGE_LEN_MIN	(4)
#define SM_DPWD_LEN_MAX			(10)
#define SM_HASH_OUT_LEN			(32)
#define SM_DPWD_PARAM_ERROR		-9001
#define SM_DPWD_NO_MEMORY		-9002

#define SM4_KEY_LEN_ERR  -10001
#define SM4_DATA_LEN_ERR -10002
#define ACTIVE_GEN_ERR -10003
#define SEED_FORMAT_ERR -10004

 

 

 
 

 

//6.  生成解PIN码
/* 
 * 功能：生成解PIN码，令牌被锁时使用该接口生成的解码串进行解码重设密码。
 * 输入参数：
 * lmk             本地存储保护密钥，BCD格式。
 * seedCiper       本地存储种子密文，BCD格式。
 * lockCode        令牌界面显示的锁住码，6~12个数字。
 * 输出参数：
 * resetCode       8个数字的解锁码。
 * 返回：
 * 大于等于0：成功。
 * */
//int UnionGenerateResetPINCode(const char *lmk, const char *seedCiper, const char *lockCode, char *resetCode);
//下列函数为扩展函数，主要是针对手机APP令牌而扩展
//7. 激活令牌
/*
 * 输入参数：
 * userID       用户ID
 * IMEI        手机设备码
 * cvk         8位校验值
 * 输出参数：
 * activeCode       12位激活码
 * 返回：
 * 大于等于0：成功。
*/
int UnionActiveTokenEx(const char* userID, const char* IMEI,const char* cvk, char *activeCode);
//8.  动态口令验证
/* 
 * 功能：验证动态口令 
 * 输入参数：
 * IMEI        手机设备码
 * activeCode       12位激活码
 * QCode          挑战码，如果不使用可送NULL或””。
 * otp             待验证的口令6个数字。
 * offset           当前系统的时钟偏移周期
 * 输出参数：
 * offset           返回时钟偏移周期，此值验证成功时有效。
 * 返回：
 * 大于等于0：成功。
 * */
int UnionVerifyOTPEx(const char* IMEI,const char *activeCode, const char *QCode, const char *otp, int *offset);

//9. 校验激活码
/*
 * 输入参数：
 * userID       用户ID
 * IMEI        手机设备码
 * activeCode       12位激活码
 * 返回：
 * 等于0：成功。
*/
//返回0则校验成功 否则校验失败
int UnionVerifyTokenActiveCode(const char* userID, const char* IMEI,const char *activeCode);

#endif

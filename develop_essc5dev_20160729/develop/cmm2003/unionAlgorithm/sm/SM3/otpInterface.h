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

 

 

 
 

 

//6.  ���ɽ�PIN��
/* 
 * ���ܣ����ɽ�PIN�룬���Ʊ���ʱʹ�øýӿ����ɵĽ��봮���н����������롣
 * ���������
 * lmk             ���ش洢������Կ��BCD��ʽ��
 * seedCiper       ���ش洢�������ģ�BCD��ʽ��
 * lockCode        ���ƽ�����ʾ����ס�룬6~12�����֡�
 * ���������
 * resetCode       8�����ֵĽ����롣
 * ���أ�
 * ���ڵ���0���ɹ���
 * */
//int UnionGenerateResetPINCode(const char *lmk, const char *seedCiper, const char *lockCode, char *resetCode);
//���к���Ϊ��չ��������Ҫ������ֻ�APP���ƶ���չ
//7. ��������
/*
 * ���������
 * userID       �û�ID
 * IMEI        �ֻ��豸��
 * cvk         8λУ��ֵ
 * ���������
 * activeCode       12λ������
 * ���أ�
 * ���ڵ���0���ɹ���
*/
int UnionActiveTokenEx(const char* userID, const char* IMEI,const char* cvk, char *activeCode);
//8.  ��̬������֤
/* 
 * ���ܣ���֤��̬���� 
 * ���������
 * IMEI        �ֻ��豸��
 * activeCode       12λ������
 * QCode          ��ս�룬�����ʹ�ÿ���NULL�򡱡���
 * otp             ����֤�Ŀ���6�����֡�
 * offset           ��ǰϵͳ��ʱ��ƫ������
 * ���������
 * offset           ����ʱ��ƫ�����ڣ���ֵ��֤�ɹ�ʱ��Ч��
 * ���أ�
 * ���ڵ���0���ɹ���
 * */
int UnionVerifyOTPEx(const char* IMEI,const char *activeCode, const char *QCode, const char *otp, int *offset);

//9. У�鼤����
/*
 * ���������
 * userID       �û�ID
 * IMEI        �ֻ��豸��
 * activeCode       12λ������
 * ���أ�
 * ����0���ɹ���
*/
//����0��У��ɹ� ����У��ʧ��
int UnionVerifyTokenActiveCode(const char* userID, const char* IMEI,const char *activeCode);

#endif

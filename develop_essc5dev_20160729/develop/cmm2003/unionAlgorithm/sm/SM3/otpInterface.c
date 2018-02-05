#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "otpInterface.h"
#include "sm3.h"
#include "sm4.h"

char* getRandom(char *data1, int len);
char hexhightoasc(int xxc);
char hexlowtoasc(int xxc);

int TruncateSM3(byte pSrc[32], int nSrcLen, byte pDst[4], int nDstSize);
int SM3_DPasswd(byte *pKey, int nKeyLen, uint64 *pTime, uint64 *pInterval, uint32 *pCounter, char *pChallenge, int nGenLen, char *pDynPwd, int nDynPwdSize);
int UnionSM3Dpwd(char *pKey, unsigned int iTime, int iInterval, int iCounter, char *pChallenge, int iGenLen, char *pwd);


int UnionVerifySN(const char* userID, const char* IMEI,const char* cvk);
int UnionGenInitSeed(const char* IMEI,char* initSeedPlain);
int UnionGenTokenWorkSeed(const char *initSeedPlain,const char *activeCode,char *workSeedPlain);

char g_padding[32][64+1];

char* getRandom(char *data1, int len)
{
    int i = 0;
    srand((int)time(0));
    i = rand()%100000000;
    snprintf(data1, len, "%08d", i);
    return data1;
}

int printHEX(char *title, unsigned char *ptr, int len)
{
	int		i;

	printf("printHEX:: %s = [0x", title);
	for(i = 0; i < len; i++)
	{
		printf(" %02X", ptr[i]);
	}
	printf("]\n");
	return 0;
}


char asctohex(char ch1,char ch2)
{
    char ch;
    if (ch1>='A') ch=(char )((ch1-0x37)<<4);
    else ch=(char)((ch1-'0')<<4);
    if (ch2>='A') ch|=ch2-0x37;
    else ch|=ch2-'0';
    return ch;
}

int aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
    int i,j;

        if (len % 2 == 0)
                j = len / 2;
        else
                j = len / 2 + 1;

    for (i = 0; i < j; i++)
        bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);

    return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
    int i;

    for (i=0;i<len;i++)
    {
        aschex[2*i]   = hexhightoasc(bcdhex[i]);
        aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
    }

    return(len*2);
}



int sm3Data(char *data, char *result)
{
	int			dataLen = 0;
	char			tmpBuf[8192+1];
	char			sm3Value[32+1];

	if(data == NULL || strlen(data) == 0)
	{
		return(0);
	}
	
	dataLen = strlen(data);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	memset(sm3Value, 0, sizeof(sm3Value));
	aschex_to_bcdhex(data, dataLen, tmpBuf);
	sm3(tmpBuf, dataLen / 2, sm3Value);
	bcdhex_to_aschex(sm3Value, 32, result);

	return(64);
}


int unionpow(int base, int pow)
{
	int		i;
	int		result;

	result = base;
	for(i = 0; i < pow - 1; i++)
	{
		result *= base;
	}
	return(result);
}

char		gunionTmpBuf[1024];

char *b_to_a(char *str1, int len)
{
	bcdhex_to_aschex(str1, len, gunionTmpBuf);
	gunionTmpBuf[len * 2] = 0;
	return(gunionTmpBuf);
}

bool IsBigEndian()
{
	union T
	{
		char		c[2];
		short		s;
	};

	union T		t;
	t.s = 0x0031;
	if(t.c[1] == 0x31)
	{
		return(true);
	}
	return(false);
}

bool isLittleEndian()
{
	return(!IsBigEndian());
}

uint32 Reverse32(uint32 x)
{
	uint32		aX[4];
	uint32		y;

	aX[0] = x;
	aX[1] = x;
	aX[2] = x;
	aX[3] = x;

	aX[0] = (aX[0] & 0x000000ff) << 24;
	aX[1] = (aX[1] & 0x0000ff00) << 8;
	aX[2] = (aX[2] & 0x00ff0000) >> 8;
	aX[3] = (aX[3] & 0xff000000) >> 24;
	//printf(" before reverse32 = %d [%02x %02x %02x %02x]\n", x, ((unsigned char *)(&x))[0], ((unsigned char *)(&x))[1], ((unsigned char *)(&x))[2], ((unsigned char *)(&x))[3]);

	 y = aX[0] | aX[1] | aX[2] | aX[3];
	//printf(" after reverse32 = %d [%02x %02x %02x %02x]\n", x, ((unsigned char *)(&y))[0], ((unsigned char *)(&y))[1], ((unsigned char *)(&y))[2], ((unsigned char *)(&y))[3]);
	return(y);

}

uint64 Reverse64(uint64 x)
{
	struct TST
	{
		uint32		w1;
		uint32		w2;
	};
	union TUN
	{
		uint64		u64;
		struct	TST	u32;
	};

	union TUN	tIn, tOut;

	tIn.u64 = x;
	//printf(" before reverse64 %llu [%02x %02x %02x %02x  %02x %02x %02x %02x]\n", x, (unsigned char)((unsigned char *)(&(tIn.u64)))[0], ((unsigned char *)(&(tIn.u32.w1)))[1], ((unsigned char *)(&(tIn.u64)))[2], ((unsigned char *)(&(tIn.u64)))[3], ((unsigned char *)(&(tIn.u64)))[4], ((unsigned char *)(&(tIn.u64)))[5], ((unsigned char *)(&(tIn.u64)))[6], ((unsigned char *)(&(tIn.u64)))[7]);
	tOut.u32.w1 = Reverse32(tIn.u32.w2);
	tOut.u32.w2 = Reverse32(tIn.u32.w1);
	//printf(" after reverse64 %llu [%02x %02x %02x %02x  %02x %02x %02x %02x]\n", x, ((unsigned char *)(&(tOut.u64)))[0], ((unsigned char *)(&(tOut.u32.w1)))[1], ((unsigned char *)(&(tOut.u64)))[2], ((unsigned char *)(&(tOut.u64)))[3], ((unsigned char *)(&(tOut.u64)))[4], ((unsigned char *)(&(tOut.u64)))[5], ((unsigned char *)(&(tOut.u64)))[6], ((unsigned char *)(&(tOut.u64)))[7]);

	return(tOut.u64);
}

sm_word ML(byte X, uint8 j)
{
	if(IsBigEndian())
	{
		return((sm_word)(X << (j % 32)));
	}
	else
	{
		return(Reverse32((sm_word)(X << (j % 32))));
	}
}

sm_word SUM(sm_word X, sm_word Y)
{
	if(IsBigEndian())
	{
		return(X + Y);
	}
	else
	{
		return(Reverse32(Reverse32(X) + Reverse32(Y)));
	}
}

int TruncateSM3(byte pSrc[32], int nSrcLen, byte pDst[4], int nDstSize)
{
	if(nSrcLen != 32 || nDstSize < 4)
	{
		return(-1);
	}
	memset(pDst, 0, nDstSize);

	byte	*S = (byte *)pSrc;
	sm_word S1 = ML(S[0], 24) | ML(S[1], 16) | ML(S[2], 8) | ML(S[3], 0);
	sm_word S2 = ML(S[4], 24) | ML(S[5], 16) | ML(S[6], 8) | ML(S[7], 0);
	sm_word S3 = ML(S[8], 24) | ML(S[9], 16) | ML(S[10], 8) | ML(S[11], 0);
	sm_word S4 = ML(S[12], 24) | ML(S[13], 16) | ML(S[14], 8) | ML(S[15], 0);
	sm_word S5 = ML(S[16], 24) | ML(S[17], 16) | ML(S[18], 8) | ML(S[19], 0);
	sm_word S6 = ML(S[20], 24) | ML(S[21], 16) | ML(S[22], 8) | ML(S[23], 0);
	sm_word S7 = ML(S[24], 24) | ML(S[25], 16) | ML(S[26], 8) | ML(S[27], 0);
	sm_word S8 = ML(S[28], 24) | ML(S[29], 16) | ML(S[30], 8) | ML(S[31], 0);

	sm_word OD = SUM(SUM(SUM(SUM(SUM(SUM(SUM(S1, S2), S3), S4), S5), S6), S7), S8);
	//printf("OD_d = %d\n", OD);
	//printf("OD_x = [%02x %02x %02x %02x]\n", ((unsigned char *)&OD)[0], ((unsigned char *)&OD)[1], ((unsigned char *)&OD)[2], ((unsigned char *)&OD)[3]);
	memcpy(pDst, &OD, sizeof(sm_word));
	return(0);
}

int SM3_DPasswd(byte *pKey, int nKeyLen, uint64 *pTime, uint64 *pInterval, uint32 *pCounter, char *pChallenge, int nGenLen, char *pDynPwd, int nDynPwdSize)
{


	if(pKey == NULL || (pTime == NULL && pCounter == NULL && pChallenge == NULL) 
			|| pDynPwd == NULL || nKeyLen < SM_DPWD_KEY_LEN_MIN || nGenLen > SM_DPWD_LEN_MAX
			|| (pChallenge != NULL && strlen(pChallenge) < SM_DPWD_CHALLENGE_LEN_MIN) || nDynPwdSize < nGenLen)
	{
		return(SM_DPWD_PARAM_ERROR);
	}
	memset(pDynPwd, 0, nDynPwdSize);
	// T = T0 / Tc
	if(pTime != NULL && pInterval != NULL && *pInterval != 0)
	{
		*pTime = (*pTime) / (*pInterval);
	}


	// Convert to big-endian
	if(!IsBigEndian())
	{
		if(pTime != NULL)
		{
			*pTime = Reverse64(*pTime);
		}
		if(pCounter != NULL)
		{
			*pCounter = Reverse32(*pCounter);
		}
	}

	int		offset = 0;
	byte		*sm_i = NULL;
	byte		sm_o[SM_HASH_OUT_LEN] = {0};
	int		sm_i_len = 0;
	int		sm_o_len = 0;
	uint32		pwd = {0};

	// ID(T|C|Q) Length at least 128 bits
	sm_i_len = (pTime ? sizeof(uint64) : 0) + (pCounter ? sizeof(uint32) : 0) + (pChallenge ? strlen(pChallenge) : 0);
	if(sm_i_len < 16)
	{
		// Fill ID to 128 bits with 0 at the end.
		sm_i_len = 16;
	}
	sm_i_len += nKeyLen;

	// Allocate IN-Data memory
	sm_i = (byte *)malloc(sm_i_len);
	if(sm_i == NULL)
	{
		return(SM_DPWD_NO_MEMORY);
	}

	memset(sm_i, 0, sm_i_len);

	// 1.KEY | ID(T|C|Q)
	memcpy(sm_i, pKey, nKeyLen);
	offset = nKeyLen;
	
	if(pTime != NULL)
	{
		memcpy(sm_i + offset, pTime, sizeof(uint64));
		offset += sizeof(uint64);
	}

	if(pCounter != NULL)
	{
		memcpy(sm_i + offset, pCounter, sizeof(uint32));
		offset += (sizeof(uint32));
	}

	if(pChallenge != NULL)
	{
		memcpy(sm_i + offset, pChallenge, strlen(pChallenge));
	}

	// 2. SM3
	//SM3(sm_i, sm_i_len, sm_o, sm_o_len);
	sm_o_len = 32;
	sm3(sm_i, sm_i_len, sm_o);

	// 3. Truncate
	TruncateSM3(sm_o, sm_o_len, (byte *)&pwd, sizeof(pwd));

	//printf("	K:[%s]\r\n",  b_to_a(pKey, nKeyLen));
	//printf("	T:[%llu]\r\n",  (pTime ? *pTime : 0));
	//printf("	C:[%d]\r\n",  (pCounter ? *pCounter : 0));
	//printf("	Q:[%s]\r\n",  b_to_a(pChallenge, pChallenge == NULL ? 0 : strlen(pChallenge)));
	//printf("	SM3-IN:[%s]\r\n",  b_to_a(sm_i, sm_i_len));
	//printf("	SM3-OUT:[%s]\r\n",  b_to_a(sm_o, sm_o_len));
	//printHEX("Cut", (unsigned char *)&pwd, sizeof(pwd));

	// 4.MOD
	if(!IsBigEndian())
	{
		pwd = Reverse32(pwd);
	}

	pwd = pwd % (int)unionpow(10, nGenLen);

	// Output
	char		szFmt[32] = {0};
	sprintf(szFmt, "%%0%dd", nGenLen);
	sprintf(pDynPwd, szFmt, pwd);

	free(sm_i);
	return(0);
}

int UnionSM3Dpwd(char *pKey, unsigned int iTime, int iInterval, int iCounter, char *pChallenge, int iGenLen, char *pwd)
{
	int			ret;
	byte			aKey[64+1];
	int			iKeyLen;
	uint64			uTc, *pTc = NULL;
	uint64			uT0;
	uint32			uC, *pC = NULL;

	iKeyLen = strlen(pKey);
	memset(aKey, 0, sizeof(aKey));
	aschex_to_bcdhex(pKey, iKeyLen, aKey);

	uTc = iTime;
	uT0 = iInterval;
	uC = iCounter;

	if(uC > 0)
	{
		pC = &uC;	
	}

	if(uTc > 0)
	{
		pTc = &uTc;	
	}

	ret = SM3_DPasswd(aKey, iKeyLen / 2, pTc, &uT0, pC, pChallenge, iGenLen, pwd, iGenLen);
	/*
	if(iCounter <= 0)
	{
		ret = SM3_DPasswd(aKey, iKeyLen / 2, &uTc, &uT0, NULL, pChallenge, iGenLen, pwd, iGenLen);
	}
	else
	{
		ret = SM3_DPasswd(aKey, iKeyLen / 2, &uTc, &uT0, &uC, pChallenge, iGenLen, pwd, iGenLen);
	}
	*/
	return(ret);
}

char hexhightoasc(int xxc)
{
    xxc&=0xf0;
    xxc = xxc>>4;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char hexlowtoasc(int xxc)
{
    xxc&=0x0f;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

//去掉种子的填充值。获取真实的种子,返回种子的实际长度
int getRealSeed(const char *seed, char* realSeed)
{
    int i = 0;
    char* pPaddingStart = NULL;
    int seedRealLen = 0;

    if (strlen(seed) != SEED_LEN*2)
    {
        return SEED_FORMAT_ERR;
    }

    initAllPadding();

    for(; i!= 32; ++i)
    {
        pPaddingStart = strstr(seed, g_padding[i]);
        if(pPaddingStart == NULL)
        {
            continue;
        }
        else
        {
            //判断padding是否还有数据
            seedRealLen = pPaddingStart-seed;
            //printf("seedRealLen:%d\n", seedRealLen);
            if (seed[seedRealLen + strlen(g_padding[i])] == '\0')
            {
                memcpy(realSeed, seed, seedRealLen);
                realSeed[seedRealLen] = '\0';
                return seedRealLen;
            }
            else
            {
                return SEED_FORMAT_ERR;
            }
        }  
    }//end for

    seedRealLen = SEED_LEN*2;
    memcpy(realSeed, seed, seedRealLen);
    realSeed[seedRealLen] = '\0';
    return 0;
}

//初始化所有的Padding
int initAllPadding()
{
    int i = 0;
    int j = 0;
    int paddingCounts = 0;
    int len = 0;
    int padding = 0;
    static int allPaddingIsInit = 0;

    if (allPaddingIsInit == 1)
    {
        return 0;
    }

    for(; i!=SEED_LEN; ++i)
    {
        padding = 16 - i % 16;
        paddingCounts = SEED_LEN - i;
        j = 0;
        len = 0;
        for(; j!=paddingCounts; j++)
        {
            len += sprintf(g_padding[i]+len, "%02X", padding);
        }
       // printf("g_padding[%d]:%s\n",i, g_padding[i]);
    }

    allPaddingIsInit = 1;

    return 0;
}





//验证sn
int UnionVerifySN(const char* userID, const char* IMEI,const char* cvk)
{

    
    char sm3In[40+32+1] = {0};
    char sm3Out[64+1] = {0};
    
    char userIDAsc[96+1] = {0};
    char IMEIAsc[48+1] = {0};
    
    bcdhex_to_aschex(userID, strlen(userID), userIDAsc);

    bcdhex_to_aschex(IMEI, strlen(IMEI), IMEIAsc);
    
    if((strlen(IMEIAsc)+strlen(userIDAsc))>72)
    {
    	memcpy(sm3In,IMEIAsc,strlen(IMEIAsc));
    	memcpy(sm3In+strlen(IMEIAsc),userIDAsc,72-strlen(IMEIAsc));
    	
    }
    else
    {
        sprintf(sm3In, "%s%s%0*d",IMEIAsc,userIDAsc,72-strlen(IMEIAsc)-strlen(userIDAsc), 0);
    }

    sm3Data(sm3In, sm3Out);
    if(strncmp(sm3Out,cvk,8) == 0)
    {
    	return 0;
    }
        
    return ACTIVE_GEN_ERR;
	
}
//生成初始种子

int UnionGenInitSeed(const char* IMEI,char* initSeedPlain)
{
     char IMEIAsc[48+1] = {0};

     bcdhex_to_aschex(IMEI, strlen(IMEI), IMEIAsc);
     sprintf(initSeedPlain, "%s%0*d",IMEIAsc, 40-strlen(IMEIAsc), 0);
     return 0;	
}
//生成激活码

//seed -> initSeedPlain
int UnionActiveTokenEx(const char* userID, const char* IMEI,const char* cvk, char *activeCode)
{

    int ret = 0;
    char data1[8+1] = {0};//存放8位随机数
    char otp[6+1] = {0};
    char initSeedPlain[40+1] = {0};
    // 验证userid和IMEI码正确性
    if((ret = UnionVerifySN(userID,IMEI,cvk))<0)
    {
    	return ret;
    }
    
    //生成原始种子
    if((ret = UnionGenInitSeed(IMEI,initSeedPlain))<0)
    {
    	return ret;
    }
         
    //产生一个8位的随机数
    getRandom(data1, sizeof(data1));

    if ((ret = UnionSM3Dpwd(initSeedPlain, 0, 1, 0, data1, 6, otp)) < 0)
    {
        return ret;
    } 

    sprintf(activeCode, "%s%s", data1, &otp[2]);//8位随机数+4位口令

    return ret;
}

int UnionGenTokenWorkSeed(const char *initSeedPlain,const char *activeCode,char *workSeedPlain)
{

    int ret = 0;
    char data1[8+1] = {0};//存放8位随机数
    char otp[6+1] = {0};
    char data1Asc[16+1] = {0};
    char otpAsc[12+1] = {0};
    char activeValueTmp[32+1] = {0};
    char sm3In[40+32+1] = {0};
    char sm3Out[64+1] = {0};
 
        
    //截取8位随机数
    
    memcpy(data1,activeCode,8);
    data1[8] = 0;

    if ((ret = UnionSM3Dpwd(initSeedPlain, 0, 1, 0, data1, 6, otp)) < 0)
    {
        return ret;
    } 

    bcdhex_to_aschex(data1, strlen(data1), data1Asc);

    bcdhex_to_aschex(otp, strlen(otp), otpAsc);


    if(strlen(data1Asc) < 32)
    {
        sprintf(activeValueTmp, "%s%0*d",data1Asc, 32-(int)strlen(data1Asc), 0);
    }
    else
    {
        return ACTIVE_GEN_ERR;
    }

    sprintf(sm3In, "%s%s", initSeedPlain, activeValueTmp);

    sm3Data(sm3In, sm3Out);

    memcpy(workSeedPlain, sm3Out, 40);

    workSeedPlain[40] = '\0';

    return ret;
}




//验证时窗口为2 周期为60 
int UnionVerifyOTPEx(const char* IMEI,const char *activeCode, const char *QCode, const char *otp, int *offset)
{
    int i = 0;
    time_t timeValue = time(0)/60;
    char realOtp[6+1] = {0};
    char initSeedPlain[40+1] = {0};
    char workSeedPlain[40+1] = {0};
    int ret = 0;
     //生成原始种子
    if((ret = UnionGenInitSeed(IMEI,initSeedPlain))<0)
    {
    	printf("initSeedPlain=[%s]\n",initSeedPlain);
    	return ret;
    }
    
    
    //生成工作种子
    if((ret = UnionGenTokenWorkSeed(initSeedPlain,activeCode,workSeedPlain))<0)
    {
    	return ret;
    }

    for (; i!=4; i++)
    {
        if ((ret = UnionSM3Dpwd(workSeedPlain, timeValue+i, 1, 0, (char *)QCode, 6, realOtp)) < 0)
        {
            return ret;
        }
       // printf("i:%d, otp:%s\n", i, realOtp);
        if (strncmp(realOtp, otp, 6) == 0)
        {
            *offset = i;
            return 0;
        }
        else
        {
            if ((ret = UnionSM3Dpwd(workSeedPlain, timeValue-i, 1, 0, (char *)QCode, 6, realOtp)) < 0)
            {
                return ret;
            }

           // printf("i:%d, otp:%s\n", i, realOtp);
            if (strncmp(realOtp, otp, 6) == 0)
            {
                *offset = i*(-1);
                return 0;
            }

        }

    }
    return -1;
    //end for

}

//返回0则校验成功 否则校验失败
int UnionVerifyTokenActiveCode(const char* userID, const char* IMEI,const char *activeCode)
{
    int ret = 0;
    char data1[8+1] = {0};
    char otp[6+1] = {0};
    char initSeedPlain[40+1] = {0};

    if((ret = UnionGenInitSeed(IMEI,initSeedPlain))<0)
    {
        return ret;
    }

    snprintf(data1, sizeof(data1), "%s", activeCode);
    if ((ret = UnionSM3Dpwd(initSeedPlain, 0, 1, 0, data1, 6, otp)) < 0)
    {
        return ret;
    }

    return memcmp(activeCode+8, otp+2, 4);
}



/*
int UnionGenerateResetPINCode(const char *lmk, const char *seedCipher, const char *lockCode, char *resetCode)
{
    int retb = 0;
    char seed[64+1] = {0};
    int ret = 0;

    if (( ret = decodeSeed(lmk, seedCipher, seed))<0)
    {
        return ret;
    }

    //printf("seed:%s\n", seed);
    if ((ret = UnionSM3Dpwd(seed, 0, 1, 0, (char *)lockCode, 8, resetCode)) < 0)
    {
        return ret;
    }

    return ret;
}
*/



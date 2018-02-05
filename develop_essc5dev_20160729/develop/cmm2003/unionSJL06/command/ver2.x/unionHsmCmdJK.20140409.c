//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionDesKey.h"

#include "emv2000AndPbocSyntaxRules.h"
#include "3DesRacalSyntaxRules.h"
#include "unionHsmCmdJK.h"
#include "commWithHsmSvr.h"

int UnionHsmCmdJKEE(char *alg, char *ikindex,char *flag, char *version,char *Goup_Index,
                char *M_number,char *Ls_Data1, char *Ls_Data2, char *Ls_Data3,char *yon,
                char *processkey,char *Key_Header_Len,char *Key_Header,char *Key_Tailer_Len,
                char *Key_Tailer,char *Mac_Header_Len,char *Mac_Header,char *date,char *mac)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;


	if ((alg == NULL) || (ikindex == NULL) || (flag == NULL) || (version == NULL) 
		|| (Goup_Index == NULL) ||(M_number == NULL)|| (yon == NULL) || (Key_Header_Len == NULL)
		 ||(Key_Header == NULL)	|| (Key_Tailer_Len == NULL) ||(Key_Tailer == NULL)|| 
		 (Mac_Header_Len == NULL) ||(Mac_Header == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJKEE:: wrong parameters!\n");
		return(errCodeParameter);
	}

	if((!UnionIsDigitChar(alg[0]))||(!UnionIsDigitChar(ikindex[0]))||(!UnionIsDigitChar(flag[0]))
		||(!UnionIsDigitChar(yon[0])))
	{
		UnionUserErrLog("in UnionHsmCmdJKEE:: parameters error!\n");
		return(errCodeParameter);
	}

	//ƴװָ��
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;

	//ƴalg
	if(atoi(alg)!= 0 && atoi(alg)!= 1)
	{
		UnionUserErrLog("in UnionHsmCmdJKEE:: alg error[%d]!\n",atoi(alg));
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, alg, 1);
	hsmCmdLen += 1;

	//ƴ ikindex
	memcpy(hsmCmdBuf+hsmCmdLen, ikindex, 1);
	hsmCmdLen += 1;	

	//ƴ��ɢ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, flag, 1);
	hsmCmdLen += 1;	

	//ƴ Version
	memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
	hsmCmdLen += 1;	

	//ƴ Goup_Index
	memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
	hsmCmdLen += 2;	

	//ƴ M_number
	if(version[0]=='F')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "FF", 2);
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
		hsmCmdLen += 2;
	}

	//ƴװ Ls_Data1
	if(atoi(flag)==1||atoi(flag)==2||atoi(flag)==3)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data1, 16);
		hsmCmdLen += 16;	
	}

	//ƴװ Ls_Data2
	if(atoi(flag)==2||atoi(flag)==3)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data2, 16);
		hsmCmdLen += 16;	
	}

	//ƴװ Ls_Data3
	if(atoi(flag)==3)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data3, 16);
		hsmCmdLen += 16;	
	}			

	//ƴװ������Կ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, yon, 1);
	hsmCmdLen += 1;		

	//ƴװ������Կ
	if(atoi(yon)==1)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, processkey, 16);
		hsmCmdLen += 16;			
	}

	//ƴװ Key_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header, atoi(Key_Header_Len)*2);
	hsmCmdLen += atoi(Key_Header_Len)*2;		

	//ƴװ Key_Tailer_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Tailer
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer, atoi(Key_Tailer_Len)*2);
	hsmCmdLen += atoi(Key_Tailer_Len)*2;

	//ƴװ Mac_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Mac_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header, atoi(Mac_Header_Len)*2);
	hsmCmdLen += atoi(Mac_Header_Len)*2;
	hsmCmdBuf[hsmCmdLen] = 0;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJKEE:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	hsmCmdBuf[ret] = 0;
	memcpy(date,hsmCmdBuf+2+2,ret-4-8);
	memcpy(mac,hsmCmdBuf+2+2+ret-4-8,8);
	return(ret-4-8);	
}

int UnionHsmCmdJKF2(char *keyIndex, char *algFlag)
{
	char            hsmCmdBuf[512+1];
        int             hsmCmdLen = 0;
	int 		ret = 0;

	if(keyIndex == NULL || algFlag == NULL)
	{
		UnionUserErrLog("in UnionHsmCmdJKF2:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));	

	memcpy(hsmCmdBuf,"F2",2);
        hsmCmdLen = 2;

	if(!UnionIsDigitChar(keyIndex[0]))
	{
		UnionUserErrLog("in UnionHsmCmdJKF2:: keyIndex error!\n");
                return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, keyIndex, 1);
	hsmCmdLen += 1;

	if(algFlag[0] != '0' && algFlag[0] != '1')
	{
		UnionUserErrLog("in UnionHsmCmdJKF2:: algFlag error!algFlag=[%s]\n", algFlag);
                return(errCodeParameter);
	}


	memcpy(hsmCmdBuf+hsmCmdLen, algFlag, 1);
	hsmCmdLen += 1;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJKF2:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	return(0);

}

int UnionHsmCmdJKF6(char *alg, char *ikindex,char *flag, char *version,char *Goup_Index,
                char *M_number,char *Ls_Data1, char *Ls_Data2, char *Key_Header_Len,
                char *Key_Header,char *Key_Tailer_Len,char *Key_Tailer,char *Mac_Header_Len,
                char *Mac_Header,char *date,char *mac)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((ikindex == NULL) || (flag == NULL) || (version == NULL) 
		|| (Goup_Index == NULL) ||(M_number == NULL)|| (Key_Header_Len == NULL)
		 ||(Key_Header == NULL)	|| (Key_Tailer_Len == NULL) ||(Key_Tailer == NULL)|| 
		 (Mac_Header_Len == NULL) ||(Mac_Header == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJKF6:: wrong parameters!\n");
		return(errCodeParameter);
	}


	if((!UnionIsDigitChar(ikindex[0]))||(!UnionIsDigitChar(flag[0])))
	{
		UnionUserErrLog("in UnionHsmCmdJKF6:: parameters error!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	//ƴװָ��
	memcpy(hsmCmdBuf,"F6",2);
	hsmCmdLen = 2;

	//ƴalg
	if(alg == NULL)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "X", 1);
		hsmCmdLen += 1;
	}else
	{
		if(alg[0] != 'X' && alg[0] != 'Y')
		{
			UnionUserErrLog("in UnionHsmCmdJKF6:: alg error!alg=[%s]\n", alg);
			return(errCodeParameter);
		}

		memcpy(hsmCmdBuf+hsmCmdLen, alg, 1);
		hsmCmdLen += 1;
	}

	//ƴ ikindex
	memcpy(hsmCmdBuf+hsmCmdLen, ikindex, 1);
	hsmCmdLen += 1;	

	//ƴ��ɢ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, flag, 1);
	hsmCmdLen += 1;	

	//ƴ Version
	memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
	hsmCmdLen += 1;	

	//ƴ Goup_Index
	memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
	hsmCmdLen += 2;	

	//ƴ M_number
	memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
	hsmCmdLen += 2;

	//ƴװ Ls_Data1
	if(atoi(flag)==1||atoi(flag)==2||atoi(flag)==3)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data1, 16);
		hsmCmdLen += 16;	
	}

	//ƴװ Ls_Data2
	if(atoi(flag)==2)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data2, 16);
		hsmCmdLen += 16;	
	}

	//ƴװ Key_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header, atoi(Key_Header_Len)*2);
	hsmCmdLen += atoi(Key_Header_Len)*2;		

	//ƴװ Key_Tailer_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Tailer
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer, atoi(Key_Tailer_Len)*2);
	hsmCmdLen += atoi(Key_Tailer_Len)*2;

	//ƴװ Mac_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Mac_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header, atoi(Mac_Header_Len)*2);
	hsmCmdLen += atoi(Mac_Header_Len)*2;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJKF6:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
	memcpy(date,hsmCmdBuf+2+2,ret-4-8);
	memcpy(mac,hsmCmdBuf+2+2+ret-4-8,8);
	return(ret-4-8);	
}


int UnionHsmCmdJK50(char *algorithmID,char *version,char *Goup_Index,char *M_number,char *Key_Select, 
		char *LsNum, char *Lsdata,char *Gcdata,char *DataLen,char *Data,char *mac)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((algorithmID == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL)|| 
		(Key_Select == NULL) || (Lsdata == NULL) || (DataLen == NULL) ||(Data == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK50:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        //ƴװָ��
        memcpy(hsmCmdBuf,"50",2);
        hsmCmdLen = 2;

        //ƴװalgorithmID
        if(atoi(algorithmID)!= 0 && atoi(algorithmID)!= 1)
        {
                UnionUserErrLog("in UnionHsmCmdJK50:: algorithmID error[%d]!\n",atoi(algorithmID));
                return(errCodeParameter);
        }

        memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);
	hsmCmdLen ++;

        //ƴ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ Key_Select
        memcpy(hsmCmdBuf+hsmCmdLen, Key_Select, 1);
        hsmCmdLen ++;

        //ƴװ LsNum
	memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
        hsmCmdLen ++;

	//ƴװ Lsdata
	memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);
	if (strncmp(Key_Select,"1",1) == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Gcdata, strlen(Gcdata));
		hsmCmdLen += strlen(Gcdata);
	}
		
	//ƴװ DataLen
	memcpy(hsmCmdBuf+hsmCmdLen, DataLen, 3);
        hsmCmdLen += 3;
		
	//ƴװ Data
	memcpy(hsmCmdBuf+hsmCmdLen, Data, strlen(Data));
        hsmCmdLen += strlen(Data);

        //�������ͨѶ
        if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJK50:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
        memcpy(mac,hsmCmdBuf+2+2,8);
        return(ret-4);
}


int UnionHsmCmdJK52(char *algorithmID,char *version,char *Goup_Index,char *M_number,char *Key_Select, 
					char *LsNum, char *Lsdata,char *Gcdata,char *mac,char *DataLen,char *Data,char *flag)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((algorithmID == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL)|| 
		(Key_Select == NULL) || (Lsdata == NULL) || (mac == NULL) || (DataLen == NULL) ||(Data == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK52:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        //ƴװָ��
        memcpy(hsmCmdBuf,"52",2);
        hsmCmdLen = 2;

        //ƴװalgorithmID
        if(atoi(algorithmID)!= 0 && atoi(algorithmID)!= 1)
        {
                UnionUserErrLog("in UnionHsmCmdJK52:: algorithmID error[%d]!\n",atoi(algorithmID));
                return(errCodeParameter);
        }

        memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);
	hsmCmdLen ++;

        //ƴ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ Key_Select
        memcpy(hsmCmdBuf+hsmCmdLen, Key_Select, 1);
        hsmCmdLen ++;

        //ƴװ LsNum
	memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
        hsmCmdLen ++;

	//ƴװ Lsdata
	memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);
		
	if (strncmp(Key_Select,"1",1) == 0)
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Gcdata, strlen(Gcdata));
		hsmCmdLen += strlen(Gcdata);
	}
		
	//ƴװ mac
	memcpy(hsmCmdBuf+hsmCmdLen, mac, 8);
        hsmCmdLen += 8;
		
	//ƴװ DataLen
	memcpy(hsmCmdBuf+hsmCmdLen, DataLen, 3);
        hsmCmdLen += 3;
		
	//ƴװ Data
	memcpy(hsmCmdBuf+hsmCmdLen, Data, strlen(Data));
        hsmCmdLen += strlen(Data);

        //�������ͨѶ
        if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
		memcpy(flag,"0",1);
                UnionUserErrLog("in UnionHsmCmdJK52:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
	memcpy(flag,"1",1);
        return(ret);
}


int UnionHsmCmdJK54(char *algorithmID,char *version,char *Goup_Index,char *M_number,char *Key_Select, 
                    char *LsNum, char *Lsdata,char *Gcdata,char *mac1,char *DataLen1,char *Data1,char *DataLen2,char *Data2,char *mac2)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((algorithmID == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL)|| 
          	(Key_Select == NULL) || (Lsdata == NULL) || (mac1 == NULL) || (DataLen1 == NULL) || 
		(Data1 == NULL) || (DataLen2 == NULL) ||(Data2 == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK54:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        //ƴװָ��
        memcpy(hsmCmdBuf,"54",2);
        hsmCmdLen = 2;

        //ƴװalgorithmID
        if(atoi(algorithmID)!= 0 && atoi(algorithmID)!= 1)
        {
                UnionUserErrLog("in UnionHsmCmdJK54:: algorithmID error[%d]!\n",atoi(algorithmID));
                return(errCodeParameter);
        }

        memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);
	hsmCmdLen ++;

        //ƴ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ Key_Select
        memcpy(hsmCmdBuf+hsmCmdLen, Key_Select, 1);
        hsmCmdLen ++;

        //ƴװ LsNum
        memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
        hsmCmdLen ++;

        //ƴװ Lsdata
        memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);

        if (strncmp(Key_Select,"1",1) == 0)
        {
                memcpy(hsmCmdBuf+hsmCmdLen, Gcdata, strlen(Gcdata));
                hsmCmdLen += strlen(Gcdata);
        }

        //ƴװ mac1
        memcpy(hsmCmdBuf+hsmCmdLen, mac1, 8);

        hsmCmdLen += 8;

        //ƴװ DataLen1
        memcpy(hsmCmdBuf+hsmCmdLen, DataLen1, 3);
        hsmCmdLen += 3;

        //ƴװ Data1
        memcpy(hsmCmdBuf+hsmCmdLen, Data1, strlen(Data1));
        hsmCmdLen += strlen(Data1);
		
	//ƴװ DataLen2
        memcpy(hsmCmdBuf+hsmCmdLen, DataLen2, 3);
        hsmCmdLen += 3;

        //ƴװ Data2
        memcpy(hsmCmdBuf+hsmCmdLen, Data2, strlen(Data2));
        hsmCmdLen += strlen(Data2);

        //�������ͨѶ
    	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJK54:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
        memcpy(mac2,hsmCmdBuf+2+2,8);
        return(ret-4);
}


int UnionHsmCmdJK56(char *algorithmID,char *version,char *Goup_Index,char *M_number,char *LsNum, char *Lsdata,char *DataLen,char *Data,char *Tac)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((algorithmID == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL) || (Lsdata == NULL) || (DataLen == NULL) || (Data == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK56:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        //ƴװָ��
        memcpy(hsmCmdBuf,"56",2);
        hsmCmdLen = 2;

        //ƴװalgorithmID
        if(atoi(algorithmID)!= 0 && atoi(algorithmID)!= 1)
        {
                UnionUserErrLog("in UnionHsmCmdJK56:: algorithmID error[%d]!\n",atoi(algorithmID));
                return(errCodeParameter);
        }

        memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);
	hsmCmdLen ++;

        //ƴ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ LsNum
        memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
        hsmCmdLen ++;

        //ƴװ Lsdata
        memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);

	//ƴװ DataLen
        memcpy(hsmCmdBuf+hsmCmdLen, DataLen,3);
        hsmCmdLen += 3;

        //ƴװ Data
        memcpy(hsmCmdBuf+hsmCmdLen, Data, strlen(Data));
        hsmCmdLen += strlen(Data);

        //�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJK56:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
        memcpy(Tac,hsmCmdBuf+2+2,16);
        return(ret-4);
}


int UnionHsmCmdJK58(char *algorithmID,char *version,char *Goup_Index,char *M_number,char *LsNum, char *Lsdata,char *mac,char *DataLen,char *Data,char *flag)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((algorithmID == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL) || (Lsdata == NULL) || (mac == NULL) ||  (DataLen == NULL) ||(Data == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK58:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        //ƴװָ��
        memcpy(hsmCmdBuf,"58",2);
        hsmCmdLen = 2;

        //ƴװalgorithmID
        if(atoi(algorithmID)!= 0 && atoi(algorithmID)!= 1)
        {
                UnionUserErrLog("in UnionHsmCmdJK58:: algorithmID error[%d]!\n",atoi(algorithmID));
                return(errCodeParameter);
        }

        memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 1);
	hsmCmdLen ++;

        //ƴ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ LsNum
        memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
        hsmCmdLen ++;

        //ƴװ Lsdata
        memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);
		
	//ƴװ mac
        memcpy(hsmCmdBuf+hsmCmdLen, mac, strlen(mac));
        hsmCmdLen += strlen(mac);

	//ƴװ DataLen
        memcpy(hsmCmdBuf+hsmCmdLen, DataLen,3);
        hsmCmdLen += 3;

        //ƴװ Data
        memcpy(hsmCmdBuf+hsmCmdLen, Data, strlen(Data));
        hsmCmdLen += strlen(Data);

        //�������ͨѶ
    	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
		memcpy(flag,"0",1);
		UnionUserErrLog("in UnionHsmCmdJK58:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf[ret] = 0;
	memcpy(flag,"1",1);

        return(ret-2);
}


int UnionHsmCmdJK59(char *LsNum,char *Key_Flag,char *version,char *Goup_Index,char *M_number,char *Lsdata,char *GC_Data,char *InputData,char *Cdata)
{
        int             ret;
        char            hsmCmdBuf[10240+1];
        int             hsmCmdLen = 0;

        if ((Key_Flag == NULL) ||  (version == NULL) || (Goup_Index == NULL) || (M_number == NULL) || (Lsdata == NULL) || (GC_Data == NULL) || (InputData == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdJK59:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
        //ƴװָ��
        memcpy(hsmCmdBuf,"59",2);
        hsmCmdLen = 2;

	//ƴװ��ɢ����
        memcpy(hsmCmdBuf+hsmCmdLen, LsNum, 1);
	hsmCmdLen ++;

	//ƴװ��Կ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Flag, 1);
	hsmCmdLen ++;

        //ƴװ Version
        memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
        hsmCmdLen += 1;

        //ƴװ Goup_Index
        memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
        hsmCmdLen += 2;

        //ƴ M_number
        memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
        hsmCmdLen += 2;

        //ƴװ Lsdata
        memcpy(hsmCmdBuf+hsmCmdLen, Lsdata, strlen(Lsdata));
        hsmCmdLen += strlen(Lsdata);
		
	if (Key_Flag[0] == '1')
	{
		//ƴװ GC_Data
		memcpy(hsmCmdBuf+hsmCmdLen, GC_Data, strlen(GC_Data));
		hsmCmdLen += strlen(GC_Data);
	}

        //ƴװ InputData
        memcpy(hsmCmdBuf+hsmCmdLen, InputData, strlen(InputData));
        hsmCmdLen += strlen(InputData);

        //�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJK59:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	UnionLog("in UnionHsmCmdJK59:: ret = %d\n",ret);
        hsmCmdBuf[ret] = 0;

	memcpy(Cdata,hsmCmdBuf+4,16);
        return(ret-4);
}


int UnionHsmCmdJKE3(char *alg, char *ikindex,char *flag,char *yon,char *processkey,char *Key_Header_Len,char *Key_Header,char *Key_Tailer_Len,
              char *Key_Tailer,char *Mac_Header_Len,char *Mac_Header,char *data,char *mac)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((alg == NULL) || (ikindex == NULL) || (flag == NULL) || (yon == NULL) || (Key_Header_Len == NULL)
		 ||(Key_Header == NULL)	|| (Key_Tailer_Len == NULL) ||(Key_Tailer == NULL)|| (Mac_Header_Len == NULL) ||(Mac_Header == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJKE3:: wrong parameters!\n");
		return(errCodeParameter);
	}

	if((!UnionIsDigitChar(alg[0]))||(!UnionIsDigitChar(ikindex[0]))||(!UnionIsDigitChar(flag[0]))
		||(!UnionIsDigitChar(yon[0])))
	{
		UnionUserErrLog("in UnionHsmCmdJKE3:: parameters error!\n");
		return(errCodeParameter);
	}


	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	//ƴװָ��
	memcpy(hsmCmdBuf,"E3",2);
	hsmCmdLen = 2;

	//ƴ�㷨ѡ��
	if(atoi(alg)!= 0 && atoi(alg)!= 1)
	{
		UnionUserErrLog("in UnionHsmCmdJKE3:: alg error[%d]!\n",atoi(alg));
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, alg, 1);
	hsmCmdLen += 1;

	//ƴ IK/TK����
	memcpy(hsmCmdBuf+hsmCmdLen, ikindex, 1);
	hsmCmdLen += 1;	

	//ƴflag
	memcpy(hsmCmdBuf+hsmCmdLen, flag, 1);
	hsmCmdLen += 1;	

	//ƴװʹ�ù�����Կ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, yon, 1);
	hsmCmdLen += 1;		

	//ƴװ������Կ
	if(atoi(yon)==1)
	{
		if (alg[0] == '0')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, processkey, 16);
			hsmCmdLen += 16;			
		}
		
		else if (alg[0] == '1')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, processkey, 32);
			hsmCmdLen += 32;			
		}
	}

	//ƴװ Key_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header, atoi(Key_Header_Len)*2);
	hsmCmdLen += atoi(Key_Header_Len)*2;		

	//ƴװ Key_Tailer_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Tailer
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer, atoi(Key_Tailer_Len)*2);
	hsmCmdLen += atoi(Key_Tailer_Len)*2;

	//ƴװ Mac_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Mac_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header, atoi(Mac_Header_Len)*2);
	hsmCmdLen += atoi(Mac_Header_Len)*2;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJKE3:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	hsmCmdBuf[ret] = 0;
	memcpy(data,hsmCmdBuf+2+2,ret-4-8);
	memcpy(mac,hsmCmdBuf+2+2+ret-4-8,8);

	return(ret-4-8);	
}


int UnionHsmCmdJKE5(char *alg, char *ikindex,char *flag, char *version,char *Goup_Index,
                char *M_number,char *Ls_Data,char *yon,char *processkey,char *Key_Header_Len,char *Key_Header,char *Key_Tailer_Len,
                char *Key_Tailer,char *Mac_Header_Len,char *Mac_Header,char *date,char *mac)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;


	if ((alg == NULL) || (ikindex == NULL) || (flag == NULL) || (version == NULL) 
		|| (Goup_Index == NULL) ||(M_number == NULL)|| (yon == NULL) || (Key_Header_Len == NULL)
		 ||(Key_Header == NULL)	|| (Key_Tailer_Len == NULL) ||(Key_Tailer == NULL)|| 
		 (Mac_Header_Len == NULL) ||(Mac_Header == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdJKE5:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

	//ƴװָ��
	memcpy(hsmCmdBuf,"E5",2);
	hsmCmdLen = 2;

	//ƴalg
	if(atoi(alg)!= 0 && atoi(alg)!= 1)
	{
		UnionUserErrLog("in UnionHsmCmdJKE5:: alg error[%d]!\n",atoi(alg));
		return(errCodeParameter);
	}

	memcpy(hsmCmdBuf+hsmCmdLen, alg, 1);
	hsmCmdLen += 1;

	//ƴ ikindex
	memcpy(hsmCmdBuf+hsmCmdLen, ikindex, 1);
	hsmCmdLen += 1;	

	//ƴ��ɢ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, flag, 1);
	hsmCmdLen += 1;	

	//ƴ Version
	memcpy(hsmCmdBuf+hsmCmdLen, version, 1);
	hsmCmdLen += 1;	

	//ƴ Goup_Index
	memcpy(hsmCmdBuf+hsmCmdLen, Goup_Index, 2);
	hsmCmdLen += 2;	

	//ƴ M_number
	if(version[0]=='F')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, "FF", 2);
		hsmCmdLen += 2;
	}
	else
	{
		memcpy(hsmCmdBuf+hsmCmdLen, M_number, 2);
		hsmCmdLen += 2;
	}

	//ƴװ Ls_Data
	if (flag[0] != '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, Ls_Data, strlen(Ls_Data));
		hsmCmdLen += strlen(Ls_Data);	
	}

	//ƴװ������Կ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, yon, 1);
	hsmCmdLen += 1;		

	//ƴװ������Կ
	if(atoi(yon)==1)
	{
		if (alg[0] == '0')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, processkey, 16);
			hsmCmdLen += 16;			
		}
		else if (alg[0] == '1')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, processkey, 32);
			hsmCmdLen += 32;			
		}
	}

	//ƴװ Key_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Header, atoi(Key_Header_Len)*2);
	hsmCmdLen += atoi(Key_Header_Len)*2;		

	//ƴװ Key_Tailer_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Key_Tailer
	memcpy(hsmCmdBuf+hsmCmdLen, Key_Tailer, atoi(Key_Tailer_Len)*2);
	hsmCmdLen += atoi(Key_Tailer_Len)*2;

	//ƴװ Mac_Header_Len
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header_Len, 2);
	hsmCmdLen += 2;	

	//ƴװ Mac_Header
	memcpy(hsmCmdBuf+hsmCmdLen, Mac_Header, atoi(Mac_Header_Len)*2);
	hsmCmdLen += atoi(Mac_Header_Len)*2;

	//�������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf, hsmCmdLen, hsmCmdBuf, sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdJKE5:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	hsmCmdBuf[ret] = 0;
	memcpy(date,hsmCmdBuf+2+2,ret-4-8);
	memcpy(mac,hsmCmdBuf+2+2+ret-4-8,8);
	return(ret-4-8);	
}

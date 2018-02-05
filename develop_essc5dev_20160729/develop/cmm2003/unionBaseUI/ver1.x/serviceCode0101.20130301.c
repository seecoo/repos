//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionREC.h"
#include "UnionMD5.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"
#include "UnionStr.h"
#include "sm2.h"
#include "otpInterface.h"


/***************************************
�������:	0101
������:		�û���¼
��������:	�û���¼
***************************************/
int UnionDealServiceCode0101(PUnionHsmGroupRec phsmGroupRec)
{
	
	 
	int	ret;
	int	i = 0;
	int	len = 0;
	int	totalNum = 0;
	int	loginTimes;
	int	maxFreeTimes;	// ������ʱ��
	int	maxWrongPasswordTimes = 5;
	int	wrongPasswordTimes = 0;
	long	freeTimes;	// �ѿ��е�ʱ��
	char	userID[64];
	char	sysID[32];
	char	userName[128];
	char	userRoleList[512];
	char	organization[128];
	char	transTime[32];
	char	createTime[32];
	char	lastLoginTime[32];
	char	tmpBuf[2048];
	char	sql[512];
	char	reqPassword[128+1];
	char	userPassword[128+1];
	char	localPassword[128+1];
	char	identifyCode[128+1];
	char	clientIPAddr[64];
	char	userStatus[32];
	char	loginFlag[32];
	char	lastLoginLocation[64];
	char	lastOperationTime[32];
	char	systemTime[32];
	char	operTermList[2048];
	char	loginSysID[32];
	char	remark[128];
	int	maxOperTerm = 100;
	char	operTermGrp[maxOperTerm][128];
	int	operTermNum = 0;
	char	ipAddr[64];
	char	passwdUpdateTime[32];
	char	userPK[512];
	char	organizationID[64];
	char	*ptr = NULL;
	int userPK_len;
	char x_coordinate[1024];
	char y_coordinate[1024];
	char r_coordinate[1024];
	char s_coordinate[1024];
	char x_coordinate_asc[1024];
	char y_coordinate_asc[1024];
	char r_coordinate_asc[1024];
	char s_coordinate_asc[1024];
	int x_coordinate_len;
	int y_coordinate_len;
	int r_coordinate_len;
	int s_coordinate_len;
	int reqPassword_len;
	int localPassword_len;
	char	loginMode[32];
	int	ukeyVerifyMode = 2;
	int	isVerifyPasswd = 1; // ʹ��usbKǩ��ʱ������Ա�����
	char	otpGrp[3][128];
	int     count;
	char 	QCode[20+1];
	int	offset;
	char	tmp[16+1];
	char	abOfMyself[40+1];
	char	clientVersion[40+1];

	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// ��ȡϵͳID
	memset(sysID,0,sizeof(sysID));
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
		return(ret);
	}

	// ��ȡʱ��
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// ��ȡ�ͻ���IP
	memset(clientIPAddr,0,sizeof(clientIPAddr));
	if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
		return(ret);
	}

	// ��ȡ��������
	memset(reqPassword,0,sizeof(reqPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",reqPassword,sizeof(reqPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/password");
		return(ret);
	}

	// ��ȡUK��֤�汾
	if ((ret = UnionReadRequestXMLPackageValue("body/ukeyVerifyMode",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		tmpBuf[ret] = 0;
		ukeyVerifyMode = atoi(tmpBuf);
	}
	
	//add by yangw 20160427 begin
	//������ԭ���пͻ��˰汾�жϣ�����ͻ���û���Ͱ汾�ţ����ֹ�ͻ��˵�½
	// ��ȡ�ͻ�Ӣ�ļ�д
	memset(abOfMyself,0,sizeof(abOfMyself));
	if ((ptr = UnionReadStringTypeRECVar("abOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadStringTypeRECVar[%s]!\n","abOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	
	strcpy(abOfMyself,ptr);
	if(strcasecmp(abOfMyself,"zyyh")==0)
	{
		// ��ȡ�ͻ��˰汾
		memset(clientVersion,0,sizeof(clientVersion));
		if ((ret = UnionReadRequestXMLPackageValue("body/clientVersion",clientVersion,sizeof(clientVersion))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: clientVersion !\n");	
			UnionSetResponseRemark("�ͻ��˰汾���ͣ�����ϵ����Ա");
			return(ret);	
		}
		
	}
	//add by yangw 20160427 begin

	// ��ȡ�û���
	//sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	snprintf(sql,sizeof(sql),"select * from sysUser left join organization on sysUser.organization = organization.organizationName where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// ��ȡ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);
	if (totalNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userID[%s] not found!\n",userID);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	
	UnionLocateXMLPackage("detail", 1);
	
	// ��ȡ�û�״̬
	memset(userStatus,0,sizeof(userStatus));
	if ((ret = UnionReadXMLPackageValue("userStatus", userStatus, sizeof(userStatus))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userStatus");
		return(ret);
	}
	
	if (!atoi(userStatus))
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userStatus[%s]!\n",userStatus);
		return(errCodeOperatorMDL_OperatorIsLocked);
	}
	
	// ��ȡ��¼��ʽ
	memset(loginMode,0,sizeof(loginMode));
	if ((ret = UnionReadXMLPackageValue("loginMode", loginMode, sizeof(loginMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadXMLPackageValue[%s]!\n","loginMode");
		return(ret);
	}

        if(loginMode[0] != '1')//��̬�����¼
        {
		// ��ȡ��֤��
		memset(identifyCode,0,sizeof(identifyCode));
		if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
			return(ret);
		}
		
		// �����֤��
		if (strlen(identifyCode) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: δ��ȡ��֤��!\n");
			return(errCodeOperatorMDL_IdentifyCodeNotExists);
		}
        }
	
	// ��ȡ��¼״̬
	memset(loginFlag,0,sizeof(loginFlag));
	if ((ret = UnionReadXMLPackageValue("loginFlag", loginFlag, sizeof(loginFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginFlag");
		return(ret);
	}
	
	// ��ȡ�ϴε�¼�ص�
	memset(lastLoginLocation,0,sizeof(lastLoginLocation));
	if ((ret = UnionReadXMLPackageValue("lastLoginLocation", lastLoginLocation, sizeof(lastLoginLocation))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastLoginLocation");
		return(ret);
	}
	
	memset(lastOperationTime,0,sizeof(lastOperationTime));
	if (atoi(loginFlag))
	{
		// ��ȡ��¼ϵͳ���
		memset(loginSysID,0,sizeof(loginSysID));
		if ((ret = UnionReadXMLPackageValue("loginSysID", loginSysID, sizeof(loginSysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginSysID");
			return(ret);
		}
		if (strcmp(sysID,loginSysID) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: sysID[%s] != loginSysID[%s]!\n",sysID,loginSysID);
			return(errCodeOperatorMDL_AlreadyLogon);
		}

		if (strcmp(clientIPAddr,lastLoginLocation) != 0)
		{
			if ( (maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfUser"))<=0)
         			maxFreeTimes = 1800;  // Ĭ��1800��
         		
         		// ��ȡ�������ʱ��
			if ((ret = UnionReadXMLPackageValue("lastOperationTime", lastOperationTime, sizeof(lastOperationTime))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastOperationTime");
				return(ret);
			}
			UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
			if ((freeTimes - maxFreeTimes) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: �û�[%s]�Ѿ���¼����ֹ��¼!\n",userID);
				return(errCodeOperatorMDL_AlreadyLogon);
			}
		}
	}

	// ����������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("wrongPasswordTimes", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","tmpBuf");
		return(ret);
	}
	
	if ((wrongPasswordTimes = atoi(tmpBuf)) >= maxWrongPasswordTimes)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: wrongPasswordTimes[%d] >= %d,�����û�!\n",wrongPasswordTimes,maxWrongPasswordTimes);
		return(errCodeOperatorMDL_PasswordLocked);
	}

	// ��ȡ����
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s%s%s",transTime,identifyCode,userPassword);
	memset(localPassword,0,sizeof(localPassword));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)localPassword);
	
	//add by yangw 20160422 begin
	//�������볤���жϣ����Ϊ40λ����ʾ����ϵͳ��Ǩ�Ƶ����ݣ���ʾ����Ϊ�ϸ�ʽ��ǿ���޸� userPassword
	
	if(strlen(userPassword)== 40)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userPassword [%s]!\n",userPassword);	
		UnionSetResponseRemark("���벻Ϊ���¸�ʽ������������");
		return(errCodeOperatorMDL_OperatorPwdOldEncType);	
	}
	
	//add by yangw 20160422 end
	
	
	
	
	
	
	//��֤��½����
	if (loginMode[0] == '2' && (ukeyVerifyMode == 1 || ukeyVerifyMode == 2))	 // USBKey��½
	{
		memset(userPK,0,sizeof(userPK));
		if ((ret = UnionReadXMLPackageValue("userPK", userPK, sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPK");
			return(ret);
		}
		userPK_len = ret;
		if(memcmp(userPK,"SM2",3) == 0)//edit by yang 20150105 add the branch 
		{
			userPK_len -= 3;
			memmove(userPK,userPK+3,userPK_len);
			userPK[userPK_len] = 0;
			
			reqPassword_len = strlen(reqPassword);
			localPassword_len = strlen(localPassword);
	
			memset(x_coordinate,0,sizeof(x_coordinate));
			memset(y_coordinate,0,sizeof(y_coordinate));
			memset(r_coordinate,0,sizeof(r_coordinate));
			memset(s_coordinate,0,sizeof(s_coordinate));
			memset(x_coordinate_asc,0,sizeof(x_coordinate_asc));
			memset(y_coordinate_asc,0,sizeof(y_coordinate_asc));
			memset(r_coordinate_asc,0,sizeof(r_coordinate_asc));
			memset(s_coordinate_asc,0,sizeof(s_coordinate_asc));
		
			//���x y
			memcpy(x_coordinate_asc,userPK,userPK_len/2);
			memcpy(y_coordinate_asc,userPK+userPK_len/2,userPK_len/2);
			
			//���ǩ��r s
			memcpy(r_coordinate_asc,reqPassword,reqPassword_len/2);
			memcpy(s_coordinate_asc,reqPassword+reqPassword_len/2,reqPassword_len/2);
			
			x_coordinate_len = aschex_to_bcdhex(x_coordinate_asc,userPK_len/2,x_coordinate);
			y_coordinate_len = aschex_to_bcdhex(y_coordinate_asc,userPK_len/2,y_coordinate);
			r_coordinate_len = aschex_to_bcdhex(r_coordinate_asc,reqPassword_len/2,r_coordinate);
			s_coordinate_len = aschex_to_bcdhex(s_coordinate_asc,reqPassword_len/2,s_coordinate);
						
			ret = sm2_verify((unsigned char *)localPassword,localPassword_len,(unsigned char *)r_coordinate,r_coordinate_len,(unsigned char *)s_coordinate,s_coordinate_len, (unsigned char *)x_coordinate,x_coordinate_len, (unsigned char *)y_coordinate,y_coordinate_len);
			if (ret!=1)
			{
				// �����û���
				memset(sql,0,sizeof(sql));
				if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
					sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
				else
					sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
		
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
				}	
				UnionUserErrLog("in UnionDealServiceCode0101:: sm2_verify failed!\n");
				return(errCodeOperatorMDL_WrongPassword);
			}
			isVerifyPasswd = 0;
		}
		else if(memcmp(userPK,"RSA",3) == 0)
		{
			userPK_len -= 3;
			memmove(userPK,userPK+3,userPK_len);
			userPK[userPK_len] = 0;
		}
	}
	else if(loginMode[0] == '1')//��̬�����¼
	{
		memset(userPK,0,sizeof(userPK));
		if ((ret = UnionReadXMLPackageValue("userPK", userPK, sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPK");
			return(ret);
		}
		//userPK�����ŵ���IMEI��activeCode
		memset(otpGrp,0,sizeof(otpGrp));
		if ((count = UnionSeprateVarStrIntoVarGrp(userPK,strlen(userPK),'|',otpGrp,3)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSeprateVarStrIntoVarGrp otpData = %s !\n",userPK);  //
			return(count);
		}
		
		// ��ȡ��ս��
		memset(QCode,0,sizeof(QCode));
		if ((ret = UnionReadRequestXMLPackageValue("body/QCode",QCode,sizeof(QCode))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/QCode");
		}
		
		//��ȡƫ����
		memset(tmp,0,sizeof(tmp));
		if ((ret = UnionReadRequestXMLPackageValue("body/offset",tmp,sizeof(tmp))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/offset");
			offset = 0;
		}
		else
		{
			offset = atoi(tmp);
		}
		//��֤����
		if((ret = UnionVerifyOTPEx(otpGrp[0],otpGrp[1],NULL,reqPassword,&offset))<0)
		{
			//������֤ʧ��
			// �����û���
			memset(sql,0,sizeof(sql));
			if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
			else
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
	
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
			}	
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionVerifyOTPEx failed!\n");
			return(errCodeOperatorMDL_WrongPassword);
		}
		isVerifyPasswd = 0;
	}

	if (isVerifyPasswd)
	{
		if (strcmp(localPassword,reqPassword) != 0)
		{
			// �����û���
			memset(sql,0,sizeof(sql));
			if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
			else
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
	
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
				//return(ret);
			}
			
			UnionUserErrLog("in UnionDealServiceCode0101:: tmpBuf = [%s] localPassword[%s] != reqPassword[%s]!\n",tmpBuf,localPassword,reqPassword);
			return(errCodeOperatorMDL_WrongPassword);
		}
	}
	// ��ȡ����
	memset(userName,0,sizeof(userName));
	if ((ret = UnionReadXMLPackageValue("userName", userName, sizeof(userName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userName");
		return(ret);
	}

	// ��ȡ��ɫ
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList", userRoleList, sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		return(ret);
	}

	// ��ȡ����
	memset(organization,0,sizeof(organization));
	if ((ret = UnionReadXMLPackageValue("organization", organization, sizeof(organization))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","organization");
		return(ret);
	}

	// ��ȡ����ID
	if ((ret = UnionReadXMLPackageValue("organizationID", organizationID, sizeof(organizationID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","organizationID");
		return(ret);
	}
	organizationID[ret] = 0;


	// ��ȡ��¼����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("loginTimes", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginTimes");
		return(ret);
	}
	loginTimes = atoi(tmpBuf);

	// ��ȡ����ʱ��
	memset(createTime,0,sizeof(createTime));
	if ((ret = UnionReadXMLPackageValue("createTime", createTime, sizeof(createTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","createTime");
		return(ret);
	}

	// ��ȡ�ϴε�¼ʱ��
	memset(lastLoginTime,0,sizeof(lastLoginTime));
	if ((ret = UnionReadXMLPackageValue("lastLoginTime", lastLoginTime, sizeof(lastLoginTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastLoginTime");
		return(ret);
	}
	// ��ȡ�����ն��б�
	memset(operTermList,0,sizeof(operTermList));
	if ((ret = UnionReadXMLPackageValue("operTermList", operTermList, sizeof(operTermList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","operTermList");
		//return(ret);
	}

	// ��ȡ�������ʱ��
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime",passwdUpdateTime,sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue [%s]!\n","passwdUpdateTime");	
		return(ret);	
	}
	passwdUpdateTime[ret] = 0;

	// ��ȡ��ע
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadXMLPackageValue("remark", remark, sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","remark");
		//return(ret);
	}

	if (strlen(operTermList) != 0)
	{
		memset(operTermGrp,0,sizeof(operTermGrp));
		if ((operTermNum = UnionSeprateVarStrIntoVarGrp(operTermList,strlen(operTermList),',',operTermGrp,maxOperTerm)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSeprateVarStrIntoVarGrp[%s]!\n",operTermList);
			return(operTermNum);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		len = 0;
		for (i = 0; i < operTermNum; i++)
		{
			len += sprintf(tmpBuf + len,"'%s',",operTermGrp[i]);
		}

		tmpBuf[len-1] = 0;	

		// ���IP��ַ
		memset(sql,0,sizeof(sql));
		sprintf(sql,"select ipAddr from operTerm where termName in (%s)",tmpBuf);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: ipAddr[%s] is invalid!\n",clientIPAddr);
			UnionSetResponseRemark("����ԱIP��ַ�Ƿ�");
			return(errCodeInvalidIPAddr);
		}

		// ��ȡ��¼����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		else
			totalNum = atoi(tmpBuf);
		
		for (i = 0; i < totalNum; i++)
		{
			if ((ret = UnionLocateXMLPackage("detail",i+1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionLocateXMLPackage[%s][%d]!\n","detail",i+1);
				return(ret);

			}

			memset(ipAddr,0,sizeof(ipAddr));
			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s][%d]!\n","ipAddr",i+1);
				return(ret);
			}

			if (strcmp(ipAddr,clientIPAddr) == 0)
			{
				break;
			}
		}
		
		if (i >= totalNum)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: cliIPAddr[%s] != ipAddr[%s]  is invalid[%d]!\n",clientIPAddr,ipAddr,i);
			UnionSetResponseRemark("����ԱIP��ַ�Ƿ�");
			return(errCodeInvalidIPAddr);
		}
	}

	// ��������Ƿ����
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionIsOverduePasswd [%s]!\n",passwdUpdateTime);	
		UnionSetResponseRemark("�����ѹ��ڣ�����������");
		return(errCodeOperatorMDL_OperatorPwdOverTime);
	}
	
	
	

	memset(systemTime,0,sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);
	
	// �����û���
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set loginFlag = 1, "
		"loginTimes = loginTimes + 1, "
		"wrongPasswordTimes = 0, "
		"lastLoginLocation = '%s', "
		"lastLoginTime = '%s', "
		"lastOperationTime = '%s', "
		"identifyCode = '', "
		"loginSysID = '%s' "
		"where userID = '%s'",
		clientIPAddr,systemTime,systemTime,sysID,userID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}
	
	// ������Ӧ����
	// ��������
	if ((ret = UnionSetResponseXMLPackageValue("body/userName",userName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/userName");
		return(ret);
	}
	// ���ý�ɫ
	if ((ret = UnionSetResponseXMLPackageValue("body/userRoleList",userRoleList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/userRoleList");
		return(ret);
	}
	// ������֯
	if ((ret = UnionSetResponseXMLPackageValue("body/organization",organization)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/organization");
		return(ret);
	}

	// ������֯ID
	if ((ret = UnionSetResponseXMLPackageValue("body/organizationID",organizationID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/organizationID");
		return(ret);
	}
	// ���õ�¼����
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",loginTimes + 1);
	if ((ret = UnionSetResponseXMLPackageValue("body/loginTimes",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/loginTimes");
		return(ret);
	}
	// ���ô���ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",createTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/createTime");
		return(ret);
	}
	// �����ϴε�¼ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginTime",lastLoginTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginTime");
		return(ret);
	}
	// �����ϴε�¼�ص�
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginLocation",lastLoginLocation)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginLocation");
		return(ret);
	}
	// ���ò����ն��б�
	if ((ret = UnionSetResponseXMLPackageValue("body/operTermList",operTermList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/operTermList");
		return(ret);
	}
	// ���ñ�ע
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/remark");
		return(ret);
	}
	
	return(0);
}


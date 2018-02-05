//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionTBLRecInterface.h"
//#include "mngSvrCommProtocol.h"
//#include "esscRes.h"
//#include "mngSvrTeller.h"
//#include "unionSvrConf.h"
#include "unionRecFile.h"

#include "unionFldValueAssign.h"
#include "unionEnumValueDef.20080724.h"

//---------------------------------------------------------------------------
/*
����	������ֵ��������ֵת��Ϊ��ʾ��ʽ
�������
	method          ��ֵ����
        value           ԭֵ
        lenOfValue      ԭֵ����
        sizeOfBuf       ���ջ����С
�������
	displayValue    ��ʾֵ
����ֵ
	>=0		��ʾֵ����
	<0		�������
*/
int UnionConvertFldValueIntoDisplayFormat(char *method,char *value,int lenOfValue,char *displayValue,int sizeOfBuf)
{
        if ((method == NULL) || (value == NULL) || (displayValue == NULL))
                return(errCodeParameter);
        if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // ö��ֵ
        {
                return(UnionReadEnumValueOtherAttr(method+strlen(conFldValueSetMethodEnumGrp),0,value,displayValue,sizeOfBuf));
        }
        strcpy(displayValue,value);
        return(lenOfValue);
}

//---------------------------------------------------------------------------
/*
����	������ֵ��������ֵ����ʾ��ʽת��Ϊʵ��ֵ
�������
	method          ��ֵ����
	displayValue    ��ʾֵ
        lenOfDisplayValue      ԭֵ����
        sizeOfBuf       ���ջ����С
�������
        value           ԭֵ
����ֵ
	>=0		��ʾֵ����
	<0		�������
*/
int UnionConvertFldValueFromDisplayFormat(char *method,char *displayValue,int lenOfDisplayValue,char *value,int sizeOfBuf)
{
        if ((method == NULL) || (value == NULL) || (displayValue == NULL))
                return(errCodeParameter);
        if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // ö��ֵ
        {
                return(UnionReadEnumValueOtherAttr(method+strlen(conFldValueSetMethodEnumGrp),1,displayValue,value,sizeOfBuf));
        }
        strcpy(value,displayValue);
        return(lenOfDisplayValue);
}

//---------------------------------------------------------------------------
/*
����	���ļ��ж�ȡ��ֵ��������
�������
	fileName	�ļ���
�������
	prec		��ֵ��������
����ֵ
	>=0		��ֵ��������Ŀ
	<0		�������
*/
int UnionReadFldValueAssignMethodGrpDef(char *fileName,PUnionFldValueAssignMethodGrp prec)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	FILE	*fp;
        int     index;

	if ((fileName == NULL) || (prec == NULL))
		return(errCodeParameter);

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFldValueAssignMethodGrpDef:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(prec,0,sizeof(prec));
	prec->fldNum = 0;
	while (!feof(fp))
	{
		if (prec->fldNum >= conMaxNumOfQueryFld)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"valueFldName",prec->fldName[prec->fldNum],sizeof(prec->fldName[prec->fldNum]))) <= 0)
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"fldAlais",prec->fldAlais[prec->fldNum],sizeof(prec->fldAlais[prec->fldNum]))) <= 0)
			strcpy(prec->fldAlais[prec->fldNum],prec->fldName[prec->fldNum]);
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"value",prec->fldValue[prec->fldNum],sizeof(prec->fldValue[prec->fldNum]))) <= 0)
			strcpy(prec->fldValue[prec->fldNum],"");
		prec->fldNum += 1;
	}
	fclose(fp);
        UnionNullLog("fldValueSetMethod defined in [%s]\n",fileName);
        for (index = 0; index < prec->fldNum; index++)
        {
                UnionNullLog("[%s] [%s] [%s]\n",prec->fldName[index],prec->fldAlais[index],prec->fldValue[index]);
        }
        UnionNullLog("fldValueSetMethod num = [%d]\n",prec->fldNum);
	return(prec->fldNum);
}

/*
����	��ָ�����ָ����ĸ�ֵ����
�������
	tblName         �����
        isFldAlaisUsed  ���������������������  1�����������0��������
        fldTag          �����������
�������
	method		�����ĸ�ֵ����
����ֵ
	>=0		��ֵ�����ĳ���
	<0		�������
*/
int UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(char *tblName,int isFldAlaisUsed,char *fldTag,char *method,int sizeOfBuf)
{
        int     ret;
        char    fileName[512+1];
        FILE    *fp;
        char    tmpBuf[512+1];
        char    fldName[100+1];
        int     len;

        // �ӱ��ж�ȡ��ֵ����
        memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetTBLRecInterfaceFileName(tblName,fileName)) < 0)
        {
                UnionUserErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: UnionGetTBLRecInterfaceFileName!\n");
                return(ret);
        }
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
                memset(fldName,0,sizeof(fldName));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"valueFldName",fldName,sizeof(fldName))) < 0)
                        continue;
                if (isFldAlaisUsed)
                {
		        if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"fldAlais",fldName,sizeof(fldName))) < 0)
			        continue;
                }
                if (strcmp(fldName,fldTag) != 0)
                        continue;
                // ��������ֵ����
                fclose(fp);
		return(UnionReadRecFldFromRecStr(tmpBuf,len,"value",method,sizeOfBuf));
        }
        fclose(fp);
        UnionUserErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: no fld assign method defined for [%s] [%s]\n",fldTag,tblName);
        return(errCodeSQLFldAssignMethodNotSet);
}

/*
����	���ĳ���ĳ���ȡֵ��
�������
	tblName		����
        fldAlais        �����
        maxValueNum     ���ɶ�����ֵ��Ŀ
�������
	varGrp		������ֵ
����ֵ
	>=0		������ֵ��Ŀ
	<0		�������
*/
int UnionGetAvailableValueGrp(char *tblName,char *fldAlais,char varGrp[][128+1],int maxValueNum)
{
        int     ret;
        char    method[512+1];
        int     methodLen;

        // ����ĸ�ֵ����
        memset(method,0,sizeof(method));
        if ((methodLen = UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(tblName,1,fldAlais,method,sizeof(method))) < 0)
        {
                UnionUserErrLog("in UnionGetAvailableValueGrp:: UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL of fld [%s] of tbl [%s]\n",fldAlais,tblName);
                return(methodLen);
        }
        if (ret == 0)
                return(0);
        if (strcmp(conFldValueSetMethodTagUserInput,method) == 0)     // �����û��������뷽��
                return(0);
        else if (strcmp(conFldValueSetMethodTagAutoSetBySoftware,method) == 0) // ϵͳ�Զ���ֵ
                return(0);
        else if (strncmp(conFldValueSetMethodDefaultSet,method,strlen(conFldValueSetMethodDefaultSet)) == 0) // ȱʡ��ֵ
        {
                if (strlen(method) - strlen(conFldValueSetMethodDefaultSet) >= sizeof(varGrp[0]))
                {
                        memcpy(varGrp[0],method+strlen(conFldValueSetMethodDefaultSet),sizeof(varGrp[0])-2);
                        varGrp[0][sizeof(varGrp[0])-1] = 0;
                }
                else
                        strcpy(varGrp[0],method+strlen(conFldValueSetMethodDefaultSet));
                return(1);
        }
        else if (strncmp(conFldValueSetMethodRefrenceOtherTBLFld,method,strlen(conFldValueSetMethodRefrenceOtherTBLFld)) == 0)   // ������������
        {
                return(UnionGetAvailableValueGrpByQueryCondition(method+strlen(conFldValueSetMethodRefrenceOtherTBLFld),
                        methodLen-strlen(conFldValueSetMethodRefrenceOtherTBLFld),
                        varGrp,maxValueNum));
        }
        else if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // ö��ֵ
        {
                return(UnionReadAllRemarkOfEnumType(method+strlen(conFldValueSetMethodEnumGrp),varGrp,maxValueNum));
        }
        else
        {
                UnionUserErrLog("in UnionGetAvailableValueGrp:: unknown method [%s] for fld [%s] of tbl [%s]\n",method,fldAlais,tblName);
                return(errCodeSQLFldAssignMethodNotSupport);
        }
}

//#pragma package(smart_init)

//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#include "unionEnumValueDef.20080724.h"

//---------------------------------------------------------------------------
/*
����	���ļ��ж�ȡö�����Ͷ���
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�ֵ��Ŀ
	<0		�������
*/
int UnionReadEnumValueGrpDef(char *enumType,PUnionEnumValueGrp prec)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	char	fileName[512+1];
	FILE	*fp;

	if ((enumType == NULL) || (prec == NULL))
		return(errCodeParameter);

	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetEnumValueDefFileName(enumType,fileName)) < 0)
	{
		UnionSystemErrLog("in UnionReadEnumValueGrpDef:: UnionGetEnumValueDefFileName [%s]\n",enumType);
		return(errCodeUseOSErrCode);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadEnumValueGrpDef:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(prec,0,sizeof(prec));
	prec->valueNum = 0;
	while (!feof(fp))
	{
		if (prec->valueNum >= conMaxNumOfEnumValuePerGrp)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"value",prec->valueDef[prec->valueNum].value,sizeof(prec->valueDef[prec->valueNum].value))) <= 0)
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"remark",prec->valueDef[prec->valueNum].remark,sizeof(prec->valueDef[prec->valueNum].remark))) <= 0)
			continue;
		prec->valueNum += 1;
	}
	fclose(fp);
	return(prec->valueNum);
}

//---------------------------------------------------------------------------
/*
����	��ָ��ö�����͵�ָ��˵����ֵ��Ӧ��ֵ������
�������
	enumType         ö��������
        isReadValueByRemark  ����˵������ֵ����  1����˵����0����ֵ
        byTag           ˵����ֵ
�������
	readTag		������ֵ��˵��
����ֵ
	>=0		�����ĳ���
	<0		�������
*/
int UnionReadEnumValueOtherAttr(char *enumType,int isReadValueByRemark,char *byTag,char *readTag,int sizeOfBuf)
{
        int     ret;
        FILE    *fp;
        char    tmpBuf[512+1];
        char    fileName[512+1];
        char    readBuf[100+1];
        int     len;

        // �ӱ��ж�ȡ��ֵ����
        memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetEnumValueDefFileName(enumType,fileName)) < 0)
        {
                UnionUserErrLog("in UnionReadEnumValueOtherAttr:: UnionGetEnumValueDefFileName!\n");
                return(ret);
        }
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadEnumValueOtherAttr:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if (isReadValueByRemark)
		{
                	memset(readBuf,0,sizeof(readBuf));
	        	if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"remark",readBuf,sizeof(readBuf))) < 0)
		        	continue;
	                if (strcmp(readBuf,byTag) != 0)
                        	continue;
                        fclose(fp);
                        return(UnionReadRecFldFromRecStr(tmpBuf,len,"value",readTag,sizeOfBuf));
                }
                else
		{
                	memset(readBuf,0,sizeof(readBuf));
                	if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"value",readBuf,sizeof(readBuf))) <= 0)
                        	continue;
	                if (strcmp(readBuf,byTag) != 0)
                        	continue;
                        fclose(fp);
                        return(UnionReadRecFldFromRecStr(tmpBuf,len,"remark",readTag,sizeOfBuf));
                }
        }
        fclose(fp);
        UnionUserErrLog("in UnionReadEnumValueOtherAttr:: no fld assign readTag defined for [%s] [%s]\n",byTag,enumType);
        return(errCodeSQLFldAssignMethodNotSet);
}

//---------------------------------------------------------------------------
/*
����	��ȡö�����͵�����ֵ
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�ֵ��Ŀ
	<0		�������
*/
int UnionReadAllValueOfEnumType(char *enumType,char value[][128+1],int maxNum)
{
	int			ret;
        int     		index;
	TUnionEnumValueGrp	rec;
	
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadEnumValueGrpDef(enumType,&rec)) < 0)
	{
		UnionSystemErrLog("in UnionReadAllValueOfEnumType:: UnionReadEnumValueGrpDef [%s]\n",enumType);
		return(ret);
	}
	for (index = 0; (index < rec.valueNum) && (index < maxNum); index++)
		strcpy(value[index],rec.valueDef[index].value);
	return(index);
}

//---------------------------------------------------------------------------
/*
����	��ȡö�����͵�����˵��
�������
	enumType	ö������
�������
	prec		ö�����Ͷ���
����ֵ
	>=0		ö�����͵�˵����Ŀ
	<0		�������
*/
int UnionReadAllRemarkOfEnumType(char *enumType,char remark[][128+1],int maxNum)
{
	int			ret;
        int     		index;
	TUnionEnumValueGrp	rec;
	
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadEnumValueGrpDef(enumType,&rec)) < 0)
	{
		UnionSystemErrLog("in UnionReadAllValueOfEnumType:: UnionReadEnumValueGrpDef [%s]\n",enumType);
		return(ret);
	}
	for (index = 0; (index < rec.valueNum) && (index < maxNum); index++)
		strcpy(remark[index],rec.valueDef[index].remark);
	return(index);
}

//#pragma package(smart_init)

//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionEnumDefFile.h"
#include "UnionStr.h"

/* ��ָ���ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	fileName	�ļ�����
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
*/
int UnionReadEnumDefFromSpecFile(char *fileName,PUnionEnumDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadEnumDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadEnumDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conEnumDeclareLineTag,strlen(conEnumDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadEnumDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->constNum >= conMaxNumOfVarPerEnum)	// ����̫��
		{
			UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: too many constNum [%d] defined in [%s]\n",pdef->constNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadConstDefFromStr(lineStr,lineLen,&(pdef->constGrp[pdef->constNum]))) < 0)
		{
			UnionAuditLog("in UnionReadEnumDefFromSpecFile:: UnionReadConstDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->constGrp[pdef->constNum].type = conVarTypeTagEnum;
		pdef->constNum += 1;
	}
	fclose(fp);
	return(0);
}

/* ��ȱʡ�����ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	nameOfType	ö������
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
*/
int UnionReadEnumDefFromDefaultDefFile(char *nameOfType,PUnionEnumDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	if ((ret = UnionReadEnumDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromSpecFile [%s] from [%s]\n",nameOfType,fileName);
		return(ret);
	}
	return(ret);
}


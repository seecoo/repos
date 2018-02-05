//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionUnionDefFile.h"
#include "UnionStr.h"

/* ��ȱʡ�����ļ���ָ�����ƵĽṹ�Ķ���
�������
	nameOfType	�ṹ����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadUnionDefFromDefaultDefFile(char *nameOfType,PUnionUnionDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	if ((ret = UnionReadUnionDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionReadUnionDefFromSpecFile [%s] from [%s]\n",nameOfType,fileName);
		return(ret);
	}
	return(ret);
}

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadUnionDefFromSpecFile(char *fileName,PUnionUnionDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadUnionDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadUnionDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conUnionDeclareLineTag,strlen(conUnionDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadUnionDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->fldNum >= conMaxNumOfVarPerUnion)	// ����̫��
		{
			UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: too many fldNum [%d] defined in [%s]\n",pdef->fldNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadVarDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionAuditLog("in UnionReadUnionDefFromSpecFile:: UnionReadVarDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->fldNum += 1;
	}
	fclose(fp);
	return(0);
}


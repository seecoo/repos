// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "unionFunDefFile.h"
#include "UnionLog.h"

/*
����	
	��ȱʡ�����ļ���ȡһ����������
�������
	funName		��������
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromDefaultDefFile(char *funName,PUnionFunDef pdef)
{
	char       record[2560+1];
        char       varDefList[2048+1];
        char       sizeList[1024+1];
        char       condition[1024+1];
        char       returnValueName[64+1];
	char       varGrp[conMaxNumOfVarPerFun][128+1];
	char       varGrp1[conMaxNumOfArrayDimision][128+1];
        int        ret;
        int        i,j;

        memset(record, 0, sizeof record);
        memset(varDefList, 0, sizeof varDefList);
        memset(sizeList, 0, sizeof sizeList);
        memset(condition, 0, sizeof condition);
        memset(varGrp, 0, sizeof varGrp);
        memset(varGrp1, 0, sizeof varGrp1);
        memset(returnValueName, 0, sizeof returnValueName);

        strcpy(condition, "funName=");
        strcat(condition, funName);
        strcat(condition, "|");
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* ���������Ƹ�ֵ */
        strcpy(pdef->funName, funName);

        /* ������ֵ���Ƹ�ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "returnValueName", returnValueName, sizeof (returnValueName));

        /* ������˵����ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->remark, sizeof (pdef->remark));

        /* ��������Ŀ��ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', varGrp, conMaxNumOfVarPerFun);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->varNum = ret;

        /* ȡ�����б� */
	for (i=0; i<pdef->varNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "funName=");
		strcat(condition, funName);
		strcat(condition, "|");
		strcat(condition, "name=");
		strcat(condition, varGrp[i]);
		strcat(condition, "|");
		/* �ӱ���ȡ��һ��record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funVarDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
			return(ret);
		}

                /* �����������͸�ֵ */
		UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->varGrp[i].varDef.nameOfType, sizeof (pdef->varGrp[i].varDef.nameOfType));

                /* ������������(�Ƿ���ָ��)��ֵ */
		UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->varGrp[i].varDef.isPointer) );

                /* ������������(�ж������뻹�����)��ֵ */
		UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "typeTag", &(pdef->varGrp[i].isOutput) );

                /* �����������Ƹ�ֵ */
		strcpy(pdef->varGrp[i].varDef.name, varGrp[i]);

                /* �жϲ����Ƿ�Ϊ���� */
		UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
		ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', varGrp1, conMaxNumOfArrayDimision);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
			return(ret);
		}
		if (ret == 0)      /* �������� */
		{
			pdef->varGrp[i].varDef.dimisionNum = 0;
		}
		else if (ret > 0)  /* ������ */
		{
			pdef->varGrp[i].varDef.dimisionNum = ret;
			/* ��ÿһά��С��ֵ */
			for(j=0; j<pdef->varGrp[i].varDef.dimisionNum; j++)
			{
				strncpy(pdef->varGrp[i].varDef.sizeOfDimision[j], varGrp1[j], 64);
			}
		}

                /* ������˵����ֵ */
		UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->varGrp[i].varDef.remark, sizeof (pdef->varGrp[i].varDef.remark));
	}

        /* ȡ�����ķ������� */
	strcpy(condition, "funName=");
	strcat(condition, funName);
	strcat(condition, "|");
	strcat(condition, "name=");
	strcat(condition, returnValueName);
	strcat(condition, "|");
	/* �ӱ���ȡ��һ��record */
	if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funVarDef", condition, record, sizeof(record)))<0)

	{
		UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
		return(ret);
	}
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->returnType.nameOfType, sizeof (pdef->returnType.nameOfType));

	UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->returnType.isPointer) );
        
        strcpy(pdef->returnType.name, returnValueName);

        return(0);
}

/*
����	
	���ļ���ȡһ����������
�������
	fileName	�ļ�����
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromSpecFile(char *fileName,PUnionFunDef pdef)
{
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((pdef == NULL) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionReadFunDefFromSpecFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// �򿪶����ļ�
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFunDefFromSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// ���ļ��ж�ȡ����
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeEnviMDL_NullLine))
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadOneLineFromTxtFile [%s] !lineNum = [%04d] ret = [%d]\n",fileName,lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))	// ע����
			continue;
		// ��������
		if (strncmp(lineStr,conFunDefTagFunName,strlen(conFunDefTagFunName)) == 0)
		{
			if ((ret = UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunName,pdef->funName,sizeof(pdef->funName))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadIntTypeRecFldFromRecStr [%s]! lineNum = [%04d]\n",conFunDefTagFunName,lineNum);
				goto errExit;
			}
			UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunRemark,pdef->remark,sizeof(pdef->remark));
			continue;
		}
		// ����ֵ����
		if (strncmp(lineStr,conFunDefTagReturnType,strlen(conFunDefTagReturnType)) == 0)
		{
			if ((ret = UnionReadVarDeclareTypeDefFromStr(lineStr+strlen(conFunDefTagReturnType),lineLen-strlen(conFunDefTagReturnType),&(pdef->returnType))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDeclareTypeDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
				goto errExit;
			}
			continue;
		}
		// ��������
		if (strncmp(lineStr,conFunDefTagVarDef,strlen(conFunDefTagVarDef)) != 0)
			continue;
		if (pdef->varNum >= conMaxNumOfVarPerFun)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: too many var [%d] defined for fun [%s]! lineNum = [%04d]\n",pdef->varNum,pdef->funName,lineNum);
			ret = errCodeCDPMDL_TooManyFunVarDefined;
			goto errExit;
		}
		// ��������
		if ((ret = UnionReadVarDefFromStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),&(pdef->varGrp[pdef->varNum].varDef))) < 0)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
			goto errExit;
		}
		// �������
		if ((ret = UnionReadIntTypeRecFldFromRecStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),conFunDefTagIsOutputVar,&(pdef->varGrp[pdef->varNum].isOutput))) < 0)
			pdef->varGrp[pdef->varNum].isOutput = 0;
		pdef->varNum += 1;
	}
	ret = 0;
errExit:
	fclose(fp);
	return(ret);
}


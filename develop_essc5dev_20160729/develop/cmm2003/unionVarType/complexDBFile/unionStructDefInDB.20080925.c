//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDefFile.h"
#include "unionSimpleTypeDefFile.h"
#include "UnionStr.h"
#include "unionVarTypeDef.h"

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadStructDefFromSpecFile(char *fileName,PUnionStructDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadStructDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadStructDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadStructDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conStructDeclareLineTag,strlen(conStructDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadStructDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadStructDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->fldNum >= conMaxNumOfVarPerStruct)	// ����̫��
		{
			UnionUserErrLog("in UnionReadStructDefFromSpecFile:: too many fldNum [%d] defined in [%s]\n",pdef->fldNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadVarDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionAuditLog("in UnionReadStructDefFromSpecFile:: UnionReadVarDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->fldNum += 1;
	}
	fclose(fp);
	return(0);
}

/* ��ȱʡ�����ļ���ָ�����ƵĽṹ�Ķ���
�������
	nameOfType	�ṹ����
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadStructDefFromDefaultDefFile(char *nameOfType,PUnionStructDef pdef)
{
	char       record[2560+1];
        char       varDefList[2048+1];
        char       condition[1024+1];
        char       sizeList[1024+1];
	char       fldGrp[conMaxNumOfVarPerStruct][128+1];
	char       fldGrp1[conMaxNumOfArrayDimision][128+1];
        int        ret;
        int        i,j;

        memset(record, 0, sizeof record);
        memset(varDefList, 0, sizeof varDefList);
        memset(condition, 0, sizeof condition);
        memset(sizeList, 0, sizeof sizeList);
        memset(fldGrp, 0, sizeof fldGrp);
        memset(fldGrp1, 0, sizeof fldGrp1);

        strcpy(condition, "typeDefName=");
        strcat(condition, nameOfType);
        strcat(condition, "|");
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("complexTypeDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadStructDefFromDefaultDefFile:: UnionReadStructDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* ���ṹ����(TypeDef)��ֵ */
        strncpy(pdef->declareDef.typeDefName, nameOfType, 48);

        /* ���ṹ����(struct�����)��ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "privateName", pdef->declareDef.structName, sizeof (pdef->declareDef.structName));

        /* ���ṹ˵����ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->declareDef.remark, sizeof (pdef->declareDef.remark));

        /* ȡ�ṹ������� */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', fldGrp, conMaxNumOfVarPerStruct);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadStructDefFromDefaultDefFile:: UnionReadStructDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->fldNum = ret;

        /* ȡ�ṹ���� */
	for (i=0; i<pdef->fldNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "typeDefName=");
		strcat(condition, nameOfType);
		strcat(condition, "|");
		strcat(condition, "name=");
		strcat(condition, fldGrp[i]);
		strcat(condition, "|");
		/* �ӱ���ȡ��һ��record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("typeVarDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadStructDefFromDefaultDefFile:: UnionReadStructDefFromDefaultDefFile!\n");
			return(ret);
		}

                /* �������͸�ֵ */
		UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->fldGrp[i].nameOfType, sizeof (pdef->fldGrp[i].nameOfType));
        
                /* ���������(�Ƿ���ָ��)��ֵ */
                UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->fldGrp[i].isPointer) );

                /* ��������Ƹ�ֵ */
		strcpy(pdef->fldGrp[i].name, fldGrp[i]);

                /* �жϲ����Ƿ�Ϊ���� */
                UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
                ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', fldGrp1, conMaxNumOfArrayDimision);
                if (ret < 0)
                {
                        UnionUserErrLog("in UnionReadStructDefFromDefaultDefFile::UnionReadStructDefFromDefaultDefFile!\n");
                        return(ret);
                }
                if (ret == 0)      /* �������� */
                {
                        pdef->fldGrp[i].dimisionNum = 0;
                }
                else if (ret > 0)  /* ������ */
                {
                        pdef->fldGrp[i].dimisionNum = ret;
                        /* ��ÿһά��С��ֵ */
                        for(j=0; j<pdef->fldGrp[i].dimisionNum; j++)
                        {
                                strncpy(pdef->fldGrp[i].sizeOfDimision[j] , fldGrp1[j], 64);
                        }
                }

                /* ������˵����ֵ */
                UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->fldGrp[i].remark, sizeof (pdef->fldGrp[i].remark));
	}

        return(0);
}

/* ���ṹ�Ķ���д�뵽ָ�����ļ���
�������
	pdef		�ṹ����
	fileName	�ļ�����
�������
	��
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteStructDefIntoSpecFile(PUnionStructDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteStructDefIntoDefaultFile(pdef,""));
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteStructDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintStructDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteStructDefIntoSpecFile:: UnionPrintStructDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* ���ṹ�Ķ���д�뵽ȱʡ���ļ���
�������
	pdef		�ṹ����
	programFileName	�ṹ���ڵĳ�������
�������
	��
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionWriteStructDefIntoDefaultFile(PUnionStructDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionSimpleTypeDef	simpleTypeDef;
	TUnionStructDef		def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	memset(&varTypeDef,0,sizeof(varTypeDef));
	if (strlen(pdef->declareDef.typeDefName) != 0)
		strcpy(varTypeDef.nameOfType,pdef->declareDef.typeDefName);
	else
		strcpy(varTypeDef.nameOfType,pdef->declareDef.structName);
	varTypeDef.typeTag = conVarTypeTagStruct;
	UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);

	if (pdef->fldNum == 0)	// �����typedef����
	{
		if (strcmp(pdef->declareDef.typeDefName,pdef->declareDef.structName) != 0)	// �������Ʋ���ͬ����ʹ��typedef��һ��struct�����������滻
		{
			memset(&simpleTypeDef,0,sizeof(simpleTypeDef));
			strcpy(simpleTypeDef.nameOfType,pdef->declareDef.structName);
			strcpy(simpleTypeDef.name,pdef->declareDef.typeDefName);
			strcpy(simpleTypeDef.remark,pdef->declareDef.remark);
			return(UnionWriteSimpleTypeDefIntoDefaultFile(&simpleTypeDef,programFileName));
		}
		// ��ʹ��typedef���ṹ������Ϊ�˿���ֱ��ʹ��
		if (!UnionExistsVarTypeDefInDefaultDefFile(pdef->declareDef.structName))	// δ����ṹ
		{
			// ���������Ͷ���
			if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
			{
				UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteStructDefToDefaultFile!\n");
				return(ret);
			}
			return(ret);
		}
		// ��ǰ�����Ͳ��ǽṹ����
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagStruct)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
		// �����ڵĶ���
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadStructDefFromDefaultDefFile(varTypeDef.nameOfType,&def)) < 0)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: UnionReadStructDefFromDefaultDefFile [%s]!\n",varTypeDef.nameOfType);
			return(ret);
		}
		strcpy(def.declareDef.typeDefName,pdef->declareDef.typeDefName);
		UnionGetDefaultFileNameOfVarTypeDef(def.declareDef.structName,fileName);
		return(UnionWriteStructDefIntoSpecFile(&def,fileName));
	}
	
	// �ǽṹ����
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// δ����ṹ
	{
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteStructDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// �Ѷ���ṹ
	{
		// ��ǰ�����Ͳ��ǽṹ����
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagStruct)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
		if (strlen(pdef->declareDef.typeDefName) == 0)
			strcpy(pdef->declareDef.typeDefName,pdef->declareDef.structName);
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWriteStructDefIntoSpecFile(pdef,fileName));
}

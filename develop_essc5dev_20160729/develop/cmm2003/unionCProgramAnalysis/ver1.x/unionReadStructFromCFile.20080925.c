//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDef.h"
#include "unionPointerDefFile.h"
#include "UnionStr.h"
#include "unionReadStructFromCFile.h"
#include "unionCommand.h"

// 判断一个名字是否是*
int UnionIsThisNameDefinedAsStarTag(char *varName)
{
	char	tmpBuf[128+1];
	char	*ptr;
	
	sprintf(tmpBuf,"is%sDefinedAsStar",varName);
	if ((ptr = getenv(tmpBuf)) == NULL)
		return(0);
	else
		return(atoi(ptr));
}

/* 从一个字符串中读取变量的定义
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	prefix		读出的前缀
	nameOfType	读出的定义的类型
	varName		读出的变量的名字
	isPointer	读出的变量是否是指针
返回值：
	>=0 		读出的串在字符串中占用的大小
	<0		出错代码	
	
*/
int UnionReadVarDefFromCProgramStr(char *str,int lenOfStr,char *prefix,char *nameOfType,char *varName,int *isPointer)
{
	int			ret;
	char			fldName[128+1];
	int			offset = 0;
	int			isUnsigned = 0;
	char			nameOfTypePrefix[128+1];

	if ((str == NULL) || (lenOfStr <= 0))
		return(errCodeParameter);
	// 读变量前缀
	memset(fldName,0,sizeof(fldName));
	if ((ret = UnionReadCProgramVarNameFromStr(str+offset,lenOfStr-offset,fldName)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromCProgramStr:: UnionReadCProgramVarNameFromStr [%s]!\n",str+offset);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: invalid c program str [%s]!\n",str+offset);
		return(errCodeParameter);
	}
	offset += ret;
	if ((strcmp(fldName,"struct") == 0) || (strcmp(fldName,"union") == 0))
	{
		strcpy(prefix,fldName);
	}
	else if (strcmp(fldName,"unsigned") == 0)	// 是无符号类型
	{
		strcpy(nameOfTypePrefix,fldName);
		isUnsigned = 1;
	}
	else 
	{
		strcpy(nameOfType,fldName);
		goto readVarName;
	}
	// 读变量类型
	memset(fldName,0,sizeof(fldName));
	if ((ret = UnionReadCProgramVarNameFromStr(str+offset,lenOfStr-offset,fldName)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromCProgramStr:: UnionReadCProgramVarNameFromStr [%s]!\n",str+offset);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: invalid c program str [%s]!\n",str+offset);
		return(errCodeParameter);
	}
	offset += ret;
	if (isUnsigned)
		sprintf(nameOfType,"%s %s",nameOfTypePrefix,fldName);
	else
		strcpy(nameOfType,fldName);
	
	// 读变量名称
readVarName:
	*isPointer = 0;
	memset(fldName,0,sizeof(fldName));
	if ((ret = UnionReadCProgramVarNameFromStr(str+offset,lenOfStr-offset,fldName)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromCProgramStr:: UnionReadCProgramVarNameFromStr [%s]!\n",str+offset);
		return(ret);
	}
	if (UnionIsBaseType(fldName) > 0)	// 变量名仍是类型定义的一部分
	{
		sprintf(nameOfTypePrefix,"%s %s",nameOfType,fldName);
		offset += ret;
		strcpy(nameOfType,nameOfTypePrefix);
		goto readVarName;
	}
	if (ret == 0)
	{
		if (str[offset] == '*')
		{
			*isPointer = 1;
			offset++;
		}
		else
		{
			UnionUserErrLog("in UnionReadStructFldDefFromStr:: invalid c program str [%s]!\n",str+offset);
			return(errCodeParameter);
		}
	}
	else
	{
		if (UnionIsThisNameDefinedAsStarTag(fldName))
			*isPointer = 1;
		else
		{
			strcpy(varName,fldName);
			return(offset+ret);
		}
	}
	offset += ret;
	memset(fldName,0,sizeof(fldName));
	if ((ret = UnionReadCProgramVarNameFromStr(str+offset,lenOfStr-offset,fldName)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromCProgramStr:: UnionReadCProgramVarNameFromStr [%s]!\n",str+offset);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: invalid c program str [%s]!\n",str+offset);
		return(errCodeParameter);
	}
	strcpy(varName,fldName);
	return(offset+ret);
}

/* 从一个字符串中读取结构域的定义
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	pfldDef		读出的域定义
返回值：
	>=0 		成功
	<0		出错代码	
	
*/
int UnionReadStructFldDefFromStr(char *str,int lenOfStr,PUnionVarDef pfldDef)
{
	int		ret;
	char		*ptr,*endPtr;
	int		offset = 0;
			
	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	if (pfldDef == NULL)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: pfldDef is null!\n");
		return(errCodeParameter);
	}
	memset(pfldDef,0,sizeof(*pfldDef));
	if ((ret = UnionReadVarDefFromCProgramStr(str+offset,lenOfStr-offset,pfldDef->typePrefix,pfldDef->nameOfType,pfldDef->name,&(pfldDef->isPointer))) < 0)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: UnionReadVarDefFromCProgramStr [%s]!\n",str+offset);
		return(ret);
	}
	offset += ret;
	ptr = str+offset;
loop:
	if (ptr >= str + lenOfStr)
		return(0);
	if ((ptr = strstr(ptr,"[")) == NULL)	// 不是数组定义
		return(offset);
	if ((endPtr = strstr(ptr,"]")) == NULL)
	{
		UnionUserErrLog("in UnionReadStructFldDefFromStr:: invalid c program str [%s]!\n",str+offset);
		return(errCodeParameter);
	}
	*endPtr = 0;
	strcpy(pfldDef->sizeOfDimision[pfldDef->dimisionNum],ptr+1);
	*endPtr = ']';
	ptr += 1;
	pfldDef->dimisionNum++;
	goto loop;
}

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
	structName	结构名称
	writeToDefFile	控制写到定义文件中，1写，0，不写
	outFp		将读出的记录输出到该文件中
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码
*/
int UnionReadSpecStructDefFromCHeaderFile(char *fileName,char *structName,PUnionStructDef pdef,int writeToDefFile,FILE *outFp)
{
	char	lineStr[10][2048+1];
	int	ret;
	int	lineLen;
	FILE	*fp;
	int	oriLineNum = 0,readLineNum;
	char	*ptr;
	int	typeDefUsed = 0;
	int	offset = 0;
	int	structNum = 0;
		
	if ((structName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecStructDefFromCHeaderFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		typeDefUsed = 0;
		// 开始读一个结构定义
		memset(pdef,0,sizeof(*pdef));
		// 从文件中读取一个有效定义行
		//memset(lineStr,0,sizeof(lineStr));
		if ((readLineNum = UnionReadCProgramLineFromFile(fp,lineStr,10,&oriLineNum)) < 0)
		{
			UnionSystemErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d] ret = [%d]\n",fileName,oriLineNum,readLineNum);
			fclose(fp);
			return(readLineNum);
		}
		if (readLineNum == 0)	// 文件结束
			break;
		// 从文件中读结构标识
		if (strncmp(lineStr[0],"struct",strlen("struct")) == 0)
			offset = strlen("struct");
		else if (strncmp(lineStr[0],"typedef struct",strlen("typedef struct")) == 0)
		{
			offset = strlen("typedef struct");
			typeDefUsed = 1;
		}
		else
			continue;	// 不是结构标识
		sscanf(lineStr[0]+offset,"%s",pdef->declareDef.structName);	// 读结构名
		if (strlen(pdef->declareDef.structName) != 0)
		{
			offset += strlen(pdef->declareDef.structName);
			offset += 1;
			if ((ret = UnionReadCProgramVarNameFromStr(lineStr[0]+offset,strlen(lineStr[0]+offset),pdef->declareDef.typeDefName)) > 0)
			{
				if (UnionIsThisNameDefinedAsStarTag(pdef->declareDef.typeDefName))	// 是*
				{
					// 是指针定义
					if (!writeToDefFile)	// 不将定义写到定义文件中
						continue;
					offset += ret;
					if ((ret = UnionReadCProgramVarNameFromStr(lineStr[0]+offset,strlen(lineStr[0]+offset),pdef->declareDef.typeDefName)) > 0)
					{
						if ((ret = UnionWriteSpecPointerDefIntoDefaultFile(pdef->declareDef.structName,pdef->declareDef.typeDefName,"",fileName)) < 0)
							UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteSpecPointerDefIntoDefaultFile!\n");
					}		
					continue;							
				}
				else
					goto compStruct;
			}
		}
		// 读 '{'行
		//memset(lineStr,0,sizeof(lineStr));
		if ((readLineNum = UnionReadCProgramLineFromFile(fp,lineStr,10,&oriLineNum)) < 0)
		{
			UnionSystemErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d] no { encountered!\n",fileName,oriLineNum);
			fclose(fp);
			return(readLineNum);
		}
		if (readLineNum == 0)	// 文件结束
			break;
		if (strncmp(lineStr[0],"{",1) != 0)
		{
			UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d] '{' expected\n",fileName,oriLineNum);
			fclose(fp);
			return(errCodeParameter);
		}
loop:		// 读域定义
		//memset(lineStr,0,sizeof(lineStr));
		if ((readLineNum = UnionReadCProgramLineFromFile(fp,lineStr,10,&oriLineNum)) < 0)
		{
			UnionSystemErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d]\n",fileName,oriLineNum);
			fclose(fp);
			return(readLineNum);
		}
		if (readLineNum == 0)	// 文件结束
			break;
		if (strncmp(lineStr[0],"}",1) == 0)	// 是结构定义结束
			goto readOneStruct;
		if (pdef->fldNum >= conMaxNumOfVarPerStruct)	// 域定义太多
		{
			UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d]! too many struct fld [%d] defined for [%s]\n",fileName,oriLineNum,pdef->fldNum,pdef->declareDef.structName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadStructFldDefFromStr(lineStr[0],strlen(lineStr[0]),&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d]! UnionReadStructFldDefFromStr! ret = [%d]\n",fileName,oriLineNum,ret);
			fclose(fp);
			return(ret);
		}
		pdef->fldNum += 1;
		goto loop;

readOneStruct:
		if (typeDefUsed)
		{
			if ((ptr = strstr(lineStr[0],";")) != NULL)
				*ptr = 0;
			sscanf(lineStr[0]+1,"%s",pdef->declareDef.typeDefName);
			if (strlen(pdef->declareDef.typeDefName) == 0)	// 结构名是空
			{
				UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d] typeDefName is null for [%s]\n",fileName,oriLineNum,pdef->declareDef.structName);
				fclose(fp);
				return(errCodeParameter);
			}
		}
compStruct:
		UnionPrintStructDefToFp(pdef,outFp);
		if (writeToDefFile)
		{
			if ((ret = UnionWriteStructDefIntoDefaultFile(pdef,fileName)) < 0)
			{
				UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteStructDefIntoDefaultFile!\n");
				goto compStruct2;
			}				
		}			
compStruct2:	
		// 比较读出的结构是否是要读的结构
		if ((strlen(structName) == 0) || (structName == NULL))
		{
			structNum++;
			continue;
		}
		if ((strcmp(structName,pdef->declareDef.structName) != 0) && (strcmp(structName,pdef->declareDef.typeDefName) != 0))	// 不是要读出的结构，继续读
			continue;
		fclose(fp);
		return(0);

	}
	fclose(fp);
	if (structNum == 0)
	{
		UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: no structDefined in this cHeaderFile\n",fileName,oriLineNum);
		return(errCodeParameter);
	}
	else
	{
		UnionSuccessLog("in UnionReadSpecStructDefFromCHeaderFile:: [%d] struct defined in [%s]\n",structNum,fileName);
		return(structNum);
	}
}

/* 将指定结构的域定义打印到文件中
输入参数
	fileName	文件名称
	structName	结构名称
	fp		文件句柄
	writeToDefFile	控制写到定义文件中，1写，0，不写
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintSpecStructDefInCHeaderFileToFp(char *fileName,char *nameOfStructDef,FILE *fp,int writeToDefFile)
{
	TUnionStructDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecStructDefFromCHeaderFile(fileName,nameOfStructDef,&def,writeToDefFile,fp)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionReadSpecStructDefFromCHeaderFile!\n");
		return(ret);
	}
	return(ret);
}

/* 将指定结构的域定义打印到文件中
输入参数
	fileName	文件名称
	fp		文件句柄
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintUserSpecStructDefInCHeaderFileToFp(char *fileName,FILE *fp)
{
	TUnionStructDef	def;
	int		ret;
	char		nameOfStructDef[128+1];	
	char		*ptr;
	
loop:
	ptr = UnionInput("请输入结构名称::");
	strcpy(nameOfStructDef,ptr);
	if (UnionIsQuit(ptr))
		return(0);
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecStructDefFromCHeaderFile(fileName,nameOfStructDef,&def,0,NULL)) < 0)
	{
		UnionUserErrLog("in UnionPrintUserSpecStructDefInCHeaderFileToFp:: UnionReadSpecStructDefFromCHeaderFile!\n");
		return(ret);
	}
	UnionPrintStructDefToFp(&def,fp);
	goto loop;
}

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
	readName	结构名称
	writeToDefFile	控制写到定义文件中，1写，0，不写
	outFp		将读出的记录输出到该文件中
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码
*/
int UnionReadSpecTypeDefFromCHeaderFile(char *fileName,char *readName,int writeToDefFile,FILE *outFp)
{
	char	lineStr[10][2048+1];
	int	ret;
	int	lineLen;
	FILE	*fp;
	int	oriLineNum = 0,readLineNum;
	int	offset = 0;
	int	structNum = 0;
	char	prefix[128+1];
	char	nameOfType[128+1];
	char	name[128+1];
	int	isPointer;
	int	readFinish = 0;	
		
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		// 开始读一个定义
		// 从文件中读取一个有效定义行
		//memset(lineStr,0,sizeof(lineStr));
		if ((readLineNum = UnionReadCProgramLineFromFile(fp,lineStr,10,&oriLineNum)) < 0)
		{
			UnionSystemErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: cprogramLine [%s] error! lineIndex = [%d] ret = [%d]\n",fileName,oriLineNum,readLineNum);
			fclose(fp);
			return(readLineNum);
		}
		if (readLineNum == 0)	// 文件结束
			break;
		// 从文件中读结构标识
		if (strncmp(lineStr[0],"typedef",strlen("typedef")) == 0)
		{
			if (strncmp(lineStr[0],"typedef struct",strlen("typedef struct")) == 0)	// 是结构变量定义
				continue;
		}
		else	// 不是变量定义
			continue;
		offset = strlen("typedef");
		// 读变量定义出来
		memset(prefix,0,sizeof(prefix));
		memset(nameOfType,0,sizeof(nameOfType));
		memset(name,0,sizeof(name));
		isPointer = 0;
		if ((ret = UnionReadVarDefFromCProgramStr(lineStr[0]+offset,strlen(lineStr[0])-offset,prefix,nameOfType,name,&isPointer)) < 0)
		{
			UnionSystemErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: UnionReadVarDefFromCProgramStr!\n");
			fclose(fp);
			return(ret);
		}
		if ((strlen(readName) == 0) || (readName == NULL))
		{
			structNum++;
			//continue;
		}
		else if (strcmp(readName,name) != 0)	// 不是要读出的定义
			continue;
		else
			readFinish = 1;
		if (isPointer)
		{
			if (writeToDefFile)
			{
				if ((ret = UnionWriteSpecPointerDefIntoDefaultFile(nameOfType,name,"",fileName)) < 0)
				{
					UnionUserErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: UnionWriteSpecPointerDefIntoDefaultFile [%s]!\n",lineStr[0]);
				}
			}				
			UnionOutputSpecPointerDef(name);
		}
		else		
		{
			if (writeToDefFile)
			{
				if ((ret = UnionWriteSpecSimpleTypeDefIntoDefaultFile(nameOfType,name,"",fileName)) < 0)
				{
					UnionUserErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: UnionWriteSpecSimpleTypeDefIntoDefaultFile [%s]!\n",lineStr[0]);
				}				
			}
			UnionOutputSpecSimpleTypeDef(name);
		}
		if (!readFinish)
			continue;
		fclose(fp);
		return(0);

	}
	fclose(fp);
	if (structNum == 0)
	{
		UnionUserErrLog("in UnionReadSpecTypeDefFromCHeaderFile:: no structDefined in this cHeaderFile\n",fileName,oriLineNum);
		return(errCodeParameter);
	}
	else
	{
		UnionSuccessLog("in UnionReadSpecTypeDefFromCHeaderFile:: [%d] typedef defined in [%s]\n",structNum,fileName);
		return(structNum);
	}
}

/* 将指定的类型定义打印到文件中
输入参数
	fileName	文件名称
	nameOfDef	结构名称
	fp		文件句柄
	writeToDefFile	控制写到定义文件中，1写，0，不写
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintSpecTypeDefInCHeaderFileToFp(char *fileName,char *nameOfDef,FILE *fp,int writeToDefFile)
{
	return(UnionReadSpecTypeDefFromCHeaderFile(fileName,nameOfDef,writeToDefFile,fp));
}


#ifndef _UNIONIO_
#define _UNIONIO_

/////////////

//包含头文件
#ifdef _WIN32_
#include <windows.h>
#include <stdio.h>
#endif

#ifndef _WIN32_ 
#define _FILE_OFFSET_BITS 64
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#endif

#ifdef _WIN32_
typedef HANDLE File;
#endif

#ifndef _WIN32_
typedef FILE* File;
#endif

#ifdef _WIN32_
typedef LPCSTR FileName;
#endif

#ifndef _WIN32_
typedef char* FileName;
#endif

#ifdef _WIN32_
typedef DWORD OpenMode;
#endif

#ifndef _WIN32_
typedef char* OpenMode;
#endif

#ifdef _WIN32_
typedef DWORD Whence;
#endif

#ifndef _WIN32_
typedef int Whence;
#endif

#ifdef _WIN32_
typedef DWORD FileLen;  
#endif

#ifndef _WIN32_
typedef long long FileLen;
#endif

//读写文件长度类型
#ifdef _WIN32_
typedef DWORD ReadFileLen;
#endif

#ifndef _WIN32_
typedef size_t  ReadFileLen;
#endif

//打开方式
#ifdef _WIN32_
#define UDesiredAccess (GENERIC_READ|GENERIC_WRITE)
#define UShareMode (FILE_SHARE_READ|FILE_SHARE_WRITE)
#define URead OPEN_EXISTING
#define UWrite CREATE_ALWAYS
#define UAppendWrite (CREATE_NEW|OPEN_ALWAYS)
#endif

#ifndef _WIN32_
#define URead "rb"
#define UWrite "wb+"
#define UAppendWrite "ab+"
#endif

//文件定位位置
#ifdef _WIN32_
#define USEEK_SET FILE_BEGIN
#define USEEK_CUR FILE_CURRENT
#define USEEK_END FILE_END
#endif

#ifndef _WIN32_
#define USEEK_SET SEEK_SET
#define USEEK_CUR SEEK_CUR
#define USEEK_END SEEK_END
#endif


//打开文件的返回值
#ifdef _WIN32_
#define OpenFail INVALID_HANDLE_VALUE
#define ReadFail 0;
#endif

#ifndef _WIN32_
#define OpenFail NULL

#endif

//定位位置类型
#ifdef _WIN32_
typedef __int64 Uoffset;
#endif

#ifndef _WIN32_
typedef long long Uoffset;
#endif


#ifdef _WIN32_
#define Ufopen(name, mode) (CreateFile((FileName)name, UDesiredAccess, UShareMode, NULL, mode, FILE_ATTRIBUTE_NORMAL, NULL))
#define Ufclose(h) (CloseHandle(h))
#define Ufflush(h) (fflush(h))
#define Ufread(handle,buf, read, realread) (ReadFile(handle, buf, read, &realread, NULL))
#define Ufwrite(handle,buf, write, realwrite) (WriteFile(handle, buf, write, &realwrite, NULL))
#define Ufclose(h) (CloseHandle(h))
#else
#define Ufopen(name, mode) (fopen((FileName)name, mode))
#define Ufclose(h) (fclose(h))
#define Ufflush(h) (fflush(h))
#define Ufread(handle,buf, read, realread) ((realread)=fread(buf, 1, read, handle))
#define Ufwrite(handle,buf, write, realwrite) ((realwrite)=fwrite(buf, write, 1, handle))
#endif

Uoffset Ufseek(File handle, Uoffset offset, Whence whence);

Uoffset UnionGetFileSize(FileName fileName);

int UEof(File handle);



#endif


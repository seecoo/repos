
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "UnionIO.h"

Uoffset Ufseek(File handle, Uoffset offset, Whence whence)
{
#ifdef _WIN32
        long high, len;
	LARGE_INTEGER   fileOff;
        fileOff.QuadPart =offset;

        if (offset < 0)
                return(-1);


	if ((fileOff.LowPart = SetFilePointer(handle, fileOff.LowPart, &fileOff.HighPart, whence)) == HFILE_ERROR)
			return(-1);

	return(fileOff.QuadPart);	

#else
        if (offset < 0)
                return(-1);

        if (fseeko(handle, offset, whence) > 0)
                return(-1);

        return(ftello(handle));
#endif
}

Uoffset UnionGetFileSize(FileName fileName)
{
        File file;
        Uoffset size;

        if ((file = Ufopen(fileName, URead)) == OpenFail)
                return(-1);

        size = Ufseek(file, 0, USEEK_END);
        Ufclose(file);
        return(size);
}

int UEof(File handle)
{
        Uoffset nowOffset;

        nowOffset = Ufseek(handle, 0, USEEK_CUR);
        if (nowOffset == Ufseek(handle, 0 , USEEK_END))
                return(1);
        else
        {
                Ufseek(handle, nowOffset, USEEK_SET);
                return(0);
        }
}

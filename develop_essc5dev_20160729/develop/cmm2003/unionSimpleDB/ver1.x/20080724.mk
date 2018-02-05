
HOMEDIR = $(CMM2003DIR)/unionSimpleDB
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = table.20080724.o	\
	allTableInfo.20080724.o	\
	recInfo.20080724.o	\
	database.20080724.o	\
	record.20080724.o
makeall:	$(objs0130)


finalProduct:	$(null)
		cp $(INCDIR)database.h				$(CMM2003INCDIR)
		cp $(INCDIR)record.h				$(CMM2003INCDIR)
		cp $(INCDIR)allTableInfo.h			$(CMM2003INCDIR)
		cp $(INCDIR)recInfo.h				$(CMM2003INCDIR)
		cp $(INCDIR)table.h				$(CMM2003INCDIR)
		cp $(INCDIR)unionDatabaseCmd.h			$(CMM2003INCDIR)
		mv *.o	$(CMM2003LIBDIR)

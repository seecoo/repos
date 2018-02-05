HOMEDIR = $(CMM2003DIR)/unionSQL
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unionSQLQueryCondition.20080724.o	\
	unionRecFile.20080724.o	\
	unionRec.20080724.o	\
	unionFldValueAssign.20080724.o	\
	unionFldGrp.20080724.o	\
	unionEnumValueDef.20080724.o	\
	unionTBLRecInterface.20080724.o	\
	unionTBLQueryConf.20080724.o
makeall:	$(objs0130)


finalProduct:	$(null)
		cp $(INCDIR)unionFldValueAssign.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionFldGrp.h			$(CMM2003INCDIR)
		cp $(INCDIR)unionTBLRecInterface.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionTBLQueryConf.h			$(CMM2003INCDIR)
		cp $(INCDIR)unionRecFile.h			$(CMM2003INCDIR)
		cp $(INCDIR)unionRec0.h				$(CMM2003INCDIR)
		cp $(INCDIR)unionSQLQueryCondition.h		$(CMM2003INCDIR)
		mv *.o	$(CMM2003LIBDIR)

HOMEDIR = $(CMM2003DIR)/unionComplexDB
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall						\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionComplexDBCommon.20080801.o		\
	unionComplexDBDataTransfer.20080801.o	\
	unionComplexDBObjectFileName.20080801.o	\
	unionComplexDBObjectSql.20080801.o		\
	unionComplexDBObjectDef.20080801.o		\
	unionComplexDBObjectChildDef.20080801.o	\
	unionComplexDBPrimaryKey.20080801.o		\
	unionComplexDBUniqueKey.20080801.o		\
	unionComplexDBRecordValue.20080801.o		\
	unionComplexDBRecordParents.20080801.o	\
	unionComplexDBRecordChild.20080801.o		\
	unionComplexDBObject.20080801.o		\
	unionComplexDBOperationDef.20080801.o		\
	unionComplexDBUnionQuery.20080801.o		\
	unionComplexDBTrigger.20080801.o		\
	unionComplexDBRecord.20080801.o \
	unionComplexDBCursors.20080801.o 
makeall:	$(objs)

finalProduct:	$(null)
	mv *.o							$(CMM2003LIBDIR)
	cp $(INCDIR)unionComplexDBDataTransfer.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBUnionQuery.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBOperationDef.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBTrigger.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBCommon.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBObjectFileName.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBObjectSql.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBObjectDef.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBObjectChildDef.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBPrimaryKey.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBUniqueKey.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBRecordValue.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBRecordParents.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBRecordChild.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBObject.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBRecord.h			$(CMM2003INCDIR)
	cp $(INCDIR)unionComplexDBCursors.h			$(CMM2003INCDIR)


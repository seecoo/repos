# 2003/09/22	Wolfgang Wang

#	01	unionMonitor

#	2003/09/22
#	libs:
#		unionMonitorForGDB.20060606.a

HOMEDIR = $(CMM2003DIR)/unionMonitor/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)
# AIX使用以下开关
# DEFINES = -D _UNIX  $(OSSPEC) -D _AIX_

all:	unionMonitorForGDB.20060606.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 unionMonitor
objs0110 = unionMonitorForGDB.20060606.o
unionMonitorForGDB.20060606.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionMonitorForGDB.20060606.a $(objs0110)

finalProducts:	$(null)
	#libs
		mv	unionMonitorForGDB.20060606.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o

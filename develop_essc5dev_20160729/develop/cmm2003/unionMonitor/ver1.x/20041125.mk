# 2003/09/22	Wolfgang Wang

#	01	unionMonitor

#	2003/09/22
#	libs:
#		unionMonitor1.0.20041125.a

HOMEDIR = $(CMM2003DIR)/unionMonitor/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)
# AIX使用以下开关
# DEFINES = -D _UNIX  $(OSSPEC) -D _AIX_

all:	unionMonitor1.0.20041125.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 unionMonitor
objs0110 = unionMonitor1.0.20041125.o
unionMonitor1.0.20041125.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionMonitor1.0.20041125.a $(objs0110)

finalProducts:	$(null)
	#includes
		cp	$(INCDIR)unionMonitor.h		$(CMM2003INCDIR)
	#libs
		mv	unionMonitor1.0.20041125.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o

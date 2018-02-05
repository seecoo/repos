#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC) -D _UnionDesKey_3_x_ -D _UnionSJL06_2_x_Above_ -D _UnionTask_3_x_
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a		\
	inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a		\
	finalProducts							\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs0520 =	outputDesKeys.20050607.o
outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a:	$(objs0520)
			ar $(OSSPECPACK) rv outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a $(objs0520)

objs0620 =	inputDesKeys.20050607.o
inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a:	$(objs0620)
			ar $(OSSPECPACK) rv inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a $(objs0620)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	outputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a		$(CMM2003LIBDIR)
		mv	inputDesKeys.DesKey_3_x.SJL06_2_x_above.20050607.a		$(CMM2003LIBDIR)

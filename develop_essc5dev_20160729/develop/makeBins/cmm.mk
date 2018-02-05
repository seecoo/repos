OSName := $(shell uname)

include dirAndLib.def

CMMBINDIR=$(subst $(UNIONREC),$(UNIONLIBDIR)/bin/,$(TESTBINDIR))

# 要生成的程序列表
all:	allBins

allBins:
ifeq ($(OSName),AIX)
	cp $(CMMBINDIR)* $(TESTBINDIR)
else
	cp -uv $(CMMBINDIR)* $(TESTBINDIR)
endif

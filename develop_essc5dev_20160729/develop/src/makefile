#需要编译的目录

dirs := $(dir $(shell find . -name makefile))
dirs := $(filter-out ./,$(dirs))

all:
	$(foreach N,$(dirs),make -C $(N);)
	@echo "编译结束..."

clean:	
	$(foreach N,$(dirs),make -C $(N) clean;)

install:

show:
	@echo 'dirs		:' $(dirs)

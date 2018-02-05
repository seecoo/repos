#需要编译的目录

dirs = $(dir $(shell find . -name makefile))
dirs := $(filter-out ./ ./unionRealDB/ver1.x/ ./unionVersion/tools/ ./unionVersion/ver1.x/,$(dirs))
end_dirs = ./unionRealDB/ver1.x/ ./unionVersion/tools/ ./unionVersion/ver1.x/

all:
	$(foreach N,$(dirs),make -C $(N) install;)
	$(foreach N,$(dirs),echo "编译$(N)...";compile.sh $(N);)
	$(foreach N,$(end_dirs),echo "编译$(N)...";compile.sh $(N);)
	@echo "编译结束..."

pre:
	$(foreach N,$(dirs),make -C $(N) install;)

clean:	
	$(foreach N,$(dirs),make -C $(N) clean;)
	$(foreach N,$(end_dirs),make -C $(N) clean;)
install:

show:
	@echo 'dirs		:' $(dirs)

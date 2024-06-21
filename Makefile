DEBUG = arm

clean: dbg_clean
	@rm -rf build
	@rm -f KaiSei
	@mkdir build

all: clean
	@cd build && cmake .. && make
	@mv build/KaiSei ./KaiSei
	@echo "KaiSei finish"

start:
	@mkdir build
	@cd build && cmake .. && make
	@mv build/KaiSei ./KaiSei

dbg_single: 
	@ ./KaiSei ./debug/test.c --debug $(DEBUG)

run: 
	@ ./KaiSei -S -o ./debug/test.S ./debug/test.c

dbg: 
	@ ./KaiSei --debug lex ./debug/test.c
	@ ./KaiSei --debug parse ./debug/test.c 
	@ ./KaiSei --debug optsym ./debug/test.c 
	@ ./KaiSei --debug ir ./debug/test.c 
	@ ./KaiSei --debug cfg ./debug/test.c 
	@ ./KaiSei --debug lva ./debug/test.c 
	@ ./KaiSei --debug ra ./debug/test.c 
	@ ./KaiSei --debug arm ./debug/test.c 

dbg_shell:
	@ ./KaiSei -S -o ./dbg/test.S ./dbg/test.c --debug shell

dbg_clean:
	@ rm -f ./debug/*.cfg
	@ rm -f ./debug/*.lex
	@ rm -f ./debug/*.sym
	@ rm -f ./debug/*.par
	@ rm -f ./debug/*.ir
	@ rm -f ./debug/*.S

help: 
	@ ./KaiSei --help
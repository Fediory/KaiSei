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
	@ ./KaiSei ./debug/test.c -S ./debug/test.S

dbg: 
	@ ./KaiSei ./debug/test.c --debug lex
	@ ./KaiSei ./debug/test.c --debug parse
	@ ./KaiSei ./debug/test.c --debug sym
	@ ./KaiSei ./debug/test.c --debug opt
	@ ./KaiSei ./debug/test.c --debug optsym
	@ ./KaiSei ./debug/test.c --debug ir
	@ ./KaiSei ./debug/test.c --debug cfg
	@ ./KaiSei ./debug/test.c --debug lva
	@ ./KaiSei ./debug/test.c --debug ra
	@ ./KaiSei ./debug/test.c --debug arm

dbg_clean:
	@ rm -f ./debug/*.cfg
	@ rm -f ./debug/*.lex
	@ rm -f ./debug/*.sym
	@ rm -f ./debug/*.par
	@ rm -f ./debug/*.ir
	@ rm -f ./debug/*.S

help: 
	@ ./KaiSei --help
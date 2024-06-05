clean:
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

lex: 
	@ ./KaiSei --debug lex ./debug/test.c

parse: 
	@ ./KaiSei --debug parse ./debug/test.c
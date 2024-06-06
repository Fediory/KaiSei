DEBUG = arm

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

run: 
	@ ./KaiSei ./debug/test.c --debug $(DEBUG)

help: 
	@ ./KaiSei --help
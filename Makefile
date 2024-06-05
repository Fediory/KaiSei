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
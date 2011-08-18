PWD = $(shell pwd)
export BUILD_ROOT = $(PWD)/.build
export INSTALL_ROOT = $(PWD)/.install

all: compile

deps: 
	echo "Getting dependencies"

copy:
	mkdir -p $(BUILD_ROOT)
	cp src/* $(BUILD_ROOT)
	cp include/* $(BUILD_ROOT)	

compile: deps copy
	make -C $(BUILD_ROOT) 

release: compile
	echo "Preparing release"

clean:
	rm -rf $(BUILD_ROOT)
	rm -rf $(INSTALL_ROOT)
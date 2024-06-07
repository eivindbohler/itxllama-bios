.PHONY: all clean

all:
	./build-target.sh

clean:
	rm -rf out
	cd seabios && ../xgcc-seabios-make.sh distclean
	cd coreboot && make distclean

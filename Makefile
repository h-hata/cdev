obj-m := add.o
all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules
clean:
	rm -fr *.ko *.o .*.cmd

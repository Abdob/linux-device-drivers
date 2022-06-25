KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := mouse.o

all: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	insmod ./mouse.ko;
	mknod -m 666 /dev/mouse c 32 0;
	echo "" > /dev/mouse;
	cat < /dev/mouse;

compile:


unload:
	-su -c " rmmod mouse;"
	-rm -fr /dev/mouse;

clean: unload
	-@rm -fr *.o mouse*.o mouse*.ko .mouse*.* mouse*.*.* .tmp_versions [mM]odule* mouse.mod .cache.mk .modules.order.cmd .Module.symvers.cmd
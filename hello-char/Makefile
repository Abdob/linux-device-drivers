KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := CDD2.o

all: run 
	#make -s clean

run: load CDD2app
# See line 1696 fops
# vi +1696 /usr/src/linux/include/linux/fs.h 
#
# line 859 struct file
# vi +859 /usr/src/linux/include/linux/fs.h 
#
# See 
# vi /usr/include/asm-generic/errno-base.h
# vi /usr/include/asm-generic/errno.h
	ls -l /dev/CDD2;
	echo "Hello World" > /dev/CDD2;
	cat < /dev/CDD2;
	echo "Hello Universe" > /dev/CDD2;
	cat < /dev/CDD2;
	./CDD2app;
	
adv:
	# since CDDNUMDEVS is 6 .. support upto 6 devices with same driver.
	mknod -m 666 /dev/CDD3 c 32 1
	mknod -m 666 /dev/CDD5 c 32 5
	mknod -m 666 /dev/CDD6 c 32 6
	mknod -m 666 /dev/CDD7 c 32 7
	ls -l /dev/CDD?
	cat /dev/CDD3
	cat /dev/CDD5
	-cat /dev/CDD6
	-cat /dev/CDD7

load: CDD2.o
	-su -c "insmod ./CDD2.ko; mknod -m 666 /dev/CDD2 c 32 0;"
	ls -l /dev/CDD2
	lsmod | grep CDD
	cat /proc/devices | grep CDD

compile CDD2.o:
	@echo 'see /usr/src/linux/Documentation/admin-guide/devices.txt'
	sleep 5;
	$(MAKE) -C $(KDIR) M=$(PWD) modules

###
###  Alternatively, you may want to use the early 2.6 syntax of
###  $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
###

CDD2app:
	-gcc -o CDD2app CDD2app.c;

unload:
	-su -c " rmmod CDD2;"
	-rm -fr /dev/CDD?;

clean: unload
	-@rm -fr *.o CDD2*.o CDD2*.ko .CDD2*.* CDD2*.*.* CDD2app .tmp_versions [mM]odule* CDD2.mod .cache.mk .modules.order.cmd .Module.symvers.cmd

mem:
	cat /proc/devices | grep mem
	ls -l /dev/* | egrep "^c" | egrep "[ 	][ 	]*1," | sort +5.0n -6.0

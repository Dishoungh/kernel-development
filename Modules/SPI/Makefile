obj-m += sh1106.o
obj-m += sh1106-low_level.o
 
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean

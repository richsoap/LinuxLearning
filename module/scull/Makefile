ifeq ($(KERNELRELEASE),)

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
		$(MAKE) -C $(KDIR) M=$(PWD) modules

modules_install:
		$(MAKE) -C $(KDIR) M=$(PWD) modules_install

clean:
		rm -rf *.o .depend *.mod.o *.mod.c Module.* modules.* *.o.cmd

.PHONY:modules modules_install clean

else

obj-m :=scull.o 

endif

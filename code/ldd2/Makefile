KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := dummy1234_plat_drv.o dummy1234_plat_dev.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

load-dummy1234-drv:
	sudo insmod dummy1234_plat_drv.ko

unload-dummy1234-drv:
	sudo rmmod dummy1234_plat_drv

load-dummy1234-dev:
	sudo insmod dummy1234_plat_dev.ko

unload-dummy1234-dev:
	sudo rmmod dummy1234_plat_dev

.PHONY: all clean load-dummy1234-drv \
	unload-dummy1234-drv load-dummy1234-dev unload-dummy1234-dev

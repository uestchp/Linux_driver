KERNEL_DIR = /home/book/jz2440/linux-2.6.22.6

all:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` modules clean
	rm -rf modules.order Module.symvers

obj-m += led_driver.o

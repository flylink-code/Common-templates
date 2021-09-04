KERN_DIR = 

make?ARCH=arm?CROSS_COMPILE=arm-linux-gnueabihf-


all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += drv_spi_templates.o

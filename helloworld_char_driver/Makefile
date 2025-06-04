obj-m := helloworld_rpi5_char_driver.o helloworld_rpi5_class_driver.o misc_rpi5_driver.o

KERNEL_DIR ?= $(HOME)/linux_rpi/linux

all:
	make -C $(KERNEL_DIR) \
	ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
	M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
	ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
	M=$(PWD) clean

deploy:
	scp *.ko balavignesh@192.168.1.7:/home/balavignesh/test

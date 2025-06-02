obj-m := helloworld_rpi5.o

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
	scp *.ko balavignesh@192.168.1.7:/home/balavignesh

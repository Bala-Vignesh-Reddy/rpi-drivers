CC = aarch64-linux-gnu-gcc

all: ioctl_test

app: ioctl_test.c
	$(CC) -o $@ $^

clean:
	rm ioctl_test

deploy: ioctl_test
	scp $^ balavignesh@192.168.1.7:/home/balavignesh/test

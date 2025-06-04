/*
 * This module uses the devtmpfs to create device node instead of doing it manually as done in helloworld_rpi5_char_driver.
 *
 * A linux driver creates/destroy the class by using following kernel APIs.
 * class_create() - creates a class for the device visible in /sys/class
 * class_destroy() - removes the class
 *
 * Linux driver creates the device node by using following kernel APIs.
 * device_create() - creates a device node in the /dev directory
 * device_destroy() - removes a device node in the /dev directory
 *
 * driver will have a class name and a device name; hello_class as a class name and mydev as the device name.
 * this results in creation of device that appears on fs at /sys/class/hello_class/mydev
 *
 *
 * alloc_chrdev_region() - automatically allocates a major number to device, registering the device class anc creating the device node.
 */

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h> /* class_create(), device_create().. */

#define DEVICE_NAME "mydev"
#define CLASS_NAME "hello_class"

static struct class *helloClass;
static struct cdev my_dev;
dev_t dev;

static int my_dev_open(struct inode *inode, struct file *file){
	pr_info("my_dev_open() is called\n");
	return 0;
}

static int my_dev_close(struct inode *inode, struct file *file){
	pr_info("my_dev_close() is called\n");
	return 0;
}

static long my_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	pr_info("my_dev_ioctl() is called. cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}

static const struct file_operations my_dev_fops = {
	.owner = THIS_MODULE,
	.open = my_dev_open,
	.release = my_dev_close,
	.unlocked_ioctl = my_dev_ioctl,
};

static int __init hello_init(void){
	int ret;
	dev_t dev_no;
	int Major;
	struct device *helloDevice;

	pr_info("Hello world init\n");

	/* Allocate dynamically device numbers (only one in this driver) */
	ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
	if (ret < 0){
		pr_info("unable to alloacte Major number \n");
		return ret;
	}

	/* getting the major number from first device identifier */
	Major = MAJOR(dev_no);

	/*getting the first device identifier, that matches with dev_no */
	dev = MKDEV(Major, 0);

	pr_info("allocated correctly with major number %d\n", Major);

	/* initialize the cdev structure and add it to kernel space */
	cdev_init(&my_dev, &my_dev_fops);
	ret = cdev_add(&my_dev, dev, 1);
	if (ret < 0){
		unregister_chrdev_region(dev, 1);
		pr_info("unable to add cdev\n");
		return ret;
	}

	/* register the device class */
	helloClass = class_create(CLASS_NAME);
	if (IS_ERR(helloClass)){
		unregister_chrdev_region(dev, 1);
		cdev_del(&my_dev);
		pr_info("failed to register device class\n");
		return PTR_ERR(helloClass);
	}
	pr_info("device class registered correctly\n");

	/* create a device node */
	helloDevice = device_create(helloClass, NULL, dev, NULL, DEVICE_NAME);
	if (IS_ERR(helloDevice)){
		unregister_chrdev_region(dev, 1);
		cdev_del(&my_dev);
		pr_info("failed to create the device\n");
		return PTR_ERR(helloDevice);
	}
	pr_info("device is created correctly\n");
	return 0;
}

static void __exit hello_exit(void){
	device_destroy(helloClass, dev);
	class_destroy(helloClass);
	cdev_del(&my_dev);
	unregister_chrdev_region(dev, 1);
	pr_info("hello world with parameter exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Balavignesh");
MODULE_DESCRIPTION("Module in which device node is created by devtmpfs");

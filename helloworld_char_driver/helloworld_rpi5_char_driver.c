#include <linux/module.h>

/* header files to support character devices */
#include <linux/cdev.h>
#include <linux/fs.h>

#define MY_MAJOR_NUM 202 /* defined major number */

/* 
 * Linux supports three types of devices: character devices, block devices and network devices
 *
 * Character Device are most common device, which are read and written directly without buffering. eg. keyboards, monitors, printers etc.
 * Block devices can only be written to and read from multiples of block size, typically 512 or 1024 bytes.
 * Network devices are accessed via the BSD socket and interface and networking subsystems.
 *
 * In Linux, every device is identified by two numbers: a major number and a minor number. These
numbers can be seen by invoking ls -l /dev on the host PC

 * MKDEV macro will combine a mjor number and monior number to a dev_t data type used to hold first device identifier
 *
 * unregister_chrdev_region() - function used to return the devices when module is removed
 *
 * file_operations structure - defines functions pointer for opening, reading from, writing to the device..
 *
 * struct cdev - used to represent character device internally, 
 * cdev_init() function call will initialize
 * cdev_add() - add the details to kernel space
 * cdev_del() - function to delete the cdev structure
 *
 * mknod - utility provided by the linux to create a device node to reference the driver
 * mknod has 4 parameters
 * first is name of device node
 * second indicates whether the driver to which device node interfaces is block driver or character driver
 * last two param are major and minor numbers.
 */

static struct cdev my_dev;

static int my_dev_open(struct inode *inode, struct file *file){
	pr_info("my_dev_open() is called.\n");
	return 0;
}

static int my_dev_close(struct inode *inode, struct file *file){
	pr_info("my_dev_close() is called.\n");
	return 0;
}

static long my_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	pr_info("my_dev_ioctl() is called. cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}

/* Declare a file_operations structure */
static const struct file_operations my_dev_fops = {
	.owner = THIS_MODULE,
	.open = my_dev_open,
	.release = my_dev_close,
	.unlocked_ioctl = my_dev_ioctl,
};

static int __init hello_init(void){
	int ret;

	dev_t dev = MKDEV(MY_MAJOR_NUM, 0); /*get first device identifier */
	pr_info("Hello world init\n");

	/* allocate all the character device identifiers,
	 * only one in this case, obtained with MKDEV macro*/
	ret = register_chrdev_region(dev, 1, "my_char_device");
	if (ret < 0){
		pr_info("Unable to allocate major number %d\n", MY_MAJOR_NUM);
		return ret;
	}

	/* Initialize the cdev structure and add it to kernel space */
	cdev_init(&my_dev, &my_dev_fops);
	ret = cdev_add(&my_dev, dev, 1);
	if (ret < 0){
		unregister_chrdev_region(dev, 1);
		pr_info("Unable to add cdev\n");
		return ret;
	}
	return 0;
}

static void __exit hello_exit(void){
	pr_info("Hello world exit\n");
	cdev_del(&my_dev);
	unregister_chrdev_region(MKDEV(MY_MAJOR_NUM, 0), 1);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Balavignesh");
MODULE_DESCRIPTION("Module that interacts with the ioctl system call");



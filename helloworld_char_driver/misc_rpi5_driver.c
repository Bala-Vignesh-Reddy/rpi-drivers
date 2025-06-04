/* Miscellaneous Character driver
 * 
 * Misc Framework is an interface that allows modules to register their individual minor numbers
 * Device driver implemented as miscellaneous characters uses the major number allocated by the kernel for miscellaneous devices. this eliminates the need to define a unique major no. for driver.
 *
 * Each probed device is dynamically assigned a minor number and is listed with directory entry within the sysfs pseudo-filesystem under /sys/class/misc/
 *
 * Major number 10 is officially assigned to misc driver. module can register individual minor number with the misc driver and take care of a small device, needing only a single entry point.
 *
 * miscdevice structure -
 * struct miscdevice {
 * 	int minor;
 * 	const char *name;
 * 	const struct file_operations *fops;
 * 	struct list_head list;
 * 	struct device *parent;
 * 	struct device *this_device;
 * 	sturct char *nodename;
 * 	umode_t mode;
 * };
 *
 * minor - minor number being registered
 * name - name for this device, found in /proc/misc file
 * fops - pointer to the file_operations structure
 * parent - pointer to device structure that represents the hardware device exposed by the driver
 *
 * misc driver exports two functions, misc_register(), misc_deregister() to register and unregister their own minor number.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static int my_dev_open(struct inode *inode, struct file *file){
	pr_info("my_dev_open() is called\n");
	return 0;
}

static int my_dev_close(struct inode *inode, struct file *file){
	pr_info("my_dev_close() is called\n");
	return 0;
}

static long my_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	pr_info("my_dev_ioctl() is called, cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}

static const struct file_operations my_dev_fops = {
	.owner = THIS_MODULE,
	.open = my_dev_open,
	.release = my_dev_close,
	.unlocked_ioctl = my_dev_ioctl,
};

static struct miscdevice helloworld_miscdevice = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mydev",
	.fops = &my_dev_fops,
};

static int __init hello_init(void){
	int ret_val;
	pr_info("Hello world init\n");

	/* register the device with kernel */
	ret_val = misc_register(&helloworld_miscdevice);

	if (ret_val != 0){
		pr_err("could not register the misc device mydev");
		return ret_val;
	}
	pr_info("mydev: got minor %i\n", helloworld_miscdevice.minor);
	return 0;
}

static void __exit hello_exit(void){
	pr_info("hello world exit\n");

	misc_deregister(&helloworld_miscdevice);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Balavignesh");
MODULE_DESCRIPTION("This is helloworld_char_driver using misc framework");

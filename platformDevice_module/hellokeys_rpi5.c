#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/miscdevice.h>

/* 
 * if we simply compile and run the driver then it will not execute, as the probe() function should 
 * be included in the main device tree.. so, there are two ways of doing it.. 
 *
 * the dt file are present in arch/arm64/boot/dts/broadcom
 * first make changes in the bcm2712-rpi-5-b.dts or bcm2712-d.dtsi
 *
 * make the necessary changes in bcm2712-d.dtsi
 * search for `soc` and add the following at the end
 * `
 *  hellokeys {
 * 	comptaible = "arrow,hellokeys";
 *  };
 * `
 * compile it `make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs`
 * 
 * use scp to send the .dtb file to pi and reboot it
 *
 * Book: **Modify the Device Tree files (located in arch/arm/boot/dts/ in the kernel source tree), 
 * including your DT device nodes. There must be a DT device node´s compatible property 
 * identical to the compatible string stored in one of the driver´s of_device_id structures.**
 *
 */

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

/* add probe() function */
static int __init my_probe(struct platform_device *pdev){
	int ret_val;
	pr_info("my_probe() function is called.\n");
	ret_val = misc_register(&helloworld_miscdevice);

	if (ret_val != 0){
		pr_err("could not register the misc device mydev");
		return ret_val;
	}
	pr_info("mydev: got minor %i\n", helloworld_miscdevice.minor);
	return 0;
}

/* add remove() function */
static void __exit my_remove(struct platform_device *pdev){
	pr_info("my_remove() function is called.\n");
	misc_deregister(&helloworld_miscdevice);
}

/* declaring list of devices supported by driver */
static const struct of_device_id my_of_ids[] = {
	{ .compatible = "arrow,hellokeys"},
	{},
};
MODULE_DEVICE_TABLE(of, my_of_ids);

/* creating a platform_driver structure */
static struct platform_driver my_platform_driver = {
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.name = "hellokeys",
		.of_match_table = my_of_ids,
		.owner = THIS_MODULE,
	}
};

/* registering platform driver */
module_platform_driver(my_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Balavignesh");
MODULE_DESCRIPTION("This is simplest platform driver");

#include <linux/module.h>

/*
 * the perm argument specifies the permissions of the 
 * corresponding file in sysfs.
 * sysfs - virtual filesystem that exports info about various kernel 
 * subsystems, hardware devices from kernel's device model to user space 
 * through virtual files.
 *
 * module_param(name, type, perm);
 *
*/

static int num = 5;

/* S_IRUGO: everyone can read the sysfs entry */
module_param(num, int, S_IRUGO);

static int __init hello_init(void){
	pr_info("parameter num = %d.\n", num);
	pr_info("hello from init\n");
	return 0;
}

static void __exit hello_exit(void){
	pr_info("bye from exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Balavignesh");
MODULE_DESCRIPTION("This modules accepts parameters");

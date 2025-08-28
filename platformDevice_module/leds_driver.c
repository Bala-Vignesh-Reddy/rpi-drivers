/*
 * @breif Linux platform driver for RGB GPIO LEDs on raspberry pi 5
 *
 * In raspberry pi 5 the GPIO is handled by the RP1 chip which is connected with the memory through PCIe.
 *
 * This driver manages three LEDs connected with GPIO of pi, using the device tree. It exposes 
 * character devices (/dev/ledred, /dev/ledgreen, /dev/ledblue) for controlling each LED via 
 * simple write operation
 *
 * Device tree was modified using the overlays method.. compatible string: "arrow,RGBleds"
 * Each child node in DT contain:
 *  - label: a string (eg. "ledred")
 *  - gpios: GPIO phandle with pin number
 *
 * Features:
 *  - creates a misc character device for each LED
 *  - parses label and GPIO from Device tree
 *  - writes to devices turn the LEDs on/off
 *  - memory and resource management using devm_* APIs
 *
 *  @detailed explanation: LED_README
 *
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#define MAX_LEDS 3

struct led_dev {
    struct miscdevice led_misc_device;
    u32 led_mask;
    const char *led_name;
    //struct gpio_desc *gpiod;
    int gpio_num;
};

struct leds_drvdata {
    struct led_dev *leds[MAX_LEDS];
    int num_leds;
};

static ssize_t led_write(struct file *file, const char __user *buff, size_t count, loff_t *ppos){
    struct led_dev *led_device = container_of(file->private_data, struct led_dev, led_misc_device);
    pr_info("LED device: %s write called\n", led_device->led_name);

    char kbuf[2];
    if (copy_from_user(kbuf, buff, 1))
        return -EFAULT;

    kbuf[1] = '\0';

    if (kbuf[0] == '1')
        gpio_set_value(led_device->gpio_num, 1);
    else if (kbuf[0] == '0')
        gpio_set_value(led_device->gpio_num, 0);
    else
        return -EINVAL;

    return count;
}

static ssize_t led_read(struct file *file, char __user *buff, size_t count, loff_t *ppos){
    struct led_dev *led_device = container_of(file->private_data, struct led_dev, led_misc_device);
    int value = gpio_get_value(led_device->gpio_num);
    char state[2] = {value ? '1': '0', '\n' };

    return simple_read_from_buffer(buff, count, ppos, state, sizeof(state));
}

static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .write = led_write,
    .read = led_read,
};

static const struct of_device_id leds_of_match[] = {
    { .compatible = "arrow,RGBleds" },
    {},
};
MODULE_DEVICE_TABLE(of, leds_of_match);

static int leds_probe(struct platform_device *pdev) {
//    struct led_dev *led_device;
    struct device_node *child;
    int ret_val;
    int gpio;

    pr_info("leds_probe() called for device: %s\n", dev_name(&pdev->dev));

    struct leds_drvdata *drvdata;
    struct led_dev *led_device;
    drvdata = devm_kzalloc(&pdev->dev, sizeof(*drvdata), GFP_KERNEL);
    if (!drvdata)
        return -ENOMEM;

    for_each_child_of_node(pdev->dev.of_node, child){
        if (drvdata->num_leds >= MAX_LEDS)
            break;

        led_device = devm_kzalloc(&pdev->dev, sizeof(*led_device), GFP_KERNEL);
        if (!led_device)
            return -ENOMEM;

        ret_val = of_property_read_string(child, "label", &led_device->led_name);
        if (ret_val){
            pr_err("failed to read label property\n");
            // return ret_val;
            continue;
        }

        led_device->led_misc_device.minor = MISC_DYNAMIC_MINOR;
        led_device->led_misc_device.name = led_device->led_name;
        led_device->led_misc_device.fops = &led_fops;

        if (strcmp(led_device->led_name, "ledred") == 0)
            led_device->led_mask = 1 << (27 % 32);
        else if (strcmp(led_device->led_name, "ledgreen") == 0)
            led_device->led_mask = 1 << (22 % 32);
        else if (strcmp(led_device->led_name, "ledblue") == 0)
            led_device->led_mask = 1 << (26 % 32);
        else {
            pr_warn("unknown led lavel: %s\n", led_device->led_name);
            //return -EINVAL;
            continue;
        }

        ret_val = misc_register(&led_device->led_misc_device); 

        if (ret_val) {
            pr_err("failed to register misc device for %s", led_device->led_name);
            continue;
        }

        drvdata->leds[drvdata->num_leds++] = led_device;
        pr_info("Registered misc device: /dev/%s\n", led_device->led_misc_device.name);

       // led_device->gpiod = devm_gpiod_get(&pdev->dev, led_device->led_name, GPIOD_OUT_LOW);
        //if (IS_ERR(led_device->gpiod)){
          //  pr_err("failed to get GPIO for %s\n", led_device->led_name);
           // continue;
        //}
        gpio = of_get_named_gpio(child, "gpios", 0);
        if (gpio < 0){
            pr_err("failed to get gpio from DT for %s\n", led_device->led_name);
            continue;
        }
        ret_val = devm_gpio_request_one(&pdev->dev, gpio, GPIOF_OUT_INIT_LOW, led_device->led_name);
        if (ret_val){
            pr_err("failed to request GPIO %d for %s\n", gpio, led_device->led_name);
            continue;
        }
        led_device->gpio_num = gpio;

    }

   // platform_set_drvdata(pdev, led_device);
    platform_set_drvdata(pdev, drvdata);
    return 0;
}

static void leds_remove(struct platform_device *pdev) {
    //struct led_dev *led_device = platform_get_drvdata(pdev);
    struct leds_drvdata *drvdata = platform_get_drvdata(pdev);
    int i;

    if(!drvdata){
        pr_err("Driver data is null!\n");
        return;
    }
    pr_info("leds_remove() called for device:%s\n", dev_name(&pdev->dev));
    pr_info("Removing %d LEDs\n", drvdata->num_leds);
    for (i=0; i < drvdata->num_leds; ++i){
        if(drvdata->leds[i]){
            misc_deregister(&drvdata->leds[i]->led_misc_device);
            pr_info("Deregistered misc device: /dev/%s\n", drvdata->leds[i]->led_misc_device.name);
        }
    }

}
    //pr_info("leds_remove() called for device: %s\n", dev_name(&pdev->dev));
    //misc_deregister(&led_device->led_misc_device);
    //pr_info("leds_remove exit\n");


static struct platform_driver leds_driver = {
    .probe = leds_probe,
    .remove = leds_remove,
    .driver = {
        .name = "rgb-leds-driver",
        .of_match_table = leds_of_match,
        .owner = THIS_MODULE,
    }
};

module_platform_driver(leds_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BalaVignesh");
MODULE_DESCRIPTION("Platform driver for RGB LEDs on pi 5");






/***************************************************************************//**
*  \file       rpi5-gpio.c
*
*  \details    Simple GPIO driver to control GPIO5, GPIO17, and GPIO19 on Raspberry Pi 5
*
*  \author     Dishoungh White II
*
*  \Tested with Linux 6.6.62+rpt-rpi-2712
*
*******************************************************************************/
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/gpio.h>     //GPIO
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>  //copy_to/from_user()

static struct gpio_desc* led = 0;

// Probe Function
static int gpio_probe(struct platform_device* pdev)
{
        struct device*          dev = &pdev->dev;
        struct device_node*     dn  = dev->of_node;

        printk(KERN_INFO "Probing GPIO Devices...\n");
        
        if (!led)
        {
                led = gpiod_get(dev, "led", GPIOD_OUT_LOW);

                if (IS_ERR(led))
                {
                        printk(KERN_ERR "Failed to probe GPIO LED\n");
                        return -ENODEV;
                }
        }

        // Turn on LED to signify success
        gpiod_set_value(led, 1);

        printk(KERN_INFO "GPIO Probe Successful\n");

        return 0;
}

static int gpio_remove(struct platform_device* pdev)
{
        struct device*          dev = &pdev->dev;
        struct device_node*     dn  = dev->of_node;

        printk(KERN_INFO "Exiting GPIO Driver...\n");
        
        // Turn off LED to signify success
        gpiod_set_value(led, 0);

        printk(KERN_INFO "GPIO Exit Successful\n");

        return 0;
}

static struct of_device_id gpio_of_device_ids[] = {
        {.compatible = "dishoungh,rpi5-gpio", },
};

static struct platform_device_id gpio_plat_device_ids[] = {
        {.name = "rpi5-gpio"},
};

static struct platform_driver gpio_parser = {
        .probe    = gpio_probe,
        .remove   = gpio_remove,
        .id_table = gpio_plat_device_ids,
        .driver   = {
                .name           = "rpi5-gpio",
                .owner          = THIS_MODULE,
                .of_match_table = gpio_of_device_ids,
        },
};

static int      __init  gpio_driver_init(void);
static void     __exit  gpio_driver_exit(void);
 
static int __init gpio_driver_init(void)
{
        return platform_driver_register(&gpio_parser);
}

static void __exit gpio_driver_exit(void)
{
        return platform_driver_unregister(&gpio_parser);
}
 
module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dishoungh White II <dishounghwhiteii@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.8");

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Dynamically allocating the Major and Minor number)
*
*  \author     Dishoungh White II
*
* *******************************************************************************/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>         // kmalloc()
#include <linux/uaccess.h>      // copy_to/from_user()

#define TIMEOUT_SEC             0       // 0 Seconds
#define TIMEOUT_NSEC            500000L // 5 milliseconds

#define MEM_SIZE                1024

static struct cdev      dishoungh_cdev;
static struct class*    dev_class;
static struct device*   node;
dev_t dev = 0;
uint8_t* kernel_buffer;

struct ioctl_arguments
{
        uint32_t arg1;
        int16_t arg2;
        uint8_t arg3[3];
};

struct ioctl_arguments kernel_parameters = {0};


#define WR_VALUE _IOW('a', 'a', struct ioctl_arguments *)
#define RD_VALUE _IOR('a', 'b', struct ioctl_arguments *)

/*
** Function Prototypes
*/

static struct hrtimer   drv_hr_timer;
static unsigned int     count = 0;

static int              drv_open(struct inode* inodeptr, struct file* fileptr);
static int              drv_release(struct inode* inodeptr, struct file* fileptr);
static ssize_t          drv_read(struct file* fileptr, char __user* buf, size_t len, loff_t* off);
static ssize_t          drv_write(struct file* fileptr, const char* buf, size_t len, loff_t* off);

static long             drv_ioctl(struct file* fileptr, unsigned int cmd, unsigned long args);

static int      __init  my_driver_init(void);
static void     __exit  my_driver_exit(void);

/*
** File Operations Structure
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = drv_open,
        .release        = drv_release,
        .read           = drv_read,
        .write          = drv_write,
        .unlocked_ioctl = drv_ioctl,
};

//Timer Callback function. This will be called when timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    printk(KERN_INFO "Timer Callback function Called [%d]\n", count++);
    hrtimer_forward_now(timer, ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC));
    return HRTIMER_RESTART;
}

/*
** "open" handler
*/
static int drv_open(struct inode* inodeptr, struct file* fileptr)
{
        printk(KERN_INFO "Driver Open Function Called!!!\n");
        return 0;
}

/*
** "close" handler
*/
static int drv_release(struct inode* inodeptr, struct file* fileptr)
{
        printk(KERN_INFO "Driver Close Function Called!!!\n");
        return 0;
}

/*
** "read" handler
*/
static ssize_t drv_read(struct file* fileptr, char __user* buf, size_t len, loff_t* off)
{
        // Copy data from kernel space to the userspace buffer
        int cpy_err = copy_to_user(buf, kernel_buffer, MEM_SIZE);
        if (cpy_err)
        {
                printk(KERN_ERR "Driver Read: ERROR!\n");
                return -1;       
        }

        printk(KERN_INFO "Driver Read: Done!\n");
        return MEM_SIZE;
}

/*
** "write handler"
*/
static ssize_t drv_write(struct file* fileptr, const char* buf, size_t len, loff_t* off)
{
        // Copy data from userspace to kernel space
        int cpy_err = copy_from_user(kernel_buffer, buf, len);
        if (cpy_err)
        {
                printk(KERN_ERR "Driver Write: ERROR!\n");
                return -1;
        }

        printk(KERN_INFO "Driver Write: Done!\n");
        return len;
}

/*
** "ioctl" handler
*/
static long drv_ioctl(struct file* fileptr, unsigned int cmd, unsigned long args)
{
        switch(cmd)
        {
                case WR_VALUE:
                        if (copy_from_user(&kernel_parameters, (struct ioctl_arguments*) args, sizeof(struct ioctl_arguments)))
                        {
                                printk(KERN_ERR "IOCTL Write: Error!\n");
                                return -1;
                        }
                        
                        printk(KERN_INFO "Argument 1    = %lu\n", kernel_parameters.arg1);
                        printk(KERN_INFO "Argument 2    = %d\n", kernel_parameters.arg2);
                        printk(KERN_INFO "Argument 3[0] = %u\n", kernel_parameters.arg3[0]);
                        printk(KERN_INFO "Argument 3[1] = %u\n", kernel_parameters.arg3[1]);
                        printk(KERN_INFO "Argument 3[2] = %u\n", kernel_parameters.arg3[2]);
                        
                        break;
                case RD_VALUE:
                        if (copy_to_user((struct ioctl_arguments*) args, &kernel_parameters, sizeof(struct ioctl_arguments)))
                        {
                                printk(KERN_ERR "IOCTL Read: Error!\n");
                                return -1;
                        }

                        printk(KERN_INFO "Retrieved kernel parameters and passed them to userspace\n");

                        break;
                default:
                        printk(KERN_INFO "Command = %u\n", cmd);
                        break;
        }

        return 0;
}

/*
** Module Init Function
*/
static int __init my_driver_init(void)
{
        // Allocate Major Number
        if ((alloc_chrdev_region(&dev, 0, 1, "Dishoungh_Device")) < 0)
        {
                printk(KERN_ERR "Cannot allocate major number for Dishoungh Device\n");
                return -1;
        }

        printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
        
        // Create character device structure
        cdev_init(&dishoungh_cdev, &fops);

        if ((cdev_add(&dishoungh_cdev, dev, 1)) < 0)
        {
                printk(KERN_ERR "Cannot add cdev structure to the system\n");
                unregister_chrdev_region(dev, 1);
                return -1;
        }

        // Create device class and check
        dev_class = class_create("dishoungh_class");
        if (IS_ERR(dev_class))
        {
                printk(KERN_ERR "Cannot create the struct class for device - Dishoungh\n");
                unregister_chrdev_region(dev, 1);
                return -1;
        }

        // Create device node within device class
        node = device_create(dev_class, NULL, dev, NULL, "dishoungh");
        if (IS_ERR(node))
        {
                printk(KERN_ERR "Cannot create the Device\n");
                class_destroy(dev_class);
                unregister_chrdev_region(dev, 1);
                return -1;
        }

        // Allocate kernel memory for buffer
        kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL);

        if (kernel_buffer == 0)
        {
                printk("Cannot allocate memory in kernel\n");
                class_destroy(dev_class);
                unregister_chrdev_region(dev, 1);
                return -1;
        }

        // Setup Timer
        ktime_t ktime = ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC);
        hrtimer_init(&drv_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        drv_hr_timer.function = &timer_callback;

        // Start Timer
        hrtimer_start(&drv_hr_timer, ktime, HRTIMER_MODE_REL);

        printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
        
        return 0;
}

/*
** Module Exit Function
*/
static void __exit my_driver_exit(void)
{
        hrtimer_cancel(&drv_hr_timer);
        kfree(kernel_buffer);
        device_destroy(dev_class, dev);
        class_destroy(dev_class);
        cdev_del(&dishoungh_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}
 
module_init(my_driver_init);
module_exit(my_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dishoungh White II <dishounghwhiteii@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.5");
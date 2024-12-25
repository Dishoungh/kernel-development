#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

#include "sh1106.h"

// SPI Slave Device
static struct spi_device* sh1106_device;

struct spi_board_info sh1106_spi_info =
{
        .modalias       = "sh1106-spi-driver",
        .max_speed_hz   = 1000000,              // 1 MHz
        .bus_num        = SPI_BUS_NUM,          // SPI 0
        .chip_select    = 0,
        .mode           = SPI_MODE_2            // CPOL = 1, CPHA = 0
};

// SPI Write Function
int SH1106_SPI_Write(uint8_t data)
{
        int ret = -1;
        uint8_t rx = 0x00;

        if (sh1106_device)
        {
                struct spi_transfer tr =
                {
                        .tx_buf = &data,
                        .rx_buf = &rx,
                        .len    = 1,
                };

                spi_sync_transfer(sh1106_device, &tr, 1);

                ret = 0;
        }
        else
        {
                printk(KERN_ERR "Error: Cannot write due to invalid SPI device pointer\n");
        }

        return ret;
}

// Init Function
static int __init sh1106_spi_init(void)
{
        // Create SPI Master
        struct spi_master* master = spi_busnum_to_master(sh1106_spi_info.bus_num);

        if (!master)
        {
                printk(KERN_ERR "SPI Master Not Found\n");
                return -ENODEV;
        }

        // Create SPI Slave
        sh1106_device = spi_new_device(master, &sh1106_spi_info);

        if (!sh1106_device)
        {
                printk(KERN_ERR "Failed to Create SPI Slave Device\n");
                return -ENODEV;
        }

        // Configure SPI
        sh1106_device->bits_per_word = 8;

        if (spi_setup(sh1106_device))
        {
                printk(KERN_ERR "Failed to setup SPI for SH1106\n");
                spi_unregister_device(sh1106_device);
                return -ENODEV;
        }

        // Initialize the Display
        SH1106_DisplayInit();
        SH1106_SetBrightness(0xFF); // Full Brightness
        SH1106_InvertDisplay(false);
        SH1106_StartScrollHorizontal(true, 0, 2);
        SH1106_SetCursor(0, 0);

        // Write String
        SH1106_String("Dishoungh's\nSPI\nDriver\n");

        msleep(20000);

        SH1106_ClearDisplay();
        SH1106_DeactivateScroll(); 

        printk(KERN_INFO "SPI Driver Registered\n");

        return 0;
}

// Exit Function
static void __exit sh1106_spi_exit(void)
{
        if (sh1106_device)
        {
                SH1106_ClearDisplay();
                SH1106_DisplayDeinit();

                spi_unregister_device(sh1106_device);

                printk(KERN_INFO "SPI Driver Unregistered\n");
        }
        else
        {
                printk(KERN_ERR "No SPI Driver Registered\n");
        }
}

module_init(sh1106_spi_init);
module_exit(sh1106_spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dishoungh White II <dishounghwhiteii@gmail.com>");
MODULE_DESCRIPTION("SH1106 (really SSD1306) SPI Device Driver");
MODULE_VERSION("1.00");
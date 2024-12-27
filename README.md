# <ins>Overview</ins>

This README will document the process of me learning kernel development and modifying the Linux kernel for the Raspberry Pi 5.

**Keep in mind, I'm only using this to document my learning experience as this is the first time I'm dabbling in Linux kernel development. This is NOT a How-To guide.**

# <ins>Getting Started</ins>

First, I forked and added the [Linux kernel for Raspberry Pi](https://github.com/Dishoungh/linux-rpi.git) as a submodule.

Now, install the Raspberry Pi Kernel Header files and Device Tree Compiler (DTC): `sudo apt install raspberrypi-kernel-headers device-tree-compiler`

Just for reference, here is the Raspberry Pi 5 Pinout diagram:

![Raspberry Pi Pinout Diagram](./Images/raspberry-pi-5-gpio-pinout-diagram.png)

# <ins>GPIO</ins>

For this, I want to control Pin 29 (GPIO 5)

Below is the circuit I lazily made on a breadboard.

![Lazy Breadboard Circuit](./Images/Ugly_Circuit_Setup.jpg)

I added the device tree overlay from "Modules/GPIO/led-demo.dts". 

```
dtc -@ -I dts -O dtb -o ./led-demo.dtbo ./led-demo.dts
sudo dtoverlay ./led-demo.dtbo
dtoverlay -l
```

Since this is my first Linux kernel driver for GPIO, it sucks. I won't worry too much about it. Honestly, I'm just doing this for learning purposes. It's way better to just export/unexport GPIO device files in `/sys/class/gpio/gpiochip571`.

How it works is that GPIO(X) is an offset of GPIO Chip 571. If you check the label for `/sys/class/gpio/gpiochip571/label`, you should get "pinctrl-rp1". This should describe the 40-pin GPIO header.

You can tell by reading the kernel debug files: `cat /sys/kernel/debug/gpio`

For example, GPIO5 should correspond to GPIO ID 571 + 5 = 576. To export that GPIO pin:

`echo 576 > /sys/class/gpio/export`

This should now create `/sys/class/gpio/gpio576`. In this directory, there should be a direction and value file. To set GPIO as input, `echo in > direction`. To set GPIO as output, `echo out > direction`. To set GPIO high, `echo 1 > value`. To set GPIO low, `echo 0 > value`.

# <ins>SPI</ins>

For the SPI, I honestly just want to see something on my oscilloscope. Unfortunately, I'm too lazy to set up something that will act as a slave device for my Raspberry Pi.

# <ins>I2C</ins>

The device I will be interacting with for I2C is the [CCS811](https://cdn.sparkfun.com/assets/2/c/c/6/5/CN04-2019_attachment_CCS811_Datasheet_v1-06.pdf).

**[The rest of this section is TBD]**

# Conclusion

Disappointingly, I wasn't able to bring myself to wanting to figure out the SH1106 or create a I2C driver on my own. Regardless, I still learned a lot. Unfortunately, I don't have a whole lot of time to mess around with kernel development anymore. But I can always come back here for reference and tackle implmenting a custom SPI and I2C solution with an Arduino Nano acting as the receiving interface device. 

# Helpful References

- [Raspberry Pi 5 Pinout Diagram](https://pinout.ai/raspberry-pi-5)
- [Linux Kernel Source for Raspberry Pi](https://github.com/raspberrypi/linux)
- [Official Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [EmbeTronix Device Driver Tutorials](https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-part-1-introduction/)
- [GPIO Drivers on RPI5](https://emlogic.no/2024/09/linux-drivers-getting-started-with-gpio-on-raspberry-pi-5/)
- [SH1106 Datasheet](https://www.displayfuture.com/Display/datasheet/controller/SH1106.pdf)
- [How to Make a Custom SPI Driver](https://mkmints.wixsite.com/embtech/post/linux-spi-driver-tutorial-building-a-custom-spi-device)
- [CCS811 I2C Driver](https://github.com/sss22213/linux_driver_for_AMS_ccs811/tree/master)
- [Johannes4Linux Kernel Tutorials](https://github.com/Johannes4Linux/Linux_Driver_Tutorial_legacy)

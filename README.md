# <ins>Overview</ins>

This README will document the process of me learning kernel development and modifying the Linux kernel for the Raspberry Pi 5.

# <ins>Getting Started</ins>

First, I forked and added the [Linux kernel for Raspberry Pi](https://github.com/Dishoungh/linux-rpi.git) as a submodule.

Now, install the Raspberry Pi Kernel Header files and Device Tree Compiler (DTC): `sudo apt install raspberrypi-kernel-headers device-tree-compiler`

Just for reference, here is the Raspberry Pi 5 Pinout diagram:

![Raspberry Pi Pinout Diagram](./Images/raspberry-pi-5-gpio-pinout-diagram.png)

# <ins>GPIO</ins>

For this, I want to control Pin 11 (GPIO 17)

Below is the circuit I lazily made on a breadboard, using 1 LED and a 2K resistor.

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

I will play with the SPI interface using the [SSD1306 OLED display](https://community.microcenter.com/kb/articles/795-inland-1-3-128x64-oled-graphic-display) I got from MicroCenter.

I connected the Raspberry Pi to the SSD1306 in the following manner (refer to the Pinout diagram above):

- GND  --> Pin 9 (Ground)
- Vcc  --> Pin 1 (3.3V)
- CLK  --> Pin 23 (SCLK)
- MOSI --> Pin 19 (MOSI)
- RES  --> Pin 18 (GPIO 24)
- DC   --> Pin 16 (GPIO 23)
- CS   --> Pin 24 (CE0)

Looking into the [RPI5 device tree](https://github.com/raspberrypi/linux/blob/rpi-6.6.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts), it seems like I'll be modifying `&spi0`.



# <ins>I2C</ins>

[TBD]

## Industrial I/O

[TBD]

# Helpful References

- [Raspberry Pi 5 Pinout Diagram](https://pinout.ai/raspberry-pi-5)
- [Linux Kernel Source for Raspberry Pi](https://github.com/raspberrypi/linux)
- [Official Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [EmbeTronix Device Driver Tutorials](https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-part-1-introduction/)
- [GPIO Drivers on RPI5](https://emlogic.no/2024/09/linux-drivers-getting-started-with-gpio-on-raspberry-pi-5/)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
# <ins>Overview</ins>

This README will document the process of me learning kernel development and modifying the Linux kernel for the Raspberry Pi 5.

# <ins>Getting Started</ins>

First, I forked and added the [Linux kernel for Raspberry Pi](https://github.com/Dishoungh/linux-rpi.git) as a submodule.

Now, install the Raspberry Pi Kernel Header files: `sudo apt install raspberrypi-kernel-headers`

# <ins>GPIO</ins>

For this, I want to control GPIO5, GPIO17, and GPIO19 as demonstrated in the image below:

![GPIO Use Pinout Diagram](./Images/RPI5_GPIO.png)

Below is the circuit I lazily made on a breadboard, using 3 LEDs and some resistors.

![Lazy Breadboard Circuit](./Images/Ugly_Circuit_Setup.jpg)

# Helpful References

- [Official Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [EmbeTronix Device Driver Tutorials](https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-part-1-introduction/)

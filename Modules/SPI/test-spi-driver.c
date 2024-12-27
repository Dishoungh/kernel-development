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
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

#define INIT_CMD	0xA5
#define EXIT_CMD	0xFF
#define READ_CMD	0x6D
#define WRITE_CMD	0x97

/* Function Prototypes*/
static int spi_read_raw(struct iio_dev* indio_dev, struct iio_chan_spec const* chan, int* val, int* val2, long mask);
static int spi_write_raw(struct iio_dev* indio_dev, struct iio_chan_spec const* chan, int val, int val2, long mask);
static int test_spi_probe(struct spi_device* client);
static void test_spi_remove(struct spi_device* client);

struct test_spi
{
	struct spi_device* client;
};

static const struct iio_chan_spec test_channels[] = {
	{
		.type = IIO_VOLTAGE,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	}
};

static const struct iio_info test_spi_info = {
	.read_raw  = spi_read_raw,
	.write_raw = spi_write_raw,
};

static struct of_device_id test_spi_ids[] = {
	{ 
		.compatible = "dishoungh,test-spi",
	},
	{
		/* sentinel */
	}
};

MODULE_DEVICE_TABLE(of, test_spi_ids);

static struct spi_device_id test_spi[] = {
	{"test_spi", 0},
	{ },
};

MODULE_DEVICE_TABLE(spi, test_spi);

static struct spi_driver test_driver = {
	.probe = test_spi_probe,
	.remove = test_spi_remove,
	.id_table = test_spi,
	.driver = {
		.name = "test_spi",
		.of_match_table = test_spi_ids,
	},
};

/* SPI Read Function */
static int spi_read_raw(struct iio_dev* indio_dev, struct iio_chan_spec const* chan, int* val, int* val2, long mask)
{
	struct test_spi* spi_ptr = iio_priv(indio_dev);

	if (mask == IIO_CHAN_INFO_RAW)
	{
		ssize_t code = spi_w8r16(spi_ptr->client, READ_CMD);

		if (code < 0)
		{
			printk(KERN_ERR "Error Reading SPI Value\n");
			return (int)code;
		}
		else
		{
			*val = code;
			printk(KERN_INFO "Read Value = %d\n", code);
		}
	}
	else
	{
		return -EINVAL;
	}

	return IIO_VAL_INT;
}

/* SPI Write Function */
static int spi_write_raw(struct iio_dev* indio_dev, struct iio_chan_spec const* chan, int val, int val2, long mask)
{
	struct test_spi* spi_ptr = iio_priv(indio_dev);

	if (mask == IIO_CHAN_INFO_RAW)
	{
		u8* tmp = (u8*) kmalloc(1 * sizeof(u8), GFP_KERNEL);

		if (!tmp)
		{
			printk(KERN_ERR "SPI Write: Out of Memory!\n");
			return -ENOMEM;
		}

		*(tmp + 0) = WRITE_CMD;
 
		int ret = spi_write(spi_ptr->client, tmp, 1);

		kfree(tmp);

		if (ret < 0)
		{
			printk(KERN_ERR "Failed to send 0x%02X through SPI\n", WRITE_CMD);
			return ret;
		}
		else
		{
			printk(KERN_INFO "Wrote 0x%02X through SPI\n", WRITE_CMD);
		}
	}
	else
	{
		return -EINVAL;
	}

	return 0;
}

/* Probe Function */
static int test_spi_probe(struct spi_device* client)
{
	struct iio_dev* indio_dev = NULL;
	struct test_spi* test_ptr = NULL;
	int ret			  = 0;

	printk(KERN_INFO "In SPI Probe Function\n");
	
	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(struct iio_dev));
	if (!indio_dev)
	{
		printk(KERN_ERR "Probe Error: Out of Memory!\n");
		return -ENOMEM;
	}

	test_ptr = iio_priv(indio_dev);
	test_ptr->client = client;

	indio_dev->name = "test_spi";
	indio_dev->info = &test_spi_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = test_channels;
	indio_dev->num_channels = ARRAY_SIZE(test_channels);

	ret = spi_setup(test_ptr->client);
	if (ret < 0)
	{
		printk(KERN_ERR "Failed to setup the SPI Bus\n");
		return ret;
	}

	u8* tmp = kmalloc(1 * sizeof(u8), GFP_KERNEL);

	if (!tmp)
	{
		printk(KERN_ERR "SPI Probe: Out of Memory!\n");
		return -ENOMEM;
	}

	*(tmp + 0) = INIT_CMD;

	ret = spi_write(test_ptr->client, tmp, 1);

	kfree(tmp);

	if (ret < 0)
	{
		printk(KERN_ERR "SPI Probe: Could not write 0x%02X\n", INIT_CMD);
	}

	spi_set_drvdata(client, indio_dev);

	return devm_iio_device_register(&client->dev, indio_dev);
}

static void test_spi_remove(struct spi_device* client)
{
	struct iio_dev* indio_dev = NULL;
	struct test_spi* test_ptr = NULL;

	printk(KERN_INFO "In SPI Exit Function\n");
	
	u8* tmp = kmalloc(1 * sizeof(u8), GFP_KERNEL);

	if (!tmp)
	{
		printk(KERN_ERR "SPI Probe: Out of Memory!\n");
		return;
	}

	*(tmp + 0) = EXIT_CMD;
	indio_dev = spi_get_drvdata(client);
	test_ptr  = iio_priv(indio_dev);

	spi_write(test_ptr->client, tmp, 1);

	kfree(tmp);
}

module_spi_driver(test_driver);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dishoungh White II <dishounghwhiteii@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - SPI Driver");
MODULE_VERSION("1.1");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


#define DEVICE_NAME		"led_driver"

#define GPIO_CONFIG_PIN(pin, mode) 	s3c2410_gpio_cfgpin(pin, mode)
#define GPIO_SET_PIN(pin, val) 		s3c2410_gpio_setpin(pin, val)

static struct class *led_class;
static struct class_device *led_class_device;

static int led_open(struct inode *inode, struct file *file)
{
	printk("led driver: open\r\n");

	GPIO_CONFIG_PIN(S3C2410_GPF4, S3C2410_GPF4_OUTP);
	GPIO_SET_PIN(S3C2410_GPF4, 0);

	GPIO_CONFIG_PIN(S3C2410_GPF5, S3C2410_GPF5_OUTP);
	GPIO_SET_PIN(S3C2410_GPF5, 0);

	GPIO_CONFIG_PIN(S3C2410_GPF6, S3C2410_GPF6_OUTP);
	GPIO_SET_PIN(S3C2410_GPF6, 0);

	return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	char val;

	copy_from_user(&val, buf, 1);

	GPIO_SET_PIN(S3C2410_GPF4, (val & 0x1));
	GPIO_SET_PIN(S3C2410_GPF5, (val & 0x1));
	GPIO_SET_PIN(S3C2410_GPF6, (val & 0x1));

	return 1;
}

static struct file_operations leds_fops = {
	.owner 	= THIS_MODULE,
	.open 	= led_open,
	.write 	= led_write,
};

int major;
static int __init led_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &leds_fops);

	led_class = class_create(THIS_MODULE, "led_driver");
	if (IS_ERR(led_class))
		return PTR_ERR(led_class);

	led_class_device = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "led");
	if (unlikely(IS_ERR(led_class_device)))
			return PTR_ERR(led_class_device);

	printk("led driver: init\r\n");

	return 0;
}

static void __exit led_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	class_device_unregister(led_class_device);
	class_destroy(led_class);
	printk("led drive: exit\r\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");

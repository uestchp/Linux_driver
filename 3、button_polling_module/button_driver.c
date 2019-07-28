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


#define DEVICE_NAME		"button_driver"

#define GPIO_CONFIG_PIN(pin, mode) 	s3c2410_gpio_cfgpin(pin, mode)
#define GPIO_SET_PIN(pin, val) 		s3c2410_gpio_setpin(pin, val)
#define GPIO_GET_PIN(pin)			s3c2410_gpio_getpin(pin)

static struct class *button_class;
static struct class_device *button_class_device;

static int button_open(struct inode *inode, struct file *file)
{
	printk("button driver: open\r\n");

	GPIO_CONFIG_PIN(S3C2410_GPF0, S3C2410_GPF0_INP);

	return 0;
}

static ssize_t button_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned char key_val[1];

	key_val[0] = GPIO_GET_PIN(S3C2410_GPF0);

	copy_to_user(buf, key_val, sizeof(key_val));

	return sizeof(key_val);
}


static struct file_operations buttons_fops = {
	.owner 	= THIS_MODULE,
	.open 	= button_open,
	.read	= button_read,
};

int major;
static int __init button_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &buttons_fops);

	button_class = class_create(THIS_MODULE, "button_driver");
	if (IS_ERR(button_class))
		return PTR_ERR(button_class);

	button_class_device = class_device_create(button_class, NULL, MKDEV(major, 0), NULL, "button");
	if (unlikely(IS_ERR(button_class_device)))
			return PTR_ERR(button_class_device);

	printk("button driver: init\r\n");

	return 0;
}

static void __exit button_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	class_device_unregister(button_class_device);
	class_destroy(button_class);
	printk("button drive: exit\r\n");
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");

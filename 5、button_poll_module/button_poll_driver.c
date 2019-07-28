#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/poll.h>

#define DEVICE_NAME		"button_driver"

#define GPIO_CONFIG_PIN(pin, mode) 	s3c2410_gpio_cfgpin(pin, mode)
#define GPIO_SET_PIN(pin, val) 		s3c2410_gpio_setpin(pin, val)
#define GPIO_GET_PIN(pin)		s3c2410_gpio_getpin(pin)

static DECLARE_WAIT_QUEUE_HEAD(button_wait_queue);

static unsigned int key_val;
static struct class *button_class;
static struct class_device *button_class_device;

static int ev_press = 0;

struct pin_desc {
	unsigned int pin;
	unsigned int key_val;
};

struct pin_desc pins_desc[4] = {
	{S3C2410_GPF0,0xF0},
}; 

static irqreturn_t button_irq(int irq, void *dev_id)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_id;
	unsigned int pin_val;
	
	pin_val = GPIO_GET_PIN(pindesc->pin);

	if(pin_val)
		key_val = 0x08 | pindesc->key_val;
	else
		key_val = pindesc->key_val;
	
	ev_press = 1;
	wake_up_interruptible(&button_wait_queue);

	return IRQ_RETVAL(IRQ_HANDLED);	
}

static int button_open(struct inode *inode, struct file *file)
{
	printk("button irq driver: open\r\n");

	request_irq(IRQ_EINT0, button_irq, IRQT_BOTHEDGE, "KEY1", &pins_desc[0]);	

	return 0;
}

static ssize_t button_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	wait_event_interruptible(button_wait_queue, ev_press);

	copy_to_user(buf, &key_val, 1);
	
	ev_press = 0;

	return sizeof(key_val);
}

static int button_close(struct inode *inode, struct file *file)
{
	free_irq(IRQ_EINT0, &pins_desc[0]);
	return 0;
}

static unsigned button_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_wait_queue, wait);

	if(ev_press)
		mask |= POLLIN | POLLRDNORM;
	return mask;	
}

static struct file_operations buttons_fops = {
	.owner 	 = THIS_MODULE,
	.open 	 = button_open,
	.read	 = button_read,
	.release = button_close,
	.poll	 = button_poll,
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

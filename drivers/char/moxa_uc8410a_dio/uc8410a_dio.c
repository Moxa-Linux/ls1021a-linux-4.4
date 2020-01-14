#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>

/* IOCTL behavior */
#define IOCTL_SET_DOUT      15
#define IOCTL_GET_DOUT      16
#define IOCTL_GET_DIN       17

/* DI/DO addresses */
#define DI_ADDR 0x78000010
#define DO_ADDR 0x78000020

/* DI/DO size (byte) */
#define DI_SIZE     1
#define DO_SIZE     1
#define DI_NUM      4
#define DO_NUM      4
#define DIO_HIGH    1
#define DIO_LOW     0

#define MOXA_DIO_MINOR  104
#define MOXA_DIO_NAME   "dio"

volatile unsigned long *DI_REG = NULL;
volatile unsigned long *DO_REG = NULL;

struct dio_set_struct {
	int io_number;
	int mode_data;// 1 for input, 0 for output, 1 for high, 0 for low
};

static int dio_open(struct inode *inode, struct file *filp)
{
	pr_debug("Moxa DIO: opened\n");
	return 0;
}

static int dio_close(struct inode *inode, struct file *filp)
{
	pr_debug("Moxa DIO: closed\n");
	return 0;
}

// args are pointed to user space's buffer
static long dio_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	struct dio_set_struct dio_set;

	if (copy_from_user(&dio_set, (struct dio_set_struct *)args, sizeof(struct dio_set_struct)))
		return -EFAULT;

	switch (cmd){
	case IOCTL_SET_DOUT :
		if (dio_set.io_number < 0 || dio_set.io_number >= DO_NUM)
			return -EFAULT;

		if (dio_set.mode_data == DIO_HIGH)
			iowrite8(ioread8(DO_REG)|(1<<(7-dio_set.io_number)), DO_REG);
		else if (dio_set.mode_data == DIO_LOW)
			iowrite8(ioread8(DO_REG)&(~(1<<(7-dio_set.io_number))), DO_REG);

		pr_debug("Moxa DIO: set DOUT to = %d\n", dio_set.mode_data);
		break;

	case IOCTL_GET_DOUT :
		if (dio_set.io_number < 0 || dio_set.io_number >= DO_NUM)
			return -EFAULT;

		if ((ioread8(DO_REG)>>4) & (1<<(3-dio_set.io_number)))
			dio_set.mode_data = 1;
		else
			dio_set.mode_data = 0;

		if (copy_to_user((struct dio_set_struct *)args, &dio_set, sizeof(struct dio_set_struct)))
			return -EFAULT;

		pr_debug("Moxa DIO: get DOUT from user = %d\n", dio_set.mode_data);
		break;

	case IOCTL_GET_DIN :
		pr_debug("Moxa DIO: get DIN from user = %d\n", dio_set.io_number);
		if (dio_set.io_number < 0 || dio_set.io_number >= DI_NUM)
			return -EFAULT;

		if ((ioread8(DI_REG)>>4) & (1<<(3-dio_set.io_number)))
			dio_set.mode_data = 1;
		else
			dio_set.mode_data = 0;

		if (copy_to_user((struct dio_set_struct *)args, &dio_set, sizeof(struct dio_set_struct)))
			return -EFAULT;

		pr_debug("Moxa DIO: get DIN from user = %d\n", dio_set.mode_data);
		break;

	default :
		pr_debug("Moxa DIO: ioctl:error\n");
		return -ENOTTY;
	}

	return 0;
}

struct file_operations dio_fops = {
	.owner = THIS_MODULE,
	.open = dio_open,
	.release = dio_close,
	.unlocked_ioctl = dio_ioctl,
};

static struct miscdevice dio_miscdev = {
	.minor = MOXA_DIO_MINOR,
	.name = MOXA_DIO_NAME,
	.fops = &dio_fops,
};

static int __init dio_module_init(void)
{
	pr_info("Moxa DIO: Initializing module\n");

	if ( misc_register(&dio_miscdev)!=0 ) {
		pr_err("Moxa DIO: Register misc failed!\n");
		return -ENOMEM;
	}

	DI_REG = ioremap_nocache(DI_ADDR, DI_SIZE);
	DO_REG = ioremap_nocache(DO_ADDR, DO_SIZE);

	return 0;
}

static void __exit dio_module_exit(void)
{
	pr_info("Moxa DIO: Cleaning up module\n");

	iounmap(DI_REG);
	iounmap(DO_REG);

	misc_deregister(&dio_miscdev);

	return;
}

module_init(dio_module_init);
module_exit(dio_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alif_Chen@moxa.com");
MODULE_DESCRIPTION("Moxa UC-8410A DIO module");

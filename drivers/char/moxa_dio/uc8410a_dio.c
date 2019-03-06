#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include "moxa_dio.h"

/* DI/DO addresses */
#define	DI_ADDR	0x78000010
#define	DO_ADDR	0x78000020

/* DI/DO size (byte) */
#define	DI_SIZE		1
#define DO_SIZE		1
#define	DI_NUM		4
#define	DO_NUM		4
#define DIO_HIGH	1
#define DIO_LOW		0

#define MOXA_DIO_MINOR	104
#define MOXA_DIO_NAME	"dio"

volatile unsigned long *DI_REG=NULL;
volatile unsigned long *DO_REG=NULL;

static int dio_open (struct inode *inode, struct file *filp)
{
	pr_debug("dio open\n");
	return 0;
}

static int dio_release (struct inode *inode, struct file *filp)
{
	pr_debug("dio release\n");
	return 0;
}

ssize_t dio_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int i;

	u8 di_status = ioread8(DI_REG);
	u8 do_status = ioread8(DO_REG);
	char di_stack_buf[DI_NUM+1];
	char do_stack_buf[DO_NUM+1];
	
	u8 bit_mask = 0x01;
	
	memset(di_stack_buf, '0', DI_NUM*sizeof(char));
	memset(do_stack_buf, '0', DO_NUM*sizeof(char));
	di_stack_buf[DI_NUM] = '\0';
	do_stack_buf[DO_NUM] = '\0';

	for (i = 0 ; i<DI_NUM ; i++) {
		if (((di_status>>4) & (bit_mask<<i)))
			di_stack_buf[i] = '1';
	}
	
	bit_mask = 0x01;
	for (i = 0 ; i<DO_NUM ; i++) {
		if (((do_status>>4) & (bit_mask<<i)))
			do_stack_buf[i] = '1';
	}
	
	return 0;
}

static long dio_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct dio_set_struct set;
	u8 bit_mask = 0x01;

	/* check data */
	if (copy_from_user(&set, (struct dio_set_struct *)arg, sizeof(struct dio_set_struct)))
		return -EFAULT;
	
	switch (cmd) {
		case IOCTL_SET_DOUT :
			if (set.io_number < 0 || set.io_number >= DO_NUM)
				return -EFAULT;

			if (set.mode_data == DIO_HIGH){
				iowrite8(ioread8(DO_REG)|(1<<(7-set.io_number)), DO_REG);
			
			}else if (set.mode_data == DIO_LOW){
				iowrite8(ioread8(DO_REG)&(~(1<<(7-set.io_number))), DO_REG);
			}

			break;
		case IOCTL_GET_DOUT :
			if (set.io_number < 0 || set.io_number >= DO_NUM)
				return -EFAULT;
			
			if ((ioread8(DO_REG)>>4) & (bit_mask<<(3-set.io_number)))
				set.mode_data = 1;
			else
				set.mode_data = 0;

			if (copy_to_user((struct dio_set_struct *)arg, &set, sizeof(struct dio_set_struct)))
				return -EFAULT;

			break;

		case IOCTL_GET_DIN :
			if (set.io_number < 0 || set.io_number >= DI_NUM)
				return -EFAULT;
			
			if ((ioread8(DI_REG)>>4) & (bit_mask<<(3-set.io_number)))
				set.mode_data = 1;
			else
				set.mode_data = 0;

			if (copy_to_user((struct dio_set_struct *)arg, &set, sizeof(struct dio_set_struct)))
				return -EFAULT;

			break;

		default :
			pr_err("ioctl:error\n");
			return -EINVAL;
	}

	return 0;
}

static struct file_operations dio_fops = {
	.owner		=	THIS_MODULE,
	.unlocked_ioctl	=	dio_ioctl,
	.read		=	dio_read,
	.open		=	dio_open,
	.release	=	dio_release,

};

static struct miscdevice dio_miscdev = {
	.minor		=	MOXA_DIO_MINOR,
	.name		=	MOXA_DIO_NAME,
	.fops		=	&dio_fops,
};


static int __init dio_init(void)
{
	pr_info("Initializing MOXA dio module\n");
	
	if ( misc_register(&dio_miscdev)!=0 ) {
		pr_err("MOXA dio driver: Register misc failed!\n");
		return -ENOMEM;
	}
	
	DI_REG = ioremap_nocache(DI_ADDR, DI_SIZE);

	DO_REG = ioremap_nocache(DO_ADDR, DO_SIZE);
	
	return 0;
}

static void __exit dio_exit(void)
{
	pr_info("MOXA dio driver: Cleaning up module\n");
	iounmap(DI_REG);
	iounmap(DO_REG);
	misc_deregister(&dio_miscdev);
	return;
}




module_init(dio_init);
module_exit(dio_exit);
MODULE_AUTHOR("HarryYJ.Jhou@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA DIO module");

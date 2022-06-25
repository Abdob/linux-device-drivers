#include <linux/version.h>
#include <generated/utsrelease.h> // kernel version > 2.6
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>		
#include <asm/uaccess.h>
#include <linux/uaccess.h> // kernel >= 4.12


MODULE_LICENSE("GPL"); 
#define MOUSE		"mouse"
#define MOUSEMAJOR	32
#define MOUSEMINOR	0	
#define MOUSENUMDEVS	6	


unsigned int counter = 0;
struct cdev cdev;
dev_t  devno;
char mouse_buf[4096];
char *mouse_storage=mouse_buf;

static int mouse_open (struct inode *inode, struct file *file)
{
	return 0;
}

static int mouse_release (struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t mouse_read (struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int len, err;
	if( counter <= 0 ) return 0;
	err = copy_to_user(buf,mouse_storage,counter);

	if (err != 0) return -EFAULT;

	len = counter;
	counter = 0;
	return len;
}


static ssize_t mouse_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int err;
	err = copy_from_user(mouse_storage,buf,count);
	if (err != 0) return -EFAULT;
	
	// typically, my device access would be here.  
	counter += count; 
	return count;
}

static struct file_operations mouse_fops =
{
	// for LINUX_VERSION_CODE 2.4.0 and later 
	owner:		THIS_MODULE, 	// struct module *owner
	open:			mouse_open, 		// open method 
	read:   	mouse_read,			// read method 
	write:  	mouse_write, 		// write method 
	release:	mouse_release 	// release method .. for close() system call
};


static int mouse_init(void)
{
	int i;
	devno = MKDEV(MOUSEMAJOR, MOUSEMINOR);

	i = register_chrdev_region(devno,MOUSENUMDEVS,MOUSE);
	if (i < 0) { printk(KERN_ALERT "Error (%d) adding mouse", i); return i;}

	cdev_init(&cdev, NULL);

	//  Step 2b of 2:  register device with kernel
    cdev.owner = THIS_MODULE;
    cdev.ops = &mouse_fops;
    i = cdev_add(&cdev, devno, MOUSENUMDEVS);
    if (i) { printk(KERN_ALERT "Error (%d) adding mouse", i); return i; }

	return 0;
}

static void mouse_exit(void)
{
	//  Step 1 of 2:  unregister device with kernel
	cdev_del(&cdev);

	//  Step 2 of 2:  Release request/reserve of Major Number from Kernel
	unregister_chrdev_region(devno, MOUSENUMDEVS);
}

module_init(mouse_init);
module_exit(mouse_exit);

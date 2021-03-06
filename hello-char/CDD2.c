// Example# 3.1b:  Simple Char Driver with Statically allocated Major#  
//                works only in 2.6  (Similar .. to Example# 3.1a)


//	using dev_t, struct cdev (2.6)

#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
#include <generated/utsrelease.h>
#else
#include <linux/utsrelease.h>
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>		// 2.6
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
#include <asm/uaccess.h>
#else
#include <linux/uaccess.h>
#endif


MODULE_LICENSE("GPL");   	//  Kernel isn't tainted .. but doesn't 
			 	//  it doesn't matter for SUSE anyways :-(  

#define CDD		"CDD2"

#define CDDMAJOR	32
#define CDDMINOR	0	// 2.6
#define CDDNUMDEVS	6	// 2.6


unsigned int counter = 0;
struct cdev cdev;
dev_t  devno;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
char CDD_buf[4096];
char *CDD_storage=CDD_buf;
#else
char CDD_storage[4096];
#endif

static int CDD_open (struct inode *inode, struct file *file)
{
	// MOD_INC_USE_COUNT;
	return 0;
}

static int CDD_release (struct inode *inode, struct file *file)
{
	// MOD_DEC_USE_COUNT;
	return 0;
}

static ssize_t CDD_read (struct file *file, char *buf, 
size_t count, loff_t *ppos)
{
	int len, err;
	if( counter <= 0 ) return 0;

	// typically, my device access would be here.  
	err = copy_to_user(buf,CDD_storage,counter);

	if (err != 0) return -EFAULT;

	len = counter;
	counter = 0;
	return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
static ssize_t CDD_write (struct file *file, const char __user *buf,
size_t count, loff_t *ppos)
#else
static ssize_t CDD_write (struct file *file, const char *buf, 
size_t count, loff_t *ppos)
#endif
{
	int err;

	err = copy_from_user(CDD_storage,buf,count);
	if (err != 0) return -EFAULT;
	
	// typically, my device access would be here.  
	counter += count; 
	return count;
}

static struct file_operations CDD_fops =
{
	// for LINUX_VERSION_CODE 2.4.0 and later 
	owner:		THIS_MODULE, 	// struct module *owner
	open:			CDD_open, 		// open method 
	read:   	CDD_read,			// read method 
	write:  	CDD_write, 		// write method 
	release:	CDD_release 	// release method .. for close() system call
};


//    #define CDD		"CDD2"

// 		#define CDDMAJOR	  32
//		#define CDDMINOR	  0	
//    #define CDDNUMDEVS	6


static int CDD_init(void)
{
	int i;

	//  Step 1a of 2:  create/populate device numbers
  //  dev_t  devno;
  //
  /*
 		* from #include/linux/kdev_t.h
 		
			#include <uapi/linux/kdev_t.h>
			#define MINORBITS	20
			#define MINORMASK((1U << MINORBITS) - 1)
			#define MAJOR(dev)((unsigned int) ((dev) >> MINORBITS))
			#define MINOR(dev)((unsigned int) ((dev) & MINORMASK))
			#define MKDEV(ma,mi) (((ma) << MINORBITS) | (mi))
		*/
  //            32        0
	devno = MKDEV(CDDMAJOR, CDDMINOR);

	//  Step 1b of 2:  request/reserve Major Number from Kernel
  // #define CDDNUMDEVS	6	// 2.6
  //                         32,0  6 .. 0-5
	i = register_chrdev_region(devno,CDDNUMDEVS,CDD);
	if (i < 0) { printk(KERN_ALERT "Error (%d) adding CDD", i); return i;}

	//  Step 2a of 2:  initialize cdev struct
		// cdev_init(&cdev, &CDD_fops);
		cdev_init(&cdev, NULL);

	//  Step 2b of 2:  register device with kernel
    cdev.owner = THIS_MODULE;
    cdev.ops = &CDD_fops;
    i = cdev_add(&cdev, devno, CDDNUMDEVS);
    if (i) { printk(KERN_ALERT "Error (%d) adding CDD", i); return i; }

	return 0;
}

static void CDD_exit(void)
{
	//  Step 1 of 2:  unregister device with kernel
	cdev_del(&cdev);

	//  Step 2 of 2:  Release request/reserve of Major Number from Kernel
	unregister_chrdev_region(devno, CDDNUMDEVS);
}

module_init(CDD_init);
module_exit(CDD_exit);

// See line 1696 fops
// vi +1696 /usr/src/linux/include/linux/fs.h 
//
// line 859 struct file
// vi +859 /usr/src/linux/include/linux/fs.h 
//
// See 
// vi /usr/include/asm-generic/errno-base.h
// vi /usr/include/asm-generic/errno.h

//字符设备驱动结构
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "cdevdemo"    //设备节点（设备文件）名字的定义


#define CDEVDEMO_MAJOR 255  /*预设cdevdemo的主设备号*/  
static int cdevdemo_major = CDEVDEMO_MAJOR;


/*
设备结构体,此结构体可以封装设备相关的一些信息等 
信号量等也可以封装在此结构中，后续的设备模块一般都 
应该封装一个这样的结构体，但此结构体中必须包含某些 
成员，对于字符设备来说，我们必须包含struct cdev cdev  */  
struct cdevdemo_dev
{
	/*	linux/cdev.h */
    struct cdev cdev;
};

struct cdevdemo_dev *cdevdemo_devp; /*设备结构体指针*/

//ioctl函数
static long cdevdemo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case 1:break;
	}
	return 0;
}

/*文件打开函数，上层对此设备调用open时会执行*/  
int cdevdemo_open(struct inode *inode, struct file *filp)     
{  
    printk(KERN_NOTICE "======== cdevdemo_open ");  
    return 0;  
}  
  
/*文件释放，上层对此设备调用close时会执行*/  
int cdevdemo_release(struct inode *inode, struct file *filp)      
{  
    printk(KERN_NOTICE "======== cdevdemo_release ");     
    return 0;  
}  
  
/*文件的读操作，上层对此设备调用read时会执行*/  
static ssize_t cdevdemo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)  
{  
    printk(KERN_NOTICE "======== cdevdemo_read ");    
}  

/* 文件操作结构体，文中已经讲过这个结构*/  
static const struct file_operations cdevdemo_fops =  
{  
    .owner = THIS_MODULE,  
    .open = cdevdemo_open,  
    .release = cdevdemo_release,  
    .read = cdevdemo_read, 
	.unlocked_ioctl = cdevdemo_ioctl
};

/*初始化并注册cdev*/  
static void cdevdemo_setup_cdev(struct cdevdemo_dev *dev, int index)  
{
	int err;

    int devno = MKDEV(cdevdemo_major, index);

    /*初始化一个字符设备，设备所支持的操作在cdevdemo_fops中*/
    cdev_init(&dev->cdev, &cdevdemo_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &cdevdemo_fops;

    err = cdev_add(&dev->cdev, devno, 1);
    if(err)
    {
        printk(KERN_NOTICE "Error %d add cdevdemo %d", err, index);
    }
}

static int __init cdevdemo_init(void)
{
	int ret;
	
	/*	<linux/types.h>
		宏定义 dev_t 1+20
		<linux/cdev.h> -> kdev_t.h 
		#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))
		*/
	dev_t devno = MKDEV(cdevdemo_major, 0);
	
	/*  <linux/fs.h>
		申请设备号，当xxx_major不为0时，表示静态指定；当为0时，表示动态申请
		静态申请设备号
		在 /linux/Documentation/devices.txt 确认空设备号
		ret = register_chrdev_region(dev_t from, unsigned count, const char *name);
		from：希望申请使用的设备号
		count：希望申请使用设备号数目
		name：设备名(在/proc/devices)   */
	if(cdevdemo_major)
	{
		ret = register_chrdev_region(devno, 1, DEVICE_NAME);
	}
	else
	{
		/*	int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, 
									unsigned int count, const char *name); */
		ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
		
		/*	<linux/cdev.h> -> kdev_t.h
			#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))*/
		cdevdemo_major = MAJOR(devno);    /*获得申请的主设备号*/
	}
	if(ret < 0)
	{
		printk(KERN_ALERT " -- chrdev_region failed\n\n");
		return ret;
	}

	/*	为设备描述结构分配内存
		<linux/slab.h>*/
	cdevdemo_devp = kmalloc(sizeof(struct cdevdemo_dev), GFP_KERNEL);
	if (!cdevdemo_devp)    /*申请失败*/
	{
		ret = -ENOMEM;
		printk(KERN_NOTICE "Error add cdevdemo");
		goto fail_malloc;
	}
	memset(cdevdemo_devp, 0, sizeof(struct cdevdemo_dev));

	cdevdemo_setup_cdev(cdevdemo_devp, 0);
	
/*  下面两行是创建了一个总线类型，会在/sys/class下生成cdevdemo目录 
    这里的还有一个主要作用是执行device_create后会在/dev/下自动生成 
    cdevdemo设备节点。而如果不调用此函数，如果想通过设备节点访问设备 
    需要手动mknod来创建设备节点后再访问。*/  
    //cdevdemo_class = class_create(THIS_MODULE, DEVICE_NAME);  
    //device_create(cdevdemo_class, NULL, MKDEV(cdevdemo_major, 0), NULL, DEVICE_NAME);
	
	return 0;
	
	fail_malloc:
		unregister_chrdev_region(devno, 1);
		return ret;
}

static void __exit cdevdemo_exit(void)
{
	cdev_del(&cdevdemo_devp->cdev);  /*注销cdev*/
	/*<linux/slab.h>*/
	kfree(cdevdemo_devp);       /*释放设备结构体内存*/
	unregister_chrdev_region(MKDEV(cdevdemo_major,0), 1);    //释放设备号
}

module_param(cdevdemo_major, int, S_IRUGO);//?????
module_init(cdevdemo_init);  
module_exit(cdevdemo_exit);
MODULE_LICENSE("GPL");

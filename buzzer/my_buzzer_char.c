#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <linux/pwm.h>


#define DEVICE_NAME		"my_buzzer" //定义设备名

#define CDEVDEMO_MAJOR 255  /*预设cdevdemo的主设备号*/  


#define PWM_1_ON		1
#define PWM_1_OFF		3

//2700hz
#define RERIOD_NS       370000
#define DETY_NS         185000


static int cdevdemo_major = CDEVDEMO_MAJOR;

struct cdevdemo_dev
{
	/*	linux/cdev.h */
    struct cdev cdev;
};

struct cdevdemo_dev *cdevdemo_devp; /*设备结构体指针*/

struct pwm_device *pwm_dev_1 = NULL;


//定义信号量，用于并发控制。lock用于互斥，因此该驱动程序只能同时有一个进程使用
//static struct semaphore lock;

//static int pwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static long pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	printk(KERN_ALERT "cmd: %d  arg:%ld\n\n",cmd,arg);
	switch(cmd)
	{
		case PWM_1_ON :
			ret = pwm_config(pwm_dev_1, DETY_NS, RERIOD_NS);
			if(ret < 0){
				printk(KERN_ALERT " -- pwm_ioctl PWM_1_ON : fail\n\n");
				return ret;
			}
			pwm_enable(pwm_dev_1);//使能
			printk(KERN_ALERT " -- pwm_ioctl PWM_1_ON : pwm_enable(pwm_dev_1)\n\n");
			break;
		
		case PWM_1_OFF :
			printk(KERN_ALERT " -----------------------------------\n\n");
			ret = pwm_config(pwm_dev_1, 0, RERIOD_NS);
			if(ret < 0){
				printk(KERN_ALERT " -- pwm_ioctl PWM_1_OFF : fail\n\n");
				return ret;
			}
			pwm_disable(pwm_dev_1);//关闭使能
			printk(KERN_ALERT " -- pwm_ioctl PWM_1_OFF : pwm_disable(pwm_dev_1)\n\n");
			break;
		
		default :
			printk(KERN_ALERT " -- pwm_ioctl default\n\n");
			break;
    }
    return 0;
}

//设备的文件操作结构体
static struct file_operations pwm_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = pwm_ioctl,
};

/*初始化并注册cdev*/  
static void cdevdemo_setup_cdev(struct cdevdemo_dev *dev, int index)  
{
	int err;

    int devno = MKDEV(cdevdemo_major, index);

    /*初始化一个字符设备，设备所支持的操作在cdevdemo_fops中*/
    cdev_init(&dev->cdev, &pwm_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &pwm_fops;

    err = cdev_add(&dev->cdev, devno, 1);
    if(err)
    {
        printk(KERN_NOTICE "Error %d add cdevdemo %d", err, index);
    }
}

static int __init pwm_init(void)
{
	int ret;
	dev_t devno = MKDEV(cdevdemo_major, 0);
	
	pwm_dev_1 = pwm_request(0,"pwm_1");		// 申请 pwm 设备
	if(pwm_dev_1 == ERR_PTR(-EBUSY) || pwm_dev_1 == ERR_PTR(-ENOENT)){
		printk(KERN_ALERT " -- pwm_request_1 fail\n\n");
		return (int)pwm_dev_1;
	}
	printk(KERN_ALERT " -- pwm_request_1 success\n\n");
	
	ret = pwm_config(pwm_dev_1, 0, RERIOD_NS);	//配置 pwm 设备
	if(ret < 0){
		printk(KERN_ALERT " -- pwm_config_1 fail\n\n");
		return ret;
	}
	printk(KERN_ALERT " -- pwm_config_1 success\n\n");
	
	ret = pwm_enable(pwm_dev_1);	//开启 pwm 设备(使能)
	if(ret < 0 ){
		printk(KERN_ALERT " -- pwm_enable_1 fail\n\n");
		return ret;
	}
	printk(KERN_ALERT " -- pwm_enable_1 success\n\n");

	
	if(cdevdemo_major)
	{
		ret = register_chrdev_region(devno, 1, DEVICE_NAME);
	}
	else
	{
		ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
		cdevdemo_major = MAJOR(devno);    /*获得申请的主设备号*/
	}
	if(ret < 0)
	{
		printk(KERN_ALERT " -- chrdev_region failed\n\n");
		return ret;
	}
	
	cdevdemo_devp = kmalloc(sizeof(struct cdevdemo_dev), GFP_KERNEL);
	if (!cdevdemo_devp)    /*申请失败*/
	{
		ret = -ENOMEM;
		printk(KERN_NOTICE "Error add cdevdemo");
		goto fail_malloc;
	}
	memset(cdevdemo_devp, 0, sizeof(struct cdevdemo_dev));

	cdevdemo_setup_cdev(cdevdemo_devp, 0);
	
	return 0;
	
		
	fail_malloc:
		unregister_chrdev_region(devno, 1);
		return ret;
}

static void __exit pwm_exit(void)
{
	cdev_del(&cdevdemo_devp->cdev);  /*注销cdev*/
	/*<linux/slab.h>*/
	kfree(cdevdemo_devp);       /*释放设备结构体内存*/
	unregister_chrdev_region(MKDEV(cdevdemo_major,0), 1);    //释放设备号
	
	pwm_config(pwm_dev_1, 0, RERIOD_NS);
	
	pwm_disable(pwm_dev_1);		//关闭 pwm 设备(使能)
	
	pwm_free(pwm_dev_1);	//释放 pwm 设备
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_LICENSE("GPL");



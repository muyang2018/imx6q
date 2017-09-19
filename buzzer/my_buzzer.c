#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/miscdevice.h>
#include <linux/fs.h>

#include <linux/pwm.h>


#define DEVICE_NAME		"my_buzzer" //定义设备名

#define PWM_1_ON		1
#define PWM_1_OFF		3

//2700hz
#define RERIOD_NS       370000
#define DETY_NS         185000

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

//混杂设备的3个必须参数
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,    //动态分配次设备号
	.name = DEVICE_NAME,            //dev/pwm_1_buzzer
	.fops = &pwm_fops,
};

static int __init pwm_init(void)
{
	int ret;
	printk(KERN_ALERT " -- pwm_init\n\n");
	
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
/*
misc_register 函数会自动创建设备节点，即设备文件。
无需 mknod 指令创建设备文件。
因为 misc_register() 会调用class_device_create()或者device_create()
*/
	printk(KERN_ALERT " -- misc_register()\n\n");
	
	ret = misc_register(&misc);		//注册一个 misc 设备
	if(ret < 0){
		printk(KERN_ALERT " -- misc_register falid!\n\n");
		return ret;
	}
	printk(KERN_ALERT " -- misc_register success\n\n");
	
	return 0;
}

static void __exit pwm_exit(void)
{
	printk(KERN_ALERT " -- pwm_exit\n\n");

/*
misc_deregister 函数注销一个混杂设备，同时销毁设备节点
*/
	printk(KERN_ALERT " -- misc_deregister()\n\n");
	misc_deregister(&misc);		//注销 misc 设备
	
	pwm_config(pwm_dev_1, 0, RERIOD_NS);
	
	pwm_disable(pwm_dev_1);		//关闭 pwm 设备(使能)
	
	pwm_free(pwm_dev_1);	//释放 pwm 设备
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_LICENSE("GPL");



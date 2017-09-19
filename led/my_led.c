#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/fs.h>

#include <mach/gpio.h>


#define DEVICE_NAME		"my_led" //定义设备名

//#define LED_ON		_IORW('k',1,unsigned long)
#define LED_ON			1
#define LED_OFF			3

//MX6Q_PAD_GPIO_2__GPIO_1_2
#define	LED_GPIO		IMX_GPIO_NR(1,2)

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	printk(KERN_ALERT " -- cmd: %d  arg: %ld\n\n",cmd,arg);
	switch(cmd)
	{
		case LED_ON :
			printk(KERN_ALERT " -- led_ioctl LED_ON\n\n");
			gpio_set_value(LED_GPIO, 1);
			printk(KERN_ALERT " -- gpio_set_value(unsigned gpio, int value)\n\n");
			break;
			
		case LED_OFF :
			printk(KERN_ALERT " -- led_ioctl LED_OFF\n\n");
			gpio_set_value(LED_GPIO, 0);
			printk(KERN_ALERT " -- gpio_set_value(unsigned gpio, int value)\n\n");
			break;
			
		default :
			printk(KERN_ALERT " -- led_ioctl default\n\n");
			break;
	}
}

//设备的文件操作结构体
static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = led_ioctl,
};

//混杂设备的3个必须参数
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,    //动态分配次设备号
	.name = DEVICE_NAME,            //dev/new_led
	.fops = &led_fops,
};

static int __init led_init(void)
{
	printk(KERN_ALERT " -- led_init\n\n");
	
	int ret;
	
	gpio_free(LED_GPIO);		//释放GPIO设备
	printk(KERN_ALERT " -- gpio_free(unsigned gpio)\n\n");
	
	ret = gpio_request(LED_GPIO, "MY_LED");		//申请GPIO设备
	if(ret < 0){
		printk(KERN_ALERT " -- gpio_request fail\n\n");
		return ret;
	}
	printk(KERN_ALERT " -- gpio_request(unsigned gpio, const char *label)\n\n");
	
	gpio_direction_output(LED_GPIO, 0);			//选择GPIO设备方向
	printk(KERN_ALERT " -- gpio_direction_output(unsigned gpio, int value)\n\n");
	
	gpio_set_value(LED_GPIO, 0);		//设置GPIO设备状态
	printk(KERN_ALERT " -- gpio_set_value(unsigned gpio, int value)\n\n");
	
	
	ret = misc_register(&misc);		//注册一个 misc 设备
	if(ret < 0){
		printk(KERN_ALERT " -- misc_register falid!\n\n");
		return ret;
	}
	printk(KERN_ALERT " -- misc_register()\n\n");
	
	return 0;
}

static void __exit led_exit(void)
{
	printk(KERN_ALERT " -- led_exit\n\n");

	gpio_free(LED_GPIO);		//释放GPIO设备
	printk(KERN_ALERT " -- gpio_free(unsigned gpio)\n\n");
	
	misc_deregister(&misc);		//注销 misc 设备
	printk(KERN_ALERT " -- misc_deregister()\n\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
#include <Linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <plat/regs-timer.h>
#include <mach/regs-irq.h>
#include <asm/mach/time.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>


#define DEVICE_NAME "pwm" //设备名


#define PWM_IOCTL_SET_FREQ 1 //定义宏变量,用于后面的 ioctl 中的控制命令
#define PWM_IOCTL_STOP 0 //定义信号量 lock


//定义信号量 lock用于互斥，因此，该驱动程序只能同时有一个进程使用
static struct semaphore lock;


/* freq: pclk/50/16/65536 ~ pclk/50/16
* if pclk = 50MHz, freq is 1Hz to 62500Hz
* human ear(人儿能辨别的) : 20Hz~ 20000Hz
*/
/*配置各个寄存器，设置PWM的频率*/
static void PWM_Set_Freq( unsigned long freq )
{
unsigned long tcon;
unsigned long tcnt;
unsigned long tcfg1;
unsigned long tcfg0;
struct clk *clk_p;
unsigned long pclk;


//设置GPB0 为TOUT0,pwm 输出
s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPB0_TOUT0);


/*读取定时器控制寄存器的数值*/
tcon = __raw_readl(S3C2410_TCON);
/*读取定时器配置寄存器1的值*/
tcfg1 = __raw_readl(S3C2410_TCFG1);
/*读取定时器配置寄存器0的值*/
tcfg0 = __raw_readl(S3C2410_TCFG0);


/*设置prescaler = 50*/
//S3C2410_TCFG_PRESCALER0_MASK定时器0 和1 的预分频值的掩码，TCFG[0~7]
tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;
tcfg0 |= (50 - 1);


/*设置分频值为16*/
//S3C2410_TCFG1_MUX0_MASK 定时器0 分割值的掩码TCFG1[0~3]
tcfg1 &= ~S3C2410_TCFG1_MUX0_MASK;
//定时器0 进行16 分割
tcfg1 |= S3C2410_TCFG1_MUX0_DIV16;

/*将设置的参数值写入相应的寄存器中*/
//把tcfg1 的值写到分割寄存器S3C2410_TCFG1 中
__raw_writel(tcfg1, S3C2410_TCFG1);
//把tcfg0 的值写到预分频寄存器S3C2410_TCFG0 中
__raw_writel(tcfg0, S3C2410_TCFG0);



/*开启对应时钟源，并获取pclk*/
clk_p = clk_get(NULL, "pclk");
//获得pclk的时钟频率
pclk = clk_get_rate(clk_p);

/*得到定时器的输入时钟，进而设置PWM的调制频率和占空比*/
tcnt = (pclk/50/16)/freq;
//PWM 脉宽调制的频率等于定时器的输入时钟
__raw_writel(tcnt, S3C2410_TCNTB(0));
//占空比是50%
__raw_writel(tcnt/2, S3C2410_TCMPB(0));


/*失能死区，开启自动重载, 关闭变相, 更新TCNTB0&TCMPB0, 启动timer0*/
tcon &= ~0x1f;
tcon |= 0xb;
//把tcon 写到计数器控制寄存器S3C2410_TCON 中
__raw_writel(tcon, S3C2410_TCON);


//clear manual update bit
tcon &= ~2;
__raw_writel(tcon, S3C2410_TCON);
}


static void pwm_stop(void)
{
//设置GPB0 为输出
s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
//设置GPB0 为低电平，使蜂鸣器停止
s3c2410_gpio_setpin(S3C2410_GPB(0), 0);
}


static int pwm_open(struct inode *inode, struct file *file)
{
if (!down_trylock(&lock)) //是否获得信号量，是down_trylock(&lock）=0，否则非0
return 0;
else
return -EBUSY; //返回错误信息：请求的资源不可用
}
static int pwm_close(struct inode *inode, struct file *file)
{
pwm_stop();
//释放信号量lock
up(&lock);
return 0;
}


/*cmd 是1，表示设置频率；cmd 是2 ，表示停止pwm*/
static int pwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
switch (cmd)
{
case PWM_IOCTL_SET_FREQ: //if cmd=1 即进入case PWM_IOCTL_SET_FREQ
if (arg == 0) //如果设置的频率参数是0
return -EINVAL; //返回错误信息，表示向参数传递了无效的参数
PWM_Set_Freq(arg); //否则设置频率
break;
case PWM_IOCTL_STOP: // if cmd=0 即进入case PWM_IOCTL_STOP
pwm_stop(); //停止蜂鸣器
break;
}
return 0; //成功返回
}


/*初始化设备的文件操作的结构体*/
static struct file_operations pwm_fops = {
.owner = THIS_MODULE,
.open = pwm_open,
.release = pwm_close,
.ioctl = pwm_ioctl,
};


static struct miscdevice misc = {
.minor = MISC_DYNAMIC_MINOR,
.name = DEVICE_NAME,
.fops = &pwm_fops,
};
static int __init pwm_init(void)
{
int ret;
init_MUTEX(&lock); //初始化一个互斥锁


ret = misc_register(&misc); //注册一个misc 设备


if(ret < 0)
{
printk(DEVICE_NAME "register falid!\n");
return ret;
}
printk (DEVICE_NAME " initialized!\n");
return 0;
}
static void __exit pwm_exit(void)
{
misc_deregister(&misc);
}
module_init(pwm_init);
module_exit(pwm_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DreamCatcher");
MODULE_DESCRIPTION("MINI2440 PWM Driver");
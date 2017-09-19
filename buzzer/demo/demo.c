#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/gpio.h>
#include <linux/pwm.h>
#include <linux/clk.h>



#define PWM1_ON          0x00010001
#define PWM1_OFF         0x00010002
#define PWM1_DUTY        0x00010003
#define PWM2_ON          0x00020001
#define PWM2_OFF         0x00020002
#define PWM2_DUTY        0x00020003
#define PWM3_ON          0x00030001
#define PWM3_OFF         0x00030002
#define PWM3_DUTY        0x00030003
#define PWM4_ON          0x00040001
#define PWM4_OFF         0x00040002
#define PWM4_DUTY        0x00040003
#define PWM_PERIOD        0x00050000

unsigned int nPWMPeriod=10000000; //PWM周期

struct pwm_device *pwm_dev_1=NULL;
struct pwm_device *pwm_dev_2=NULL;
struct pwm_device *pwm_dev_3=NULL;
struct pwm_device *pwm_dev_4=NULL;

static long pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


static struct file_operations pwm_fops =
{
    .owner = THIS_MODULE,
    .unlocked_ioctl = pwm_ioctl
};

static struct miscdevice pwm_misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mypwm",             //dev/mypwm
    .fops = &pwm_fops
};

static long pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        int ret=-1;

        switch (cmd)
        {
        case PWM1_ON:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_1, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_1 ioctl fail\r\n");
                        return ret;
                }
                pwm_enable(pwm_dev_1);
               
                break;
        case PWM1_OFF:
                ret = pwm_config(pwm_dev_1, 0, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_1 ioctl fail\r\n");
                        return ret;
                }
                pwm_disable(pwm_dev_1);
               
                break;
        case PWM1_DUTY:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_1, arg, nPWMPeriod);
                if(ret < ret)
                {
                        printk("pwm_dev_1 ioctl config err\r\n");
                        return 0;
                }
               
                break;
        case PWM2_ON:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_2, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_2 ioctl fail\r\n");
                        return ret;
                }
                pwm_enable(pwm_dev_2);
               
                break;
        case PWM2_OFF:
                ret = pwm_config(pwm_dev_2, 0, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_2 ioctl fail\r\n");
                        return ret;
                }
                pwm_disable(pwm_dev_2);
               
                break;
        case PWM2_DUTY:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_2, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_2 ioctl config err\r\n");
                        return ret;
                }               
               
                break;
        case PWM3_ON:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_3, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_3 ioctl fail\r\n");
                        return ret;
                }
                pwm_enable(pwm_dev_3);
               
                break;
        case PWM3_OFF:
                ret = pwm_config(pwm_dev_3, 0, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_3 ioctl fail\r\n");
                        return ret;
                }
                pwm_disable(pwm_dev_3);
               
                break;
        case PWM3_DUTY:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_3, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_3 ioctl config err\r\n");
                        return ret;
                }
               
                break;
#if 1
        case PWM4_ON:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_4, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_4 ioctl fail\r\n");
                        return ret;
                }
                pwm_enable(pwm_dev_4);
               
                break;
        case PWM4_OFF:
                ret = pwm_config(pwm_dev_4, 0, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_4 ioctl fail\r\n");
                        return ret;
                }
                pwm_disable(pwm_dev_4);
               
                break;
        case PWM4_DUTY:
                arg =  (arg>2000000 ? 2000000 : arg);
                ret = pwm_config(pwm_dev_4, arg, nPWMPeriod);
                if(ret < 0)
                {
                        printk("pwm_dev_4 ioctl config err\r\n");
                        return ret;
                }
               
                break;
        case PWM_PERIOD:
                arg =  (arg<2200000 ? 2200000 : arg);
                nPWMPeriod = (unsigned int)arg;
               
                break;
#endif
        default:
                return ret;
        }

        return ret;
}


static int pwm_init(void)
{
    int ret=-1;
       
    printk("regisger pwm_misc device\r\n");
    pwm_dev_1 = pwm_request(0, "pwm_1");
    if(pwm_dev_1==ERR_PTR(-EBUSY) || pwm_dev_1==ERR_PTR(-ENOENT))
        {
        printk("pwm_dev_1 register fail\r\n");
                return (int)pwm_dev_1;
    }

        pwm_dev_2 = pwm_request(1, "pwm_2");
        if(pwm_dev_2==ERR_PTR(-EBUSY) || pwm_dev_2==ERR_PTR(-ENOENT))
        {
                printk("pwn_dev_2 register fail\r\n");
                return (int)pwm_dev_2;
        }
       
        pwm_dev_3 = pwm_request(2, "pwm_3");
        if(pwm_dev_3==ERR_PTR(-EBUSY) || pwm_dev_3==ERR_PTR(-ENOENT))
        {
                printk("pwn_dev_3 register fail\r\n");
                return (int)pwm_dev_3;
        }
       
        pwm_dev_4 = pwm_request(3, "pwm_4");
        if(pwm_dev_4==ERR_PTR(-EBUSY) || pwm_dev_4==ERR_PTR(-ENOENT))
        {
                printk("pwn_dev_4 register fail\r\n");
                return 0;
        }

    printk("pwconfig\r\n");
        ret = pwm_config(pwm_dev_1, 1000000, nPWMPeriod);
        if(ret < 0)
        {
                printk("pwm_config_1 init fail\r\n");
                return ret;
        }
    printk("pwconfig1\r\n");
       
        ret = pwm_config(pwm_dev_2, 1000000, nPWMPeriod);
        if(ret < 0)
        {
                printk("pwm_config_2 init fail\r\n");
                return ret;
        }
    printk("pwconfig2\r\n");
       
        ret = pwm_config(pwm_dev_3, 1000000, nPWMPeriod);
        if(ret < 0)
        {
                printk("pwm_config_3 init fail\r\n");
                return ret;
        }
    printk("pwconfig3\r\n");
       
        ret = pwm_config(pwm_dev_4, 1000000, nPWMPeriod);
        if(ret < 0)
        {
                printk("pwm_config_4 init fail\r\n");
                return ret;
        }
    printk("pwconfig4\r\n");

        ret = pwm_enable(pwm_dev_1);
        if(ret == 0)
        {
                printk("pwm_enable_dev_1 init success\r\n");
        }
        else if(ret < 0 )
        {
                printk("pwm_enable_dev_1 init fail\r\n");
                return ret;
        }
       
        ret = pwm_enable(pwm_dev_2);
        if(ret == 0)
        {
                printk("pwm_enable_dev_2 init success\r\n");
        }
        else if(ret < 0 )
        {
                printk("pwm_enable_dev_2 init fail\r\n");
                return ret;
        }

        ret = pwm_enable(pwm_dev_3);
        if(ret == 0)
        {
                printk("pwm_enable_dev_3 init success\r\n");
        }
        else if(ret < 0 )
        {
                printk("pwm_enable_dev_3 init fail\r\n");
                return ret;
        }

        ret = pwm_enable(pwm_dev_4);
        if(ret == 0)
        {
                printk("pwm_enable_dev_4 init success\r\n");
        }
        else if(ret < 0 )
        {
                printk("pwm_enable_dev_4 init fail\r\n");
                return ret;
        }
       
    ret = misc_register(&pwm_misc);

        return ret;
}

static void pwm_exit(void)
{
    printk("unregister pwm_misc device\r\n");
    misc_deregister(&pwm_misc);

        pwm_config(pwm_dev_1,0,10000000);
        pwm_disable(pwm_dev_1);
        pwm_free(pwm_dev_1);
       
        pwm_config(pwm_dev_2,0,10000000);
        pwm_disable(pwm_dev_2);
        pwm_free(pwm_dev_2);
       
        pwm_config(pwm_dev_3,0,10000000);
        pwm_disable(pwm_dev_3);
        pwm_free(pwm_dev_3);
       
        pwm_config(pwm_dev_4,0,10000000);
        pwm_disable(pwm_dev_4);
        pwm_free(pwm_dev_4);
}

module_init(pwm_init);
module_exit(pwm_exit);
MODULE_LICENSE("GPL");
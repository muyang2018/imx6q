#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/ioctl.h>


#define DEVICE_NAME		"my_led"	//定义设备名

#define LED_ON			1
#define LED_OFF			3

int main(void)
{
	int fd;
	int i,choice;
	
	fd = open("/dev/my_led", O_RDWR);
	if(fd < 0){
		printf(" -- /dev/my_led open fail");
		return -1;
	}
	
	printf("Please input your choise:\n");
	printf(" 1)LED_ON\n");
	printf(" 2)LED_OFF\n");
	printf(" 3)LED_DELAY\n");
	printf("input: ");
	scanf("%d",&choice);
	if(choice==1)
	{
		ioctl(fd, LED_ON, 0);
	}
	if(choice==2)
	{
		ioctl(fd, LED_OFF, 0);
	}
	if(choice==3)
	{
		ioctl(fd, LED_ON, 0);
		usleep(5000000);
		ioctl(fd, LED_OFF, 0);
	}
	close(fd);
	return 0;
}
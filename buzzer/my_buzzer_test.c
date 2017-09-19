#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/ioctl.h>

#define DEVICE_NAME		"my_buzzer" //定义设备名

#define PWM_1_ON        1
#define PWM_1_OFF       3

int main(void)
{
	int fd;
	int i,choice;
	
	fd = open("/dev/my_buzzer", O_RDWR);
	if(fd < 0){
		printf(" -- /dev/my_buzzer open fail");
		return -1;
	}
	
	printf("Please input your choise:\n");
	printf(" 1)PWM_1_ON\n");
	printf(" 2)PWM_1_OFF\n");
	printf(" 3)PWM_1_DELAY\n");
	printf("input: ");
	scanf("%d",&choice);
	if(choice==1)
	{
		ioctl(fd, PWM_1_ON, 0);
	}
	if(choice==2)
	{
		ioctl(fd, PWM_1_OFF, 0);
	}
	if(choice==3)
	{
		ioctl(fd, PWM_1_ON, 0);
		usleep(5000000);
		ioctl(fd, PWM_1_OFF, 0);
	}
	close(fd);
	return 0;
}




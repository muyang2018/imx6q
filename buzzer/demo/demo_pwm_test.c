#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PWM_ON  0x100001
#define PWM_OFF 0x100002

int main(void)
{
    int fd;
    int a;

    fd = open("/dev/mypwm", O_RDWR);
    if (fd < 0)
        return -1;

    while(1) {
            ioctl(fd, PWM_ON);
    }
    close(fd);
    return 0;
}

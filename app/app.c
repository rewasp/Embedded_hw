#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<syscall.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define DEV_NAME "/dev/dev_driver" // driver name
#define SYSCALL_NUM 376   //  맞나? 확인 필요!

#define DEV_IOCTL_BASE 'p'
#define DEVIOC_WRITE    _IOW(DEV_IOCTL_BASE, 0, int)


int main(int argc, char *argv[]){

    int delay;
    int repeat;
    unsigned char number[4];
    int stream;
    int fd;

    delay = (unsigned char)atoi(argv[1]);
    repeat = (unsigned char)atoi(argv[2]);

    strncpy(number, argv[3], 4);
 
    stream = syscall(SYSCALL_NUM, delay, repeat, number);

    fd = open(DEV_NAME, O_WRONLY);

    ioctl(fd, DEVIOC_WRITE, &stream);

    close(fd);

    return 0;
}

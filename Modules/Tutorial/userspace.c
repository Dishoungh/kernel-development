/***************************************************************************//**
*  \file       test_app.c
*
*  \details    Userspace application to test the Device driver
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
*
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
 
struct ioctl_arguments
{
        uint32_t arg1;
        int16_t arg2;
        uint8_t arg3[3];
};

#define WR_VALUE _IOW('a', 'a', struct ioctl_arguments *)
#define RD_VALUE _IOR('a', 'b', struct ioctl_arguments *)

 
int main()
{
        int fd;
 
        printf("\nOpening Driver\n");
        fd = open("/dev/dishoungh", O_RDWR);
        if(fd < 0) {
                perror("Cannot open\n");
                return 0;
        }

	struct ioctl_arguments testParam = 
	{
		.arg1 = 23320823,
		.arg2 = -3921,
		.arg3 = {23, 170, 78},
	};

        ioctl(fd, WR_VALUE, (struct ioctl_arguments*) &testParam); 
        printf("Reading Value from Driver\n");

	struct ioctl_arguments readParam = {0};

        ioctl(fd, RD_VALUE, (struct ioctl_arguments*) &readParam);

	printf("Arg1 = %lu\n", readParam.arg1);
	printf("Arg2 = %d\n", readParam.arg2);
	printf("Arg3[0] = %u\n", readParam.arg3[0]);
	printf("Arg3[1] = %u\n", readParam.arg3[1]);
	printf("Arg3[2] = %u\n", readParam.arg3[2]);

        printf("Closing Driver\n");
        close(fd);
}
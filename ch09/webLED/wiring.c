#include <string.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

void cds_function(int* threshold)
{
	int fd;
	int a2dChannel = 0;
	int a2dVal;

	if ((fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x48)) < 0) {
		perror("wiringPiI2CSetupInterface");
	}
	
	while (1) {
		wiringPiI2CWrite(fd, 0x00 | a2dChannel);

		a2dVal = wiringPiI2CRead(fd);

		printf("val: [%d], threshold: [%d] - ", a2dVal, *threshold);

		if (a2dVal < *threshold) {
			printf("Bright!!\n");
		}
		else {
			printf("Dark!!\n");
		}

		delay(1000);
	}
}

// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TSL2571
// This code is designed to work with the TSL2571_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Light?sku=TSL2571_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, TSL2571 I2C address is 0x39(57)
	ioctl(file, I2C_SLAVE, 0x39);

	// Select enable register(0x00 | 0x80)
	// Set Power ON, ALS Enabled(0x0B)
	char config[2] = {0};
	config[0] = 0x00 | 0x80;
	config[1] = 0x0B;
	write(file, config, 2);
	// Select ALS time register(0x01 | 0x80)
	// Atime = 2.72 ms, max count = 1023(0xFF)
	config[0] = 0x01 | 0x80;
	config[1] = 0xFF;
	write(file, config, 2);
	// Select wait time register(0x03 | 0x80)
	// Wtime = 2.72 ms(0xFF)
	config[0] = 0x03 | 0x80;
	config[1] = 0xFF;
	write(file, config, 2);
	// Select control register(0x0F | 0x80)
	// Gain = 1x(0x20)
	config[0] = 0x0F | 0x80;
	config[1] = 0x20;
	write(file, config, 2);
	sleep(1);

	// Read 4 bytes of data from register(0x14 | 80)
	// temp msb, temp lsb
	char reg[1] = {0x14 | 0x80};
	write(file, reg, 1);
	char data[4] = {0};
	if(read(file, data, 4) != 4)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		// Convert the data
		int c0Data = (data[1] * 256 + data[0]);
		int c1Data = (data[3] * 256 + data[2]);
		float CPL = (2.72 * 1.0) / 53.0;
		float luminance1 = (1 * c0Data - 2.0 * c1Data) / CPL;
		float luminance2 = (0.6 * c0Data - 1.00 * c1Data) / CPL;
		float luminance = 0.0;

		if((luminance1 > 0) && (luminance1 > luminance2))
		{
			luminance = luminance1;
		}
		else if((luminance2 > 0) && (luminance2 > luminance1))
		{
			luminance = luminance2;
		}

		// Output data to screen
		printf("Ambient Light luminance : %.2f lux \n", luminance);
	}
}

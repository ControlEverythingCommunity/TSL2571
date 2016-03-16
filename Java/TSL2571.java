// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TSL2571
// This code is designed to work with the TSL2571_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Light?sku=TSL2571_I2CS#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class TSL2571
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, TSL2571 I2C address is 0x39(57)
		I2CDevice device = bus.getDevice(0x39);
		
		// Select enable register
		// Set Power ON, ALS Enabled
		device.write(0x00 | 0x80, (byte)0x0B);
		
		// Select ALS time register
		// Atime = 2.72 ms, max count = 1023
		device.write(0x01 | 0x80, (byte)0xFF);
		
		// Select wait time register
		// Wtime = 2.72 ms
		device.write(0x03 | 0x80, (byte)0xFF);
		
		// Select control register
		// Gain = 1x
		device.write(0x0F | 0x80, (byte)0x20);
		Thread.sleep(800);
		
		// Read 6 Bytes of data from address 0x14(20)
		// c0Data lsb, c0Data msb, c1Data lsb, c1Data msb
		byte[] data = new byte[4];
		device.read(0x14 | 0x80, data, 0, 4);
		
		// Convert the data
		int c0Data = ((data[1] & 0xFF) * 256) + (data[0] & 0xFF);
		int c1Data = ((data[3] & 0xFF) * 256) + (data[2] & 0xFF);
		double CPL = (2.72 * 1.0) / 53.0;
		double luminance1 = (1 * c0Data - 2.0 * c1Data) / CPL;
		double luminance2 = (0.6 * c0Data - 1.00 * c1Data) / CPL;
		double luminance = 0.0;
		
		if((luminance1 > 0) && (luminance1 > luminance2))
		{
			luminance = luminance1;
		}
		else if((luminance2 > 0) && (luminance2 > luminance1))
		{
			luminance = luminance2;
		}
		
		// Output data to screen
		System.out.printf("Ambient Light luminance : %.2f lux%n", luminance);
	}
}
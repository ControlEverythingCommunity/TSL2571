// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TSL2571
// This code is designed to work with the TSL2571_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Light?sku=TSL2571_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// I2C address of the TSL2571 0x39(57)
#define Addr 0x39

float luminance = 0;
void setup()
{
    // Set variable
    Particle.variable("i2cdevice", "TSL2571");
    Particle.variable("luminance", luminance);
    
    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise serial communication, set baud rate = 9600
    Serial.begin(9600);

    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select enable register
    Wire.write(0x00 | 0x80);
    // Set power on, ALS enabled
    Wire.write(0x01);
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(500);

    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select enable register
    Wire.write(0x00 | 0x80);
    // Set power on, ALS enabled
    Wire.write(0x0B);
    // Stop I2C Transmission
    Wire.endTransmission();

    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select ALS time register
    Wire.write(0x01 | 0x80);
    // Atime = 2.72 ms, max count = 1023
    Wire.write(0xFF);
    // Stop I2C Transmission
    Wire.endTransmission();

    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select Wait time register
    Wire.write(0x03 | 0x80);
    // Wtime = 2.72 ms
    Wire.write(0xFF);
    // Stop I2C Transmission
    Wire.endTransmission();

    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select control register
    Wire.write(0x0F | 0x80);
    // Gain = 1x
    Wire.write(0x20);
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(300);
}

void loop()
{
    unsigned int data[4];
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write(0x14 | 0x80);
    // Stop I2C Transmission
    Wire.endTransmission();
    
    // Request 4 bytes of data
    Wire.requestFrom(Addr, 4);
    
    // Read 4 bytes of data
    // c0Data lsb, c0Data msb, c1Data lsb, c1Data msb
    if(Wire.available() == 4)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
      data[2] = Wire.read();
      data[3] = Wire.read();
    }

    // Convert the data
    int c0Data = (data[1] * 256) + data[0];
    int c1Data = (data[3] * 256) + data[2];
    float CPL = (2.72 * 1.0) / 53.0;
    float luminance1 = (1 * c0Data - 2.0 * c1Data) / CPL;
    float luminance2 = (0.6 * c0Data - 1.00 * c1Data) / CPL;
    
    if((luminance1 > 0) && (luminance1 > luminance2))
    {
      luminance = luminance1;
    }
    else if((luminance2 > 0) && (luminance2 > luminance1))
    {
      luminance = luminance2;
    }

    // Output data to dashboard
    Particle.publish("Ambient Light luminance : ", String(luminance));
    delay(1000);
}


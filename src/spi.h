#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>
using std::cout;
using std::endl;
#include <unistd.h>	// for usleep

#include <pigpio.h>

#define BAUD_32K	( 32000 )
#define BAUD_100K	( 100000 )
#define BAUD_500K	( 500000 )
#define BAUD_1M		( 1000000 )
#define BAUD_2M		( 2000000 )
#define BAUD_5M		( 5000000 )
#define BAUD_10M	( 10000000 )
#define BAUD_20M	( 20000000 )

#define SPI_MODE_0	( 0 )
#define SPI_MODE_1	( 1 )

#pragma once
class Spi
{
public:
	// Constructor
	Spi( unsigned channel, unsigned config )
	{
		// squirl away our private variables
		channel_ = channel;

        // 			@ MISO @ MOSI @ SCLK @ CE0 @ CE1 @ CE2
		// Main SPI @    9 @   10 @   11 @   8 @   7 @   -
		// Mode POL PHA
		// 0    0   0
		// 1    0   1		<-- falling edge of the clock
		// 2    1   0
		// 3    1   1


		// configure SPI
		// config is for all the config bits, but they defualt to zero for us except for the mode
		handle_ = spiOpen( channel_, BAUD_2M, config );
		if( handle_ < 0 )
			throw std::runtime_error( "failed to construct: SPI" );
		cout << "d2a SPI Handle: " << handle_ << endl;

	}


	// spi data routines
	int write( char *data, unsigned count );
	int read( char *buff, unsigned count );
	int xfer( char *data, char *rxBuf, unsigned count );


private:
	unsigned channel_ = 0;
	int handle_ = 0;
};
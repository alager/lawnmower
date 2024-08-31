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


#pragma once
class Spi
{
public:
	// int handle;

	// Constructor
	Spi( unsigned channel )
	{
		// squirl away our private variables
		channel_ = channel;

		// configure SPI
		handle_ = spiOpen( channel_, BAUD_2M, 0 );
		if( handle_ < 0 )
			throw std::runtime_error( "failed to construct: SPI" );
		cout << "d2a SPI Handle: " << handle_ << endl;

	}

	// spi data routines
	int write( char *data, unsigned count );
	int read( char *buff, unsigned count );
	int xfer( char *data, char *rxBuf, unsigned count );


private:
	unsigned channel_;
	int handle_;
};
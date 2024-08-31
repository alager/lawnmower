#include <stdexcept>
#include <iostream>
using std::cout;
using std::endl;
#include <unistd.h>	// for usleep
#include <pigpio.h>
#include "spi.h"


// #define BAUD_32K	( 32000 )
// #define BAUD_100K	( 100000 )
// #define BAUD_500K	( 500000 )
// #define BAUD_1M		( 1000000 )
// #define BAUD_2M		( 2000000 )
// #define BAUD_5M		( 5000000 )
// #define BAUD_10M	( 10000000 )
// #define BAUD_20M	( 20000000 )

#define DAC_SPI_CHANNEL		( 0 )

// Chip register defines
#define DAC_WRITE			( 0 << 23 )
#define DAC_READ			( 1 << 23 )

#define REG_DAC_VALUE		( 0 << 19 )
#define REG_OUTPUT_RANGE	( 1 << 19 )
#define REG_POWER_CTRL		( 2 << 19 )
#define REG_CTRL			( 3 << 19 )

#define DAC_NONE			( 0x00 << 16 )
#define DAC_A				( 0x00 << 16 )
#define DAC_B				( 0x01 << 16 )
#define DAC_C				( 0x02 << 16 )
#define DAC_D				( 0x03 << 16 )
#define DAC_ALL				( 0x04 << 16 )

#define RANGE_05_0			( 0x00 << 0 )
#define RANGE_10_0			( 0x01 << 0 )
#define RANGE_10_8			( 0x02 << 0 )

#define CTRL_REG_LOAD		( 0x05 << 16 )

#define ENABLE_10V_GPIO		( 27 )

#define DAC_LDAC	(24)

#pragma once
class D2a
{
public:
	
	// Constructor
	D2a() : D2a( DAC_LDAC )
	{

	}

	// Constructor
	D2a( unsigned int ldac	)
	{
		// squirl away our private variables
		ldac_ = ldac;

		// Initialize Pi GPIO
		if ( gpioInitialise() < 0 ) 
			throw std::runtime_error( "failed to construct: GPIOInit" );

		// Configure the d2a control pins
		gpioSetMode( ldac_, PI_OUTPUT);
		gpioWrite( ldac_, PI_LOW );

		// activate the 10V supply enable
		// this only affects the first run after power up
		// as the pin state stays high even after program exit
		gpioSetMode( ENABLE_10V_GPIO, PI_OUTPUT);
		gpioWrite( ENABLE_10V_GPIO, PI_HIGH );

		

		// // configure SPI
		spi_ = new Spi( DAC_SPI_CHANNEL );
		// handle_ = spiOpen( 0, BAUD_2M, 0 );
		// if( handle_ < 0 )
		// 	throw std::runtime_error( "failed to construct: SPI" );
		// cout << "d2a SPI Handle: " << handle_ << endl;

		// set output range
		reg__ = 0;
		reg__ = (DAC_WRITE) | (REG_OUTPUT_RANGE) | (DAC_ALL) | (RANGE_10_8);
		write( reg__ );

		// set the power control power up a,b,c and ref
		// must delay 10us before doing more with the dac after this command
		reg__ = 0;
		reg__ = DAC_WRITE |  REG_POWER_CTRL | DAC_NONE | 0x0017;
		write( reg__ );
		usleep( 10 );

		// clear all the dac outputs to 0.
		// the data 0x5555 is a don't care value
		reg__ = 0;
		reg__ = DAC_WRITE |  REG_CTRL | DAC_ALL | 0x5555;
		write( reg__ );

	}

	// Destructor
	~D2a()
	{
		// spiClose( (unsigned int)handle_ );
	}

	// // spi data routines
	int write( uint32_t data );
	int read( uint8_t * buff );
	int xfer( uint32_t data, char *rxBuf );

	// data conversions
	
	// % to voltage
	float percent2Voltage( float percent );

	// voltage to d2a register value
	uint32_t voltage2reg( float voltage );

	void ldacHigh( void );
	void ldacLow( void );


	// user API
	int set( char dac, float percent );


private:
	unsigned int ldac_;
	Spi *spi_;
	// int handle_;


	// internal data buffer for SPI transfers
	// the AD5754R d2a only has a 24bit control register and is MSB first
	/*----------------------------------------------------------------------
	| DB23 | DB22 | DB21 | DB20 | DB19 | DB18 | DB17 | DB16 | DB15  to DB0 |
	| R/W  | Zero | REG2 | REG1 | REG0 | A2   | A1   | A0   |      Data    |
	------------------------------------------------------------------------ */
	uint8_t	reg_[4];
	uint32_t reg__;
};
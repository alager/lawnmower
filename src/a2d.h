#include <stdexcept>
#include <iostream>
using std::cout;
using std::endl;
#include <pigpio.h>

#define BAUD_32K	( 32000 )
#define BAUD_100K	( 100000 )
#define BAUD_500K	( 500000 )
#define BAUD_1M		( 1000000 )
#define BAUD_2M		( 2000000 )
#define BAUD_5M		( 5000000 )
#define BAUD_10M	( 10000000 )
#define BAUD_20M	( 20000000 )

class a2d
{
public:
	

	// Constructor
	a2d(
		unsigned int clock,
		unsigned int miso,
		unsigned int mosi,
		unsigned int chipSelect
		)
	{
		// squirl away our private variables
		clock_ = clock;
		miso_ = miso;
		mosi_ = mosi;
		chipSelect_ = chipSelect;

		// Initialize Pi GPIO
		if ( gpioInitialise() < 0 ) 
			throw std::runtime_error( "failed to construct: GPIOInit" );

		// Configure the SPI pins
		handle_ = spiOpen( 0, BAUD_32K, 0 );
		if( handle_ < 0 )
			throw std::runtime_error( "failed to construct: SPI" );
		cout << "a2d SPI Handle: " << handle_ << endl;

	}

	// Destructor
	~a2d()
	{
		// spiClose( (unsigned int)handle_ );
	}

	int write( uint8_t * data, unsigned int count );
	int read( uint8_t * data, unsigned int count );
	int xfer( char *txBuf, char *rxBuf, unsigned count );


private:
	unsigned int clock_;
	unsigned int miso_;
	unsigned int mosi_;
	unsigned int chipSelect_;
	int handle_;
};
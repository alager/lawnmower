#include <stdexcept>
#include <iostream>
#include <cstring>
#include  <iomanip>

#include <arpa/inet.h>
using std::cout;
using std::endl;
#include <pigpio.h>
#include "spi.h"


#define A2D_SPI_CHANNEL		( 1 )

// Command Register addresses
#define CMD_NO_OP		( 0x0000 )
#define CMD_STDBY		( 0x8200 )
#define CMD_PWR_DN		( 0x8300 )
#define CMD_RST			( 0x8500 )
#define CMD_AUTO_RST	( 0xa000 )
#define CMD_MAN_Ch_0	( 0xc000 )
#define CMD_MAN_Ch_1	( 0xc400 )
#define CMD_MAN_Ch_2	( 0xc800 )
#define CMD_MAN_Ch_3	( 0xcc00 )
#define CMD_MAN_Ch_4	( 0xd000 )
#define CMD_MAN_Ch_5	( 0xd400 )
#define CMD_MAN_Ch_6	( 0xd800 )
#define CMD_MAN_Ch_7	( 0xdc00 )
#define CMD_MAN_AUX		( 0xe000 )


// Program Register addresses
#define AUTO_SEQ_EN		( 0x0100 << 1 )
#define CHNL_PWR_DWN	( 0x0200 << 1 )
#define FEATURE			( 0x0300 << 1 )
#define RANGE_CH0		( 0x0500 << 1 )
#define RANGE_CH1		( 0x0600 << 1 )
#define RANGE_CH2		( 0x0700 << 1 )
#define RANGE_CH3		( 0x0800 << 1 )
#define RANGE_CH4		( 0x0900 << 1 )
#define RANGE_CH5		( 0x0a00 << 1 )
#define RANGE_CH6		( 0x0b00 << 1 )
#define RANGE_CH7		( 0x0c00 << 1 )
#define ALRM_OVR		( 0x1000 << 1 )
#define ALRM_TRP_1_3	( 0x1100 << 1 )
#define ALRM_ACT_1_3	( 0x1200 << 1 )
#define ALRM_TRP_4_7	( 0x1300 << 1 )
#define ALRM_ACT_4_7	( 0x1400 << 1 )
// alarm thresholds not done for now
#define CMD_READ_BAK	( 0x3f00 << 1 )
#define WRITE			( 0x0100 )
#define READ			( 0x0000 )

#define CHAN_0			( 0x00 )
#define CHAN_1			( 0x01 )
#define CHAN_2			( 0x02 )
#define CHAN_3			( 0x03 )
#define CHAN_4			( 0x04 )
#define CHAN_5			( 0x05 )
#define CHAN_6			( 0x06 )
#define CHAN_7			( 0x07 )

// PGA Gain
#define GAIN_1_25		( 0x06 )



#pragma once
class A2d
{
public:

	// Constructor
	A2d()
	{

		// Initialize Pi GPIO
		if ( gpioInitialise() < 0 ) 
			throw std::runtime_error( "failed to construct: GPIOInit" );

		// configure SPI
		spi_ = new Spi( A2D_SPI_CHANNEL );

		// configure the A2D

		// set the auto scan channels
		reg__ = 0;
		//			0x0200		0x100			0x01			0x02				0x04			0x08				0x10
		//			0x031f
		reg__ = ( AUTO_SEQ_EN | WRITE | ( 1 << CHAN_0 ) | ( 1 << CHAN_1 ) | ( 1 << CHAN_2 ) | ( 1 << CHAN_3 ) | ( 1 << CHAN_4 ) );
cout << "A2d reg: 0x" << std::setfill('0') << std::setw(4) << std::right << std::hex << reg__ << endl;
		write( reg__ );

		// power down the unused channels
		reg__ = 0;
		reg__ = ( CHNL_PWR_DWN | WRITE | ( 1 << CHAN_5 ) | ( 1 << CHAN_6 ) | ( 1 << CHAN_7 ) );
// cout << "A2d reg: 0x" << std::setfill('0') << std::setw(4) << std::right<< std::hex << reg__ << endl;
		write( reg__ );

		// set the PGA gain to 1.25.  Output is 1.25 * Vref => 1.25 * 4.096 = 5.12V
		setRange( RANGE_CH0, GAIN_1_25 );
		setRange( RANGE_CH1, GAIN_1_25 );
		setRange( RANGE_CH2, GAIN_1_25 );
		setRange( RANGE_CH3, GAIN_1_25 );
		setRange( RANGE_CH4, GAIN_1_25 );

		// set auto mode to start conversions
		auto_rst();
	}

	// Destructor
	~A2d()
	{
		// spiClose( (unsigned int)handle_ );
	}

	int write( uint32_t data );
	int writeCmd( uint32_t data );
	int read( char *buff );
	int xfer( char *data, char *rxBuf );

	void setRange( unsigned chan, uint8_t gain );
	void auto_rst( void );
	void no_op( void );
	float update( unsigned idx );




private:
	// internal data buffer for SPI transfers
	// the ADS8674 a2d has a 32bit control register and is MSB first
	// The command register is a 16-bit, write-only register that is used to set the operating mode
	// The program register is a 16-bit read or write register followed by 8 bits of data (24 bits total)
	/*		------------------------------------
			| REGISTER ADDRESS | WR/RD |  Data |
			|      15 - 9      |   8   | 7 - 0 |
			------------------------------------ */

	Spi *spi_;
	uint8_t	reg_[4];
	uint32_t reg__;	

};
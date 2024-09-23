#include "a2d.h"

// a class for working with the d2a chip

// write 16 bits of register followed by 8 bits of 0s
int A2d::write( uint32_t data )
{
	// flip endian for SPI chip required order
	data = htonl( data << 16 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spi_->write( (char *)reg_, 3 );
}


// write the 16 bit command followed by 16 bits of 0s
int A2d::writeCmd( uint32_t data )
{
	// flip endian for SPI chip required order
	data = htonl( data << 16 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spi_->write( (char *)reg_, 4 );
}


int A2d::read( char * data )
{
	return spi_->read( (char *)data, 3 );
}


int A2d::xfer( uint32_t data, char *rxBuf )
{
	// return spi_->xfer( txBuf, rxBuf, 3 );
	// flip endian for SPI chip required order
	data = htonl( data << 16 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// 16 bits of command and 16 clocks to read data (32 total)
	return spi_->xfer( (char *)reg_, rxBuf, 4 );
}


void A2d::setRange( unsigned chan, uint8_t gain )
{
	reg__ = 0;
	reg__ = ( chan | WRITE | gain );
	write( reg__ );
cout << "A2d range reg: 0x" << std::setfill('0') << std::setw(4) << std::right << std::hex << reg__ << endl;

}

void A2d::auto_rst( void )
{
	writeCmd( CMD_AUTO_RST );
}

void A2d::no_op( void )
{
	writeCmd( CMD_NO_OP );
}

// read the specified channel and return it as a float
float A2d::update( unsigned idx )
{
	( void )idx;
	std::array <char, 4> rxData;	//C++ array
	uint16_t data;

	// cout << std::right << std::hex << std::uppercase << std::setfill('0');

	reg__ = CMD_NO_OP;
	xfer( reg__, rxData.data());
	
	// assemble the received bytes together into a 16 bit type and right align the data to bit 0
	data = static_cast<uint16_t>(( rxData.at(2) << 8 ) | rxData.back() ) >> 2;
	// cout << "0x" << std::setw(4) << data << endl;

	return static_cast<float>(data);
}
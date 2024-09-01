#include "a2d.h"

// a class for working with the d2a chip

int A2d::write( uint32_t data )
{
	// flip endian for SPI chip required order
	data = htonl( data << 16 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spi_->write( (char *)reg_, 3 );
}


int A2d::read( char * data )
{
	return spi_->read( (char *)data, 3 );
}


int A2d::xfer( char *txBuf, char *rxBuf )
{
	return spi_->xfer( txBuf, rxBuf, 3 );
}


void A2d::setRange( unsigned chan, uint8_t gain )
{
	reg__ = 0;
	reg__ = ( chan | WRITE | gain );
	write( reg__ );
cout << "A2d range reg: 0x" << std::setfill('0') << std::setw(4) << std::right << std::hex << reg__ << endl;

}
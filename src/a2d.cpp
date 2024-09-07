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

void A2d::auto_rst( void )
{
	writeCmd( CMD_AUTO_RST );
}

void A2d::no_op( void )
{
	writeCmd( CMD_NO_OP );
}

#include "spi.h"

int Spi::write( char *data, unsigned count )
{
	// // flip endian for SPI chip required order
	// data = htonl( data << 8 );

	// // copy to the class register array
	// memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spiWrite( ( unsigned int )handle_, data, count );
}


int Spi::read( char *buff, unsigned count )
{
	return spiRead( ( unsigned int )handle_, buff, count );
}


int Spi::xfer( char *data, char *rxBuf, unsigned count )
{
	// flip endian for SPI chip required order
	// data = htonl( data );

	// // copy to the class register array
	// memcpy( reg_, &data, sizeof( reg_ ) );

	return spiXfer( (unsigned int)handle_, data, rxBuf,  count );
}


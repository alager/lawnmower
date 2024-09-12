
#include "spi.h"

int Spi::write( char *data, unsigned count )
{
	// write the spi data
	return spiWrite( ( unsigned int )handle_, data, count );
}


int Spi::read( char *buff, unsigned count )
{
	return spiRead( ( unsigned int )handle_, buff, count );
}


int Spi::xfer( char *data, char *rxBuf, unsigned count )
{
	return spiXfer( (unsigned int)handle_, data, rxBuf,  count );
}


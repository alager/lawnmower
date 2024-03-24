#include "a2d.h"

// a class for working with the d2a chip

int a2d::write( uint8_t * data, unsigned int count )
{
	return spiWrite( ( unsigned int )handle_, (char *)data, count );
}


int a2d::read( uint8_t * data, unsigned int count )
{
	return spiRead( ( unsigned int )handle_, (char *)data, count );
}


int a2d::xfer( char *txBuf, char *rxBuf, unsigned int count )
{
	return spiXfer( (unsigned int)handle_, txBuf, rxBuf,  count );
}

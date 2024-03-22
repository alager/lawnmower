#include "d2a.h"

// a class for working with the d2a chip

int d2a::write( uint8_t * data, unsigned int count )
{
	spiWrite( ( unsigned int )handle_, (char *)data, count );
	return 0;
}


int d2a::read( uint8_t * data, unsigned int count )
{
	spiRead( ( unsigned int )handle_, (char *)data, count );
	return 0;
}
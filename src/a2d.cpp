#include "a2d.h"

// a class for working with the d2a chip

int a2d::write( uint8_t * data, unsigned int count )
{
	spiWrite( ( unsigned int )handle_, (char *)data, count );
	return 0;
}


int a2d::read( uint8_t * data, unsigned int count )
{
	spiRead( ( unsigned int )handle_, (char *)data, count );
	return 0;
}
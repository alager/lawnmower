#include <cstring>
#include <arpa/inet.h>
#include "d2a.h"

// a class for working with the d2a chip

int d2a::write( uint32_t data )
{
	// flip endian for SPI chip required order
	data = htonl( data << 8 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spiWrite( ( unsigned int )handle_, (char *)reg_, 3 );
}


int d2a::read( uint8_t * buff )
{
	return spiRead( ( unsigned int )handle_, (char *)buff, 3 );
}


int d2a::xfer( uint32_t data, char *rxBuf )
{
	// flip endian for SPI chip required order
	data = htonl( data );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	return spiXfer( (unsigned int)handle_, (char *)reg_, rxBuf,  3 );
}


// convert from percentage to a voltage
// this system has  a max output of 10V, so
// 100% --> 10V
// 10% --> 1V
float d2a::percent2Voltage( float percent )
{
	return percent / 10.0f;
}

// voltage to d2a register value
// Vout = Vref * Gain * D / 2^n
// where:
// D is the decimal equivalent of the code loaded to the DAC.
// n is the bit resolution of the DAC.
// VREFIN is the reference voltage 2.5V.
// Gain is an internal gain the value of which depends on the
// output range selected by the user as shown in Table 7.
uint32_t d2a::voltage2reg( float voltage )
{
	// D = ( Vout * 2^n ) / ( Vref * Gain )

	// Gain for 10.8V is 4.32
	// n = 16 bits
	// Vref = 2.5V
	return (uint32_t)(( voltage * 65535.0f ) / ( 2.5f * 4.32f ));
}


// user level command to set the output between 0-100%
int d2a::set( char dac, float percent )
{
	uint32_t mydac = 0;
	float voltage = percent2Voltage( percent );
	uint32_t reg = voltage2reg( voltage );

	// cout << "DAC: " << dac << ", percent: " << percent << endl;
	// cout << "voltage: " << voltage << ", reg: " <<  reg << endl;

	if( dac == 'a' )
		mydac = DAC_A;
	else
	if( dac == 'b' )
		mydac = DAC_B;
	else
	if( dac == 'c' )
		mydac = DAC_C;

	return write( DAC_WRITE | REG_DAC_VALUE | mydac | reg );
}


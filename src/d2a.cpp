#include "d2a.h"

// a class for working with the D2a chip

int D2a::write( uint32_t data )
{
	// flip endian for SPI chip required order
	data = htonl( data << 8 );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// write the spi data
	return spi_->write( (char *)reg_, 3 );
}


int D2a::read( uint8_t * buff )
{
	// return spiRead( ( unsigned int )handle_, (char *)buff, 3 );
	return spi_->read( (char *)buff, 3 );
}

// This function transfers count bytes of data from txBuf to the SPI
// device associated with the handle.  Simultaneously count bytes of
// data are read from the device and placed in rxBuf.
int D2a::xfer( uint32_t data, char *rxBuf )
{
	// flip endian for SPI chip required order
	data = htonl( data );

	// copy to the class register array
	memcpy( reg_, &data, sizeof( reg_ ) );

	// return spiXfer( (unsigned int)handle_, (char *)reg_, rxBuf,  3 );

	return spi_->xfer( (char *)reg_, rxBuf,  3 );
}


// convert from percentage to a voltage
// this system has  a max output of 10V, so
// 100% --> 10V
// 10% --> 1V
float D2a::percent2Voltage( float percent )
{
	return percent / 10.0f;
}

// voltage to D2a register value
// Vout = Vref * Gain * D / 2^n
// where:
// D is the decimal equivalent of the code loaded to the DAC.
// n is the bit resolution of the DAC.
// VREFIN is the reference voltage 2.5V.
// Gain is an internal gain the value of which depends on the
// output range selected by the user as shown in Table 7.
uint32_t D2a::voltage2reg( float voltage )
{
	// D = ( Vout * 2^n ) / ( Vref * Gain )

	// Gain for 10.8V is 4.32
	// n = 16 bits
	// Vref = 2.5V
	return (uint32_t)(( voltage * 65535.0f ) / ( 2.5f * 4.32f ));
}

void D2a::ldacHigh( void )
{
	gpioWrite( ldac_, PI_HIGH );
}
void D2a::ldacLow( void )
{
	gpioWrite( ldac_, PI_LOW );
}


// user level command to set the output between 0-100%
int D2a::set( char dac, float percent )
{
	uint32_t mydac = 0;

	float voltage = percent2Voltage( percent );
	uint32_t reg = voltage2reg( voltage );

	cout << "DAC: " << dac << ", percent: " << percent << endl;
	//cout << "voltage: " << voltage << ", reg: " <<  reg << endl;

	if( dac == 'a' )
		mydac = DAC_A;
	else
	if( dac == 'b' )
		mydac = DAC_B;
	else
	if( dac == 'c' )
		mydac = DAC_C;

	// write the output value to the selected dac
	int retVal = write( DAC_WRITE | REG_DAC_VALUE | mydac | reg );

	if ( retVal == 3 )
	{
		// update the dac output buffer
		return write ( DAC_WRITE | REG_CTRL | CTRL_REG_LOAD );
	}
	else
	{
		return retVal;
	}
}

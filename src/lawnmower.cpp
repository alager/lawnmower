#include "lawnmower.h"
#include "d2a.h"
#include "a2d.h"
#include <unistd.h>	// for sleep()

int main() 
{
	uint8_t buff[ 8 ] = { 0xaa, 0x23, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f };
	// Create a file stream object
	// std::ofstream outfile("hello_world.txt");

	// Write the string "hello world" to the file
	// outfile << "hello world4";

	// Close the file stream object
	// outfile.close();

	cout << "pre init" << endl;
	if (gpioInitialise()<0) 
		return 1;

	cout << "post init" << endl;
	
	// int handle_ = spiOpen( 0, BAUD_32K, 0 );
	// create a d2a object
	d2a ad5754( SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS_D2A );

	cout << "writing to SPI " << sizeof( buff ) << " bytes" << endl;
	ad5754.write( buff, sizeof( buff ) );
	// spiWrite( ( unsigned int )handle_, (char *)buff, sizeof( buff ) );


	// this is the end
	gpioTerminate();
	return 0;
}

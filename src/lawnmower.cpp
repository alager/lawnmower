#include "lawnmower.h"
#include "d2a.h"
#include "a2d.h"
#include <unistd.h>	// for sleep()

int main() 
{
	// uint32_t buff = { 0xaabb1155 };
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
	d2a ad5754( SPI_LDAC );
	int foo = ad5754.set( 'a', 25 );
	cout << "set return: " << foo << endl;

	// cout << "writing to SPI " << sizeof( buff ) << " bytes" << endl;
	// cout << "write return: " << ad5754.write( buff ) << endl;

	// create a a2d object
	// a2d ads8678( SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS_D2A );


	// this is the end
	gpioTerminate();
	return 0;
}

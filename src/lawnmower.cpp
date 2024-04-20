#include "lawnmower.h"

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


	cout << "post init" << endl;
	
	// create a motors object
	Motors mtr = Motors();
	

	mtr.forward( 50.5 );

	sleep( 15 );
	mtr.estop( );

	// create a a2d object
	// a2d ads8678( SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS_D2A );


	// this is the end
	gpioTerminate();
	return 0;
}


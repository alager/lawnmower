#include "lawnmower.h"

#include <unistd.h>	// for sleep()

// create a global motors object
Motors mtr = Motors();

int main() 
{
	// uint32_t buff = { 0xaabb1155 };
	// Create a file stream object
	// std::ofstream outfile("hello_world.txt");

	// Write the string "hello world" to the file
	// outfile << "hello world4";

	// Close the file stream object
	// outfile.close();

	mtr.init();
	cout << "post init" << endl;
	
	
	

	mtr.forward( 16.5 );

	sleep( 60 );
	// usleep( 500000 ); // 0.5s
	mtr.estop( );

	// create a a2d object
	// a2d ads8678( SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS_D2A );


	// this is the end
	gpioTerminate();
	return 0;
}


#include "lawnmower.h"

// create a global motors object
Motors* mtr = new Motors();

int main() 
{
	// uint32_t buff = { 0xaabb1155 };
	// Create a file stream object
	// std::ofstream outfile("hello_world.txt");

	// Write the string "hello world" to the file
	// outfile << "hello world4";

	// Close the file stream object
	// outfile.close();

	// Set up the signal handler
	signal(SIGINT, sigINT_handler);

	mtr->init();
	cout << "post init" << endl;

	mtr->forward( 23.0f );
	sleep( 5 );
	mtr->forward( 16.0f );
	sleep( 5 );
	// usleep( 500000 ); // 0.5s
	mtr->estop( );

	// this is the end
	gpioTerminate();
	return 0;
}


// turn off esential GPIO
// void sigINT_handler( Motors *mtr, int signum )
void sigINT_handler( int signum )
{
	delete mtr;			// invoke destructor
	exit( signum );		// exit program
}

#include "lawnmower.h"


int main() {
	// Create a file stream object
	std::ofstream outfile("hello_world.txt");

	// Write the string "hello world" to the file
	outfile << "hello world4";

	// Close the file stream object
	outfile.close();

	cout << "pre init" << endl;
	if (gpioInitialise()<0) 
		return 1;

	cout << "post init" << endl;
	gpioSetMode(HALL, PI_INPUT);
	gpioSetPullUpDown(HALL, PI_PUD_UP);

	printf("GPIO14 is level %d\n", gpioRead(HALL));
	gpioSetPullUpDown(HALL, PI_PUD_DOWN);
	printf("GPIO14 is level %d\n", gpioRead(HALL));


	//gpioSetAlertFunc(HALL, alert);

	//sleep(secs);

	gpioTerminate();
	return 0;
}

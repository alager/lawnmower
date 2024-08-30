#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
#include <csignal>


#include <pigpio.h>
#include "motors.h"

// Pi GPIO defines
#define SPI_CLK		(11)
#define SPI_MISO	(9)
#define SPI_MOSI	(10)
#define SPI_CS_D2A	(8)


void signal_handler(int signum);

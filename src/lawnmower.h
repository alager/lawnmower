#include <functional>
#include <iostream>
using std::cout;
using std::endl;
using std::bind;
// using namespace placeholders;
#include <fstream>
#include <csignal>	// to catch sigINT
#include <unistd.h>	// for sleep()
#include "crow_all.h"



#include <pigpio.h>
#include "motors.h"

// Pi GPIO defines
#define SPI_CLK		(11)
#define SPI_MISO	(9)
#define SPI_MOSI	(10)
#define SPI_CS_D2A	(8)


void sigINT_handler( int signum);
void msleep( uint16_t time );
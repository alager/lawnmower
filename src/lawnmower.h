#include <iostream>
using std::cout;
using std::endl;
#include <fstream>

#include <pigpio.h>

#define SPI_CLK		(11)
#define SPI_MISO	(9)
#define SPI_MOSI	(10)
#define SPI_CS_D2A	(8)

#define BAUD_32K	( 32000 )
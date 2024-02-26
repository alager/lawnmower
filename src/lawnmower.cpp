#include <iostream>
#include <fstream>

#include <pigpio.h>

int main() {
  // Create a file stream object
  std::ofstream outfile("hello_world.txt");

  // Write the string "hello world" to the file
  outfile << "hello world2";

  // Close the file stream object
  outfile.close();

  return 0;
}

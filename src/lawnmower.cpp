#include <iostream>
#include <fstream>

int main() {
  // Create a file stream object
  std::ofstream outfile("hello_world.txt");

  // Write the string "hello world" to the file
  outfile << "hello world";

  // Close the file stream object
  outfile.close();

  return 0;
}

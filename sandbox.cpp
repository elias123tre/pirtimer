#include <iostream>
#include <fstream>

double timeout()
{
  std::ifstream fs;
  fs.open("./timeout.val");
  if (fs.is_open())
  {
    double num = 0.0;
    fs >> num;
    return num;
  }
  else
  {
    std::cerr << "Error opening file" << std::endl;
    return -1;
  }
}

int main(/* int argc, char const *argv[] */)
{
  std::cout << timeout() << std::endl;
  return 0;
}

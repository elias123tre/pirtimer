#include <utility>
#include <cstdint>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <list>

class config
{
private:
  std::list<std::pair<char *, uint32_t>> values;
  std::fstream file;

public:
  config(char *filename);
  ~config();

  int get(char *key);
  void set(char *key);
};

config::config(char *filename)
{
  file.unsetf(std::ios_base::skipws);
  file.open(filename);
  if (file.is_open())
  {
    std::string line;
    while (getline(file, line))
    {
      char *line_str;
      char *key;
      key = std::strtok(strcpy(line_str, line.c_str()), "=");
      while (key)
      {
        key = strtok(NULL, "=");
      }
      values.push_back(std::pair<char *, uint32_t>("Test", 32));
    }
  }
}

int get(char *key)
{
  return 0;
}

void set(char *key)
{
}

config::~config()
{
  file.close();
  // delete[] values;
}

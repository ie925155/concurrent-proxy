#include "string-utils.h"
#include <locale>

using namespace std;

string trim(string &str)
{
  size_t first = str.find_first_not_of(' ');
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

string rtrim(string &str)
{
  size_t last = str.find_last_not_of(' ');
  return str.substr(0, last);
}

std::string toLowerCase(const std::string &str)
{
  string tmp(str);
  locale loc;
  string::size_type len = str.length();
  for (string::size_type i=0; i<len; i++)
    std::tolower(tmp[i],loc);
  return tmp;
}

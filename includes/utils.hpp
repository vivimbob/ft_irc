#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstring>

namespace utils
{
  bool is_nickname_valid(std::string nick);
  bool is_letter(char c);
  bool is_digit(char c);
  bool is_special(char c);
}

#endif /* UTILS_HPP */
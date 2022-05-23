#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstring>
#include <vector>
#include <sstream>

namespace utils
{
  bool is_nickname_valid(std::string nick);
  bool is_letter(char c);
  bool is_digit(char c);
  bool is_special(char c);
  void split_by_comma(std::vector<const std::string> &splited_params, const std::string &params);
}

#endif /* UTILS_HPP */
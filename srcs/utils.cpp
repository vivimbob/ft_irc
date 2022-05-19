#include "../includes/utils.hpp"

namespace utils
{
  bool
    is_nickname_valid(std::string nick)
  {
    if (nick.length() > 9)
      return false;
    size_t index = 0;
    if (!is_letter(nick[index]))
      return false;
    ++index;
    for (; index < nick.length(); ++index)
      if (!is_letter(nick[index]) && !is_special(nick[index]) && !is_digit(nick[index]))
        return false;
    return true;
  }

  bool
    is_letter(char c)
  {
    return std::isalpha(c);
  }

  bool
    is_digit(char c)
  {
    return std::isdigit(c);
  }

  bool
    is_special(char c)
  {
    const char* const special_lists = "[]\\`-^{|}";

    return std::memchr(special_lists, c, sizeof(special_lists));
  }
}
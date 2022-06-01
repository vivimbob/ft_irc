#include "../includes/utils.hpp"

namespace utils
{
  static inline bool
    is_special(char c)
  {
    const char* const special_lists = "[]\\`-^{|}";

    return std::memchr(special_lists, c, sizeof(special_lists));
  }
  
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
      if (!std::isalpha(nick[index]) && !std:isdigit(nick[index]) && !is_special(nick[index]))
        return false;
    return true;
  }

  bool
    is_channel_prefix(const std::string &chan)
  {
    return (chan[0] == '#' || chan[0] == '&');
  }

  bool
    is_channel_name_valid(const std::string &chan)
  {
    if (chan.length() > 50)
      return false;
    size_t index = 0;
    for (; index < chan.length(); ++index)
      if (chan[index] == ' ' || chan[index] == ',' || chan[index] == 0x7)
        return false;
    return true;
  }
  
  void
    split_by_comma(std::vector<const std::string> &splited_params, const std::string &params)
  {
    std::istringstream iss(params);

    std::string elem;
    while (std::getline(iss, elem, ','))
        splited_params.push_back(elem);
  }
}

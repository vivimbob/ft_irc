#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>

typedef std::map<const std::string, const std::string> ChannelKeyPairMap;

namespace utils
{
  bool is_nickname_valid(std::string nick); bool is_channel_prefix(const std::string &chan);
  bool is_channel_name_valid(const std::string &chan);
  void split_by_comma(std::vector<const std::string> &splited_params, const std::string &params);
}

#endif /* UTILS_HPP */

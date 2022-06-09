#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>
#include "channel.hpp"

typedef std::map<const std::string, const std::string> ChannelKeyPairMap;

class Channel;

namespace utils
{
  struct ClientInfo
  {
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string servername;
  };

  bool is_nickname_valid(std::string nick);
  bool is_channel_prefix(const std::string &chan);
  bool is_channel_name_valid(const std::string &chan);
  std::string attach_channel_symbol(Channel *channel);
  void split_by_comma(std::vector<const std::string> &splited_params, const std::string &params);
  ClientInfo parse_client_info(std::string client_str);
}

#endif /* UTILS_HPP */

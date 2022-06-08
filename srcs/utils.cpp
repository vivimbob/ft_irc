#include "../includes/utils.hpp"

namespace utils
{
  static inline bool
    is_special(char c)
  {
    return std::memchr("[]\\`-^{|}", c, 8); 
  }
  
   bool
    is_nickname_valid(std::string nick)
  {
    if (nick.length() > 9)
      return false;
    if (!std::isalpha(nick[0]))
      return false;
    for (size_t index = 1; index < nick.length(); ++index)
      if (!std::isalpha(nick[index]) && !std::isdigit(nick[index]) && !is_special(nick[index]))
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
    for (size_t index = 0; index < chan.length(); ++index)
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

  ClientInfo
    parse_client_info(std::string client_info)
  {
    const char *offset_exclamation_mark = strchr(client_info.data(), '!'); 
    const char *offset_percent_mark= strchr(client_info.data(), '!'); 
    // \! 가 있는 경우
      // 뒤에 @이 있어야함
      // 앞에 있는 건 nick
      // /! @ 사이있는 건 user
      // @ 뒤는 host
    // %가 있는 경우
      // 뒤에 @가 있을 수도 있음
      // 맨 앞은 user
      // % 뒤는 host
      // @ 이 있는 경우 servername
    // @가 있는 경우
      // 앞에서 나머지 경우가 다 처리 됐으니
      // user@severname 케이스만 처리하면 됨
    // 어떤 기호도 없는 경우
      //nickname임
  }
}

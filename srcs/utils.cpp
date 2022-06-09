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

  std::string
    attach_channel_symbol(Channel *channel)
  {
    if (channel->is_private_mode())
      return "*";
    else if (channel->is_secret_mode())
      return "@";
    return "=";
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
    parse_client_info(std::string client_str)
  {
    const char *offset_exclamation_mark = strchr(client_str.data(), '!');
    const char *offset_percent_sign = strchr(client_str.data(), '%'); 
    const char *offset_at_sign = strchr(client_str.data(), '@'); 
	ClientInfo client;

	if (offset_exclamation_mark && offset_exclamation_mark < offset_at_sign) // nick!user@host
	{
		client.nickname = client_str.substr(0, offset_exclamation_mark - client_str.data());
		client.username = client_str.substr(offset_exclamation_mark - client_str.data() + 1, offset_at_sign - offset_exclamation_mark - 1);
		client.hostname = client_str.substr(offset_at_sign - client_str.data() + 1);
	}
	else if (offset_percent_sign)
	{
		if (!offset_at_sign) // user%host
		{
			client.username = client_str.substr(0, offset_percent_sign - client_str.data());
			client.hostname = client_str.substr(offset_percent_sign - client_str.data() + 1);
		}
		else if (offset_percent_sign < offset_at_sign) // user%host@servername
		{
			client.username = client_str.substr(0, offset_percent_sign - client_str.data());
			client.hostname = client_str.substr(offset_percent_sign - client_str.data() + 1, offset_at_sign - offset_percent_sign - 1);
			client.servername = client_str.substr(offset_at_sign - client_str.data() + 1);
		}
	}
	else if (offset_at_sign) //user@servsername
	{
		client.username = client_str.substr(0, offset_at_sign - client_str.data());
		client.servername = client_str.substr(offset_at_sign - client_str.data() + 1);
	}
	else //nickname
		client.nickname = client_str;
	return client;
  }
}

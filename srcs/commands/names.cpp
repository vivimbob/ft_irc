#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/server.hpp"

void
  Server::m_process_names_command(Client &client, Message &msg)
{
  (void)client;
  if (msg.get_params().empty())
  {
    ChannelMap::const_iterator channel_it = m_channel_map.begin();
    std::vector<std::string> asterisk_channel;
    for (; channel_it != m_channel_map.end(); ++channel_it)
    {
      const std::string &channel_name = channel_it->first;
      Channel *channel = channel_it->second;
      if (channel->is_private_mode() || channel->is_secret_mode())
        asterisk_channel.push_back(channel_name);
      
    }

  }
}
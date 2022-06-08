#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void iterate_channel(Channel *channel, Client &client, Message &msg)
{
  const size_t number_of_clients = channel->get_user_list().size();
  if (client.is_already_joined(channel) || (!channel->is_private_mode() && !channel->is_secret_mode()))
  {
    const std::string &symbol = utils::get_channel_symbol(channel);
    const std::string &topic = channel->get_channel_topic();
    client.push_message(msg.rpl_list(symbol + channel->get_channel_name(), std::to_string(number_of_clients), topic));
  }
  else if (channel->is_private_mode())
    client.push_message(msg.rpl_list("Prv", std::to_string(number_of_clients), ""));
}

void Server::m_process_list_command(Client &client, Message &msg)
{
  if (msg.get_params().empty())
  {
    ChannelMap::const_iterator channel_it = m_channel_map.begin();
    Channel *channel = channel_it->second;
    for (; channel_it != m_channel_map.end(); ++channel_it)
      iterate_channel(channel, client, msg);
  }
  else if (msg.get_params().size() == 1)
  {
    std::vector<const std::string> channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);
    std::vector<const std::string>::const_iterator channel_it = channel_list.begin();
    for (; channel_it != channel_list.end(); ++channel_it)
    {
      if (m_channel_map.count(*channel_it))
        iterate_channel(m_channel_map[*channel_it], client, msg);
    }
  }
  client.push_message(msg.rpl_listend());
}
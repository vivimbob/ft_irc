#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void Server::m_process_invite_command(Client &client, Message &msg)
{
  if (msg.get_params().size() < 2)
  {
    client.push_message(msg.err_need_more_params(), Logger::Debug);
    return ;
  }
  else if (msg.get_params().size() == 2)
  {
    const std::string nickname = msg.get_params()[0];
    if (!m_client_map.count(nickname))
    {
      client.push_message(msg.err_no_such_nick(nickname), Logger::Debug);
      return ;
    }
    const std::string channel_name = msg.get_params()[1];
    if (!m_channel_map.count(channel_name))
    {
      client.push_message(msg.err_no_such_channel(channel_name), Logger::Debug);
      return ;
    }
    Channel *channel = m_channel_map[channel_name];
    if (!client.is_already_joined(channel))
    {
      client.push_message(msg.err_not_on_channel(channel_name), Logger::Debug);
      return ;
    }
    if (m_client_map[nickname]->is_already_joined(channel))
    {
      client.push_message(msg.err_user_on_channel(nickname, channel_name), Logger::Debug);
      return ;
    }
    if (channel->is_invite_only_mode() && !channel->is_operator(client))
    {
      client.push_message(msg.err_chanoprivs_needed(channel_name), Logger::Debug);
      return ;
    }
    client.push_message(msg.rpl_inviting(channel_name, nickname));
  }
}
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
  const std::string nickname = msg.get_params()[0];
  const std::string channel_name = msg.get_params()[1];
  Channel *channel = m_channel_map[channel_name];
  Client *target_client = m_client_map[nickname];

  if (!m_client_map.count(nickname))
  {
    client.push_message(msg.err_no_such_nick(nickname), Logger::Debug);
    return ;
  }
  if (!m_channel_map.count(channel_name))
  {
    client.push_message(msg.err_no_such_channel(channel_name), Logger::Debug);
    return ;
  }
  if (!client.is_already_joined(channel))
  {
    client.push_message(msg.err_not_on_channel(channel_name), Logger::Debug);
    return ;
  }
  if (target_client->is_already_joined(channel))
  {
    client.push_message(msg.err_user_on_channel(nickname, channel_name), Logger::Debug);
    return ;
  }
  if (channel->is_invite_only_mode() && !channel->is_operator(client))
  {
    client.push_message(msg.err_chanoprivs_needed(channel_name), Logger::Debug);
    return ;
  }
  client.push_message(msg.rpl_inviting(nickname, channel_name));
  m_prepare_to_send(*target_client, msg.build_invite_reply(nickname, channel_name));
  channel->add_user_invitation_list(*target_client);
}
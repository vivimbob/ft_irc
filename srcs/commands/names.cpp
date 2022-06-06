#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/server.hpp"

void
  Server::m_process_names_command(Client &client, Message &msg)
{
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
      else
      {
        const Channel::MemberMap &user_list = channel->get_user_list();
        Channel::MemberMap::const_iterator user = user_list.begin();
        std::queue<const std::string> nick_queue;
        for (; user != user_list.end(); ++user)
        {
            if (!user->first->is_invisible())
            {
              if (channel->is_operator(user->first))
                nick_queue.push("@" + user->first->get_nickname());
              else if (channel->is_voice_mode(user->first))
                nick_queue.push("+" + user->first->get_nickname());
              else
                nick_queue.push(user->first->get_nickname());
            }
        }
        client.push_message(msg.rpl_namreply("=" + channel_name, nick_queue));
      }
    }
    std::vector<std::string>::const_iterator asterisk_channel_it = asterisk_channel.begin();
    const Channel::MemberMap &user_list = m_channel_map[*asterisk_channel_it]->get_user_list();
    Channel::MemberMap::const_iterator user = user_list.begin();
    std::queue<const std::string> nick_queue;
    for (;asterisk_channel_it != asterisk_channel.end(); ++asterisk_channel_it)
    {
      if (!user->first->is_invisible())
        nick_queue.push(user->first->get_nickname());
    }
    ClientMap::const_iterator client_it = m_client_map.begin();
    for (;client_it != m_client_map.end(); ++client_it)
    {
      if (client_it->second->get_channel_list().empty())
        nick_queue.push(client_it->first);
    }
    client.push_message(msg.rpl_namreply("*", nick_queue));
    client.push_message(msg.rpl_endofnames("*"));
  }
}
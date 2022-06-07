#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/server.hpp"

inline std::string
  get_channel_symbol(Channel *channel)
{
  if (channel->is_private_mode())
    return "*";
  else if (channel->is_secret_mode())
    return "@";
  return "=";
}

inline void
  store_nickname_in_queue(Channel *channel, Client &client, std::queue<const std::string> &nick_queue)
{
  if (channel->is_operator(client))
    nick_queue.push("@" + client.get_nickname());
  else if (channel->is_voice_mode(client))
    nick_queue.push("+" + client.get_nickname());
  else
    nick_queue.push(client.get_nickname());
}

inline void
  iterate_user_not_joined(Channel *channel, std::queue<const std::string>& nick_queue, bool is_public = false)
{
  const Channel::MemberMap &user_list = channel->get_user_list();
  Channel::MemberMap::const_iterator user = user_list.begin();
  for (; user != user_list.end(); ++user)
  {
    if (!user->first->is_invisible())
    {
      if (is_public)
        store_nickname_in_queue(channel, *user->first, nick_queue);
      else
        nick_queue.push(user->first->get_nickname());
    }
  }
}

inline void
  iterate_user_joined(Channel *channel, std::queue<const std::string>& nick_queue)
{
  const Channel::MemberMap &user_list = channel->get_user_list();
  Channel::MemberMap::const_iterator user = user_list.begin();
  for (; user != user_list.end(); ++user)
    store_nickname_in_queue(channel, *user->first, nick_queue);
}

void
  Server::m_process_names_command(Client &client, Message &msg)
{
  // 채널 이름 드러내는 경우
  if (msg.get_params().empty()) // 파라미터 없을 때
  {
    ChannelMap::const_iterator channel_it = m_channel_map.begin();
    std::vector<std::string> asterisk_channel;
    for (; channel_it != m_channel_map.end(); ++channel_it) // 현재 개설되어있는 채널 순회
    {
      const std::string &channel_name = channel_it->first;
      Channel *channel = channel_it->second;
      std::queue<const std::string> nick_queue;
      if (client.is_already_joined(channel)) // 해당 클라이언트가 채널에 가입되어 있을 때
      {
        std::string symbol = get_channel_symbol(channel);
        iterate_user_joined(channel, nick_queue);
        client.push_message(msg.rpl_namreply(symbol + channel_name, nick_queue));
      }
      else if (channel->is_private_mode() || channel->is_secret_mode()) // 가입 안되어있고 채널이 private, secret 모드일 때
        asterisk_channel.push_back(channel_name);
      else // 가입 안되어있고 채널이 public일 때
      {
        iterate_user_not_joined(channel, nick_queue, true);
        client.push_message(msg.rpl_namreply("=" + channel_name, nick_queue));
      }
    }
    // 채널이 private, secret 모드인데 클라이언트가 가입되어 있지 않거나
    // 클라이언트가 어느 채널에도 속하지 않을 때(채널 이름 *로 해야하는 경우)
    std::vector<std::string>::const_iterator asterisk_channel_it = asterisk_channel.begin();
    std::queue<const std::string> nick_queue;
    for (; asterisk_channel_it != asterisk_channel.end(); ++asterisk_channel_it) // 가입 안되어있고 채널이 private, secret 모드일 때
    {
      Channel *channel = m_channel_map[*asterisk_channel_it];
      iterate_user_not_joined(channel, nick_queue);
    }
    ClientMap::const_iterator client_it = m_client_map.begin();
    for (;client_it != m_client_map.end(); ++client_it) // 클라이언트가 어느 채널에도 속하지 않을 때
    {
      if (client_it->second->get_channel_list().empty())
        nick_queue.push(client_it->first);
    }
    client.push_message(msg.rpl_namreply("*", nick_queue));
    client.push_message(msg.rpl_endofnames("*"));
  }
  else if (msg.get_params().size() == 1) // 파라미터 있을 때
  {
    std::vector<const std::string> channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);
    std::vector<const std::string>::const_iterator channel_it = channel_list.begin();
    for (; channel_it != channel_list.end(); ++channel_it) // ','로 구분되어 저장된 채널 리스트 순회
    {
      if (!m_channel_map.count(*channel_it))
        continue;
      const std::string &channel_name = *channel_it;
      Channel *channel = m_channel_map[channel_name];
      std::queue<const std::string> nick_queue;
      if (client.is_already_joined(channel)) // 해당 클라이언트가 채널에 가입되어 있을 때
      {
        std::string symbol = get_channel_symbol(channel);
        iterate_user_joined(channel, nick_queue);
        client.push_message(msg.rpl_namreply(symbol + channel_name, nick_queue));
      }
      else // 가입되어 있지 않을 때
      {
        if (!channel->is_private_mode() && !channel->is_secret_mode()) // 일반(public) 채널일떄
        {
          iterate_user_not_joined(channel, nick_queue, true);
          client.push_message(msg.rpl_namreply("=" + channel_name, nick_queue));
        }
        else if (channel->is_private_mode()) // private 채널일 때
        {
          iterate_user_not_joined(channel, nick_queue);
          client.push_message(msg.rpl_namreply("*", nick_queue));
        }
        else // secret, 잘못된 채널일 때
          client.push_message(msg.rpl_endofnames(channel_name));
      }
    }
    client.push_message(msg.rpl_endofnames(*(--channel_it)));
  }
}
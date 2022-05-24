#include "../includes/channel.hpp"
#include "../includes/logger.hpp"

Channel::Channel(const std::string &name, Client &client)
 : m_channel_name(name),
   m_channel_init_time(std::time(NULL))
{
	mode.p = false;
	mode.s = false;
	mode.i = false;
	mode.t = false;
	mode.n = false;
	mode.m = false;
	mode.l = false;
	mode.b = false;
	mode.k = false;
}

Channel::~Channel()
{
}

const std::string&
  Channel::m_get_channel_name() const
{
  return m_channel_name;
}

const std::string&
  Channel::m_get_channel_topic() const
{
  return m_channel_topic;
}

const std::string&
  Channel::m_get_channel_mode() const
{
  return m_channel_mode;
}

const std::string&
  Channel::m_get_key() const
{
  return m_key;
}

const std::string& 
  Channel::m_get_user_mode(Client &client)
{
  return m_user_mode[client.m_get_socket()];
}

void
  Channel::m_set_channel_name(const std::string &name)
{
  this->m_channel_name = name;
}

void
  Channel::m_set_channel_topic(const std::string &topic)
{
  this->m_channel_topic = topic;
}

void
  Channel::m_set_channel_mode(const std::string &chan_mode)
{
  this->m_channel_mode = chan_mode;
}

void
  Channel::m_set_key(const std::string &key)
{
  this->m_key = key;
}

void
  Channel::m_set_user_mode(Client &client, std::string &user_mode)
{
  m_user_mode[client.m_get_socket()] = user_mode;
}

void
  Channel::m_join(Client &client)
{
  std::string temp_mode;
  if (!m_is_empty())
  {
    temp_mode = "o";
    m_operator_lists.push_back(&client);
    m_set_user_mode(client, temp_mode);
    m_oper_privilage = true;
  }
  m_user_lists[client.m_get_socket()] = &client;
  // server에서 channel topic, channel user list 전송
}

void Channel::m_invite(Client &client)
{
  m_invited = true;
}

void
  Channel::m_display_channel_info(void)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's name : " << this->m_channel_name;
  Logger().info() << "channel's init time : " << this->m_channel_init_time;
  Logger().info() << "channel's topic : " << this->m_channel_topic;
}

void
  Channel::m_display_topic(Client &client)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's topic : " << this->m_channel_topic;
}

void
  Channel::m_display_names(Client &client)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's name : " << this->m_channel_name;
}

bool
  Channel::m_is_empty() const
{
  return m_user_lists.empty();
}

void
  Channel::m_add_operator(Client &client)
{
  m_operator_lists.push_back(&client);
  m_user_mode[client.m_get_socket()] = "o";
}

void
  Channel::m_delete_operator(Client &client)
{
  if (m_user_mode[client.m_get_socket()] == "-o")
  {
    size_t i;
    for (i = 0; i < m_operator_lists.size(); ++i)
      if (m_operator_lists[i] == &client)
        break;
    m_operator_lists.erase(m_operator_lists.begin() + i);
  }
}

void
  Channel::m_add_user(Client &client)
{
  m_user_lists[client.m_get_socket()] = &client;
}

void
  Channel::m_delete_user(Client &client)
{
  m_user_lists.erase(m_user_lists.find(client.m_get_socket()));
}


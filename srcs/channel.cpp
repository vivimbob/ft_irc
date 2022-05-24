#include "../includes/channel.hpp"
#include "../includes/logger.hpp"
#include <utility>

Channel::Channel(const std::string &name, const std::string &key, Client &client)
 : m_channel_name(name),
  m_key(key), 
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
  m_user_limits = 42;
}

Channel::~Channel(void)
{
}

const std::string&
  Channel::m_get_channel_name(void) const
{
  return m_channel_name;
}

const std::string&
  Channel::m_get_channel_topic(void) const
{
  return m_channel_topic;
}

const std::string&
  Channel::m_get_key(void) const
{
  return m_key;
}

const size_t&
  Channel::m_get_user_limits(void) const
{
  return m_user_limits;
}

const std::map<Client*, MemberShip>&
  Channel::m_get_user_lists(void)
{
  return m_user_lists;
}

const bool
  Channel::m_get_mode_limit(void)
{
  return mode.l;
}

const bool
  Channel::m_get_mode_invite_only(void)
{
  return mode.i;
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
  Channel::m_set_key(const std::string &key)
{
  this->m_key = key;
}

void
  Channel::m_join(Client &client)
{
	m_add_user(client);
  if (m_user_lists.size() == 1)
	  m_add_operator(client);
  // server에서 channel topic, channel user list 전송
}

void Channel::m_invite(Client &client)
{
	mode.i = true;
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
	m_user_lists[&client].mode.o = true;
}

void
  Channel::m_delete_operator(Client &client)
{
	m_user_lists[&client].mode.o = false;
}

void
  Channel::m_add_user(Client &client)
{
  m_user_lists.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
  Channel::m_delete_user(Client &client)
{
  m_user_lists.erase(&client);
}


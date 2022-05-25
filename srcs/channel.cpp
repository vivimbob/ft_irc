#include "../includes/channel.hpp"
#include "../includes/logger.hpp"
#include <utility>

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
	mode.k = false;
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

std::string
  Channel::m_get_channel_mode(void)
{
	std::string message;

	message.push_back('+');
	if (mode.p)
		message.push_back('p');
	if (mode.s)
		message.push_back('s');
	if (mode.i)
		message.push_back('i');
	if (mode.t)
		message.push_back('t');
	if (mode.n)
		message.push_back('n');
	if (mode.m)
		message.push_back('m');
	if (mode.k)
		message.push_back('k');
	return message;
}

const std::string&
  Channel::m_get_key(void) const
{
  return m_key;
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

bool
  Channel::m_is_operator(Client &client)
{
	return m_user_lists.find(&client)->second.mode.o;
}

bool
  Channel::m_is_user_on_channel(Client *client)
{
	return m_user_lists.count(client);
}

void
  Channel::m_add_operator(Client &client)
{
	m_user_lists.find(&client)->second.mode.o = true;
}

void
  Channel::m_delete_operator(Client &client)
{
	m_user_lists.find(&client)->second.mode.o = true;
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

void
  Channel::m_set_private_flag(bool toggle)
{
	mode.p = toggle;
	if (mode.p == true && mode.s == true)
		mode.s = false;
}

void
  Channel::m_set_secret_flag(bool toggle)
{
	mode.s = toggle;
	if (mode.p == true && mode.s == true)
		mode.p = false;
}

void
  Channel::m_set_invite_flag(bool toggle)
{
	mode.i = toggle;
}

void
  Channel::m_set_topic_flag(bool toggle)
{
	mode.t = toggle;
}

void
  Channel::m_set_no_messages_flag(bool toggle)
{
	mode.n = toggle;
}

void
  Channel::m_set_moderate_flag(bool toggle)
{
	mode.m = toggle;
}

void
  Channel::m_set_key_flag(bool toggle, std::string key)
{
	mode.k = toggle;
	if (toggle == true)
		m_key = key;
}

void
  Channel::m_set_limit(size_t limit)
{
	m_user_limits = limit;
}

void
  Channel::m_set_operator_flag(bool toggle, Client *client)
{
	m_user_lists.find(client)->second.mode.o = toggle;
}

void
  Channel::m_set_voice_flag(bool toggle, Client *client)
{
	m_user_lists.find(client)->second.mode.v = toggle;
}

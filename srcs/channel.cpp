#include "../includes/channel.hpp"
#include "../includes/logger.hpp"
#include <utility>

Channel::Channel(const std::string &name, const std::string &key)
 : m_channel_name(name),
  m_channel_init_time(std::time(NULL)),
  m_key(key)
{
	m_mode.p = false;
	m_mode.s = false;
	m_mode.i = false;
	m_mode.t = false;
	m_mode.n = false;
	m_mode.m = false;
	m_mode.l = false;
	m_mode.k = key.empty() ? false : true;
  m_user_limits = 42;
}

Channel::~Channel(void)
{
}

const std::string&
  Channel::get_channel_name(void) const
{
  return m_channel_name;
}

const std::string&
  Channel::get_channel_topic(void) const
{
  return m_channel_topic;
}

std::string
  Channel::get_channel_mode(void)
{
	std::string message;

	message.push_back('+');
	if (m_mode.p)
		message.push_back('p');
	if (m_mode.s)
		message.push_back('s');
	if (m_mode.i)
		message.push_back('i');
	if (m_mode.t)
		message.push_back('t');
	if (m_mode.n)
		message.push_back('n');
	if (m_mode.m)
		message.push_back('m');
	if (m_mode.k)
		message.push_back('k');
	if (m_mode.l)
		message.push_back('l');
	return message;
}

const std::string&
  Channel::get_key(void) const
{
  return m_key;
}

const size_t&
  Channel::get_user_limits(void) const
{
  return m_user_limits;
}

const std::map<Client*, MemberShip>&
  Channel::get_user_lists(void)
{
  return m_user_lists;
}

void
  Channel::set_channel_name(const std::string &name)
{
  this->m_channel_name = name;
}

void
  Channel::set_channel_topic(const std::string &topic)
{
  this->m_channel_topic = topic;
}

void
  Channel::set_private_flag(bool toggle)
{
	m_mode.p = toggle;
	if (m_mode.p == true && m_mode.s == true)
		m_mode.s = false;
}

void
  Channel::set_secret_flag(bool toggle)
{
	m_mode.s = toggle;
	if (m_mode.p == true && m_mode.s == true)
		m_mode.p = false;
}

void
  Channel::set_invite_flag(bool toggle)
{
	m_mode.i = toggle;
}

void
  Channel::set_topic_flag(bool toggle)
{
	m_mode.t = toggle;
}

void
  Channel::set_no_messages_flag(bool toggle)
{
	m_mode.n = toggle;
}

void
  Channel::set_moderate_flag(bool toggle)
{
	m_mode.m = toggle;
}

void
  Channel::set_key_flag(bool toggle, std::string key)
{
	m_mode.k = toggle;
	if (toggle == true)
		m_key = key;
}

void
  Channel::set_limit(bool toggle, size_t limit)
{
	m_user_limits = limit;
	m_mode.l = toggle;
}

void
  Channel::set_operator_flag(bool toggle, Client *client)
{
	m_user_lists.find(client)->second.mode.o = toggle;
}

void
  Channel::set_voice_flag(bool toggle, Client *client)
{
	m_user_lists.find(client)->second.mode.v = toggle;
}

void
  Channel::join(Client &client)
{
	add_user(client);
  if (m_user_lists.size() == 1)
	  add_operator(client);
  // server에서 channel topic, channel user list 전송
}

void Channel::invite(void)
{
	m_mode.i = true;
}

void
  Channel::display_channel_info(void)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's name : " << this->m_channel_name;
  Logger().info() << "channel's init time : " << this->m_channel_init_time;
  Logger().info() << "channel's topic : " << this->m_channel_topic;
}

void
  Channel::display_topic(Client &client)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's topic : " << this->m_channel_topic;
  (void)client;
}

void
  Channel::display_names(Client &client)
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's name : " << this->m_channel_name;
  (void)client;
}

bool
  Channel::is_empty() const
{
  return m_user_lists.empty();
}

bool
  Channel::is_operator(Client &client)
{
	return m_user_lists.find(&client)->second.mode.o;
}

bool
  Channel::is_user_on_channel(Client *client)
{
	return m_user_lists.count(client);
}

bool
  Channel::is_protected_topic_mode(void)
{
	return m_mode.t;
}

bool
  Channel::is_limit_mode(void)
{
  return m_mode.l;
}

bool
  Channel::is_invite_only_mode(void)
{
  return m_mode.i;
}

bool
  Channel::is_key_mode(void)
{
  return m_mode.k;
}

void
  Channel::add_operator(Client &client)
{
	m_user_lists.find(&client)->second.mode.o = true;
}

void
  Channel::delete_operator(Client &client)
{
	m_user_lists.find(&client)->second.mode.o = true;
}

void
  Channel::add_user(Client &client)
{
  m_user_lists.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
  Channel::delete_user(Client &client)
{
  m_user_lists.erase(&client);
}

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
	m_user_limit = 42;
	m_mode_string_need_update = true;
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
  Channel::get_channel_mode(Client *client)
{
	if (m_mode_string_need_update)
	{
		m_mode_string.clear();
		m_mode_string.push_back('+');
		if (m_mode.p)
			m_mode_string.push_back('p');
		if (m_mode.s)
			m_mode_string.push_back('s');
		if (m_mode.i)
			m_mode_string.push_back('i');
		if (m_mode.t)
			m_mode_string.push_back('t');
		if (m_mode.n)
			m_mode_string.push_back('n');
		if (m_mode.m)
			m_mode_string.push_back('m');
		if (m_mode.k)
			m_mode_string.push_back('k');
		if (m_mode.l)
			m_mode_string.push_back('l');
		m_mode_string_need_update = false;
	}
	MemberShip &member = m_user_list.find(client)->second;
	std::string member_mode_string;
	if (member.mode.o)
		member_mode_string.push_back('o');
	if (member.mode.v)
		member_mode_string.push_back('v');
	return m_mode_string + member_mode_string;
}

const std::string&
  Channel::get_key(void) const
{
  return m_key;
}

const size_t&
  Channel::get_user_limit(void) const
{
  return m_user_limit;
}

const std::map<Client*, MemberShip>&
  Channel::get_user_list(void)
{
  return m_user_list;
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
	if (m_mode.p == toggle)
		return;
	m_mode.p = toggle;
	m_mode_string_need_update = true;
	if (m_mode.p == true && m_mode.s == true)
		m_mode.s = false;
}

void
  Channel::set_secret_flag(bool toggle)
{
	if (m_mode.s == toggle)
		return;
	m_mode.s = toggle;
	m_mode_string_need_update = true;
	if (m_mode.p == true && m_mode.s == true)
		m_mode.p = false;
}

void
  Channel::set_invite_flag(bool toggle)
{
	if (m_mode.i == toggle)
		return;
	m_mode.i = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_topic_flag(bool toggle)
{
	if (m_mode.t == toggle)
		return;
	m_mode.t = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_no_messages_flag(bool toggle)
{
	if (m_mode.n == toggle)
		return;
	m_mode.n = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_moderate_flag(bool toggle)
{
	if (m_mode.m == toggle)
		return;
	m_mode.m = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_key_flag(bool toggle, std::string key)
{
	if (toggle == true)
		m_key = key;
	if (m_mode.k == toggle)
		return;
	m_mode.k = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_limit(bool toggle, size_t limit)
{
	m_user_limit = limit;
	if (m_mode.l == toggle)
		return;
	m_mode.l = toggle;
	m_mode_string_need_update = true;
}

void
  Channel::set_operator_flag(bool toggle, Client *client)
{
	m_user_list.find(client)->second.mode.o = toggle;
}

void
  Channel::set_voice_flag(bool toggle, Client *client)
{
	m_user_list.find(client)->second.mode.v = toggle;
}

bool
  Channel::is_empty()
{
  return m_user_list.empty();
}

bool
  Channel::is_full()
{
  return m_user_list.size() >= m_user_limit;
}

bool
  Channel::is_operator(Client &client)
{
	return m_user_list.find(&client)->second.mode.o;
}

bool
  Channel::is_user_on_channel(Client *client)
{
	return m_user_list.count(client);
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
  Channel::add_user(Client &client)
{
  m_user_list.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
  Channel::delete_user(Client &client)
{
  m_user_list.erase(&client);
}

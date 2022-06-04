#include "../includes/client.hpp"
#include "../includes/logger.hpp"
#include "../includes/channel.hpp"

Client::Client(sockaddr_in client_addr, int client_fd)
  : m_client_addr(client_addr),
    m_client_fd(client_fd),
    m_pass_registered(false),
    m_nick_registered(false),
    m_user_registered(false)
{
	m_mode.invisible = false;
	m_mode.server_notice = false;
	m_mode.wallops = false;
	m_mode.operater = false;
	m_mode_string_need_update = true;
  m_channel_limits = 10;
}

Client::~Client(void)
{
  while (m_commands.size())
  {
    delete m_commands.front();
    m_commands.pop();
  }
}

sockaddr_in
  Client::get_client_addr(void)
{
  return m_client_addr;
}

int
  Client::get_socket(void)
{
  return m_client_fd;
}

char*
  Client::get_client_IP(void)
{
  return inet_ntoa(m_client_addr.sin_addr);
}

const std::string&
  Client::get_nickname(void) const
{
  return m_nickname;
}

const std::string&
  Client::get_username(void) const
{
  return m_username;
}

const std::string&
  Client::get_hostname(void) const
{
  return m_hostname;
}

std::queue<Message *>&
  Client::get_commands(void)
{
  return m_commands;
}

std::string&
  Client::get_recv_buffer(void)
{
  return m_recv_buffer;
}

SendBuffer&
  Client::get_send_buffer(void)
{
  return m_send_buffer;
}

size_t& Client::get_channel_limits(void)
{
  return m_channel_limits;
}

std::string
  Client::get_usermode(void)
{
	if (m_mode_string_need_update)
	{
		m_mode_string.clear();
		m_mode_string.push_back('+');
		if (m_mode.invisible)
			m_mode_string.push_back('i');
		if (m_mode.operater)
			m_mode_string.push_back('o');
		if (m_mode.server_notice)
			m_mode_string.push_back('s');
		if (m_mode.wallops)
			m_mode_string.push_back('w');
		m_mode_string_need_update = false;
	}
	return m_mode_string;
}

const std::set<Channel *>&
  Client::get_channel_list(void) const
{
  return m_channel_list;
}

void
  Client::set_nickname(const std::string &nickname)
{
  m_nickname = nickname;
  m_nick_registered = true;
}

void
  Client::set_username(const std::string &username)
{
  m_username = username;
  m_user_registered = true;
  Logger().debug() << get_client_IP()  << " set username to " << m_username;
}

void
  Client::set_hostname(const std::string &hostname)
{
  m_hostname = hostname;
  Logger().debug() << get_client_IP()  << " set hostname to " << m_hostname;
}

void
  Client::set_servername(const std::string &servername)
{
  m_servername = servername;
  Logger().debug() << get_client_IP()  << " set servername to " << m_servername;
}

void
  Client::set_realname(const std::string &realname)
{
  m_realname = realname;
  Logger().debug() << get_client_IP()  << " set realname to " << m_realname;
}

void
  Client::set_password_flag(void)
{
  m_pass_registered = true;
}

void
  Client::set_invisible_flag(bool toggle)
{
	if (m_mode.invisible == toggle)
		return ;
	m_mode.invisible = toggle;
	m_mode_string_need_update = true;
}

void
  Client::set_operator_flag(bool toggle)
{
	if (m_mode.operater == toggle)
		return ;
	m_mode.operater = toggle;
	m_mode_string_need_update = true;
}

void
  Client::set_server_notice_flag(bool toggle)
{
	if (m_mode.server_notice == toggle)
		return ;
	m_mode.server_notice = toggle;
	m_mode_string_need_update = true;
}

void
  Client::set_wallops_flag(bool toggle)
{
	if (m_mode.wallops == toggle)
		return ;
	m_mode.wallops = toggle;
	m_mode_string_need_update = true;
}

bool
  Client::is_registered(void) const
{
  return m_pass_registered & m_nick_registered & m_user_registered;
}

bool
  Client::is_pass_registered(void) const
{
  return m_pass_registered;
}

bool
  Client::is_nick_registered(void) const
{
  return m_nick_registered;
}

bool
  Client::is_user_registered(void) const
{
  return m_user_registered;
}

bool
  Client::is_join_available(void) const
{
	return m_channel_list.size() < m_channel_limits;
}

bool
  Client::is_already_joined(Channel *channel)
{
	return m_channel_list.count(channel);
}

void
  Client::push_message(const std::string &message)
{
	m_send_buffer.append(message);
}

void
  Client::push_message(const std::string &message, int level)
{
	m_send_buffer.append(message);
	Logger().log(level)
		<< "Server Send to " << m_nickname << " [" << message << ']';
}

std::string
  Client::make_nickmask(void)
{
  return m_nickname + '!' + m_username + '@' + m_hostname; 
}

void
  Client::insert_channel(Channel *channel)
{
	m_channel_list.insert(channel);
}

void
  Client::erase_channel(Channel *channel)
{
	m_channel_list.erase(channel);
}

void
  Client::leave_all_channel(void)
{
	std::set<Channel *>::iterator it = m_channel_list.begin();
	std::set<Channel *>::iterator ite = m_channel_list.end();

	for (;it != ite; ++it)
		(**it).delete_user(*this);
}

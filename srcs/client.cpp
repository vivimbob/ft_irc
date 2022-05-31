#include "../includes/client.hpp"
#include "../includes/logger.hpp"

Client::Client(sockaddr_in client_addr, int client_fd)
  : m_client_addr(client_addr),
    m_client_fd(client_fd),
    m_pass_registered(false),
    m_nick_registered(false),
    m_user_registered(false)
{
	m_mode.i = false;
	m_mode.s = false;
	m_mode.w = false;
	m_mode.o = false;
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
  Client::m_get_client_addr(void)
{
  return m_client_addr;
}

int
  Client::m_get_socket(void)
{
  return m_client_fd;
}

char*
  Client::m_get_client_IP(void)
{
  return inet_ntoa(m_client_addr.sin_addr);
}

const std::string&
  Client::m_get_password(void) const
{
  return m_password;
}

const std::string&
  Client::m_get_nickname(void) const
{
  return m_nickname;
}

const std::string&
  Client::m_get_username(void) const
{
  return m_username;
}

const std::string&
  Client::m_get_hostname(void) const
{
  return m_hostname;
}

std::string
  Client::m_get_usermode(void)
{
	std::string message;

	message.push_back('+');
	if (m_mode.i)
		message.push_back('i');
	if (m_mode.o)
		message.push_back('o');
	if (m_mode.s)
		message.push_back('s');
	if (m_mode.w)
		message.push_back('w');
	return message;
}

const std::map<const std::string, const std::string>&
  Client::m_get_channel_lists(void) const
{
  return m_chan_key_lists;
}

void
  Client::m_set_password(const std::string &pw)
{
  m_password = pw;
  m_pass_registered = true;
}

void
  Client::m_set_nickname(const std::string &nickname)
{
  m_nickname = nickname;
  m_nick_registered = true;
}

void
  Client::m_set_username(const std::string &username)
{
  m_username = username;
  m_user_registered = true;
}

void
  Client::m_set_hostname(const std::string &hostname)
{
  m_hostname = hostname;
}

bool
  Client::m_is_registered(void) const
{
  return m_pass_registered & m_nick_registered & m_user_registered;
}

bool
  Client::m_is_pass_registered(void) const
{
  return m_pass_registered;
}

bool
  Client::m_is_nick_registered(void) const
{
  return m_nick_registered;
}

bool
  Client::m_is_user_registered(void) const
{
  return m_user_registered;
}

void
  Client::append(const std::string &message, Logger::Level level)
{
	m_send_buffer.append(message);
}

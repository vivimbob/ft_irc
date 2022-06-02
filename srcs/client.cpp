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
  Client::get_password(void) const
{
  return m_password;
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

std::queue<IRCMessage *>&
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

ChannelKeyPairMap&
  Client::get_chan_key_lists(void)
{
  return m_chan_key_lists;
}

size_t& Client::get_channel_limits(void)
{
  return m_channel_limits;
}

std::string
  Client::get_usermode(void)
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

const std::set<const std::string>&
  Client::get_channel_list(void) const
{
  return m_channel_list;
}

void
  Client::set_password(const std::string &pw)
{
  m_password = pw;
  m_pass_registered = true;
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
}

void
  Client::set_hostname(const std::string &hostname)
{
  m_hostname = hostname;
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

void
  Client::set_invisible_flag(bool toggle)
{
  m_mode.i = toggle;
}

void
  Client::set_operator_flag(bool toggle)
{
  m_mode.o = toggle;
}

void
  Client::set_server_notice_flag(bool toggle)
{
  m_mode.s = toggle;
}

void
  Client::set_wallops_flag(bool toggle)
{
  m_mode.w = toggle;
}

std::string
  Client::make_nickmask(void)
{
  return m_nickname + '!' + m_username + '@' + m_hostname; 
}

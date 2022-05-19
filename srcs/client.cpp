#include "../includes/client.hpp"
#include "../includes/logger.hpp"

Client::Client(sockaddr_in client_addr, int client_fd)
  : m_client_addr(client_addr),
    m_client_fd(client_fd),
    m_nick_registered(false),
    m_user_registered(false)
{
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

void
  Client::m_set_password(const std::string &pw)
{
  m_password = pw;
}

void
  Client::m_set_nickname(const std::string &nickname)
{
  m_nickname = nickname;
}

void
  Client::m_set_username(const std::string &username)
{
  m_username = username;
}

const bool
  Client::m_is_nick_registered(void) const
{
  return m_nick_registered;
}

const bool
  Client::m_is_user_registered(void) const
{
  return m_user_registered;
}

void
  Client::m_set_nick_registered(bool b)
{
  m_nick_registered = b;
}

void
  Client::m_set_user_registered(bool b)
{
  m_user_registered = b;
}
#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include "../includes/logger.hpp"

Client::Client(sockaddr_in client_addr, int client_fd)
    : m_client_addr(client_addr),
      m_client_fd(client_fd),
      m_hostname("ft_irc"),
      m_servername("ft_irc"),
      m_pass_registered(false),
      m_nick_registered(false),
      m_user_registered(false)
{
}

Client::~Client()
{
    while (m_commands.size())
    {
        delete m_commands.front();
        m_commands.pop();
    }
}

sockaddr_in
    Client::get_client_addr()
{
    return m_client_addr;
}

int
    Client::get_socket()
{
    return m_client_fd;
}

char*
    Client::get_client_IP()
{
    return inet_ntoa(m_client_addr.sin_addr);
}

const std::string&
    Client::get_nickname() const
{
    return m_nickname;
}

const std::string&
    Client::get_username() const
{
    return m_username;
}

const std::string&
    Client::get_hostname() const
{
    return m_hostname;
}

std::queue<Message*>&
    Client::get_commands()
{
    return m_commands;
}

std::string&
    Client::get_recv_buffer()
{
    return m_recv_buffer;
}

SendBuffer&
    Client::get_send_buffer()
{
    return m_send_buffer;
}

size_t
    Client::get_channel_limits()
{
    return m_channel_limits;
}

const std::set<Channel*>&
    Client::get_channel_list() const
{
    return m_channel_list;
}

void
    Client::set_nickname(const std::string& nickname)
{
    m_nickname        = nickname;
    m_nick_registered = true;
}

void
    Client::set_username(const std::string& username)
{
    m_username        = username;
    m_user_registered = true;
    Logger().debug() << get_client_IP() << " set username to " << m_username;
}

void
    Client::set_realname(const std::string& realname)
{
    m_realname = realname;
    Logger().debug() << get_client_IP() << " set realname to " << m_realname;
}

void
    Client::set_password_flag()
{
    m_pass_registered = true;
}

bool
    Client::is_registered() const
{
    return m_pass_registered & m_nick_registered & m_user_registered;
}

bool
    Client::is_pass_registered() const
{
    return m_pass_registered;
}

bool
    Client::is_nick_registered() const
{
    return m_nick_registered;
}

bool
    Client::is_user_registered() const
{
    return m_user_registered;
}

bool
    Client::is_join_available() const
{
    return m_channel_list.size() < m_channel_limits;
}

bool
    Client::is_already_joined(Channel* channel)
{
    return m_channel_list.count(channel);
}

bool
    Client::is_same_client(utils::ClientInfo client_info)
{
    if (!client_info.nickname.empty() && m_nickname != client_info.nickname)
        return false;
    if (!client_info.username.empty() && m_username != client_info.username)
        return false;
    if (!client_info.hostname.empty() && m_hostname != client_info.hostname)
        return false;
    if (!client_info.servername.empty() &&
        m_servername != client_info.servername)
        return false;
    return true;
}
void
    Client::push_message(const std::string& message)
{
    m_send_buffer.append(message);
}

void
    Client::push_message(const std::string& message, int level)
{
    m_send_buffer.append(message);
    Logger().log(level) << "Server Send to " << m_nickname << " [" << message
                        << ']';
}

std::string
    Client::make_nickmask()
{
    return m_nickname + '!' + m_username + '@' + m_hostname;
}

void
    Client::insert_channel(Channel* channel)
{
    m_channel_list.insert(channel);
}

void
    Client::erase_channel(Channel* channel)
{
    m_channel_list.erase(channel);
}

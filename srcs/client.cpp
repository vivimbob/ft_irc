#include "../includes/client.hpp"
#include "../includes/channel.hpp"
#include "../includes/ft_ircd.hpp"

/* client class constructor and destructor begin */

Client::Client(sockaddr_in addr, int fd)
    : _addr(addr),
      _fd(fd),
      m_pass_registered(false),
      m_nick_registered(false),
      m_user_registered(false)
{
    _buffers.requests.from = this;
}

Client::~Client()
{
}

/* client class constructor and destructor end */

/* client class getter begin */

sockaddr_in
    Client::get_addr()
{
    return _addr;
}

int
    Client::get_socket()
{
    return _fd;
}

char*
    Client::get_IP()
{
    return inet_ntoa(_addr.sin_addr);
}

const Client::t_names&
    Client::get_names() const
{
    return _names;
}

std::queue<Message*>&
    Client::get_commands()
{
    return _commands;
}

Client::t_buffers&
    Client::get_buffers()
{
    return _buffers;
}

const std::set<Channel*>&
    Client::get_joined_list() const
{
    return _joined_list;
}

/* client class getter end */

/* client class setter begin */

void
    Client::set_nickname(const std::string& nickname)
{
    _names.nick       = nickname;
    m_nick_registered = true;
}

void
    Client::set_username(const std::string& username)
{
    _names.user       = username;
    m_user_registered = true;
}

void
    Client::set_realname(const std::string& realname)
{
    _names.real = realname;
}

void
    Client::set_password_flag()
{
    m_pass_registered = true;
}

/* client class setter end */

/* client class is_function begin */

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
    return _joined_list.size() < CHANNEL_USER_LIMIT;
}

bool
    Client::is_already_joined(Channel* channel)
{
    return _joined_list.count(channel);
}

/* client class is_function end */

/* client class other function begin */

void
    Client::push_message(const std::string& message)
{
    _buffers.to_client.append(message);
}

std::string
    Client::make_nickmask()
{
    return _names.nick + '!' + _names.user + '@' + _names.host;
}

void
    Client::insert_channel(Channel* channel)
{
    _joined_list.insert(channel);
}

void
    Client::erase_channel(Channel* channel)
{
    _joined_list.erase(channel);
}

/* client class other function end */

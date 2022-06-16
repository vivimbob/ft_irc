#include "../includes/channel.hpp"
#include "../lib/logger.hpp"
#include <utility>

/* channel class constructor and destructor begin */

Channel::Channel(const std::string& name) : m_channel_name(name)
{
}

Channel::~Channel()
{
}

/* channel class constructor and destructor end */

/* channel class getter begin */

const std::string&
    Channel::get_channel_name() const
{
    return m_channel_name;
}

const std::string&
    Channel::get_channel_topic() const
{
    return m_channel_topic;
}

const std::map<Client*, MemberShip>&
    Channel::get_user_list()
{
    return m_member_list;
}

/* channel class getter end */

/* channel class setter begin */

void
    Channel::set_channel_name(const std::string& name)
{
    this->m_channel_name = name;
}

void
    Channel::set_channel_topic(const std::string& topic)
{
    this->m_channel_topic = topic;
}

void
    Channel::set_operator_flag(bool toggle, Client* client)
{
    m_member_list.find(client)->second.mode_operater = toggle;
}

/* channel class setter end */

/* channel class is_function begin */

bool
    Channel::is_empty()
{
    return m_member_list.empty();
}

bool
    Channel::is_full()
{
    return m_member_list.size() >= CHANNEL_USER_LIMIT;
}

bool
    Channel::is_operator(Client& client)
{
    if (m_member_list.find(&client) == m_member_list.end())
        return false;
    return m_member_list.find(&client)->second.mode_operater;
}

bool
    Channel::is_user_on_channel(Client* client)
{
    return m_member_list.count(client);
}

/* channel class is_function end */

/* channel class user function begin */

void
    Channel::add_user(Client& client)
{
    m_member_list.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
    Channel::delete_user(Client& client)
{
    m_member_list.erase(&client);
}

/* channel class user function end */

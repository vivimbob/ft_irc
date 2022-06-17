#include "../includes/channel.hpp"
#include "../lib/logger.hpp"
#include <utility>

/* channel class constructor and destructor begin */

Channel::Channel(const std::string& name) : _name(name)
{
}

Channel::~Channel()
{
}

/* channel class constructor and destructor end */

/* channel class getter begin */

const std::string&
    Channel::get_name() const
{
    return _name;
}

const std::string&
    Channel::get_topic() const
{
    return _topic;
}

const std::map<Client*, MemberShip>&
    Channel::get_user_list()
{
    return _member_list;
}

/* channel class getter end */

/* channel class setter begin */

void
    Channel::set_name(const std::string& name)
{
    this->_name = name;
}

void
    Channel::set_topic(const std::string& topic)
{
    this->_topic = topic;
}

void
    Channel::set_operator_flag(bool toggle, Client* client)
{
    _member_list.find(client)->second.mode_operater = toggle;
}

/* channel class setter end */

/* channel class is_function begin */

bool
    Channel::is_empty()
{
    return _member_list.empty();
}

bool
    Channel::is_full()
{
    return _member_list.size() >= CHANNEL_USER_LIMIT;
}

bool
    Channel::is_operator(Client& client)
{
    if (_member_list.find(&client) == _member_list.end())
        return false;
    return _member_list.find(&client)->second.mode_operater;
}

bool
    Channel::is_user_on_channel(Client* client)
{
    return _member_list.count(client);
}

/* channel class is_function end */

/* channel class user function begin */

void
    Channel::add_user(Client& client)
{
    _member_list.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
    Channel::delete_user(Client& client)
{
    _member_list.erase(&client);
}

/* channel class user function end */

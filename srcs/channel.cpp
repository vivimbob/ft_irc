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
    Channel::get_users()
{
    return _members;
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
    Channel::set_operator(Client* client)
{
    _members.find(client)->second.mode_operater = true;
}

/* channel class setter end */

/* channel class is_function begin */

bool
    Channel::is_empty()
{
    return _members.empty();
}

bool
    Channel::is_full()
{
    return _members.size() >= CHANNEL_USER_LIMIT;
}

bool
    Channel::is_operator(Client& client)
{
    if (_members.find(&client) == _members.end())
        return false;
    return _members.find(&client)->second.mode_operater;
}

bool
    Channel::is_joined(Client* client)
{
    return _members.count(client);
}

/* channel class is_function end */

/* channel class user function begin */

void
    Channel::join(Client& client)
{
    _members.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
    Channel::part(Client& client)
{
    _members.erase(&client);
}

/* channel class user function end */

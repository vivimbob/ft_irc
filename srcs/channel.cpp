#include "../includes/channel.hpp"
#include "../includes/client.hpp"

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

const Channel::t_membermap&
    Channel::get_members()
{
    return _members;
}

const std::string&
    Channel::get_prefix(Client* client)
{
    return _members[client];
}

bool
    Channel::get_status(TYPE type)
{
    if (type == INVITE)
        return _status.invite;
    else if (type == TOPIC)
        return _status.topic;
    else if (type == NOMSG)
        return _status.nomsg;
    return false;
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
    if (_members.find(client)->second.find('@') == std::string::npos)
        _members[client].insert(0, "@");
}

void
    Channel::set_status(TYPE type, bool state)
{
    switch (type)
    {
        case INVITE:
            _status.invite = state;
            break;
        case TOPIC:
            _status.topic = state;
            break;
        case NOMSG:
            _status.nomsg = state;
            break;
        default:
            break;
    }
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
    return (_members.find(&client)->second.find('@') != std::string::npos);
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
    _members.insert(std::make_pair(&client, t_membership()));
    client.joined(this);
}

void
    Channel::part(Client& client)
{
    _members.erase(&client);
    client.parted(this);
}

/* channel class user function end */

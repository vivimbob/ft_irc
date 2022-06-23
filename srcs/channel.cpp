#include "../includes/channel.hpp"
#include "../includes/client.hpp"
#include "../includes/log.hpp"

/* channel class constructor and destructor begin */

Channel::Channel(const std::string& name, Client* client) : _name(name)
{
    _status.state = 0;
    this->join(client);
    this->set_operator(client);
    log::print() << "new channel: " << name << log::endl;
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

std::string
    Channel::get_status()
{
    std::string str;
    if (_status.invite)
        str += 'i';
    if (_status.topic)
        str += 't';
    if (_status.nomsg)
        str += 'n';
    return str;
}

bool
    Channel::is_signed()
{
    if (_reserved.sign.state)
        return true;
    return false;
}

bool
    Channel::is_reserved()
{
    if (_reserved.flags.state)
        return true;
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

void
    Channel::set_status(std::string& result)
{
    std::string changed;
    bool        sign = _reserved.sign.positive ? true : false;

    if (_reserved.flags.invite && (sign != _status.invite))
    {
        _status.invite = sign;
        changed.push_back('i');
    }
    if (_reserved.flags.nomsg && (sign != _status.nomsg))
    {
        _status.nomsg = sign;
        changed.push_back('n');
    }
    if (_reserved.flags.topic && (sign != _status.topic))
    {
        _status.topic = sign;
        changed.push_back('t');
    }
    if (!changed.size())
        return;
    sign == true ? result.push_back('+') : result.push_back('-');
    result.append(changed);
}

void
    Channel::reserve_flags(const char c)
{
    switch (c)
    {
        case 'i':
            _reserved.flags.invite = true;
            break;
        case 'n':
            _reserved.flags.nomsg = true;
            break;
        case 't':
            _reserved.flags.topic = true;
            break;
        default:
            break;
    }
}

void
    Channel::reserve_sign(const char c)
{
    if (c == '+')
        _reserved.sign.positive = true;
    else
        _reserved.sign.negative = true;
}

void
    Channel::reserve_clear()
{
    _reserved.sign.state  = 0;
    _reserved.flags.state = 0;
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
    Channel::is_operator(Client* client)
{
    if (_members.find(client) == _members.end())
        return false;
    return (_members.find(client)->second.find('@') != std::string::npos);
}

bool
    Channel::is_joined(Client* client)
{
    return _members.count(client);
}

bool
    Channel::is_invited(Client* client)
{
    return _invitations.count(client);
}

/* channel class is_function end */

/* channel class user function begin */

void
    Channel::join(Client* client)
{
    _members.insert(std::make_pair(client, t_membership()));
    client->joined(this);
    _invitations.erase(client);
}

void
    Channel::part(Client* client)
{
    _members.erase(client);
    client->parted(this);
}

void
    Channel::invitation(Client* client)
{
    _invitations.insert(client);
}
/* channel class user function end */

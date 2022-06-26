#include "../includes/channel.hpp"
#include "../includes/client.hpp"
#include "../includes/log.hpp"

/* channel class constructor and destructor begin */

Channel::Channel(const std::string& name, Client* client)
    : _name(name),
      _operator(client)
{
    _status.state         = 0;
    _reserved.sign.state  = 0;
    _reserved.flags.state = 0;
    _operator->joined(this);
    log::print() << "new channel: " << name << log::endl;
}

Channel::~Channel()
{
}

/* channel class constructor and destructor end */

/* channel class getter begin */

const Channel::t_citer_member
    Channel::find(Client* client)
{
    t_citer_member iter = _members.begin();
    t_citer_member end  = _members.end();
    while (iter != end && *iter != client)
        ++iter;
    return iter;
}

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

const Channel::t_vector_member&
    Channel::get_members()
{
    return _members;
}

bool
    Channel::get_status(e_type type)
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

Client*
    Channel::get_operator()
{
    return _operator;
}

bool
    Channel::is_signed()
{
    return _reserved.sign.state;
}

bool
    Channel::is_reserved()
{
    return _reserved.flags.state;
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
    Channel::set_status(e_type type, bool state)
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
    return _members.empty() && (_operator == nullptr);
}

bool
    Channel::is_full()
{
    return (_members.size() + (_operator == nullptr ? 0 : 1)
            >= CHANNEL_USER_MAX);
}

bool
    Channel::is_operator(Client* client)
{
    return (_operator == client);
}

bool
    Channel::is_joined(Client* client)
{
    return (is_operator(client) || (find(client) != _members.end()));
}

bool
    Channel::is_invited(Client* client)
{
    return _invitees.count(client);
}

/* channel class is_function end */

/* channel class user function begin */

void
    Channel::join(Client* client)
{
    _members.push_back(client);
    client->joined(this);
    _invitees.erase(client);
}

void
    Channel::part(Client* client)
{
    if (is_operator(client))
        _operator = nullptr;
    else if (*find(client) == client)
        _members.erase(find(client));
    client->parted(this);
}

void
    Channel::invitation(Client* client)
{
    _invitees.insert(client);
}
/* channel class user function end */

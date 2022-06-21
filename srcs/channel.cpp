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

std::string
    Channel::set_status(t_status status)
{
    std::string on  = "+";
    std::string off = "-";

    if (_status.invite != status.invite)
    {
        _status.invite = status.invite;
        if (status.invite)
            on.push_back('i');
        else
            off.push_back('i');
    }
    if (_status.topic != status.topic)
    {
        _status.topic = status.topic;
        if (status.topic)
            on.push_back('t');
        else
            off.push_back('t');
    }
    if (_status.nomsg != status.nomsg)
    {
        _status.nomsg = status.nomsg;
        if (status.nomsg)
            on.push_back('n');
        else
            off.push_back('n');
    }
    return (on.size() > 1 ? on : "") + (off.size() > 1 ? off : "");
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

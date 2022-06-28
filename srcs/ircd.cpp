#include "../includes/ft_ircd.hpp"
#include "../includes/irc.hpp"
#include <sstream>

IRCD::t_cstr_vector
    IRCD::split(const std::string& params, char delimiter)
{
    IRCD::t_cstr_vector splited;
    std::istringstream  iss(params);
    std::string         element;

    while (std::getline(iss, element, delimiter))
        splited.push_back(element);
    return splited;
}

e_type
    IRCD::get_type(const std::string& command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return UNKNOWN;
}

static inline bool
    is_special(const char c)
{
    return std::memchr(SPECIALCHAR, c, 9);
}

e_result
    IRCD::m_is_valid(e_type type)
{
    if (type == NICK)
    {
        if (NICK_LENGTH_MAX < _target->length())
            return ERROR;
        if (!std::isalpha((*_target)[0]))
            return ERROR;
        for (size_t index = 1; index < _target->length(); ++index)
            if (!std::isalpha((*_target)[index])
                && !std::isdigit((*_target)[index])
                && !is_special((*_target)[index]))
                return ERROR;
    }
    if (type == CHANNEL_PREFIX)
    {
        if ((*_target)[0] != CHANNEL_PREFIX)
            return ERROR;
    }
    if (type == CHANNEL_NAME)
    {
        if (CHANNEL_LENGTH_MAX < _target->length())
            return ERROR;
        for (size_t index = 0; index < _target->length(); ++index)
            if (std::memchr(CHSTRING, (*_target)[index], 5))
                return ERROR;
    }
    return OK;
}

e_result
    IRCD::m_to_client(std::string message)
{
    _to_client->buffer.append(message);
    return ERROR;
}

void
    IRCD::m_to_client(Client& client, const std::string& message)
{
    client.get_buffers().to_client.buffer.append(message);
    _ft_ircd->toggle(client, EVFILT_READ);
}

void
    IRCD::m_to_channel(const std::string& message)
{
    Channel::t_citer_member iter = _channel->get_members().begin();
    Channel::t_citer_member end  = _channel->get_members().end();
    if (_channel->get_operator() && _channel->get_operator() != _client)
        m_to_client(*_channel->get_operator(), message);
    for (; iter != end; ++iter)
        if (*iter != _client)
            m_to_client(**iter, message);
}

void
    IRCD::m_to_channels(const std::string& message)
{
    Client::t_citer   iter = _client->get_channels().begin();
    Client::t_citer   end  = _client->get_channels().end();
    std::set<Client*> check;

    for (_channel = *iter; iter != end; _channel = *(++iter))
    {
        Channel::t_citer_member users = _channel->get_members().begin();
        Channel::t_citer_member u_end = _channel->get_members().end();
        for (; users != u_end; ++users)
            if (!check.count(*users) && *users != _client)
            {
                check.insert(*users);
                IRCD::m_to_client(**users, message);
            }
    }
}

void
    IRCD::m_disconnect(const std::string& message)
{
    _ft_ircd->m_disconnect(message);
}

void
    IRCD::parse_parameter(std::vector<std::string>& parameter)
{
    for (_offset = 0;
         (_index = _buffer.find_first_not_of(' ')) != (int)std::string::npos;)
    {
        _offset = _buffer.find_first_of(' ', _index);
        if ((_offset != (int)std::string::npos) && _buffer[_index] != ':')
            parameter.push_back(_buffer.substr(_index, _offset - _index));
        else
        {
            if (_buffer[_index] == ':')
                ++_index;
            parameter.push_back(_buffer.substr(_index));
            break;
        }
        _buffer.erase(0, _offset);
    }
    _buffer.clear();
}

void
    IRCD::parse_command(std::string& command)
{
    for (_offset = 0; (command[_offset] != ' ' && command[_offset] != '\0');
         ++_offset)
        if ((unsigned)command[_offset] - 'a' < 26)
            command[_offset] ^= 0b100000;
    _buffer = command.substr(_offset);
    command.erase(_offset);
}

void
    IRCD::parse_request(Client::t_request& request)
{
    _request = &request;
    if (_request->command.size() && (_request->command.front() == ':'))
    {
        _request->command.erase(0, _request->command.find_first_of(' '));
        _request->command.erase(0, _request->command.find_first_not_of(' '));
    }
    if (_request->command.size())
    {
        parse_command(_request->command);
        if (_buffer.size())
            parse_parameter(_request->parameter);
    }
}

void
    IRCD::registration()

{
    _map.client[_client->get_names().nick] = _client;
    m_to_client(rpl_welcome());
    log::print() << _client->get_names().nick << " is registered" << log::endl;
}

void
    IRCD::m_bot_initialize()
{
    _map.client[NAME_BOT] = &_bot;
}

IRCD::~IRCD()
{
}

IRCD::IRCD()
{
    _commands.push_back(&IRCD::empty);
    _commands.push_back(&IRCD::pass);
    _commands.push_back(&IRCD::nick);
    _commands.push_back(&IRCD::user);
    _commands.push_back(&IRCD::quit);
    _commands.push_back(&IRCD::join);
    _commands.push_back(&IRCD::part);
    _commands.push_back(&IRCD::topic);
    _commands.push_back(&IRCD::names);
    _commands.push_back(&IRCD::list);
    _commands.push_back(&IRCD::invite);
    _commands.push_back(&IRCD::kick);
    _commands.push_back(&IRCD::mode);
    _commands.push_back(&IRCD::privmsg);
    _commands.push_back(&IRCD::notice);
    _commands.push_back(&IRCD::ping);
    _commands.push_back(&IRCD::pong);
    _commands.push_back(&IRCD::unknown);
    _commands.push_back(&IRCD::unregistered);
    m_mode_initialize();
    m_bot_initialize();
}

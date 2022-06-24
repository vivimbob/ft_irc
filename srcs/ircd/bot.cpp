#include "../../includes/ircd.hpp"
#include <utility>

IRCD::Bot::Bot()
{
    _names.nick   = NAME_BOT;
    _names.host   = NAME_HOST;
    _names.server = NAME_SERVER;

    _commands.push_back(&Bot::m_help);
    _commands.push_back(&Bot::m_datetime);
    _commands.push_back(&Bot::m_game_coin);

    _command_to_type.insert(std::make_pair("/help", HELP));
    _command_to_type.insert(std::make_pair("/time", DATETIME));
    _command_to_type.insert(std::make_pair("/coin", COIN));
}

IRCD::Bot::~Bot()
{
}

std::string
    IRCD::Bot::get_prefix()
{
    return (":" + this->get_nickmask() + " ");
}

void
    IRCD::Bot::m_send(const std::string& message)
{
    _buffers.buffer.append(message);
}

void
    IRCD::Bot::m_help()
{
    m_send(get_prefix() + "command list : [/help, /datetime, /coin]\r\n");
}

void
    IRCD::Bot::m_datetime()
{
    time_t m_raw_time;
    char   buffer[50];
    time(&m_raw_time);
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&m_raw_time)))
        m_send(get_prefix() + "The current datetime is " + std::string(buffer)
               + "" + IRCD::Bot::_endl);
}

void
    IRCD::Bot::m_game_coin()
{
    m_send(get_prefix() + "The result of tossing a coin is "
           + std::string(std::rand() % 2 == 0 ? "head" : "tail") + " "
           + IRCD::Bot::_endl);
}

e_bot
    IRCD::Bot::m_get_type(const std::string& command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return NONE;
}

// :test!a@localhost PRIVMSG bot :/help

void
    IRCD::Bot::m_parse_command(std::string& command)
{
    int offset;

    command.erase(0, command.find_first_of(' '));
    command.erase(0, command.find_first_not_of(' '));

    for (offset = 0; (command[offset] != ' ' && command[offset] != '\0');
         ++offset)
        if ((unsigned)command[offset] - 'a' < 26)
            command[offset] ^= 0b100000;

    IRCD::Bot::_command = command.substr(0, offset);
    offset              = command.find_first_of(' ');

    command.erase(0, offset);

    command.erase(0, command.find_first_not_of(' '));
    command.erase(0, command.find_first_of(' '));

    command.erase(0, command.find_first_not_of(' '));
    if (command[0] == ':')
        offset = command.find_first_not_of(':');

    command.erase(0, offset);

    if ((_type = IRCD::Bot::m_get_type(command)) < NONE)
    {
        (this->*IRCD::Bot::_commands[_type])();
    }
    log::print() << _type << log::endl;
}

void
    IRCD::Bot::receive()
{
    log::print() << _buffers.to_client.buffer << log::endl;
    std::string buffer = _buffers.to_client.buffer;

    m_parse_command(buffer);

    _buffers.to_client.buffer.clear();
}

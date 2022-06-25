#include "../../includes/ircd.hpp"
#include <utility>
#include <vector>

IRCD::Bot::Bot()
{
    _names.nick   = NAME_BOT;
    _names.host   = NAME_HOST;
    _names.server = NAME_SERVER;

    _commands.push_back(&Bot::m_help);
    _commands.push_back(&Bot::m_datetime);
    _commands.push_back(&Bot::m_game_coin);

    _command_to_type.insert(std::make_pair(":/help\r\n", HELP));
    _command_to_type.insert(std::make_pair(":/time\r\n", DATETIME));
    _command_to_type.insert(std::make_pair(":/coin\r\n", COIN));
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

std::vector<std::string>
    split(const std::string& params, char delimiter)
{
    std::vector<std::string> splited;
    std::istringstream       iss(params);
    std::string              element;

    while (std::getline(iss, element, delimiter))
        splited.push_back(element);
    return splited;
}

void
    IRCD::Bot::m_parse_command(std::string& command)
{
    IRCD::Bot::_parameter = split(command, ' ');

    if ((_type = IRCD::Bot::m_get_type(IRCD::Bot::_parameter[3])) < NONE)
        (this->*IRCD::Bot::_commands[_type])();
}

void
    IRCD::Bot::receive()
{
    m_parse_command(_buffers.to_client.buffer);

    _buffers.to_client.buffer.clear();
}

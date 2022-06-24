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

void
    IRCD::Bot::m_parse_command(std::string& command)
{
    (void)command;
}

void
    IRCD::Bot::m_parse_parameter(std::vector<std::string>& parameter)
{
    (void)parameter;
}

void
    IRCD::Bot::receive()
{
    log::print() << _buffers.to_client.buffer << log::endl;
    _buffers.to_client.buffer.clear();
}

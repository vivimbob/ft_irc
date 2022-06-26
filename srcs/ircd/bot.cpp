#include "../../includes/ircd.hpp"
#include <utility>
#include <vector>

/* ircd::bot class constructor and destructor begin */

IRCD::Bot::Bot()
{
    _endl              = "\r\n";
    _status.registered = REGISTERED;
    _names.nick        = NAME_BOT;
    _names.host        = NAME_HOST;
    _names.server      = NAME_SERVER;

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

/* ircd::bot class constructor and destructor end */

/* ircd::bot class getter begin */

std::string
    IRCD::Bot::m_get_client_nick(const std::string& prefix)
{
    return (prefix.substr(1, prefix.find_first_of('!') - 1));
}

e_bot
    IRCD::Bot::m_get_type(const std::string& command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return NONE;
}

/* ircd::bot class getter end */

/* ircd::bot class command functions begin */

void
    IRCD::Bot::m_help(const std::string& prefix)
{
    m_send("NOTICE " + m_get_client_nick(prefix)
           + " :command list : [/help, /time, /coin]" + IRCD::Bot::_endl);
}

void
    IRCD::Bot::m_datetime(const std::string& prefix)
{
    time_t m_raw_time;
    char   buffer[50];
    time(&m_raw_time);
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&m_raw_time)))
        m_send("NOTICE " + m_get_client_nick(prefix)
               + " :The current datetime is " + std::string(buffer)
               + IRCD::Bot::_endl);
}

void
    IRCD::Bot::m_game_coin(const std::string& prefix)
{
    m_send("NOTICE " + m_get_client_nick(prefix)
           + " :The result of tossing a coin is "
           + std::string(std::rand() % 2 == 0 ? "head" : "tail")
           + IRCD::Bot::_endl);
}

/* ircd::bot class command functions end */

/* ircd::bot class parse function begin */

void
    IRCD::Bot::m_parse_command(std::string& command)
{
    std::vector<const std::string> params = IRCD::split(command, ' ');
    if ((_type = IRCD::Bot::m_get_type(params[3])) < NONE)
        (this->*IRCD::Bot::_commands[_type])(params[0]);
}

/* ircd::bot class parse function end */

/* ircd::bot class receive and send function begin */

void
    IRCD::Bot::m_send(const std::string& message)
{
    _buffers.buffer.append(message);
}

void
    IRCD::Bot::receive()
{
    m_parse_command(_buffers.to_client.buffer);
    _buffers.to_client.buffer.clear();
}

/* ircd::bot class receive and send function end */
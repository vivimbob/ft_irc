#include "../includes/ircd.hpp"
#include <utility>

IRCD::Bot::Bot(const std::string& nick)
{
    _names.nick   = nick;
    _names.host   = HOSTNAME;
    _names.server = SERVERNAME;

    log::print() << "BOT is created" << log::endl;

    _commands.push_back(&Bot::m_help);
    _commands.push_back(&Bot::m_datetime);
    _commands.push_back(&Bot::m_game_coin);

    _command_to_type.insert(std::make_pair("!HELP", HELP));
    _command_to_type.insert(std::make_pair("!DATETIME", DATETIME));
    _command_to_type.insert(std::make_pair("!COIN", COIN));
}

IRCD::Bot::~Bot()
{
}

std::string
    IRCD::Bot::get_prefix()
{
    std::string msg;
    msg = msg + ":" + this->get_nickmask() + " ";
    return msg;
}

void
    IRCD::Bot::m_to_client(Client& client, const std::string& str)
{
    client.get_buffers().to_client.buffer.append(str);
    // _event->toggle(client, EVFILT_READ);
}

void
    IRCD::Bot::m_help(Client& client)
{
    m_to_client(client, get_prefix()
                            + "command list : [!help, !datetime, !coin]"
                            + IRCD::Bot::_endl);
    m_to_client(client, get_prefix()
                            + "format -> '[privmsg] [bot name] :[bot command]'"
                            + IRCD::Bot::_endl);
    m_to_client(client, get_prefix() + "example -> 'privmsg BOT :!help'"
                            + IRCD::Bot::_endl);
}

void
    IRCD::Bot::m_datetime(Client& client)
{
    time_t m_raw_time;
    char   buffer[50];
    time(&m_raw_time);
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&m_raw_time)))
        m_to_client(client, get_prefix() + "The current datetime is "
                                + std::string(buffer) + "" + IRCD::Bot::_endl);
}

void
    IRCD::Bot::m_game_coin(Client& client)
{
    m_to_client(client,
                get_prefix() + "The result of tossing a coin is "
                    + std::string(std::rand() % 2 == 0 ? "head" : "tail") + " "
                    + IRCD::Bot::_endl);
}

BOTTYPE
IRCD::Bot::m_get_type(std::string command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return NONE;
}

void
    IRCD::Bot::m_parse_command(std::string& command)
{
    int offset = _buffer.find_first_of(' ');
    command    = _buffer.substr(0, offset);
    _buffer.erase(0, offset);
}

void
    IRCD::Bot::m_parse_parameter(std::vector<std::string>& parameter)
{
    int offset;
    int index;

    for (offset = 0;
         (index = _buffer.find_first_not_of(' ')) != (int)std::string::npos;)
    {
        offset = _buffer.find_first_of(' ', index);
        if ((offset != (int)std::string::npos) && _buffer[index] != ':')
            parameter.push_back(_buffer.substr(index, offset - index));
        else
        {
            if (_buffer[index] == ':')
                ++index;
            parameter.push_back(_buffer.substr(index));
            break;
        }
        _buffer.erase(0, offset);
    }
    _buffer.clear();
}

void
    IRCD::Bot::handler(Client& client)
{
    if (_buffers.to_client.buffer.empty())
        return;

    Client::t_to_client& to_client = _buffers.to_client;

    while (to_client.buffer.size())
    {
        _buffer = to_client.buffer;

        _buffer.erase(0, _buffer.find_first_of(' '));
        _buffer.erase(0, _buffer.find_first_not_of(' '));
        m_parse_command(_command);
        if (_command == "PRIVMSG" && _buffer.size())
        {
            m_parse_parameter(_parameter);
            std::string& bot_command = _parameter[1];
            for (int offset = 0;
                 (bot_command[offset] != ' ' && bot_command[offset] != '\0');
                 ++offset)
                if ((unsigned)bot_command[offset] - 'a' < 26)
                    bot_command[offset] ^= 0b100000;
            _type = m_get_type(bot_command);
            if (_type < NONE)
                (this->*Bot::_commands[_type])(client);
        }
    }
}

#ifndef BOT_HPP
#define BOT_HPP

#include "client.hpp"
#include "event.hpp"
#include "irc.hpp"
#include "log.hpp"
#include "resources.hpp"

class Bot : public Client
{
  public:
    typedef std::vector<void (Bot::*)(Client&)> t_commands;
    typedef std::map<std::string, BOTTYPE>      t_typemap;

  private:
    t_commands               _commands;
    t_typemap                _command_to_type;
    std::string              _buffer;
    std::string              _command;
    std::vector<std::string> _parameter;
    BOTTYPE                  _type;
    std::string              _endl;
    // Event*                   _event;

    void m_help(Client&);
    void m_datetime(Client&);
    void m_coin(Client&);

    BOTTYPE m_get_type(std::string);
    void    m_parse_command(std::string&);
    void    m_parse_parameter(std::vector<std::string>&);

    std::string get_nickmask_prefix();
    void        m_to_client(Client&, const std::string&);

  public:
    Bot(const std::string&);
    ~Bot();
    void handler(Client&);
};

#endif /* BOT_HPP */
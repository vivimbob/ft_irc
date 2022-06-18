#ifndef DAEMON_HPP
#define DAEMON_HPP

#include "../includes/command.hpp"
#include "../includes/message.hpp"
#include "../includes/utils.hpp"
#include "resources.hpp"
#include <string>

class FT_IRCD;

class Daemon : public Command
{
  public:
    friend class Client;
    // typedef std::map<std::string, void (Daemon::*)(Message&)> Command_Map;
    typedef std::vector<void (Daemon::*)(Message&)> Command;
    typedef std::map<TYPE, std::string>             CommandMap;
    typedef std::map<std::string, TYPE>             TypeMap;

  private:
    void m_mode_channel(Message& message, const std::string& channel_name);
    void m_mode_user(Message& message, const std::string& nickname);

  protected:
    Command    _command;
    TypeMap    _command_to_type;
    CommandMap _type_to_command;

    FT_IRCD*    _ft_ircd;
    TYPE        get_type(std::string command);
    std::string get_command(TYPE type);

	void m_void(Client::t_request request);
    void m_pass(Client::t_request request);
    void m_nick(Client::t_request request);
    void m_user(Client::t_request request);
    void m_join(Client::t_request request);
    void m_mode(Client::t_request request);
    void m_quit(Client::t_request request);
    void m_topic(Client::t_request request);
    void m_part(Client::t_request request);
    void m_names(Client::t_request request);
    void m_list(Client::t_request request);
    void m_invite(Client::t_request request);
    void m_privmsg(Client::t_request request);
    void m_notice(Client::t_request request);
    void m_kick(Client::t_request request);
    Daemon();
    ~Daemon();
};

#endif /* DAEMON_HPP */

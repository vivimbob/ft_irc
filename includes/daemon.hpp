#ifndef DAEMON_HPP
#define DAEMON_HPP

#include "../includes/message.hpp"
#include "../includes/utils.hpp"
#include "resources.hpp"
#include <string>

class FT_IRCD;

class Daemon
{
  public:
    friend class Client;
    typedef std::map<std::string, void (Daemon::*)(Message&)> Command_Map;
    typedef std::map<TYPE, std::string>                       CommandMap;
    typedef std::map<std::string, TYPE>                       TypeMap;

  private:
    void m_mode_channel(Message& message, const std::string& channel_name);
    void m_mode_user(Message& message, const std::string& nickname);

  protected:
    Command_Map _command_map;
    Command_Map _cmd_connection;
    TypeMap     _command_to_type;
    CommandMap  _type_to_command;

    FT_IRCD*    _ft_ircd;
    TYPE        get_type(std::string command);
    std::string get_command(TYPE type);

    void m_pass(Message& message);
    void m_nick(Message& message);
    void m_user(Message& message);
    void m_join(Message& message);
    void m_mode(Message& message);
    void m_quit(Message& message);
    void m_topic(Message& message);
    void m_part(Message& message);
    void m_names(Message& message);
    void m_list(Message& message);
    void m_invite(Message& message);
    void m_privmsg(Message& message);
    void m_notice(Message& message);
    void m_kick(Message& message);
    Daemon();
    ~Daemon();
};

#endif /* DAEMON_HPP */

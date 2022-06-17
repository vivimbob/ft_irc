#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/message.hpp"
#include "../includes/utils.hpp"
#include <string>

class Server;

class FT_IRC
{
  public:
    typedef std::map<std::string, void (FT_IRC::*)(Client&, Message&)>
                                            CommandMap;
    typedef std::map<std::string, Client*>  ClientMap;
    typedef std::map<std::string, Channel*> ChannelMap;

  private:
    CommandMap m_initial_command_map();
    CommandMap m_initial_register_cmd_map();

    void m_mode_channel(Message& message, const std::string& channel_name);
    void m_mode_user(Message& message, const std::string& nickname);

  protected:
    std::string _password;
    CommandMap  _command_map;
    CommandMap  _register_cmd_map;
    ClientMap   _client_map;
    ChannelMap  _channel_map;
    Server*     _server;
    void        m_pass(Message& message);
    void        m_nick(Message& message);
    void        m_user(Message& message);
    void        m_join(Message& message);
    void        m_mode(Message& message);
    void        m_quit(Message& message);
    void        m_topic(Message& message);
    void        m_part(Message& message);
    void        m_names(Message& message);
    void        m_list(Message& message);
    void        m_invite(Message& message);
    void        m_privmessage(Message& message);
    void        m_notice(Message& message);
    void        m_kick(Message& message);
    FT_IRC();
    ~FT_IRC();
};

#endif /* SERVER_HPP */

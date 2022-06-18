#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "../includes/client.hpp"
#include <vector>

class FT_IRCD;

class Command
{
  protected:
    FT_IRCD*                                                    _ft_ircd;
    typedef std::map<TYPE, std::string>                         CommandMap;
    typedef std::map<std::string, TYPE>                         TypeMap;
    typedef std::vector<void (Command::*)(Client::t_requests&)> Handler;
    CommandMap _type_to_command;
    TypeMap    _command_to_type;
    Handler    _handler;

    Command();
    ~Command();

    void m_empty(Client::t_requests& requests);
    void m_pass(Client::t_requests& requests);
    void m_nick(Client::t_requests& requests);
    void m_user(Client::t_requests& requests);
    void m_quit(Client::t_requests& requests);
    void m_join(Client::t_requests& requests);
    void m_part(Client::t_requests& requests);
    void m_topic(Client::t_requests& requests);
    void m_names(Client::t_requests& requests);
    void m_list(Client::t_requests& requests);
    void m_invite(Client::t_requests& requests);
    void m_kick(Client::t_requests& requests);
    void m_mode(Client::t_requests& requests);
    void m_privmsg(Client::t_requests& requests);
    void m_notice(Client::t_requests& requests);
    void m_unknown(Client::t_requests& requests);
    void m_unregistered();
    bool m_checker(Client::t_requests& requests);

    void m_to_client(Client& client, const std::string& str_msg);
    void m_to_members(Channel*           channel,
                      const std::string& msg,
                      Client*            exclusion = nullptr);
    void m_to_members(Client&            client,
                      const std::string& msg,
                      Client*            exclusion = nullptr);
};

#endif /* COMMAND_HPP */

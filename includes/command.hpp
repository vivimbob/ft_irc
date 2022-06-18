#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "../includes/client.hpp"
#include <vector>

class Command
{
  protected:
    typedef std::vector<void (Command::*)(Client::t_requests&)> Handler;
    Handler                                                     _handler;
    Command();
    ~Command();

    void m_empty(Client::t_requests& requests);
    void m_pass(Client::t_requests& requests);
    void m_nick(Client::t_requests& requests);
    void m_user(Client::t_requests& requests);
    void m_join(Client::t_requests& requests);
    void m_mode(Client::t_requests& requests);
    void m_quit(Client::t_requests& requests);
    void m_topic(Client::t_requests& requests);
    void m_part(Client::t_requests& requests);
    void m_names(Client::t_requests& requests);
    void m_list(Client::t_requests& requests);
    void m_invite(Client::t_requests& requests);
    void m_privmsg(Client::t_requests& requests);
    void m_notice(Client::t_requests& requests);
    void m_kick(Client::t_requests& requests);
};

#endif /* COMMAND_HPP */

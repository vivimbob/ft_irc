#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "../includes/ICommand.hpp"
#include "../includes/client.hpp"
#include <vector>

class FT_IRCD;

class IRC : public ICommand
{
  public:
    typedef struct s_target
    {
        Client*            client;
        Client::t_request* request;
    } t_target;

  private:
    void empty();
    void pass();
    void nick();
    void user();
    void quit();
    void join();
    void part();
    void topic();
    void names();
    void list();
    void invite();
    void kick();
    void mode();
    void privmsg();
    void notice();
    void unknown();
    void unregistered();

    bool m_checker();

    void m_mode_channel(Client::t_requests& requests,
                        const std::string&  channel_name);
    void m_mode_user(Client::t_requests& requests, const std::string& nickname);

    void m_to_client(Client& client, const std::string& str_msg);
    void m_to_members(Channel*           channel,
                      const std::string& msg,
                      Client*            exclusion = nullptr);
    void m_to_members(Client&            client,
                      const std::string& msg,
                      Client*            exclusion = nullptr);

  protected:
    FT_IRCD*                             _ft_ircd;
    typedef std::map<TYPE, std::string>  IRCMap;
    typedef std::map<std::string, TYPE>  TypeMap;
    typedef std::vector<void (IRC::*)()> Command;
    IRCMap                               _type_to_command;
    TypeMap                              _command_to_type;
    Command                              _commands;
    Client::t_requests*                  _requests;
    Client*                              _client;
    Client::t_request*                   _request;

    IRC();
    ~IRC();

    TYPE get_type(std::string command);
};

#endif /* COMMAND_HPP */

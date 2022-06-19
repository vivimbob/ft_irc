#ifndef IRCD_HPP
#define IRCD_HPP

#include "irc.hpp"

class IRCD : public IRC
{
  public:
    typedef struct s_map
    {
        ClientMap  client;
        ChannelMap channel;
    } t_map;

    typedef std::vector<void (IRCD::*)()> Command;

  private:
    const std::string* _target;
    void               m_mode_channel(const std::string&);
    void               m_mode_user(const std::string&);
    void               m_to_client(Client&, const std::string&);
    void               m_to_channel(Channel& channel, const std::string&);
    void               m_to_channels(const std::string&);

    RESULT m_is_valid(TYPE);
    RESULT m_to_client(std::string);
    RESULT m_empty();
    RESULT m_pass();
    RESULT m_nick();
    RESULT m_user();
    RESULT m_quit();
    RESULT m_join();
    RESULT m_part();
    RESULT m_topic();
    RESULT m_names();
    RESULT m_list();
    RESULT m_invite();
    RESULT m_kick();
    RESULT m_mode();
    RESULT m_privmsg();
    RESULT m_notice();
    RESULT m_unknown();
    RESULT m_unregistered();

  protected:
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
    void registration();
    IRCD();
    ~IRCD();
    Command _commands;

    TYPE get_type(std::string command);
};

#endif /* IRCD_HPP */

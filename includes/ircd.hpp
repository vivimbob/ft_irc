#ifndef IRCD_HPP
#define IRCD_HPP

#include "irc.hpp"

class IRCD : public IRC
{
  public:
    typedef struct s_map
    {
        t_clientmap  client;
        t_channelmap channel;
    } t_map;

    typedef std::vector<void (IRCD::*)()>  t_commands;
    typedef std::vector<const std::string> t_cstr_vector;
    typedef t_cstr_vector::iterator        t_iter;

  private:
    const std::string* _target;
    const std::string* _target_sub;
    std::string        _buffer;
    Client*            _fixed;
    int                _offset;
    int                _index;

    // utility start
    RESULT m_is_valid(TYPE);
    RESULT m_to_client(std::string);
    void   m_to_client(Client&, const std::string&);
    void   m_to_channel(const std::string&);

  protected:
    void              m_to_channels(const std::string&);
    TYPE              get_type(std::string command);
    void              registration();
    void              parse_command(std::string&);
    void              parse_parameter(std::vector<std::string>&);
    void              parse_request(Client::t_request&);
    Channel::t_status parse_flag(const std::string&);
    t_commands        _commands;
    // utiluity end

  private:
    RESULT m_pass();
    RESULT m_nick();
    RESULT m_user();
    RESULT m_join(PHASE, Channel* = nullptr);
    RESULT m_part(PHASE);
    RESULT m_topic();
    RESULT m_names();
    RESULT m_list();
    RESULT m_invite();
    RESULT m_kick(PHASE);
    RESULT m_mode(PHASE);
    RESULT m_privmsg(PHASE);

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
    IRCD();
    ~IRCD();
};

#endif /* IRCD_HPP */

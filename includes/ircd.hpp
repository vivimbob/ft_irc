#ifndef IRCD_HPP
#define IRCD_HPP

#include "irc.hpp"
#include "resources.hpp"

class IRCD : public IRC
{
  public:
    typedef struct s_map
    {
        t_map_client  client;
        t_map_channel channel;
    } t_map;

    typedef std::vector<void (IRCD::*)()>  t_commands;
    typedef std::vector<const std::string> t_cstr_vector;
    typedef t_cstr_vector::iterator        t_iter;

  private:
    class Bot : public Client
    {
      public:
        typedef std::vector<void (Bot::*)(Client&)> t_vector_commands;
        typedef std::vector<std::string>            t_vector_str;
        typedef std::map<std::string, BOTTYPE>      t_map_type;

      private:
        t_vector_commands _commands;
        t_map_type        _command_to_type;
        std::string       _buffer;
        std::string       _command;
        t_vector_str      _parameter;
        BOTTYPE           _type;
        std::string       _endl;

        void m_help(Client&);
        void m_datetime(Client&);
        void m_game_coin(Client&);

        BOTTYPE m_get_type(std::string);
        void    m_parse_command(std::string&);
        void    m_parse_parameter(std::vector<std::string>&);

        std::string get_prefix();
        void        m_to_client(Client&, const std::string&);

      public:
        Bot();
        Bot(const std::string&);
        ~Bot();
        void handler(Client&);
    };

    IRCD::Bot          _bot;
    const std::string* _target;
    const std::string* _target_sub;
    std::string        _buffer;
    Client*            _fixed;
    int                _offset;
    int                _index;

    RESULT m_is_valid(TYPE);
    RESULT m_to_client(std::string);
    void   m_to_client(Client&, const std::string&);
    void   m_to_channel(const std::string&);
    void   m_mode_valid(const char);
    void   m_mode_invalid(const char);
    void   m_mode_sign(const char);
    void   m_mode_initialize();
    void   m_bot_initialize();

  protected:
    void       m_to_channels(const std::string&);
    TYPE       get_type(std::string command);
    void       registration();
    void       parse_parameter(std::vector<std::string>&);
    void       parse_command(std::string&);
    void       parse_request(Client::t_request&);
    RESULT     parse_flag(const std::string&);
    t_commands _commands;
    t_map      _map;
    void (IRCD::*_modes[128])(const char);

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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "resources.hpp"

class Client;

class Channel
{
  public:
    typedef std::string                   t_str_info;
    typedef std::map<Client*, t_str_info> t_map_member;
    typedef std::set<Client*>             t_set_invitee;
    typedef t_map_member::const_iterator  t_citer_member;

    typedef union
    {
        struct
        {
            unsigned char negative : 1;
            unsigned char positive : 1;
        };
        unsigned char state;
    } t_sign;

    typedef union
    {
        struct
        {
            unsigned char invite : 1;
            unsigned char topic : 1;
            unsigned char nomsg : 1;
        };
        unsigned char state;
    } t_status;

    typedef struct s_reserved
    {
        t_sign   sign;
        t_status flags;
    } t_reserved;

  private:
    std::string   _name;
    std::string   _topic;
    t_map_member  _members;
    t_status      _status;
    t_reserved    _reserved;
    t_set_invitee _invitees;

    Channel();
    Channel(const Channel&);
    Channel& operator=(const Channel&);
    bool     m_set_status(const bool&, unsigned char&);

  public:
    Channel(const std::string& name, Client* client);
    ~Channel();

    const std::string&  get_name() const;
    const std::string&  get_topic() const;
    const t_map_member& get_members();
    const std::string&  get_prefix(Client*);
    bool                get_status(e_type);

    std::string get_status();
    void        set_name(const std::string&);
    void        set_topic(const std::string&);
    void        set_operator(Client*);
    void        set_status(e_type, bool);
    void        set_status(std::string&);
    void        reserve_clear();
    void        reserve_sign(const char);
    void        reserve_flags(const char);

    bool is_empty();
    bool is_full();
    bool is_operator(Client*);
    bool is_joined(Client*);
    bool is_invited(Client*);
    bool is_signed();
    bool is_reserved();

    void join(Client*);
    void part(Client*);
    void invitation(Client*);
};

#endif /* CHANNEL_HPP */

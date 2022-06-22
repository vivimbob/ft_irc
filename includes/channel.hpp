#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "resources.hpp"

class Client;

class Channel
{
  public:
    typedef std::string                     t_membership;
    typedef std::map<Client*, t_membership> t_membermap;
    typedef std::set<Client*>               t_invitation;
    typedef t_membermap::const_iterator     t_citer;

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
    std::string  _name;
    std::string  _topic;
    t_membermap  _members;
    t_status     _status;
    t_reserved   _reserved;
    t_invitation _invitations;

    Channel();
    Channel(const Channel&);
    Channel& operator=(const Channel&);

  public:
    Channel(const std::string& name, Client* client);
    ~Channel();

    const std::string& get_name() const;
    const std::string& get_topic() const;
    const t_membermap& get_members();
    const std::string& get_prefix(Client*);
    bool               get_status(TYPE);

    std::string get_status();
    void        set_name(const std::string&);
    void        set_topic(const std::string&);
    void        set_operator(Client*);
    void        set_status(TYPE, bool);
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

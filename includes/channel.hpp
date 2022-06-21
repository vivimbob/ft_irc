#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "resources.hpp"

class Client;

class Channel
{
  public:
    typedef std::string                     t_membership;
    typedef std::map<Client*, t_membership> t_membermap;
    typedef t_membermap::const_iterator     t_citer;

    typedef struct s_status
    {
        unsigned char invite : 1;
        unsigned char topic : 1;
        unsigned char nomsg : 1;
    } t_status;

  private:
    std::string _name;
    std::string _topic;
    t_membermap _members;
    t_status    _status;

    Channel();
    Channel(const Channel&);
    Channel& operator=(const Channel&);

  public:
    Channel(const std::string& name);
    ~Channel();

    const std::string& get_name() const;
    const std::string& get_topic() const;
    const t_membermap& get_members();
    const std::string& get_prefix(Client*);
    bool               get_status(TYPE);

    void set_name(const std::string&);
    void set_topic(const std::string&);
    void set_operator(Client*);
    void set_status(TYPE, bool);

    bool is_empty();
    bool is_full();
    bool is_operator(Client&);
    bool is_joined(Client*);

    void join(Client&);
    void part(Client&);
};

#endif /* CHANNEL_HPP */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "membership.hpp"
#include "resources.hpp"
#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>

class Client;

class Channel
{
  public:
    typedef std::string t_membership;

    typedef std::map<Client*, t_membership> MemberMap;
    typedef MemberMap::const_iterator       CITER;

  private:
    std::string _name;
    std::string _topic;
    MemberMap   _members;

    Channel();
    Channel(const Channel&);
    Channel& operator=(const Channel&);

  public:
    Channel(const std::string& name);
    ~Channel();

    const std::string& get_name() const;
    const std::string& get_topic() const;
    const MemberMap&   get_members();
    const std::string& get_prefix(Client*);

    void set_name(const std::string& name);
    void set_topic(const std::string& topic);
    void set_operator(Client* client);

    bool is_empty();
    bool is_full();
    bool is_operator(Client& client);
    bool is_joined(Client* client);

    void join(Client& client);
    void part(Client& client);
};

#endif /* CHANNEL_HPP */

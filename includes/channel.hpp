#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "membership.hpp"
#include "message.hpp"
#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>

class Client;

class Channel
{
  private:
    struct s_mode
    {
        bool private_channel;
        bool secret;
        bool invite;
        bool topic;
        bool no_message;
        bool moderate;
        bool key;
        bool limit;
    };

  public:
    typedef std::map<Client*, MemberShip> MemberMap;

  private:
    std::string       m_channel_name;
    time_t            m_channel_init_time;
    MemberMap         m_member_list;
    std::set<Client*> m_invitation_list;
    std::string       m_channel_topic;
    std::string       m_key;
    size_t            m_user_limit;

    bool        m_mode_string_need_update;
    std::string m_mode_string;
    s_mode      m_mode;

    Channel();
    Channel(const Channel& cp);
    Channel& operator=(const Channel& cp);

  public:
    Channel(const std::string& name, const std::string& key);
    ~Channel();

    const std::string& get_channel_name() const;
    const std::string& get_channel_topic() const;
    std::string        get_channel_mode(Client* client);
    const std::string& get_key() const;
    const size_t&      get_user_limit() const;
    const MemberMap&   get_user_list();

    void set_channel_name(const std::string& name);
    void set_channel_topic(const std::string& topic);
    void set_private_flag(bool toggle);
    void set_secret_flag(bool toggle);
    void set_invite_flag(bool toggle);
    void set_topic_flag(bool toggle);
    void set_no_messages_flag(bool toggle);
    void set_moderate_flag(bool toggle);
    void set_key_flag(bool toggle, std::string key = std::string());
    void set_limit(bool toggle, size_t limit);
    void set_operator_flag(bool toggle, Client* client);
    void set_voice_flag(bool toggle, Client* client);

    bool is_empty();
    bool is_full();
    bool is_operator(Client& client);
    bool is_voice_mode(Client& client);
    bool is_user_on_channel(Client* client);
    bool is_user_on_invitation_list(Client* client);
    bool is_protected_topic_mode();
    bool is_invite_only_mode();
    bool is_limit_mode();
    bool is_key_mode();
    bool is_private_mode();
    bool is_secret_mode();

    void add_user(Client& client);
    void delete_user(Client& client);
    void add_user_invitation_list(Client& client);
    void delete_user_invitation_list(Client& client);
};

#endif /* CHANNEL_HPP */

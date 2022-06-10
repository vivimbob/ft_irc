#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include "message.hpp"
#include "membership.hpp"

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
	typedef std::map<Client *, MemberShip> MemberMap;

  private:
    std::string m_channel_name;
    time_t m_channel_init_time;
    MemberMap m_member_list;
    std::set<Client *> m_invitation_list;
    std::string m_channel_topic;
    std::string m_key;
	size_t m_user_limit;

	bool m_mode_string_need_update;
	std::string m_mode_string;
	s_mode m_mode;
    
    Channel(void);
    Channel(const Channel& cp);
    Channel &operator=(const Channel& cp);

  public:
    Channel(const std::string &name, const std::string &key);
    ~Channel(void);

    const std::string &get_channel_name(void) const;
    const std::string &get_channel_topic(void) const;
    std::string get_channel_mode(Client *client);
    const std::string &get_key(void) const;
    const size_t &get_user_limit(void) const;
    const MemberMap &get_user_list(void);

    void set_channel_name(const std::string &name);
    void set_channel_topic(const std::string &topic);
	void set_private_flag(bool toggle);
	void set_secret_flag(bool toggle);
	void set_invite_flag(bool toggle);
	void set_topic_flag(bool toggle);
	void set_no_messages_flag(bool toggle);
	void set_moderate_flag(bool toggle);
	void set_key_flag(bool toggle, std::string key = std::string());
	void set_limit(bool toggle, size_t limit);
	void set_operator_flag(bool toggle, Client *client);
	void set_voice_flag(bool toggle, Client *client);

    bool is_empty(void);
    bool is_full(void);
	bool is_operator(Client &client);
    bool is_voice_mode(Client &client);
	bool is_user_on_channel(Client *client);
  bool is_user_on_invitation_list(Client *client);
	bool is_protected_topic_mode(void);
    bool is_invite_only_mode(void);
    bool is_limit_mode(void);
    bool is_key_mode(void);
    bool is_private_mode(void);
    bool is_secret_mode(void);

    void add_user(Client &client);
    void delete_user(Client &client);
};

#endif /* CHANNEL_HPP */

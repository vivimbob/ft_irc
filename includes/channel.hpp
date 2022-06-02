#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "ircmessage.hpp"
#include "client.hpp"
#include "membership.hpp"

class Channel
{
  private:
    struct s_mode
	{
		bool p:1;
		bool s:1;
		bool i:1;
		bool t:1;
		bool n:1;
		bool m:1;
		bool k:1;
		bool l:1;
	};

  public:
	typedef std::map<Client *, MemberShip> MemberMap;
	typedef std::vector<std::string> BanMasks;

  private:
    std::string m_channel_name;
    time_t m_channel_init_time;
    MemberMap m_user_lists;
	BanMasks m_ban_masks;
    std::string m_channel_topic;
    std::string m_key;
	size_t m_user_limits;

	s_mode m_mode;
    
    Channel(void);
    Channel(const Channel& cp);
    Channel &operator=(const Channel& cp);

  public:
    Channel(const std::string &name, const std::string &key);
    ~Channel(void);
    const std::string &get_channel_name(void) const;
    const std::string &get_channel_topic(void) const;
    std::string get_channel_mode(void);
    const std::string &get_key(void) const;
    const size_t &get_user_limits(void) const;
    const MemberMap &get_user_lists(void);
    bool get_mode_limit(void);
    bool get_mode_invite_only(void);
    bool get_mode_key(void);
    void set_channel_name(const std::string &name);
    void set_channel_topic(const std::string &topic);
    void set_mode_key(bool b);
    void set_key(const std::string &key);
    void join(Client &client);
    void invite(void);
    void display_channel_info(void);
    void display_topic(Client &client);
    void display_names(Client &client);
    bool is_empty(void) const;
	bool is_operator(Client &client);
	bool is_user_on_channel(Client *client);
	bool is_protected_topic(void);
    void add_operator(Client &client);
    void delete_operator(Client &client);
    void add_user(Client &client);
    void delete_user(Client &client);

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
};

#endif /* CHANNEL_HPP */

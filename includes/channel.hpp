#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "IRCMessage.hpp"
#include "client.hpp"

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
		bool l:1;
		bool b:1;
		bool k:1;
	};

    std::string m_channel_name;
    time_t m_channel_init_time;
    std::map<int, Client*> m_user_lists;
    std::vector<Client*> m_operator_lists;
    std::string m_channel_topic;
    // banned_list는 추후에 
    std::vector<Client*> m_banned_lists;
    std::string m_channel_mode;
    std::map<int, std::string> m_user_mode;
    std::string m_key;

	s_mode mode;
    
    Channel(void);
    Channel(const Channel& cp);
    Channel &operator=(const Channel& cp);

  public:
    Channel(const std::string &name, Client &client);
    ~Channel(void);
    const std::string &m_get_channel_name(void) const;
    const std::string &m_get_channel_topic(void) const;
    const std::string &m_get_channel_mode(void) const;
    const std::string &m_get_key(void) const;
    const std::string &m_get_user_mode(Client &);
    void m_set_channel_name(const std::string &name);
    void m_set_channel_topic(const std::string &topic);
    void m_set_channel_mode(const std::string &chan_mode);
    void m_set_key(const std::string &key);
    void m_set_user_mode(Client &client, std::string &user_mode);
    void m_join(Client &client);
    void m_invite(Client &client);
    void m_display_channel_info(void);
    void m_display_topic(Client &client);
    void m_display_names(Client &client);
    bool m_is_empty(void) const;
    void m_add_operator(Client &client);
    void m_delete_operator(Client &client);
    void m_add_user(Client &client);
    void m_delete_user(Client &client);

};

#endif /* CHANNEL_HPP */

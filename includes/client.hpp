#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <set>
#include "sendbuffer.hpp"
#include "message.hpp"
#include "utils.hpp"

class Client
{
private:
	struct s_mode
	{
		bool i:1;
		bool s:1;
		bool w:1;
		bool o:1;
	};

    sockaddr_in m_client_addr;
    int m_client_fd;
    SendBuffer m_send_buffer;
    std::string m_recv_buffer;
    std::string m_password;
    std::string	m_nickname;
    std::string	m_username;
    std::string m_hostname;
    std::queue<Message *> m_commands;
	std::set<const std::string> m_channel_list;
    size_t m_channel_limits;

	bool m_pass_registered:1;
    bool m_nick_registered:1;
    bool m_user_registered:1;

	s_mode m_mode;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client(void);
    sockaddr_in get_client_addr(void);
    int get_socket(void);
    char* get_client_IP(void);
    const std::string	&get_password(void) const;
    const std::string	&get_nickname(void) const;
    const std::string	&get_username(void) const;
    const std::string	&get_hostname(void) const;
    std::queue<Message *> &get_commands(void);
    std::string &get_recv_buffer(void);
    SendBuffer &get_send_buffer(void);
    size_t &get_channel_limits(void);
	  std::string	get_usermode(void);
    const std::set<const std::string> &get_channel_list(void) const;

    void set_password(const std::string &pw);
    void set_nickname(const std::string &nickname);
    void set_username(const std::string &username);
    void set_hostname(const std::string &hostname);
	void set_invisible_flag(bool toggle);
	void set_operator_flag(bool toggle);
	void set_server_notice_flag(bool toggle);
	void set_wallops_flag(bool toggle);

    bool is_registered(void) const;
    bool is_pass_registered(void) const;
    bool is_nick_registered(void) const;
    bool is_user_registered(void) const;
	bool is_join_available(void) const;
	bool is_already_joined(const std::string &channel_name);

	void push_message(const std::string &message);
	void push_message(const std::string &message, int level);

	std::string make_nickmask(void);
	
	void insert_channel(const std::string &channel_name);
	void erase_channel(const std::string &channel_name);
};

#endif /* CLIENT_HPP */

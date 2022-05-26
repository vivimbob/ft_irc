#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <map>
#include "sendbuffer.hpp"
#include "IRCMessage.hpp"

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
    std::queue<IRCMessage *>m_commands;
    std::map<const std::string, const std::string> m_chan_key_lists;
    size_t m_channel_limits;

	  bool m_pass_registered:1;
    bool m_nick_registered:1;
    bool m_user_registered:1;

	s_mode m_mode;

    friend class Server;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client(void);
    sockaddr_in m_get_client_addr(void);
    int m_get_socket(void);
    char* m_get_client_IP(void);
    const std::string	&m_get_password(void) const;
    const std::string	&m_get_nickname(void) const;
    const std::string	&m_get_username(void) const;
    const std::string	&m_get_hostname(void) const;
	  std::string	m_get_usermode(void);
    const std::map<const std::string, const std::string> &m_get_channel_lists(void) const;
    bool m_is_registered(void) const;
    bool m_is_pass_registered(void) const;
    bool m_is_nick_registered(void) const;
    bool m_is_user_registered(void) const;
    void m_set_password(const std::string &pw);
    void m_set_nickname(const std::string &nickname);
    void m_set_username(const std::string &username);
    void m_set_hostname(const std::string &hostname);

};

#endif /* CLIENT_HPP */

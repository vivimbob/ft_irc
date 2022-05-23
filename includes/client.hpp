#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include "sendbuffer.hpp"
#include "IRCMessage.hpp"

class Client
{
  private:
    sockaddr_in m_client_addr;
    int m_client_fd;
    SendBuffer m_send_buffer;
    std::string m_recv_buffer;
    std::string m_password;
    std::string	m_nickname;
		std::string	m_username;
    std::queue<IRCMessage *>m_commands;

	bool m_pass_registered:1;
    bool m_nick_registered:1;
    bool m_user_registered:1;

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
		const bool m_is_registered(void) const;
		const bool m_is_pass_registered(void) const;
		const bool m_is_nick_registered(void) const;
		const bool m_is_user_registered(void) const;
    void m_set_password(const std::string &pw);
		void m_set_nickname(const std::string &nickname);
		void m_set_username(const std::string &username);
};

#endif /* CLIENT_HPP */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
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
    std::vector<IRCMessage>m_commands;

    friend class Server;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client();
    sockaddr_in m_get_client_addr();
    int m_get_socket();
    char* m_get_client_IP();
		const std::string	&m_get_password() const;
    const std::string	&m_get_nickname() const;
		const std::string	&m_get_username() const;
    void m_set_password(const std::string &pw);
		void m_set_nickname(const std::string &nickname);
		void m_set_username(const std::string &username);

};

#endif /* CLIENT_HPP */
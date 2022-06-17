#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "../lib/logger.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

class Socket
{
  public:
    typedef struct s_socket
    {
        int         fd;
        sockaddr_in addr;
        socklen_t   len;
    } t_socket;

  private:
    void m_create();
    void m_bind(int port);
    void m_listen();
    Socket(const Socket& copy);
    Socket& operator=(const Socket& copy);

  protected:
    t_socket _socket;
    Socket();
    ~Socket();
    void m_initialize(int port);
};

#endif /* SOCKET_HPP */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "log.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

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
    void m_accept();
    Socket(const Socket&);
    Socket& operator=(const Socket&);

  protected:
    t_socket _socket;
    Socket();
    ~Socket();
    int  accept(sockaddr* const);
    void initialize(int port);
};

#endif /* SOCKET_HPP */

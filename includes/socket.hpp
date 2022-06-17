#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "../lib/logger.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

class Socket
{
  private:
    void m_create();
    void m_bind(int port);
    void m_listen();
    Socket(const Socket& copy);
    Socket& operator=(const Socket& copy);

  protected:
    Socket();
    ~Socket();
    void      m_initialize(int port);
    int       _fd;
    socklen_t _socklen;
};

#endif /* SOCKET_HPP */

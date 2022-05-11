#include "../includes/server.hpp"
#include "../includes/logger.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <arpa/inet.h>

Server::Server(int argc, char **argv)
    : m_kq(-1),
      m_listen_fd(-1),
      m_port(-1),
      m_password(NULL)
{
}

Server::Server(int port, std::string password)
    : m_kq(-1),
      m_listen_fd(-1),
      m_port(port),
      m_password(password)
{
}

Server::~Server(void)
{
}

void
    Server::create_socket(void)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd == -1)
    {
        Logger().fatal() << "Failed to create socket. errno " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Create socket " << m_listen_fd;
}

void
    Server::bind_socket(void)
{
    memset(&m_sockaddr, 0, sizeof(sockaddr_in));
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_addr.s_addr = INADDR_ANY;
    m_sockaddr.sin_port = htons(m_port);

    if (bind(m_listen_fd, (struct sockaddr *)&m_sockaddr, sizeof(sockaddr_in)) == -1)
    {
        Logger().fatal() << "Failed to bind to port " << m_port << ". errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Bind port: " << m_port << " IP: " <<  inet_ntoa(m_sockaddr.sin_addr);
}

void
    Server::listen_socket(void)
{
    if (listen(m_listen_fd, SOMAXCONN) == -1)
    {
        Logger().fatal() << "Failed to listen on socket. errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Listen on socket";
    fcntl(m_listen_fd, F_SETFL, O_NONBLOCK);
    Logger().trace() << "socket set nonblock";
}

void
    Server::create_kqueue(void)
{
    m_kq = kqueue();
    if (m_kq == -1)
    {
        Logger().fatal() << "Failed to allocate kqueue. errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Allocate kqueue " << m_kq;
    update_event(m_listen_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().trace() << "Socket " << m_listen_fd << " update to kqueue";
}

void
    Server::update_event(int ident, short filter, u_short flags, u_int fflags, int data, void *udata)
{
    struct kevent kev;
	EV_SET(&kev, ident, filter, flags, fflags, data, udata);
	kevent(m_kq, &kev, 1, NULL, 0, NULL);
}
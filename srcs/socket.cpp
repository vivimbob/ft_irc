#include "../includes/socket.hpp"

void
    Socket::m_create()
{
    if ((_socket.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Logger().error() << "Failed to create socket. errno " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Create socket " << _socket.fd;
    int toggle = 1;
    setsockopt(_socket.fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&toggle,
               sizeof(toggle));
}

void
    Socket::m_bind(int port)
{
    memset(&_socket.addr, 0, sizeof(sockaddr_in));
    _socket.addr.sin_family      = AF_INET;
    _socket.addr.sin_addr.s_addr = INADDR_ANY;
    _socket.addr.sin_port        = htons(port);

    if (bind(_socket.fd, (struct sockaddr*)&_socket.addr,
             sizeof(sockaddr_in)) == -1)
    {
        Logger().error() << "Failed to bind to port and address" << port
                         << ". errno: " << errno << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Bind Port :" << port
                    << " IP :" << inet_ntoa(_socket.addr.sin_addr);
}

void
    Socket::m_listen()
{
    if (listen(_socket.fd, SOMAXCONN) == -1)
    {
        Logger().error() << "Failed to listen on socket. errno: " << errno
                         << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Listen on socket";
    fcntl(_socket.fd, F_SETFL, O_NONBLOCK);
    Logger().info() << "Socket set nonblock";
}

void
    Socket::m_initialize(int port)
{
    m_create();
    m_bind(port);
    m_listen();
}

Socket::Socket()
{
    _socket.len = sizeof(_socket.addr);
    _socket.fd  = -1;
}

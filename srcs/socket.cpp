#include "../includes/socket.hpp"

Socket::Socket() : _socklen(sizeof(sockaddr_in)), _fd(-1)
{
}

void
    Socket::m_create()
{
    if ((Socket::_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Logger().error() << "Failed to create socket. errno " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Create socket " << Socket::_fd;
    int toggle = 1;
    setsockopt(Socket::_fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&toggle,
               sizeof(toggle));
}

void
    Socket::m_bind(int port)
{
    memset(&_sockaddr, 0, sizeof(sockaddr_in));
    _sockaddr.sin_family      = AF_INET;
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
    _sockaddr.sin_port        = htons(port);

    if (bind(Socket::_fd, (struct sockaddr*)&_sockaddr, sizeof(sockaddr_in)) ==
        -1)
    {
        Logger().error() << "Failed to bind to port and address" << port
                         << ". errno: " << errno << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Bind Port :" << port
                    << " IP :" << inet_ntoa(_sockaddr.sin_addr);
}

void
    Socket::m_listen()
{
    if (listen(Socket::_fd, SOMAXCONN) == -1)
    {
        Logger().error() << "Failed to listen on socket. errno: " << errno
                         << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Listen on socket";
    fcntl(Socket::_fd, F_SETFL, O_NONBLOCK);
    Logger().info() << "Socket set nonblock";
}

void
    Socket::m_initialize(int port)
{
    m_create();
    m_bind(port);
    m_listen();
}

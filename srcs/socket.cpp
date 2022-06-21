#include "../includes/socket.hpp"
#include "../includes/client.hpp"
#include <fcntl.h>
#include <sys/event.h>
#include <unistd.h>

Socket::~Socket()
{
}

void
    Socket::m_disconnect()
{
    close(_fd);
}

void
    Socket::m_create()
{
    if ((_socket.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        log::print() << "Failed to create socket. errno " << errno << ":"
                     << strerror(errno) << log::endl;
        exit(EXIT_FAILURE);
    }
    log::print() << "Create socket " << _socket.fd << log::endl;
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

    if (bind(_socket.fd, (struct sockaddr*)&_socket.addr, sizeof(sockaddr_in))
        == -1)
    {
        log::print() << "Failed to bind to port and address" << port
                     << ". errno: " << errno << ":" << strerror(errno)
                     << log::endl;
        exit(EXIT_FAILURE);
    }
    log::print() << "Bind Port :" << port
                 << " IP :" << inet_ntoa(_socket.addr.sin_addr) << log::endl;
}

void
    Socket::m_listen()
{
    if (listen(_socket.fd, SOMAXCONN) == -1)
    {
        log::print() << "Failed to listen on socket. errno: " << errno << ":"
                     << strerror(errno) << log::endl;
        exit(EXIT_FAILURE);
    }
    log::print() << "Listen on socket" << log::endl;
    fcntl(_socket.fd, F_SETFL, O_NONBLOCK);
    log::print() << "Socket set nonblock" << log::endl;
}

ssize_t
    Socket::receive(const struct kevent& event)
{
    _fd     = event.ident;
    _result = recv(_fd, _buffer, event.data, 0);

    if (_result == 0)
        m_disconnect();
    return _result;
}

ssize_t
    Socket::send(const struct kevent& event)
{
    _fd = event.ident;
    Client::t_to_client& to_client
        = ((Client*)event.udata)->get_buffers().to_client;
    _remain = to_client.queue.front().size() - to_client.offset;
    return (_result
            = ::send(_fd, to_client.queue.front().data() + to_client.offset,
                     event.data < _remain ? event.data : _remain, 0));
}

int
    Socket::accept()
{
    _fd = ::accept(_socket.fd, (sockaddr*)(&_addr), &_socket.len);

    if (_fd == -1)
        log::print() << "Failed to accept client errno: " << log::endl;
    else
        fcntl(_fd, F_SETFL, O_NONBLOCK);
    return _fd;
}

void
    Socket::disconnect(int fd)
{
    close(fd);
}

void
    Socket::initialize(int port)
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

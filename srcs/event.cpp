#include "../includes/event.hpp"

void
    Event::toggle(Client& client, int EVFILT_TYPE)
{
    int ident = client.get_socket();

    Event::set(ident, EVFILT_TYPE, EV_DISABLE, 0, 0, &client);
    Event::set(ident, (EVFILT_TYPE == EVFILT_READ ? EVFILT_WRITE : EVFILT_READ),
               EV_ENABLE, 0, 0, &client);
}

void
    Event::set(int     identity,
               short   filter,
               u_short flags,
               u_int   fflags,
               int     data,
               void*   udata)
{
    struct kevent kev;
    EV_SET(&kev, identity, filter, flags, fflags, data, udata);
    kevent(_kqueue, &kev, 1, NULL, 0, NULL);
}

void
    Event::m_create_kqueue(int socket_fd)
{
    _kqueue = kqueue();
    if (_kqueue == -1)
    {
        Logger().error() << "Failed to allocate kqueue. errno: " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Allocate kqueue " << _kqueue;
    Event::set(socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().info() << "Listen socket(" << socket_fd
                    << ") assign read event to kqueue";
}

#include "../includes/event.hpp"

void
    Event::m_set(int     identity,
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
    Event::toggle(Client& client, int EVFILT_TYPE)
{
    int ident = client.get_fd();

    m_set(ident, EVFILT_TYPE, EV_DISABLE, 0, 0, &client);
    m_set(ident, (EVFILT_TYPE == EVFILT_READ ? EVFILT_WRITE : EVFILT_READ),
          EV_ENABLE, 0, 0, &client);
}

void
    Event::remove(int fd)
{
    m_set(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    m_set(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
}

void
    Event::add(Client* client)
{
    m_set(client->get_fd(), EVFILT_READ, EV_ADD, 0, 0, client);
    m_set(client->get_fd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, client);
}

void
    Event::initialize(int socket_fd)
{
    _kqueue = kqueue();
    if (_kqueue == -1)
    {
        log::print() << "Failed to allocate kqueue. errno: " << errno << ":"
                     << strerror(errno) << log::endl;
        exit(FAILURE);
    }
    log::print() << "Allocate kqueue " << _kqueue << log::endl;
    m_set(socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    log::print() << "Listen socket(" << socket_fd
                 << ") assign read event to kqueue" << log::endl;
}

Event::Event()
{
}

Event::~Event()
{
}

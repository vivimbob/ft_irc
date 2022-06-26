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
    ::kevent(_kqueue, &kev, 1, NULL, 0, NULL);
}

void
    Event::toggle(int EVFILT_TYPE)
{
    m_set(_events[_index].ident, EVFILT_TYPE, EV_DISABLE, 0, 0,
          (Client*)_events[_index].udata);
    m_set(_events[_index].ident,
          (EVFILT_TYPE == EVFILT_READ ? EVFILT_WRITE : EVFILT_READ), EV_ENABLE,
          0, 0, (Client*)_events[_index].udata);
}

void
    Event::toggle(Client& client, int EVFILT_TYPE)
{
    m_set(client.get_fd(), EVFILT_TYPE, EV_DISABLE, 0, 0, &client);
    m_set(client.get_fd(),
          (EVFILT_TYPE == EVFILT_READ ? EVFILT_WRITE : EVFILT_READ), EV_ENABLE,
          0, 0, &client);
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

int
    Event::kevent()
{
    _count = ::kevent(_kqueue, NULL, 0, _events, EVENTS_MAX, NULL);
    log::print() << _count << " new kevent" << log::endl;
    return (_count);
}

void
    Event::initialize(int socket_fd)
{
    if ((_kqueue = ::kqueue()) == -1)
    {
        log::print() << "kqueue failed errno: " << errno << ":"
                     << strerror(errno) << log::endl;
        exit(FAILURE);
    }
    log::print() << "kqueue: " << _kqueue << log::endl;
    m_set(socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
}

Event::Event()
{
}

Event::~Event()
{
}

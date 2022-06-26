#ifndef EVENT_HPP
#define EVENT_HPP

#include "client.hpp"
#include "log.hpp"
#include <sys/event.h>
#include <sys/types.h>

class Client;

class Event
{
  private:
    Event(const Event&);
    Event& operator=(const Event&);
    void   m_set(int     identity,
                 short   filter,
                 u_short flags,
                 u_int   fflags,
                 int     data,
                 void*   udata);
    int    _kqueue;
    int    _count;

  protected:
    struct kevent _events[EVENTS_MAX];
    int           _index;
    void          toggle(int EVFILT_TYPE);
    void          toggle(Client&, int EVFILT_TYPE);
    void          initialize(int socket_fd);
    void          remove(int fd);
    void          add(Client*);
    int           kevent();

    Event();
    ~Event();
};

#endif /* EVENT_HPP */

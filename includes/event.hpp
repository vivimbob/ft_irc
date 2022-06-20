#ifndef EVENT_HPP
#define EVENT_HPP
#include "client.hpp"
#include "log.hpp"
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

class Client;

class Event
{
  private:
    Event(const Event&);
    Event& operator=(const Event&);

  protected:
    int  _kqueue;
    void set(int     identity,
             short   filter,
             u_short flags,
             u_int   fflags,
             int     data,
             void*   udata);
    void toggle(Client& client, int EVFILT_TYPE);
    void initialize(int socket_fd);
    Event();
    ~Event();
};

#endif /* EVENT_HPP */

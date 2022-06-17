#include "../lib/logger.hpp"
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

class Client;

class Event
{
  private:
    Event(const Event& copy);
    Event& operator=(const Event& copy);

  protected:
    int  _kqueue;
    void set(int     identity,
             short   filter,
             u_short flags,
             u_int   fflags,
             int     data,
             void*   udata);
    void toggle(Client& client, int EVFILT_TYPE);
    void m_create_kqueue(int socket_fd);
    Event();
    ~Event();
};

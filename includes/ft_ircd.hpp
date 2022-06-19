#pragma once
#ifndef FT_TEMP_HPP
#define FT_TEMP_HPP

#include "../lib/logger.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "event.hpp"
#include "ircd.hpp"
#include "resources.hpp"
#include "socket.hpp"
#include "utils.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class FT_IRCD : public Socket, public Event, public IRCD
{
  public:
    friend class IRCD;

  private:
    char          _buffer[IPV4_MTU_MAX];
    struct kevent _events[EVENTS_MAX];
    t_map         _map;

  private:
    FT_IRCD();
    FT_IRCD(const FT_IRCD&);
    FT_IRCD& operator=(const FT_IRCD&);

    void m_accept();
    void m_receive(struct kevent&);
    void m_send(struct kevent&);

    void m_handler();
    void m_handler(Client::t_request&);

    void m_disconnect(std::string reason = "");

  public:
    ~FT_IRCD();
    FT_IRCD(int port, char* password);
    void run();
};
#endif /* FT_TEMP_HPP */

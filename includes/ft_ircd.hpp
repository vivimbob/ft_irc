#pragma once
#ifndef FT_TEMP_HPP
#define FT_TEMP_HPP

#include "channel.hpp"
#include "client.hpp"
#include "event.hpp"
#include "ircd.hpp"
#include "resources.hpp"
#include "socket.hpp"

class FT_IRCD : public Socket, public Event, public IRCD
{
  public:
    friend class IRCD;

  private:
    t_map _map;

  private:
    FT_IRCD();
    FT_IRCD(const FT_IRCD&);
    FT_IRCD& operator=(const FT_IRCD&);

    void m_accept();
    void m_receive();
    void m_requests_handler();
    void m_disconnected(std::string reason = "");
    void m_disconnect(std::string reason = "");
    void m_send();

  public:
    FT_IRCD(int port, char* password);
    ~FT_IRCD();
    void run();
};
#endif /* FT_TEMP_HPP */

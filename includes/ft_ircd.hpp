#pragma once
#ifndef FT_TEMP_HPP
#define FT_TEMP_HPP

#include "bot.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "event.hpp"
#include "ircd.hpp"
#include "resources.hpp"
#include "socket.hpp"

class Bot;

class FT_IRCD : public Socket, public Event, public IRCD
{
  public:
    friend class IRCD;

  private:
    IRCD::t_map _map;
    Bot*        _bot;

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
    void m_create_bot();

  public:
    FT_IRCD(int port, const char* const password);
    ~FT_IRCD();
    void run();
};
#endif /* FT_TEMP_HPP */

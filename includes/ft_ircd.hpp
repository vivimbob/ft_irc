#ifndef FT_IRCD_HPP
#define FT_IRCD_HPP

#include "../lib/logger.hpp"
#include "buffer.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "daemon.hpp"
#include "event.hpp"
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

class FT_IRCD : public Socket, public Event, public Daemon
{
  public:
    friend class Daemon;
    typedef std::map<std::string, Client*>  ClientMap;
    typedef std::map<std::string, Channel*> ChannelMap;
    typedef struct s_map
    {
        ClientMap  client;
        ChannelMap channel;
    } t_map;

  private:
    char          _buffer[IPV4_MTU_MAX];
    struct kevent _events[EVENTS_MAX];
    std::string   _password;
    t_map         _map;

  private:
    FT_IRCD();
    FT_IRCD(const FT_IRCD& FT_IRCD);
    FT_IRCD& operator=(const FT_IRCD& FT_IRCD);

    void m_accept();
    void m_receive(struct kevent& event);
    void m_send(struct kevent& event);

    void m_requests_handler(Client& client, std::queue<std::string>& requests);
    void m_disconnect(Client& client, std::string reason = "");
    void m_regist(Client& client, Message& msg);

    void m_prepare_to_send(Client& client, const std::string& str_msg);
    void m_send_to_channel(Channel*           channel,
                           const std::string& msg,
                           Client*            exclusion = nullptr);
    void m_send_to_channel(Client&            client,
                           const std::string& msg,
                           Client*            exclusion = nullptr);

  public:
    ~FT_IRCD();
    FT_IRCD(int port, char* password);
    void run();
};
#endif /* FT_IRCD_HPP */

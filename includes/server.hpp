#ifndef Server_HPP
#define Server_HPP

#include "../lib/logger.hpp"
#include "buffer.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "event.hpp"
#include "ft_irc.hpp"
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

class Server : public Socket, public Event, public FT_IRC
{
  public:
    friend class FT_IRC;

  private:
    char          _buffer[IPV4_MTU_MAX];
    struct kevent _events[EVENTS_MAX];

  private:
    Server();
    Server(const Server& Server);
    Server& operator=(const Server& Server);

    void m_accept();
    void m_receive(struct kevent& event);
    void m_send(struct kevent& event);

    void m_requests_handler(Client& client, std::queue<std::string>& requests);
    void m_disconnect_client(Client& client, std::string reason = "");

    void m_register_client(Client& client, Message& msg);

    void m_prepare_to_send(Client& client, const std::string& str_msg);
    void m_send_to_channel(Channel*           channel,
                           const std::string& msg,
                           Client*            exclusion = nullptr);
    void m_send_to_channel(Client&            client,
                           const std::string& msg,
                           Client*            exclusion = nullptr);

  public:
    ~Server();
    Server(int port, char* password);
    void run();
};
#endif /* Server_HPP */

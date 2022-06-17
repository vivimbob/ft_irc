#ifndef SERVER_HPP
#define SERVER_HPP

#include "../lib/logger.hpp"
#include "buffer.hpp"
#include "channel.hpp"
#include "client.hpp"
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

class Server : public Socket, public Event
{
  public:
    typedef std::map<std::string, Client*>  ClientMap;
    typedef std::map<std::string, Channel*> ChannelMap;
    typedef std::map<std::string, void (Server::*)(Client&, Message&)>
        CommandMap;

  private:
    int               _kqueue;
    int               _fd;
    std::string       _password;
    char              _buffer[IPV4_MTU_MAX];
    struct kevent     _events[EVENTS_MAX];
    ClientMap         m_client_map;
    ChannelMap        m_channel_map;
    static CommandMap m_channel_command_map;
    static CommandMap m_register_command_map;

  private:
    Server();
    Server(const Server& server);
    Server& operator=(const Server& server);

    static CommandMap m_initial_channel_command_map();
    static CommandMap m_initial_register_command_map();

    void m_process_pass_command(Client& client, Message& msg);
    void m_process_nick_command(Client& client, Message& msg);
    void m_process_user_command(Client& client, Message& msg);
    void m_process_join_command(Client& client, Message& msg);
    void m_process_mode_command(Client& client, Message& msg);
    void m_process_quit_command(Client& client, Message& msg);
    void m_process_topic_command(Client& client, Message& msg);
    void m_process_part_command(Client& Client, Message& msg);
    void m_process_channel_mode_command(Client&            client,
                                        Message&           msg,
                                        const std::string& channel_name);
    void m_process_user_mode_command(Client&            client,
                                     Message&           msg,
                                     const std::string& nickname);
    void m_process_names_command(Client& client, Message& msg);
    void m_process_list_command(Client& client, Message& msg);
    void m_process_invite_command(Client& client, Message& msg);
    void m_process_privmsg_command(Client& client, Message& msg);
    void m_process_notice_command(Client& client, Message& msg);
    void m_process_kick_command(Client& client, Message& msg);

    void m_set_event(int     identity,
                     short   filter,
                     u_short flags,
                     u_int   fflags,
                     int     data,
                     void*   udata);
    void m_toggle_event(Client& client, int EVFILT_TYPE);
    void m_accept();
    void m_receive(struct kevent& event);
    void m_send(struct kevent& event);

    void m_requests_handler(std::queue<Message*>& requests);
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

#endif /* SERVER_HPP */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/event.h>
#include "./client.hpp"
#include "./sendbuffer.hpp"
#include "./channel.hpp"
#include "utils.hpp"

#define QUEUE_SIZE 1024
#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

class Server
{
	public:
		typedef std::map<std::string, Client*> ClientMap;
		typedef std::map<std::string, Channel*> ChannelMap;
        typedef std::map<std::string, void (Server::*)(Client&, IRCMessage&)> CommandMap;

    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        std::string m_password;
        sockaddr_in m_sockaddr;
        char m_read_buffer[IPV4_MTU_MAX];
        struct kevent m_event_list[QUEUE_SIZE];
        ClientMap m_client_map;
		    ChannelMap m_channel_map;
        static CommandMap m_command_map;
    
    private:
        Server(void);
        Server(const Server& cp);
        Server& operator=(const Server& cp);
    public:
        Server(int argc, char **argv);
        ~Server(void);
    
    private:
        void m_create_socket(void);
        void m_bind_socket(void);
        void m_listen_socket(void);
        void m_create_kqueue(void);
        void m_accept_client(void);
        void m_receive_client_msg(Client &client, int bytes);
        void m_send_client_msg(Client &client, int bytes);
        void m_update_event(int identity, short filter, u_short flags, u_int fflags, int data, void *udata);
        void m_disconnect_client(Client &client);
        void m_handle_messages(Client &client);

        void m_register_client(Client &client, IRCMessage &msg);
        void m_prepare_to_send(Client &client, const std::string &str_msg);
        void m_send_to_channel(Channel *channel, const std::string &msg);
        void m_send_to_channel(Client &client, const std::string &msg);
        void m_send_to_users(Client &client, IRCMessage &msg);

        static CommandMap m_initial_command_map(void);

        void m_process_pass_command(Client &client, IRCMessage &msg);
        void m_process_nick_command(Client &client, IRCMessage &msg);
        void m_process_user_command(Client &client, IRCMessage &msg);
        void m_process_join_command(Client &client, IRCMessage &msg);
        void m_process_mode_command(Client &client, IRCMessage &msg);
        void m_process_quit_command(Client &client, IRCMessage &msg);
        void m_process_topic_command(Client &client, IRCMessage &msg);
        void m_process_part_command(Client &Client, IRCMessage &msg);

        void m_join_channel(Client &client, IRCMessage &msg, ChannelKeyPairMap &chan_key_pair);

    public:
        void run(void);
};

#endif /* SERVER_HPP */

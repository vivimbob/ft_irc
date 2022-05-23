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

#define QUEUE_SIZE 1024
#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

class Server
{
	public:
		typedef std::map<int, Client*> Regstration_State_Map;
		typedef std::map<std::string, Client*> Client_map;
		typedef std::map<std::string, Channel*> Channel_map;
        typedef std::map<std::string, void (Server::*)(Client&, IRCMessage&)> Command_Map;

    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        std::string m_password;
        sockaddr_in m_sockaddr;
        char m_read_buffer[IPV4_MTU_MAX];
        struct kevent m_event_list[QUEUE_SIZE];
		Regstration_State_Map m_registration_state_map;
        Client_map m_client_map;
		Channel_map m_channel_map;
        static Command_Map m_command_map;
    
    private:
        Server(void);
        Server(const Server& cp);
        Server& operator=(const Server& cp);
    public:
        Server(int argc, char **argv);
        Server(int port, std::string password);
        ~Server(void);
    
    private:
        void create_socket(void);
        void bind_socket(void);
        void listen_socket(void);
        void create_kqueue(void);
        void accept_client(void);
        void receive_client_msg(unsigned int clientfd, int bytes);
        void send_client_msg(unsigned int clientfd, int bytes);
        void update_event(int identity, short filter, u_short flags, u_int fflags, int data, void *udata);
        void disconnect_client(unsigned int clientfd);
        void handle_messages(Client &client);

        void prepare_to_send(Client &client, const std::string &str_msg);

        static Command_Map initial_command_map(void);

        void process_pass_command(Client &client, IRCMessage &msg);
        void process_nick_command(Client &client, IRCMessage &msg);
        void process_user_command(Client &client, IRCMessage &msg);
        void process_mode_command(Client &client, IRCMessage &msg);

    public:
        void run(void);
};

#endif /* SERVER_HPP */

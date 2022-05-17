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

#define QUEUE_SIZE 1024
#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

class Server
{
    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        std::string m_password;
        sockaddr_in m_sockaddr;
        char m_read_buffer[IPV4_MTU_MAX];
        struct kevent m_event_list[QUEUE_SIZE];
        std::map<int, Client*> m_client_map;
    
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

    public:
        void run(void);
};

#endif /* SERVER_HPP */
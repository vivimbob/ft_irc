#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/event.h>
#include "./sendbuffer.hpp"

#define QUEUE_SIZE 1024

class Server
{
    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        std::string m_password;
        sockaddr_in m_sockaddr;
        struct kevent m_event_list[QUEUE_SIZE];
        struct m_client_info
        {
            sockaddr_in m_client_addr;
            int m_client_fd;
            SendBuffer m_send_buffer;
            std::string m_recv_buffer;

            m_client_info(sockaddr_in client_addr, int client_fd) 
              : m_client_addr(client_addr), m_client_fd(client_fd)
            {
            } 

            sockaddr_in m_get_client_addr()
            {
                return m_client_addr;
            }

            int m_get_socket()
            {
                return m_client_fd;
            }

            char* m_get_client_IP()
            {
                return inet_ntoa(m_client_addr.sin_addr);
            }
        };
        std::map<int, m_client_info*> m_client_map;
    
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

    public:
        void run(void);
};

#endif /* SERVER_HPP */
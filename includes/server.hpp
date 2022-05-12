#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>

class Server
{
    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        std::string m_password;
        sockaddr_in m_sockaddr;
    
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
        void update_event(int ident, short filter, u_short flags, u_int fflags, int data, void *udata);

    public:
        void run(void);
};

#endif /* SERVER_HPP */
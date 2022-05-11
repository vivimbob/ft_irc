#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server
{
    private:
        int m_kq;
        int m_listen_fd;
        int m_port;
        char* m_password;
    
    private:
        Server(void);
        Server(const Server& cp);
        Server& operator=(const Server& cp);
    public:
        Server(int argc, char **argv);
        Server(int port, std::string password);
        ~Server(void);
    
    public:
        void create_socket(void);
        void bind_socket(void);
        void listen_socket(void);
        void crate_kqueue(void);
        void assign_event(void);
        void get_event(void);
};
#endif /* SERVER_HPP */

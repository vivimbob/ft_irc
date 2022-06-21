#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "resources.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

class Channel;

class Client
{
  public:
    typedef struct s_names
    {
        std::string nick;
        std::string user;
        std::string host;
        std::string server;
        std::string real;
    } t_names;

    typedef struct s_request
    {
        std::string              command;
        std::vector<std::string> parameter;
        TYPE                     type;
        s_request(std::string line, TYPE type) : command(line), type(type){};
    } t_request;

    typedef struct s_requests
    {
        Client*                       from;
        std::queue<Client::t_request> queue;
    } t_requests;

    typedef struct s_to_client
    {
        int                     offset;
        std::queue<std::string> queue;
    } t_to_client;

    typedef struct s_buffers
    {
        int         offset;
        std::string buffer;
        t_requests  requests;
        t_to_client to_client;
    } t_buffers;

    typedef union
    {
        struct
        {
            unsigned char pass : 1;
            unsigned char nick : 1;
            unsigned char user : 1;
        };
        unsigned char registered;
    } t_status;

    typedef std::set<Channel*>::const_iterator t_citer;

  private:
    sockaddr_in        _addr;
    int                _fd;
    t_names            _names;
    t_buffers          _buffers;
    std::set<Channel*> _channels;
    t_status           _status;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client();
    sockaddr_in               get_addr();
    int                       get_fd();
    char*                     get_IP();
    const t_names&            get_names() const;
    t_buffers&                get_buffers();
    const std::set<Channel*>& get_channels() const;
    bool                      get_status(TYPE);

    void set_nickname(const std::string&);
    void set_username(const std::string&);
    void set_realname(const std::string&);
    void set_status(TYPE);

    bool is_registered() const;
    bool is_joined(Channel*);

    std::string get_nickmask();

    void joined(Channel*);
    void parted(Channel*);
};

#endif /* CLIENT_HPP */

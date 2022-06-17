#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_ircd.hpp"
#include "message.hpp"
#include "sendbuffer.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <queue>
#include <set>

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

    typedef struct s_buffers
    {
        Buffer request;
        Buffer response;
    } t_buffers;

  private:
    sockaddr_in          _addr;
    int                  _fd;
    Buffer               m_send_buffer;
    std::string          m_recv_buffer;
    t_names              _names;
    t_buffers            _buffers;
    std::queue<Message*> m_commands;
    std::set<Channel*>   m_channel_list;

    bool m_pass_registered;
    bool m_nick_registered;
    bool m_user_registered;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client();
    sockaddr_in               get_addr();
    int                       get_socket();
    char*                     get_IP();
    const t_names&            get_names() const;
    std::queue<Message*>&     get_commands();
    std::string&              get_recv_buffer();
    Buffer&                   get_send_buffer();
    const std::set<Channel*>& get_joined_list() const;

    void set_nickname(const std::string& nickname);
    void set_username(const std::string& username);
    void set_realname(const std::string& realname);
    void set_password_flag();
    void set_invisible_flag(bool toggle);
    void set_operator_flag(bool toggle);
    void set_server_notice_flag(bool toggle);
    void set_wallops_flag(bool toggle);

    bool is_registered() const;
    bool is_pass_registered() const;
    bool is_nick_registered() const;
    bool is_user_registered() const;
    bool is_join_available() const;
    bool is_already_joined(Channel* channel);
    bool is_invisible() const;

    void push_message(const std::string& message);

    std::string make_nickmask();

    void insert_channel(Channel* channel);
    void erase_channel(Channel* channel);
};

#endif /* CLIENT_HPP */

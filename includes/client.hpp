#ifndef CLIENT_HPP
#define CLIENT_HPP

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
  private:
    sockaddr_in          m_client_addr;
    int                  m_client_fd;
    SendBuffer           m_send_buffer;
    std::string          m_recv_buffer;
    std::string          m_nickname;
    std::string          m_username;
    const std::string    m_hostname;
    const std::string    m_servername;
    std::string          m_realname;
    std::queue<Message*> m_commands;
    std::set<Channel*>   m_channel_list;
    static const size_t  m_channel_limits = 10;

    bool m_pass_registered;
    bool m_nick_registered;
    bool m_user_registered;

  public:
    Client(sockaddr_in client_addr, int client_fd);
    ~Client();
    sockaddr_in               get_client_addr();
    int                       get_socket();
    char*                     get_client_IP();
    const std::string&        get_nickname() const;
    const std::string&        get_username() const;
    const std::string&        get_hostname() const;
    const std::string&        get_realname() const;
    std::queue<Message*>&     get_commands();
    std::string&              get_recv_buffer();
    SendBuffer&               get_send_buffer();
    size_t                    get_channel_limits();
    const std::set<Channel*>& get_channel_list() const;

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
    bool is_same_client(utils::ClientInfo client_info);

    void push_message(const std::string& message);

    std::string make_nickmask();

    void insert_channel(Channel* channel);
    void erase_channel(Channel* channel);
};

#endif /* CLIENT_HPP */

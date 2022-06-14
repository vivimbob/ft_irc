#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../includes/logger.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

Server::CommandMap Server::m_register_command_map =
    Server::m_initial_register_command_map();
Server::CommandMap Server::m_channel_command_map =
    Server::m_initial_channel_command_map();

Server::CommandMap
    Server::m_initial_register_command_map()
{
    Server::CommandMap temp_map;

    temp_map.insert(std::make_pair("PASS", &Server::m_process_pass_command));
    temp_map.insert(std::make_pair("NICK", &Server::m_process_nick_command));
    temp_map.insert(std::make_pair("USER", &Server::m_process_user_command));
    temp_map.insert(std::make_pair("QUIT", &Server::m_process_quit_command));

    return (temp_map);
}

Server::CommandMap
    Server::m_initial_channel_command_map()
{
    Server::CommandMap temp_map;

    temp_map.insert(std::make_pair("PASS", &Server::m_process_pass_command));
    temp_map.insert(std::make_pair("NICK", &Server::m_process_nick_command));
    temp_map.insert(std::make_pair("USER", &Server::m_process_user_command));
    temp_map.insert(std::make_pair("JOIN", &Server::m_process_join_command));
    temp_map.insert(std::make_pair("MODE", &Server::m_process_mode_command));
    temp_map.insert(std::make_pair("QUIT", &Server::m_process_quit_command));
    temp_map.insert(std::make_pair("TOPIC", &Server::m_process_topic_command));
    temp_map.insert(std::make_pair("PART", &Server::m_process_part_command));
    temp_map.insert(std::make_pair("NAMES", &Server::m_process_names_command));
    temp_map.insert(std::make_pair("LIST", &Server::m_process_list_command));
    temp_map.insert(
        std::make_pair("INVITE", &Server::m_process_invite_command));
    temp_map.insert(
        std::make_pair("PRIVMSG", &Server::m_process_privmsg_command));
    temp_map.insert(
        std::make_pair("NOTICE", &Server::m_process_notice_command));
    temp_map.insert(std::make_pair("KICK", &Server::m_process_kick_command));

    return (temp_map);
}

void
    Server::m_create_socket()
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd == -1)
    {
        Logger().error() << "Failed to create socket. errno " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Create socket " << m_listen_fd;
    int toggle = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&toggle,
               sizeof(toggle));
}

void
    Server::m_bind_socket()
{
    memset(&m_sockaddr, 0, sizeof(sockaddr_in));
    m_sockaddr.sin_family      = AF_INET;
    m_sockaddr.sin_addr.s_addr = INADDR_ANY;
    m_sockaddr.sin_port        = htons(m_port);

    if (bind(m_listen_fd, (struct sockaddr*)&m_sockaddr, sizeof(sockaddr_in)) ==
        -1)
    {
        Logger().error() << "Failed to bind to port and address" << m_port
                         << ". errno: " << errno << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Bind Port :" << m_port
                    << " IP :" << inet_ntoa(m_sockaddr.sin_addr);
}

void
    Server::m_listen_socket()
{
    if (listen(m_listen_fd, SOMAXCONN) == -1)
    {
        Logger().error() << "Failed to listen on socket. errno: " << errno
                         << ":" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Listen on socket";
    fcntl(m_listen_fd, F_SETFL, O_NONBLOCK);
    Logger().info() << "Socket set nonblock";
}

void
    Server::m_update_event(int     identity,
                           short   filter,
                           u_short flags,
                           u_int   fflags,
                           int     data,
                           void*   udata)
{
    struct kevent kev;
    EV_SET(&kev, identity, filter, flags, fflags, data, udata);
    kevent(m_kq, &kev, 1, NULL, 0, NULL);
}

void
    Server::m_create_kqueue()
{
    m_kq = kqueue();
    if (m_kq == -1)
    {
        Logger().error() << "Failed to allocate kqueue. errno: " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Allocate kqueue " << m_kq;
    m_update_event(m_listen_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().info() << "Listen socket(" << m_listen_fd
                    << ") assign read event to kqueue";
}

void
    Server::m_accept_client()
{
    sockaddr_in client_addr;
    int         client_addr_len = sizeof(client_addr);
    int         client_fd       = -1;

    client_fd = accept(m_listen_fd, (sockaddr*)(&client_addr),
                       (socklen_t*)(&client_addr_len));
    if (client_fd == -1)
    {
        Logger().error() << "Failed to accept client. errno: " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }

    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    Client* client_info = new Client(client_addr, client_fd);

    m_update_event(client_fd, EVFILT_READ, EV_ADD, 0, 0, client_info);
    m_update_event(client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0,
                   client_info);

    Logger().info() << "Accept client [address:"
                    << inet_ntoa(client_addr.sin_addr) << ":"
                    << client_addr.sin_port << "fd:" << client_fd << ']';
}

void
    Server::m_handle_messages(Client& client)
{
    while (client.get_commands().size())
    {
        Message* message = client.get_commands().front();
        Logger().debug() << client.get_nickname() << " send ["
                         << message->get_message() << ']';
        client.get_commands().pop();
        message->parse_message();
        if (m_command_map.count(message->get_command()))
            (this->*m_command_map[message->get_command()])(client, *message);
        else
            client.push_message(message->err_unknown_command(), Logger::Debug);
        delete message;
    }
}

void
    Server::m_disconnect_client(Client& client)
{
    const unsigned int& clientfd = client.get_socket();

    Logger().info() << "Client disconnect [address :" << client.get_client_IP()
                    << ':' << client.get_client_addr().sin_port
                    << " FD :" << clientfd << ']';

    m_update_event(clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    m_update_event(clientfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

    std::string message;
    if (client.get_commands().empty())
        message = Message(&client, "QUIT").build_quit_reply();
    else
        message = client.get_commands().front()->build_quit_reply();

    m_send_to_channel(client, message);
    client.leave_all_channel();
    m_client_map.erase(client.get_nickname());
    delete &client;
    close(clientfd);
}

void
    Server::m_register_client(Client& client, Message& msg)
{
    m_client_map[client.get_nickname()] = &client;
    client.push_message(msg.rpl_welcome(), Logger::Debug);
    Logger().info() << client.get_nickname() << " is registered to server";
}

void
    Server::m_receive_client_msg(Client& client, int data_len)
{
    const unsigned int& clientfd = client.get_socket();

    char* buffer = m_read_buffer;
    if (data_len <= IPV4_MTU_MIN)
        data_len = IPV4_MTU_MAX;

    ssize_t recv_data_len = recv(clientfd, buffer, data_len, 0);

    if (recv_data_len > 0)
    {
        std::string& recv_buffer = client.get_recv_buffer();
        recv_buffer.append(buffer, recv_data_len);

        size_t position = recv_buffer.find_first_of("\r\n", 0);
        while (position != static_cast<size_t>(std::string::npos))
        {
            client.get_commands().push(new Message(
                &client, std::string(recv_buffer.begin(),
                                     recv_buffer.begin() + position)));
            recv_buffer.erase(0, position + 2);
            position = recv_buffer.find_first_of("\r\n", 0);
        }

        Logger().info() << "Receive Message(" << client.get_commands().size()
                        << ") from " << client.get_nickname();

        if (client.get_commands().size())
        {
            m_handle_messages(client);
            m_update_event(clientfd, EVFILT_READ, EV_DISABLE, 0, 0, &client);
            m_update_event(clientfd, EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
            Logger().trace() << client.get_nickname() << " disable read event";
            Logger().trace() << client.get_nickname() << " enable write event";
        }
    }
    else if (recv_data_len == 0)
        m_disconnect_client(client);
}

void
    Server::m_send_client_msg(Client& client, int available_bytes)
{
    SendBuffer&         send_buffer      = client.get_send_buffer();
    int                 remain_data_len  = 0;
    int                 attempt_data_len = 0;
    const unsigned int& clientfd         = client.get_socket();

    if (available_bytes > IPV4_MTU_MAX)
        available_bytes = IPV4_MTU_MAX;
    else if (available_bytes == 0)
        available_bytes = IPV4_MTU_MIN;

    remain_data_len = send_buffer.size() - send_buffer.get_offset();

    if (available_bytes >= remain_data_len)
        attempt_data_len = remain_data_len;
    else
        attempt_data_len = available_bytes;

    ssize_t send_data_len =
        send(clientfd, send_buffer.data() + send_buffer.get_offset(),
             attempt_data_len, 0);

    if (send_data_len >= 0)
    {
        Logger().info() << "Server send to " << client.get_nickname();
        send_buffer.set_offset(send_buffer.get_offset() + send_data_len);
        Logger().trace() << "Send " << send_data_len << " bytes from ["
                         << clientfd << "] client";
        if (send_buffer.size() <= send_buffer.get_offset())
        {
            if (send_buffer.size())
                send_buffer.clear();
            Logger().trace() << "Empty buffer from [" << clientfd << "] client";
            m_update_event(clientfd, EVFILT_READ, EV_ENABLE, 0, 0, &client);
            m_update_event(clientfd, EVFILT_WRITE, EV_DISABLE, 0, 0, &client);
            Logger().trace() << client.get_nickname() << " enable read event";
            Logger().trace() << client.get_nickname() << " disable write event";
        }
    }
}

void
    Server::m_prepare_to_send(Client& client, const std::string& str_msg)
{
    client.push_message(str_msg);
    m_update_event(client.get_socket(), EVFILT_READ, EV_DISABLE, 0, 0, &client);
    m_update_event(client.get_socket(), EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
    Logger().trace() << client.get_nickname() << " disable read event";
    Logger().trace() << client.get_nickname() << " enable write event";
}

void
    Server::m_send_to_channel(Channel* channel, const std::string& msg)
{
    const Channel::MemberMap&          user_list = channel->get_user_list();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :"
                     << channel->get_channel_name();
    for (; user != user_list.end(); ++user)
        m_prepare_to_send(*user->first, msg);
}

void
    Server::m_send_to_channel(Client& client, const std::string& msg)
{
    std::set<Channel*>::iterator it = client.get_channel_list().begin();
    for (; it != client.get_channel_list().end(); ++it)
        m_send_to_channel(*it, msg);
}

void
    Server::m_initialize_server()
{
    m_create_socket();
    m_bind_socket();
    m_listen_socket();
    m_create_kqueue();
}

Server::~Server()
{
}

Server::Server(int argc, char** argv) : m_kq(-1), m_listen_fd(-1), m_port(-1)
{
    if (argc != 3)
    {
        Logger().error() << "Usage :" << argv[0] << " <port> <password>";
        exit(EXIT_FAILURE);
    }
    m_port = atoi(argv[1]);
    if (m_port < 0 || m_port > 65535)
        Logger().error() << m_port << "is out of Port range (0 ~ 65535)";
    m_password = argv[2];
    Logger().info() << "Server start";
    m_initialize_server();
}

void
    Server::run()
{
    int event_count = 0;

    Logger().info() << "[Server running]";
    while (true)
    {
        event_count = kevent(m_kq, NULL, 0, m_event_list, QUEUE_SIZE, NULL);
        Logger().trace() << event_count << " new kevent";
        for (int i = 0; i < event_count; ++i)
        {
            struct kevent& event = m_event_list[i];
            if (event.ident == (unsigned int)m_listen_fd)
                m_accept_client();
            else if (event.filter == EVFILT_READ)
                m_receive_client_msg(*(Client*)event.udata, event.data);
            else if (event.filter == EVFILT_WRITE)
                m_send_client_msg(*(Client*)event.udata, event.data);
        }
    }
}

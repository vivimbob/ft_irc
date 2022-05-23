#include "../includes/server.hpp"
#include "../includes/logger.hpp"
#include "../includes/client.hpp"
#include "../includes/utils.hpp"
#include "../includes/reply.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

Server::CommandMap Server::m_command_map = Server::initial_command_map();

Server::Server(int argc, char **argv)
    : m_kq(-1),
      m_listen_fd(-1),
      m_port(-1)
{
    if (argc != 3)
    {
        Logger().fatal() << "Usage :" << argv[0] << " <port> <password>";
        exit(EXIT_FAILURE);
    }
    m_port = atoi(argv[1]);
    if (m_port < 0 || m_port > 65535)
        Logger().fatal() << m_port << "is out of Port range (0 ~ 65535)";
    m_password = argv[2];
    create_socket();
    bind_socket();
    listen_socket();
    create_kqueue();
}

Server::Server(int port, std::string password)
    : m_kq(-1),
      m_listen_fd(-1),
      m_port(port),
      m_password(password)
{
    if (m_port < 0 || m_port > 65535)
        Logger().fatal() << m_port << "is out of Port range (0 ~ 65535)";
    create_socket();
    bind_socket();
    listen_socket();
    create_kqueue();
}

Server::~Server(void)
{
}

void
    Server::create_socket(void)
{
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd == -1)
    {
        Logger().fatal() << "Failed to create socket. errno " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Create socket " << m_listen_fd;
}

void
    Server::bind_socket(void)
{
    memset(&m_sockaddr, 0, sizeof(sockaddr_in));
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_addr.s_addr = INADDR_ANY;
    m_sockaddr.sin_port = htons(m_port);

    if (bind(m_listen_fd, (struct sockaddr *)&m_sockaddr, sizeof(sockaddr_in)) == -1)
    {
        Logger().fatal() << "Failed to bind to port and address" << m_port << ". errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Bind Port :" << m_port << " IP :" <<  inet_ntoa(m_sockaddr.sin_addr);
}

void
    Server::listen_socket(void)
{
    if (listen(m_listen_fd, SOMAXCONN) == -1)
    {
        Logger().fatal() << "Failed to listen on socket. errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Listen on socket";
    fcntl(m_listen_fd, F_SETFL, O_NONBLOCK);
    Logger().trace() << "Socket set nonblock";
}

void
    Server::create_kqueue(void)
{
    m_kq = kqueue();
    if (m_kq == -1)
    {
        Logger().fatal() << "Failed to allocate kqueue. errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().trace() << "Allocate kqueue " << m_kq;
    update_event(m_listen_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().trace() << "Socket " << m_listen_fd << " update to kqueue";
}

void
    Server::accept_client(void)
{
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client_fd = -1;

    client_fd = accept(m_listen_fd, (sockaddr*)(&client_addr), (socklen_t*)(&client_addr_len));
    if (client_fd == -1)
    {
        Logger().fatal() << "Failed to accept client. errno: " << errno;
        exit(EXIT_FAILURE);
    }

    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    Client* client_info = new Client(client_addr, client_fd);

    update_event(client_fd, EVFILT_READ, EV_ADD, 0, 0, client_info);
    update_event(client_fd, EVFILT_WRITE , EV_ADD | EV_DISABLE, 0, 0, client_info);

    Logger().trace() << "Accept client " << client_fd;
}

void
    Server::handle_messages(Client &client)
{
    while (client.m_commands.size())
    {
        IRCMessage *message = client.m_commands.front();
        client.m_commands.pop();
        message->parse_message();
        if (m_command_map.count(message->get_command()))
            (this->*m_command_map[message->get_command()])(client, *message);
        else
            Logger().error() << "ERR_UNKNOWNCOMMAND <" << message->get_command() << "> :Unknown command";//ERR_UNKNOWNCOMMAND
        delete message;
    }
}

void
    Server::receive_client_msg(Client &client, int data_len)
{
	const unsigned int &clientfd = client.m_get_socket();

    char *buffer = m_read_buffer;
    if (data_len <= IPV4_MTU_MIN)
      data_len = IPV4_MTU_MAX;

    ssize_t recv_data_len = recv(clientfd, buffer, data_len, 0);

    if (recv_data_len > 0) 
    {
        std::string &recv_buffer = client.m_recv_buffer;
        recv_buffer += buffer;
        
        int position = recv_buffer.find_first_of("\r\n", 0);
        while (position != std::string::npos)
        {
            client.m_commands.push(new IRCMessage(clientfd,
						std::string(recv_buffer.begin(), recv_buffer.begin() + position)));
            recv_buffer.erase(0, position + 2);
            position = recv_buffer.find_first_of("\r\n", 0);
        }

        Logger().trace() << "Receive Message";

        if (client.m_commands.size())
        {
            Logger().trace() << "Handle Messages";
            handle_messages(client);
            update_event(clientfd, EVFILT_READ, EV_DISABLE, 0, 0, &client);
            update_event(clientfd, EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
            client.m_recv_buffer.clear();
        }
    }
    else if (recv_data_len == 0)
        disconnect_client(client);
}

void
    Server::send_client_msg(Client &client, int available_bytes)
{
    SendBuffer &send_buffer = client.m_send_buffer;
    int remain_data_len = 0;
    int attempt_data_len = 0;
	const unsigned int &clientfd = client.m_get_socket();

    if (available_bytes > IPV4_MTU_MAX)
        available_bytes = IPV4_MTU_MAX;
    else if (available_bytes == 0)
        available_bytes = IPV4_MTU_MIN;

    remain_data_len = send_buffer.size() - send_buffer.get_offset();

    if (available_bytes >= remain_data_len)
        attempt_data_len = remain_data_len;
    else
        attempt_data_len = available_bytes;

    ssize_t send_data_len = send(clientfd,
			send_buffer.data() + send_buffer.get_offset(), attempt_data_len, 0);
  
    if (send_data_len >= 0)
    {
        Logger().trace() << "Send ok " << clientfd;
        send_buffer.set_offset(send_buffer.get_offset() + send_data_len);
        Logger().trace() << "Send " << send_data_len << " bytes from [" << clientfd << "] client";
        if (send_buffer.size() <= send_buffer.get_offset())
        {
            if (send_buffer.size())
              send_buffer.clear();
            Logger().trace() << "Empty buffer from [" << clientfd << "] client";
            update_event(clientfd, EVFILT_READ, EV_ENABLE, 0, 0, &client);
            update_event(clientfd, EVFILT_WRITE, EV_DISABLE, 0, 0, &client);
        }
    }
}

void
    Server::update_event(int identity, short filter, u_short flags, u_int fflags, int data, void *udata)
{
    struct kevent kev;
	EV_SET(&kev, identity, filter, flags, fflags, data, udata);
	kevent(m_kq, &kev, 1, NULL, 0, NULL);
}

void
    Server::run(void)
{
    int event_count = 0;

    while (true)
    {
        event_count = kevent(m_kq, NULL, 0, m_event_list, QUEUE_SIZE, NULL);
        for (int i = 0; i < event_count; ++i)
        {
            struct kevent &event = m_event_list[i];
            if (event.ident == (unsigned int)m_listen_fd)
                accept_client();
            else if (event.filter == EVFILT_READ)
                receive_client_msg(*(Client *)event.udata, event.data);
            else if(event.filter == EVFILT_WRITE)
                send_client_msg(*(Client *)event.udata, event.data);
        }
    }
}

void
    Server::disconnect_client(Client &client)
{
	const unsigned int& clientfd = client.m_get_socket();

    Logger().trace() << "Client disconnect IP :" << client.m_get_client_IP()
    << " FD :" << clientfd;

    update_event(clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    update_event(clientfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    
	m_client_map.erase(client.m_get_nickname());
    delete &client;
    close(clientfd);
}

Server::CommandMap
    Server::initial_command_map()
{
    Server::CommandMap temp_map;

    temp_map.insert(std::make_pair("PASS", &Server::process_pass_command));
    temp_map.insert(std::make_pair("NICK", &Server::process_nick_command));
    temp_map.insert(std::make_pair("USER", &Server::process_user_command));
    temp_map.insert(std::make_pair("MODE", &Server::process_mode_command));

    return (temp_map);
}

void
    Server::prepare_to_send(Client &client, const std::string &str_msg)
{
    client.m_send_buffer.append(str_msg);
    update_event(client.m_get_socket(), EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
    update_event(client.m_get_socket(), EVFILT_READ, EV_DISABLE, 0, 0, &client);
}

void
    Server::process_pass_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
        return ;
    }
    if (client.m_is_registered())
    {
        client.m_send_buffer.append(msg.err_already_registred(client));
        Logger().error() << "ERR_ALREADYREGISTRED :You may not reregister";
        return ;
    }
    client.m_set_password(msg.get_params()[0]);
    Logger().trace() << "Set password :" << client.m_get_password();
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		m_client_map[client.m_get_nickname()] = &client;
}

void
    Server::process_nick_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.m_send_buffer.append(msg.err_no_nickname_given(client));
        Logger().error() << "ERR_NONICKNAMEGIVEN :No nickname given";
        return ;
    }
    
    const std::string &nickname = msg.get_params()[0];

    if (!utils::is_nickname_valid(nickname))
    {
        prepare_to_send(client, msg.err_erroneus_nickname(client, nickname));
        Logger().error() << "ERR_ERRONEUSNICKNAME <" << nickname << "> :Erroneus nickname";
        return ; 
    }

	if (m_client_map.count(nickname))
	{
    	Logger().error() << "ERR_NICKNAMEINUSE <" << nickname << "> :Nickname is already in use";
        return ;
        // kill command 중복된 닉네임 가진 모든 클라이언트 연결 해제
	}

    client.m_set_nickname(nickname);
    Logger().trace() << "Set nickname :" << nickname;

    if (client.m_is_registered())
    {
		ClientMap::iterator it = m_client_map.begin();
        for (; it != m_client_map.end(); ++it)
        {
            prepare_to_send(*it->second, ":" + client.m_get_nickname() + " NICK " + nickname);
        }
    }

	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		m_client_map[client.m_get_nickname()] = &client;
}

void
  Server::process_user_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() != 4)
    {
        client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
        return ;
    }

    if (client.m_is_registered())
    {
        client.m_send_buffer.append(msg.err_already_registred(client));
        Logger().error() << "ERR_ALREADYREGISTRED :You may not reregister";
        return ;
    }

    const std::string &username = msg.get_params()[0];
    client.m_set_username(username);
    Logger().trace() << "Set username :" << username;
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		m_client_map[client.m_get_nickname()] = &client;
}

void
	Server::process_mode_command(Client &client, IRCMessage &msg)
{
	if (msg.get_params().size() < 2)
	{
		client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
		return ;
	}
	
	std::string target = msg.get_params()[0];
	if (strchr("&#+!", target[0]) != NULL)
	{
		//Channel Mode
		//서버 체널 관리 변수에서 실제로 있는지 체크
		//	없으면 ERR_NOSUCHCHANNEL
	}
	else
	{
		//User Mode
	}
}

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
	Logger().info() << "Server start";
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
	Logger().info() << "Server start";
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
    Logger().info() << "Create socket " << m_listen_fd;
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
    Logger().info() << "Bind Port :" << m_port << " IP :" <<  inet_ntoa(m_sockaddr.sin_addr);
}

void
    Server::listen_socket(void)
{
    if (listen(m_listen_fd, SOMAXCONN) == -1)
    {
        Logger().fatal() << "Failed to listen on socket. errno: " << errno;
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Listen on socket";
    fcntl(m_listen_fd, F_SETFL, O_NONBLOCK);
    Logger().info() << "Socket set nonblock";
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
    Logger().info() << "Allocate kqueue " << m_kq;
    update_event(m_listen_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().info() << "Listen socket(" << m_listen_fd << ") assign read event to kqueue";
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

    Logger().info() << "Accept client [address:"
		<< inet_ntoa(client_addr.sin_addr) << ":" << client_addr.sin_port <<
		"fd:" << client_fd << ']';
}

void
    Server::handle_messages(Client &client)
{
    while (client.m_commands.size())
    {
        IRCMessage *message = client.m_commands.front();
		Logger().debug() << client.m_get_nickname() << " send [" << message->get_message() << ']';
        client.m_commands.pop();
        message->parse_message();
        if (m_command_map.count(message->get_command()))
            (this->*m_command_map[message->get_command()])(client, *message);
        else
		{
			client.m_send_buffer.append(message->err_unknown_command());
            Logger().trace() << message->err_unknown_command();
		}
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
        recv_buffer.append(buffer, recv_data_len);
        
        size_t position = recv_buffer.find_first_of("\r\n", 0);
        while (position != static_cast<size_t>(std::string::npos))
        {
            client.m_commands.push(new IRCMessage(&client,
						std::string(recv_buffer.begin(), recv_buffer.begin() + position)));
            recv_buffer.erase(0, position + 2);
            position = recv_buffer.find_first_of("\r\n", 0);
        }

        Logger().info()
			<< "Receive Message(" << client.m_commands.size() <<
			") from " << client.m_get_nickname();

        if (client.m_commands.size())
        {
            handle_messages(client);
            update_event(clientfd, EVFILT_READ, EV_DISABLE, 0, 0, &client);
            update_event(clientfd, EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
			Logger().trace() << client.m_get_nickname() << " disable read event";
			Logger().trace() << client.m_get_nickname() << " enable write event";
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
        Logger().info() << "Server send to " << client.m_get_nickname();
        send_buffer.set_offset(send_buffer.get_offset() + send_data_len);
        Logger().trace() << "Send " << send_data_len << " bytes from [" << clientfd << "] client";
        if (send_buffer.size() <= send_buffer.get_offset())
        {
            if (send_buffer.size())
              send_buffer.clear();
            Logger().trace() << "Empty buffer from [" << clientfd << "] client";
            update_event(clientfd, EVFILT_READ, EV_ENABLE, 0, 0, &client);
            update_event(clientfd, EVFILT_WRITE, EV_DISABLE, 0, 0, &client);
			Logger().trace() << client.m_get_nickname() << " enable read event";
			Logger().trace() << client.m_get_nickname() << " disable write event";
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

	Logger().info() << "[Sever runnig]";
    while (true)
    {
        event_count = kevent(m_kq, NULL, 0, m_event_list, QUEUE_SIZE, NULL);
		Logger().trace() << event_count << " new kevent";
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

    Logger().info() << "Client disconnect [address :"
		<< client.m_get_client_IP() << ':' << client.m_get_client_addr().sin_port 
    	<< " FD :" << clientfd << ']';

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
    temp_map.insert(std::make_pair("JOIN", &Server::process_join_command));
    temp_map.insert(std::make_pair("MODE", &Server::process_mode_command));
    temp_map.insert(std::make_pair("QUIT", &Server::process_quit_command));
    temp_map.insert(std::make_pair("TOPIC", &Server::process_topic_command));
    temp_map.insert(std::make_pair("PART", &Server::process_part_command));


    return (temp_map);
}

void
    Server::prepare_to_send(Client &client, const std::string &str_msg)
{
    client.m_send_buffer.append(str_msg);
	Logger().trace() << client.m_get_nickname() << " [" << str_msg << ']';
    update_event(client.m_get_socket(), EVFILT_READ, EV_DISABLE, 0, 0, &client);
    update_event(client.m_get_socket(), EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
	Logger().trace() << client.m_get_nickname() << " disable read event";
	Logger().trace() << client.m_get_nickname() << " enable write event";
}

void
	Server::register_client(Client &client, IRCMessage &msg)
{
	m_client_map[client.m_get_nickname()] = &client;
	client.m_send_buffer.append(msg.rpl_welcome());
	Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_welcome() << ']';
	Logger().info() << client.m_get_nickname() << " is registed to server"; 
}

void
    Server::process_pass_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.m_send_buffer.append(msg.err_need_more_params());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
        return ;
    }
    if (client.m_is_registered())
    {
        client.m_send_buffer.append(msg.err_already_registred());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_already_registred() << ']';
        return ;
    }
    client.m_set_password(msg.get_params()[0]);
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}

void
    Server::process_nick_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.m_send_buffer.append(msg.err_no_nickname_given());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_no_nickname_given() << ']';
        return ;
    }
    
    const std::string &nickname = msg.get_params()[0];

    if (!utils::is_nickname_valid(nickname))
    {
        prepare_to_send(client, msg.err_erroneus_nickname(nickname));
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_erroneus_nickname(nickname) << ']';
        return ; 
    }

	if (m_client_map.count(nickname))
	{
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_nickname_in_use(nickname) << ']';
        return ;
        // kill command 중복된 닉네임 가진 모든 클라이언트 연결 해제
	}


    if (client.m_is_registered())
    {
		ClientMap::iterator it = m_client_map.begin();
        for (; it != m_client_map.end(); ++it)
        {
            prepare_to_send(*it->second, ":" + client.m_get_nickname() + " NICK " + nickname + "\r\n");
        }
    }
    Logger().debug() << client.m_get_client_IP()  << " change nick to " << nickname;
    client.m_set_nickname(nickname);

	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}

void
  Server::process_user_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() != 4)
    {
        client.m_send_buffer.append(msg.err_need_more_params());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
        return ;
    }

    if (client.m_is_registered())
    {
        client.m_send_buffer.append(msg.err_already_registred());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_already_registred() << ']';
        return ;
    }

    const std::string &username = msg.get_params()[0];
    const std::string &hostname = msg.get_params()[1];
    client.m_set_username(username);
    client.m_set_hostname(hostname);
    Logger().debug() << client.m_get_client_IP()  << " set username to " << username;
    Logger().debug() << client.m_get_client_IP()  << " set hostname to " << hostname;
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}

void
    Server::process_join_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() < 1)
    {
        client.m_send_buffer.append(msg.err_need_more_params());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
        return ;
    }

    std::map<const std::string, const std::string> channel_key_pair;
    std::vector<const std::string> splited_channel;

    utils::split_by_comma(splited_channel, msg.get_params()[0]);

    if (msg.get_params().size() == 2) // key가 있을 때
    {
        std::vector<const std::string> splited_key;
        utils::split_by_comma(splited_key, msg.get_params()[1]);
        if ((splited_channel.size() < splited_key.size())) // key개수가 채널 개수보다 많을 때
        {
            client.m_send_buffer.append(msg.err_bad_channel_key(msg.get_command()));
			Logger().trace() << client.m_get_nickname() << " [" << msg.err_bad_channel_key(msg.get_command()) << ']';
            return ;
        }
        std::vector<const std::string>::iterator itc = splited_channel.begin();
        std::vector<const std::string>::iterator itk = splited_key.begin();
        for (; itc != splited_channel.end(); ++itc)
        {
            if (itk == splited_key.end())
            {
                channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, ""));
                continue;
            }
            else
            {
                channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, *itk));
                ++itk;
            }
        }
    }
    else // key가 없을 때
    {
        std::vector<const std::string>::iterator itc = splited_channel.begin();
        for (; itc != splited_channel.end(); ++itc)
          channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, ""));
    }
    join_channel(client, msg, channel_key_pair);
}

void
    Server::join_channel(Client &client, IRCMessage &msg, std::map<const std::string, const std::string> &chan_key_pair)
{
    typedef std::map<const std::string, const std::string> chanKeyPair;
    chanKeyPair::iterator pair_it = chan_key_pair.begin();
    for (; pair_it != chan_key_pair.end(); ++pair_it) // 채널과 키쌍을 순회하면서 확인
    {
        if (!utils::is_channel_prefix(pair_it->first) || !utils::is_channel_name_valid(pair_it->first)) // 채널이름 앞에 #, & 있는지, 이름이 유효한지
        {
            client.m_send_buffer.append(msg.err_bad_chan_mask(pair_it->first));
			Logger().trace() << client.m_get_nickname() << " [" << msg.err_bad_chan_mask(pair_it->first) << ']';
            return ;
        }

        ChannelMap::iterator map_it = m_channel_map.find(pair_it->first);
        if ((map_it != m_channel_map.end()) && ((m_channel_map[pair_it->first]->m_get_mode_key()) && (pair_it->second != map_it->second->m_get_key()))) // join할 채널 이름은 있는데 키가 안 맞는 경우
        {
            client.m_send_buffer.append(msg.err_bad_channel_key(pair_it->first));
			Logger().trace() << client.m_get_nickname() << " [" << msg.err_bad_channel_key(msg.get_command()) << ']';
            return ;
        }
        else if (map_it == m_channel_map.end()) // join할 채널이 없는 경우(새로 만듦)
        {
            if (client.m_chan_key_lists.size() >= client.m_channel_limits) //join할 클라이언트가 이미 참여할 수 있는 채널 갯수에 도달했을때
            {
                client.m_send_buffer.append(msg.err_too_many_channels(pair_it->first));
				Logger().trace() << client.m_get_nickname() << " [" << msg.err_too_many_channels(pair_it->first) << ']';
                return ;
            }
            m_channel_map.insert(std::make_pair(pair_it->first, new Channel(pair_it->first, pair_it->second)));
            if (!pair_it->second.empty())// key값이 존재하면 key mosde 설정
                m_channel_map[pair_it->first]->m_set_mode_key(true);
            m_channel_map[pair_it->first]->m_add_user(client);
            m_channel_map[pair_it->first]->m_add_operator(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().info() << "Create new channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.m_get_nickname();
        }
        else // join할 채널이 존재하는 경우
        {
            size_t temp_channel_users = m_channel_map[pair_it->first]->m_get_user_lists().size();
            if (m_channel_map[pair_it->first]->m_get_mode_limit() && (temp_channel_users >= m_channel_map[pair_it->first]->m_get_user_limits())) // 현재 채널이 포함할 수 있는 최대 유저 수에 도달했을 때
            {
                client.m_send_buffer.append(msg.err_channel_is_full(pair_it->first));
				Logger().trace() << client.m_get_nickname() << " [" << msg.err_channel_is_full(pair_it->first) << ']';
                return ;
            }
            if (m_channel_map[pair_it->first]->m_get_mode_invite_only()) // invite-only인 경우
            {
                client.m_send_buffer.append(msg.err_invite_only_chan(pair_it->first));
				Logger().trace() << client.m_get_nickname() << " [" << msg.err_invite_only_chan(pair_it->first) << ']';
                return ;
            }
            if (client.m_chan_key_lists.count(pair_it->first)) // 이미 join된 경우
            {
                client.m_send_buffer.append(":You have already joined in <" + pair_it->first + "> channel\r\n");
                Logger().trace() << "Already joined: " << pair_it->first << " channel";
                return ;
            }
            if ((m_channel_map[pair_it->first]->m_get_mode_key()) && (pair_it->second != map_it->second->m_get_key())) // key mode인데 key가 안 맞을 때
            {
                client.m_send_buffer.append(msg.err_bad_channel_key(pair_it->first));
				Logger().trace() << client.m_get_nickname() << " [" << msg.err_bad_channel_key(pair_it->first) << ']';
                return ;
            }
            m_channel_map[pair_it->first]->m_add_user(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().info() << "Join channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.m_get_nickname();
        }
        const Channel::MemberMap &user_list = m_channel_map[pair_it->first]->m_get_user_lists();
        Channel::MemberMap::const_iterator user = user_list.begin();
        std::queue<const std::string> temp_nick_queue;
        for (; user != user_list.end(); ++user)
            temp_nick_queue.push(user->first->m_get_nickname());
        prepare_to_send(client, msg.rpl_namreply(pair_it->first, temp_nick_queue));
    Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_namreply(pair_it->first, temp_nick_queue) << ']';
    }
}

void
	Server::process_mode_command(Client &client, IRCMessage &msg)
{
	if (msg.get_params().size() < 1)
	{
		  client.m_send_buffer.append(msg.err_need_more_params());
		  Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
		  return ;
	}
	
	const std::string &target = msg.get_params()[0];

	if (strchr("&#", target[0]) != NULL)
	{
		std::string channel_name = target.substr(1);
		if (!m_channel_map.count(channel_name))
		{
			client.m_send_buffer.append(msg.err_no_such_channel(target));
		  Logger().trace() << client.m_get_nickname() << " [" << msg.err_no_such_channel(target) << ']';
			return ;
		}
		Channel *channel = m_channel_map.at(channel_name);
		if (msg.get_params().size() < 2)
		{
			client.m_send_buffer.append(msg.rpl_channel_mode_is(target, channel->m_get_channel_mode()));
			Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, channel->m_get_channel_mode()) << ']';
			return ;
		}
		if (!channel->m_is_operator(client))
		{
			client.m_send_buffer.append(msg.err_chanoprivs_needed(target));
			Logger().trace() << client.m_get_nickname() << " [" << msg.err_chanoprivs_needed(target) << ']';
			return ;
		}

		std::vector<std::string>::const_iterator parameter = msg.get_params().begin() + 1;
		std::vector<std::string>::const_iterator parameter_end = msg.get_params().end();
		std::string::const_iterator mode = parameter->begin();
		std::string::const_iterator mode_end = parameter->end();
		int	parameter_need_mode_count = 0;

		bool toggle = true;
		while (mode != mode_end && parameter_need_mode_count < 3)
		{
			switch (*mode)
			{
				case '+':
				case '-':
					break;
				case 'p':
					channel->m_set_private_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
				case 's':
					channel->m_set_secret_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
					break;
				case 'i':
					channel->m_set_invite_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
				case 't':
					channel->m_set_topic_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
				case 'n':
					channel->m_set_no_messages_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
				case 'm':
					channel->m_set_moderate_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					break;
				case 'k':
					if (toggle == true)
					{
						++parameter_need_mode_count;
						if (++parameter == parameter_end)
						{
							client.m_send_buffer.append(msg.err_need_more_params());
							Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
							break;
						}
						channel->m_set_key_flag(true, *parameter);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode, *parameter));
						Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode, *parameter) << ']';
					}
					else
					{
						channel->m_set_key_flag(false);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode));
						Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode) << ']';
					}
					break;
//				case 'b':
//					++parameter_need_mode_count;
//					if (++parameter == parameter_end)
//						;//rpl_ban
//					else
//						;//add ban mark to channel
//					break;
				case 'l':
					++parameter_need_mode_count;
					if (++parameter == parameter_end)
					{
						client.m_send_buffer.append(msg.err_need_more_params());
						Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
						break;
					}
					channel->m_set_limit(toggle, atoi(parameter.base()->data()));
					client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode, *parameter));
					Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode, *parameter) << ']';
					break;
				case 'o':
					++parameter_need_mode_count;
					if (++parameter == parameter_end)
					{
						client.m_send_buffer.append(msg.err_need_more_params());
						Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
						break;
					}
					{
						ClientMap::iterator iterator_user = m_client_map.find(*parameter.base());
						if (iterator_user == m_client_map.end())
						{
							client.m_send_buffer.append(msg.err_no_such_nick(iterator_user->first));
							Logger().trace() << client.m_get_nickname() << " [" << msg.err_no_such_nick(iterator_user->first) << ']';
							break;//no such nick;
						}
						Client * user = iterator_user->second;
						if (!channel->m_is_user_on_channel(user))
						{
							client.m_send_buffer.append(msg.err_not_on_channel(target));
							Logger().trace() << client.m_get_nickname() << " [" << msg.err_not_on_channel(target) << ']';
							break;//no to channel
						}
						channel->m_set_operator_flag(toggle, user);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode, *parameter));
						Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode, *parameter) << ']';
					}
					break;
				case 'v':
					++parameter_need_mode_count;
					if (++parameter == parameter_end)
					{
						client.m_send_buffer.append(msg.err_need_more_params());
						Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
						break;
					}
					{
						ClientMap::iterator iterator_user = m_client_map.find(*parameter.base());
						if (iterator_user == m_client_map.end())
						{
							client.m_send_buffer.append(msg.err_no_such_nick(iterator_user->first));
							Logger().trace() << client.m_get_nickname() << " [" << msg.err_no_such_nick(iterator_user->first) << ']';
							break;//no such nick;
						}
						Client * user = iterator_user->second;
						if (!channel->m_is_user_on_channel(user))
						{
							client.m_send_buffer.append(msg.err_not_on_channel(target));
							Logger().trace() << client.m_get_nickname() << " [" << msg.err_not_on_channel(target) << ']';
							break;//no to channel
						}
						channel->m_set_voice_flag(toggle, user);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(target, toggle, *mode, *parameter));
						Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_channel_mode_is(target, toggle, *mode, *parameter) << ']';
					}
					break;
				default:
					client.m_send_buffer.append(msg.err_unknown_mode(*mode));
					Logger().trace() << client.m_get_nickname() << " [" << msg.err_unknown_mode(*mode) << ']';
			}
			toggle = *mode == '-' ? false : true;
			++mode;
		}
	}
	else
	{
		if (target != client.m_get_nickname())
		{
			client.m_send_buffer.append(msg.err_users_dont_match());
			Logger().trace() << client.m_get_nickname() << " [" << msg.err_users_dont_match() << ']';
			return ;
		}

		if (msg.get_params().size() == 1)
		{
			client.m_send_buffer.append(msg.rpl_user_mode_is(client.m_get_usermode()));
			Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_user_mode_is(client.m_get_usermode()) << ']';
			return;
		}

		std::string::const_iterator it = msg.get_params()[1].begin();
		std::string::const_iterator ite = msg.get_params()[1].end();

		bool toggle = true;
		while (it != ite)
		{
			switch (*it)
			{
				case '+':
				case '-':
					break;
				case 'i':
					client.m_mode.i = toggle;
					toggle = true;
					break;
				case 'o':
					if (toggle == false)
						client.m_mode.o = toggle;
					toggle = true;
					break;
				case 's':
					client.m_mode.s = toggle;
					toggle = true;
					break;
				case 'w':
					client.m_mode.w = toggle;
					toggle = true;
					break;
				default:
					client.m_send_buffer.append(msg.err_u_mode_unknown_flag());
					Logger().trace() << client.m_get_nickname() << " [" << msg.err_u_mode_unknown_flag() << ']';
			}
			toggle = *it == '-' ? false : true;
			++it;
		}
		client.m_send_buffer.append(msg.rpl_user_mode_is(client.m_get_usermode()));
		Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_user_mode_is(client.m_get_usermode()) << ']';
	}
}

void
    Server::process_quit_command(Client &client, IRCMessage &msg)
{
    send_to_channel(client, build_messages(client, msg));
    disconnect_client(client);
}

void
    Server::process_topic_command(Client &client, IRCMessage &msg)
{
	if (msg.get_params().empty())
	{
		client.m_send_buffer.append(msg.err_need_more_params());
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
	}

	const std::string& channel_name = msg.get_params()[0];
	
	if (!m_channel_map.count(channel_name))
	{
		client.m_send_buffer.append(msg.err_no_such_channel(channel_name));
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_no_such_channel(channel_name) << ']';
	}

	Channel* channel = m_channel_map[channel_name];

	if (msg.get_params().size() == 1)
	{
		client.m_send_buffer.append(msg.rpl_topic(channel_name, channel->m_get_channel_topic()));
		Logger().trace() << client.m_get_nickname() << " [" << msg.rpl_topic(channel_name, channel->m_get_channel_topic()) << ']';
		return ;
	}

	if (channel->m_is_protected_topic() && !channel->m_is_operator(client))	
	{
		client.m_send_buffer.append(msg.err_chanoprivs_needed(channel_name));
		Logger().trace() << client.m_get_nickname() << " [" << msg.err_chanoprivs_needed(channel_name) << ']';
		return ;
	}

	if (msg.get_params().size() == 2)
	{
		channel->m_set_channel_topic(msg.get_params()[1]);
	}
}

void
    Server::process_part_command(Client &client, IRCMessage &msg)
{
		if (msg.get_params().size() < 1 || msg.get_params().size() > 2)
    {
        client.m_send_buffer.append(msg.err_need_more_params());
        Logger().info() << client.m_get_nickname() << " [" << msg.err_need_more_params() << ']';
        return ;
    }
}

void
  Server::send_to_channel(Channel *channel, const std::string &msg)
{
	const Channel::MemberMap &user_list = channel->m_get_user_lists();
	Channel::MemberMap::const_iterator user = user_list.begin();
	
	Logger().trace() << "send message to channel :" << channel->m_get_channel_name();
	for (; user != user_list.end(); ++user)
		prepare_to_send(*user->first, msg);
}

void
  Server::send_to_channel(Client &client, const std::string &msg)
{
    std::map<const std::string, const std::string>::iterator it = client.m_chan_key_lists.begin();
    for (; it != client.m_chan_key_lists.end(); ++it)
		send_to_channel(m_channel_map[it->first], msg);	
}

std::string
  Server::build_messages(Client &client, IRCMessage &msg)
{
    std::string temp_param = "";
    if (!msg.get_params().empty())
        temp_param = msg.get_params()[0];
    std::string temp_com = msg.get_command();
    std::string temp_msg;

    if (temp_com == "QUIT")
    {
        temp_msg = client.m_get_nickname() + '!' + client.m_get_username() + '@' + client.m_get_hostname();
        temp_msg += " QUIT :" + temp_param + "\r\n";
    }
    return temp_msg;
}

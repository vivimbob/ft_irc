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
		{
			client.m_send_buffer.append(message->err_unknown_command(client, message->get_command()));
            Logger().error() << "ERR_UNKNOWNCOMMAND <" << message->get_command() << "> :Unknown command";
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
    temp_map.insert(std::make_pair("JOIN", &Server::process_join_command));
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


    if (client.m_is_registered())
    {
		    ClientMap::iterator it = m_client_map.begin();
        for (; it != m_client_map.end(); ++it)
        {
            prepare_to_send(*it->second, ":" + client.m_get_nickname() + " NICK " + nickname + "\r\n");
        }
    }
    client.m_set_nickname(nickname);
    Logger().trace() << "Set nickname :" << nickname;

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
    Server::process_join_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() < 1)
    {
        client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
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
            client.m_send_buffer.append(msg.err_bad_channel_key(client, msg.get_command()));
            Logger().error() << "ERR_BADCHANNELKEY <" << msg.get_command() << "> :Cannot join channel (+k)";
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
            client.m_send_buffer.append(msg.err_bad_chan_mask(client, pair_it->first));
            Logger().error() << "ERR_BADCHANMASK <" << msg.get_command() << "> :Bad Channel Mask";
            return ;
        }

        ChannelMap::iterator map_it = m_channel_map.find(pair_it->first);
        if ((map_it != m_channel_map.end()) && ((m_channel_map[pair_it->first]->m_get_mode_key()) && (pair_it->second != map_it->second->m_get_key()))) // join할 채널 이름은 있는데 키가 안 맞는 경우
        {
            client.m_send_buffer.append(msg.err_bad_channel_key(client, pair_it->first));
            Logger().error() << "ERR_BADCHANNELKEY <" << client.m_get_nickname() << "> <" << pair_it->first << "> :Cannot join channel (+k)";
            return ;
        }
        else if (map_it == m_channel_map.end()) // join할 채널이 없는 경우(새로 만듦)
        {
            if (client.m_chan_key_lists.size() >= client.m_channel_limits) //join할 클라이언트가 이미 참여할 수 있는 채널 갯수에 도달했을때
            {
                client.m_send_buffer.append(msg.err_too_many_channels(client, pair_it->first));
                Logger().error() << "ERR_TOOMANYCHANNELS <" << client.m_get_nickname() << "> <" << pair_it->first << "> :You have joined too many channels";
                return ;
            }
            m_channel_map.insert(std::make_pair(pair_it->first, new Channel(pair_it->first, pair_it->second)));
            if (!pair_it->second.empty())// key값이 존재하면 key mosde 설정
                m_channel_map[pair_it->first]->m_set_mode_key(true);
            m_channel_map[pair_it->first]->m_add_user(client);
            m_channel_map[pair_it->first]->m_add_operator(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().trace() << "Create new channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.m_get_nickname();
        }
        else // join할 채널이 존재하는 경우
        {
            size_t temp_channel_users = m_channel_map[pair_it->first]->m_get_user_lists().size();
            if (m_channel_map[pair_it->first]->m_get_mode_limit() && (temp_channel_users >= m_channel_map[pair_it->first]->m_get_user_limits())) // 현재 채널이 포함할 수 있는 최대 유저 수에 도달했을 때
            {
                client.m_send_buffer.append(msg.err_channel_is_full(client, pair_it->first));
                Logger().error() << "ERR_CHANNELISFULL <" << client.m_get_nickname() << "> <" << pair_it->first << "> :Cannot join channel (+l)";
                return ;
            }
            if (m_channel_map[pair_it->first]->m_get_mode_invite_only()) // invite-only인 경우
            {
                client.m_send_buffer.append(msg.err_invite_only_chan(client, pair_it->first));
                Logger().error() << "ERR_INVITEONLYCHAN <" << client.m_get_nickname() << "> <" << pair_it->first << "> :Cannot join channel (+i)";
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
                client.m_send_buffer.append(msg.err_bad_channel_key(client, pair_it->first));
                Logger().error() << "ERR_BADCHANNELKEY <" << client.m_get_nickname() << "> <" << pair_it->first << "> :Cannot join channel (+k)";
                return ;
            }
            m_channel_map[pair_it->first]->m_add_user(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().trace() << "Join channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.m_get_nickname();
        }
        ClientMap::iterator it = m_client_map.begin();
        std::queue<const std::string> temp_nick_queue;
        for (; it != m_client_map.end(); ++it)
            temp_nick_queue.push(it->first);
        prepare_to_send(client, msg.rpl_namreply(client, pair_it->first, temp_nick_queue));
        Logger().info() << "RPL_NAMREPLY";
    }
}

void
	Server::process_mode_command(Client &client, IRCMessage &msg)
{
	if (msg.get_params().size() < 1)
	{
		  client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
      Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
		  return ;
	}
	
	const std::string &target = msg.get_params()[0];

	if (strchr("&#", target[0]) != NULL)
	{
		std::string channel_name = target.substr(1);
		if (!m_channel_map.count(channel_name))
		{
			client.m_send_buffer.append(msg.err_no_such_channel(client, target));
			Logger().error() << "ERR_NOSUCHCHANNEL :" << msg.err_no_such_channel(client, target);
			return ;
		}
		Channel *channel = m_channel_map.at(channel_name);
		if (msg.get_params().size() < 2)
		{
			client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, channel->m_get_channel_mode()));
			Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, channel->m_get_channel_mode());
			return ;
		}
		if (!channel->m_is_operator(client))
		{
			client.m_send_buffer.append(msg.err_chanoprivs_needed(client, target));
			Logger().trace() << "ERR_CHANOPRIVSNEEDED: " << msg.err_chanoprivs_needed(client, target);
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
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
				case 's':
					channel->m_set_secret_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
					break;
				case 'i':
					channel->m_set_invite_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
				case 't':
					channel->m_set_topic_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
				case 'n':
					channel->m_set_no_messages_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
				case 'm':
					channel->m_set_moderate_flag(toggle);
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
					Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
					break;
				case 'k':
					if (toggle == true)
					{
						++parameter_need_mode_count;
						if (++parameter == parameter_end)
						{
							client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
							Logger().error() << "ERR_NEEDMOREPARAMS :" << msg.err_need_more_params(client, msg.get_command());
							break;
						}
						channel->m_set_key_flag(true, *parameter);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter));
						Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter);
					}
					else
					{
						channel->m_set_key_flag(false);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode));
						Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode);
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
						client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        				Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
						break;
					}
					channel->m_set_limit(toggle, atoi(parameter.base()->data()));
					client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter));
						Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter);
					break;
				case 'o':
					++parameter_need_mode_count;
					if (++parameter == parameter_end)
					{
						client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        				Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
						break;
					}
					{
						ClientMap::iterator iterator_user = m_client_map.find(*parameter.base());
						if (iterator_user == m_client_map.end())
						{
							client.m_send_buffer.append(msg.err_no_such_nick(client, iterator_user->first));
        					Logger().error() << "ERR_NOSUCHNICK :" << msg.err_no_such_nick(client, iterator_user->first);
							break;//no such nick;
						}
						Client * user = iterator_user->second;
						if (!channel->m_is_user_on_channel(user))
						{
							client.m_send_buffer.append(msg.err_not_on_channel(client, target));
							Logger().error() << "ERR_NOTONCHANNEL :" << msg.err_not_on_channel(client, target);
							break;//no to channel
						}
						channel->m_set_operator_flag(toggle, user);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter));
						Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter);
					}
					break;
				case 'v':
					++parameter_need_mode_count;
					if (++parameter == parameter_end)
					{
						client.m_send_buffer.append(msg.err_need_more_params(client, msg.get_command()));
        				Logger().error() << "ERR_NEEDMOREPARAMS <" << msg.get_command() << "> :Not enough parameters";
						break;
					}
					{
						ClientMap::iterator iterator_user = m_client_map.find(*parameter.base());
						if (iterator_user == m_client_map.end())
						{
							client.m_send_buffer.append(msg.err_no_such_nick(client, iterator_user->first));
        					Logger().error() << "ERR_NOSUCHNICK :" << msg.err_no_such_nick(client, iterator_user->first);
							break;//no such nick;
						}
						Client * user = iterator_user->second;
						if (!channel->m_is_user_on_channel(user))
						{
							client.m_send_buffer.append(msg.err_not_on_channel(client, target));
							Logger().error() << "ERR_NOTONCHANNEL :" << msg.err_not_on_channel(client, target);
							break;//no to channel
						}
						channel->m_set_voice_flag(toggle, user);
						client.m_send_buffer.append(msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter));
						Logger().trace() << "RPL_CHANNELMODEIS :" << msg.rpl_channel_mode_is(client, target, toggle, *mode, *parameter);
					}
					break;
				default:
					client.m_send_buffer.append(msg.err_unknown_mode(client, *mode));
					Logger().error() << "ERR_UNKNOWNMODE <" << msg.get_command() << "> :Unknown MODE flag :" << *mode;
			}
			toggle = *mode == '-' ? false : true;
			++mode;
		}
	}
	else
	{
		if (target != client.m_get_nickname())
		{
			client.m_send_buffer.append(msg.err_users_dont_match(client));
			Logger().error() << "ERR_USERDONTMATCH <" << msg.get_command() << "> :Cant change mode for other users";
			return ;
		}

		if (msg.get_params().size() == 1)
		{
			client.m_send_buffer.append(msg.rpl_user_mode_is(client, client.m_get_usermode()));
			Logger().trace() << msg.rpl_user_mode_is(client, client.m_get_usermode());
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
					client.m_send_buffer.append(msg.err_u_mode_unknown_flag(client));
					Logger().error() << "ERR_UMODEUNKNOWNFLAG <" << msg.get_command() << "> :Unknown MODE flag :" << *it;
			}
			toggle = *it == '-' ? false : true;
			++it;
		}
		client.m_send_buffer.append(msg.rpl_user_mode_is(client, client.m_get_usermode()));
		Logger().trace() << client.m_get_nickname() << " user mode change " << client.m_get_usermode();
	}
}

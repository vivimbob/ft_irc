#include "../includes/ft_ircd.hpp"

void
    FT_IRCD::m_prepare_to_send(Client& client, const std::string& str_msg)
{
    client.push_message(str_msg);
    Event::toggle(client, EVFILT_READ);
}

void
    FT_IRCD::m_send_to_channel(Channel*           channel,
                               const std::string& msg,
                               Client*            exclusion)
{
    const Channel::MemberMap&          user_list = channel->get_members();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :" << channel->get_name();
    for (; user != user_list.end(); ++user)
        if (user->first != exclusion)
            m_prepare_to_send(*user->first, msg);
}

void
    FT_IRCD::m_send_to_channel(Client&            client,
                               const std::string& msg,
                               Client*            exclusion)
{
    std::set<Channel*>::iterator it = client.get_joined_list().begin();
    for (; it != client.get_joined_list().end(); ++it)
        m_send_to_channel(*it, msg, exclusion);
}

void
    FT_IRCD::m_handler(Client::t_request& request)
{
    if (request.line.size() && (request.line[0] == ':'))
    {
        request.line.erase(0, request.line.find_first_of(' '));
        request.line.erase(0, request.line.find_first_not_of(' '));
    }
    if (request.line.size())
    {
        int offset;
        int fixed;

        for (offset = 0;
             (request.line[offset] != ' ' && request.line[offset] != '\0');
             ++offset)
            if ((unsigned)request.line[offset] - 'a' < 26)
                request.line[offset] ^= 0b100000;
        request.type = Daemon::get_type(request.line.substr(0, offset));
        fixed        = offset;
        while (request.line[offset] == ' ')
            ++offset;
        if (request.line[offset] != '\0')
            request.parameter.push_back(request.line.substr(offset));
        request.line.erase(fixed);

        std::vector<std::string>::iterator iter = request.parameter.begin();
        offset                                  = 0;

        while ((iter != request.parameter.end()) && (offset < iter->size()))
        {
            if (iter->c_str()[offset] == ':')
                iter->erase(0, offset + 1);
            else
            {
                offset = iter->find_first_of(' ', offset);
                fixed  = offset;
                offset = iter->find_first_not_of(' ', offset);
                if (iter->c_str()[offset] != '\0')
                    request.parameter.push_back(iter->substr(fixed, offset));
                iter->erase(fixed);
            }
            offset = 0;
            ++iter;
        }
    }
}

void
    FT_IRCD::m_handler(Client::t_requests& requests)
{
    while (requests.queue.size())
    {
        Client::t_request& request = requests.queue.front();
        m_handler(request);

        // switch?
        // ERROR, EMPTY는 default나 다음으로 직행
        // PASS~QUIT이면 실행
        // JOIN~NOTICE면 등록됐을 경우 실행
        //	안됐으면 에러메시지
        // UNKNOWN이면 에러메시지
        if (request.type >= PASS && request.type <= QUIT)
            Daemon::_command[request.type](request);
        else if (request.type >= JOIN && request.type <= NOTICE)
        {
            if (requests.from->is_registered())
                ; //    (this->*_command_map[command])(message);
            else
                ; //  utils::push_message(*requests.from,
            //                    message.err_not_registered());
        }
        else if (!command.empty())
            utils::push_message(*requests.from, message.err_unknown_command());
        requests.queue.pop();
    }
}

void
    FT_IRCD::m_disconnect(Client& client, std::string reason)
{
    const int& fd = client.get_socket();

    Logger().info() << "Client disconnect [address :" << client.get_IP() << ':'
                    << client.get_addr().sin_port << " FD :" << fd << ']';

    Event::set(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    Event::set(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    close(fd);

    std::set<Channel*>::iterator it  = client.get_joined_list().begin();
    std::set<Channel*>::iterator ite = client.get_joined_list().end();
    std::set<Client*>            check;

    for (; it != ite; ++it)
    {
        const Channel::MemberMap&          users = (*it)->get_members();
        Channel::MemberMap::const_iterator user  = users.begin();

        for (; user != users.end(); ++user)
            if (!check.count(user->first) && user->first != &client)
            {
                check.insert(user->first);
                m_prepare_to_send(*user->first,
                                  Message("QUIT").build_quit_reply(reason));
            }
        (*it)->part(client);
        if ((*it)->is_empty())
        {
            _map.channel.erase((*it)->get_name());
            delete (*it);
        }
    }
    _map.client.erase(client.get_names().nick);
    delete &client;
}

void
    FT_IRCD::m_regist(Client& client, Message& msg)
{
    _map.client[client.get_names().nick] = &client;
    utils::push_message(client, msg.rpl_welcome());
    Logger().info() << client.get_names().nick << " is registered to server";
}

void
    FT_IRCD::m_send(struct kevent& event)
{
    Client&             client          = *(Client*)event.udata;
    Buffer&             buffer          = client.get_buffers().to_client;
    int                 remain_data_len = buffer.size() - buffer.get_offset();
    const unsigned int& clientfd        = client.get_socket();

    ssize_t send_data_len =
        send(clientfd, buffer.data() + buffer.get_offset(),
             event.data < remain_data_len ? event.data : remain_data_len, 0);

    if (send_data_len >= 0)
    {
        Logger().info() << "Daemon send to " << client.get_names().nick;
        buffer.set_offset(buffer.get_offset() + send_data_len);
        Logger().trace() << "Send " << send_data_len << " bytes from ["
                         << clientfd << "] client";
        if (buffer.size() <= buffer.get_offset())
        {
            if (buffer.size())
                buffer.clear();
            Logger().trace() << "Empty buffer from [" << clientfd << "] client";
            Event::toggle(client, EVFILT_WRITE);
        }
    }
}

void
    FT_IRCD::m_receive(struct kevent& event)
{
    Client& client = *(Client*)event.udata;
    ssize_t length = recv(event.ident, _buffer, event.data, 0);

    if (length > 0)
    {
        Client::t_buffers& buffers = client.get_buffers();
        buffers.buffer.append(_buffer, length);
        while ((buffers.offset = buffers.buffer.find_first_of("\r\n", 0)) !=
               (int)std::string::npos)
        {
            buffers.requests.queue.push(Client::s_request(
                buffers.buffer.substr(0, buffers.offset), UNKNOWN));
            buffers.buffer.erase(0, buffers.offset + 2);
        }
        if (buffers.requests.queue.size())
            m_handler(buffers.requests);
        if (buffers.to_client.size())
            Event::toggle(client, EVFILT_READ);
    }
    else if (length == 0)
        m_disconnect(client, "connection closed");
}

void
    FT_IRCD::m_accept()
{
    sockaddr_in addr;
    int         fd = accept(_socket.fd, (sockaddr*)(&addr), &_socket.len);

    if (fd == -1)
    {
        Logger().error() << "Failed to accept client errno: ";
        return;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);
    Client* client = new Client(addr, fd);

    Event::set(fd, EVFILT_READ, EV_ADD, 0, 0, client);
    Event::set(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, client);

    Logger().info() << "Accept client [address:" << inet_ntoa(addr.sin_addr)
                    << ":" << addr.sin_port << "fd:" << fd << ']';
}

// struct kevent {
//	uintptr_t       ident;  /* identifier for this event */
//	int16_t         filter; /* filter for event */
//	uint16_t        flags;  /* general flags */
//	uint32_t        fflags; /* filter-specific flags */
//	intptr_t        data;   /* filter-specific data */
//	void            *udata; /* opaque user data identifier */
// };

void
    FT_IRCD::run()
{
    register int count;
    register int index;

    Logger().info() << "[Daemon is running]";
    while (true)
    {
        count = kevent(Event::_kqueue, NULL, 0, _events, EVENTS_MAX, NULL);
        Logger().trace() << count << " new kevent";
        for (index = 0; index < count; ++index)
        {
            if (_events[index].ident == (unsigned)_socket.fd)
                FT_IRCD::m_accept();
            else if (_events[index].filter == EVFILT_READ)
                FT_IRCD::m_receive(_events[index]);
            else if (_events[index].filter == EVFILT_WRITE)
                FT_IRCD::m_send(_events[index]);
        }
    }
}

FT_IRCD::~FT_IRCD()
{
}

FT_IRCD::FT_IRCD(int port, char* password) : _password(password)
{
    Socket::m_initialize(port);
    Event::m_create_kqueue(_socket.fd);
    Daemon::_ft_ircd = this;
}

int
    main(int argc, char** argv)
{
    int port;

    if (argc != 3)
    {
        Logger().error() << "Usage :" << argv[0] << " <port> <password>";
        return EXIT_FAILURE;
    }
    if (PORT_MAX < (unsigned)(port = atoi(argv[1])))
    {
        Logger().error() << port << "is out of Port range (0 ~ 65535)";
        return EXIT_FAILURE;
    }

    FT_IRCD(port, argv[2]).run();
}

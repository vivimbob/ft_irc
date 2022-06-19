#include "../includes/ft_ircd.hpp"
#include <cstddef>

void
    FT_IRCD::m_disconnect(Client& client, std::string reason)
{
    const int& fd = client.get_socket();

    Logger().info() << "Client disconnect [address :" << client.get_IP() << ':'
                    << client.get_addr().sin_port << " FD :" << fd << ']';

    Event::set(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    Event::set(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    close(fd);

    std::set<Channel*>::iterator it  = client.get_channels().begin();
    std::set<Channel*>::iterator ite = client.get_channels().end();
    std::set<Client*>            check;

    for (; it != ite; ++it)
    {
        const Channel::MemberMap&          users = (*it)->get_members();
        Channel::MemberMap::const_iterator user  = users.begin();

        for (; user != users.end(); ++user)
            if (!check.count(user->first) && user->first != &client)
            {
                check.insert(user->first);
                // IRCD::m_to_client(*user->first,
                //                   IRC("QUIT").build_quit_reply(reason));
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
    FT_IRCD::m_send(struct kevent& event)
{
    // Client&             client          = *(Client*)event.udata;
    // t_to_client&        buffer          = client.get_buffers().to_client;
    // int                 remain_data_len = buffer.size() -
    // buffer.get_offset(); const unsigned int& clientfd        =
    // client.get_socket();

    // ssize_t send_data_len =
    //     send(clientfd, buffer.data() + buffer.get_offset(),
    //          event.data < remain_data_len ? event.data : remain_data_len, 0);

    // if (send_data_len >= 0)
    //{
    //     Logger().info() << "IRC send to " << client.get_names().nick;
    //     buffer.set_offset(buffer.get_offset() + send_data_len);
    //     Logger().trace() << "Send " << send_data_len << " bytes from ["
    //                      << clientfd << "] client";
    //     if (buffer.size() <= buffer.get_offset())
    //     {
    //         if (buffer.size())
    //             buffer.clear();
    //         Logger().trace() << "Empty buffer from [" << clientfd << "]
    //         client"; Event::toggle(client, EVFILT_WRITE);
    //     }
    // }
}

void
    FT_IRCD::m_handler(Client::t_request& request)
{
    if (request.command.size() && (request.command[0] == ':'))
    {
        request.command.erase(0, request.command.find_first_of(' '));
        request.command.erase(0, request.command.find_first_not_of(' '));
    }
    if (request.command.size())
    {
        int         offset;
        int         fixed;
        std::string buffer;

        for (offset = 0; (request.command[offset] != ' '
                          && request.command[offset] != '\0');
             ++offset)
            if ((unsigned)request.command[offset] - 'a' < 26)
                request.command[offset] ^= 0b100000;
        request.type = get_type(request.command.substr(0, offset));
        if (request.command[offset] != '\0')
            buffer = request.command.substr(offset);
        request.command.erase(offset);

        offset = 0;
        for (int index = 0;
             buffer.size() && offset != std::string::npos
             && (fixed = buffer.find_first_not_of(' ')) != std::string::npos;
             ++index)
        {
            if (buffer[fixed] != ':')
            {
                offset = buffer.find_first_of(' ', fixed);
                if (offset != std::string::npos)
                    request.parameter.push_back(buffer.substr(fixed, offset));
                else
                {
                    request.parameter.push_back(buffer.substr(fixed));
                    break;
                }
                buffer.erase(0, offset);
            }
            else
            {
                request.parameter.push_back(buffer.substr(fixed + 1));
                break;
            }
            ++index;
        }
    }
}

void
    FT_IRCD::m_handler()
{
    IRC::_requests  = &_client->get_buffers().requests;
    IRC::_to_client = &_client->get_buffers().to_client;
    while (_requests->queue.size())
    {
        IRC::_request = &_requests->queue.front();
        m_handler(*IRC::_request);

        if (IRC::_request->type != EMPTY && IRC::_request->type != UNKNOWN)
        {
            if ((((unsigned)IRC::_request->type) - 1) < CONNECTION)
            {
                (this->*IRCD::_commands[IRC::_request->type])();
                if (_client->is_registered()
                    && !(_map.client.count(_client->get_names().nick)))
                    IRCD::registration();
            }
            else
            {
                if (IRC::_client->is_registered())
                    (this->*IRCD::_commands[IRC::_request->type])();
                else
                    (this->*IRCD::_commands[UNREGISTERED])();
            }
        }
        else
            (this->*IRCD::_commands[IRC::_request->type])();
        IRC::_requests->queue.pop();
    }
    IRC::_requests  = nullptr;
    IRC::_to_client = nullptr;
}

void
    FT_IRCD::m_receive(struct kevent& event)
{
    IRC::_client   = (Client*)event.udata;
    ssize_t length = recv(event.ident, _buffer, event.data, 0);

    if (length > 0)
    {
        Client::t_buffers& buffers = _client->get_buffers();
        buffers.buffer.append(_buffer, length);
        while ((buffers.offset = buffers.buffer.find_first_of("\r\n", 0))
               != (int)std::string::npos)
        {
            buffers.requests.queue.push(Client::s_request(
                buffers.buffer.substr(0, buffers.offset), UNKNOWN));
            buffers.buffer.erase(0, buffers.offset + 2);
        }
        if (buffers.requests.queue.size())
            m_handler();
        if (buffers.to_client.queue.size())
            Event::toggle(*_client, EVFILT_READ);
    }
    else if (length == 0)
        m_disconnect(*_client, "connection closed");
    IRC::_client = nullptr;
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

    Logger().info() << "[FT_IRCD is running]";
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

FT_IRCD::FT_IRCD(int port, char* password)
{
    Socket::m_initialize(port);
    Event::m_create_kqueue(_socket.fd);
    IRC::_password = password;
    IRC::_ft_ircd  = this;
}

int
    main(int argc, char** argv)
{
    int port;

    if (argc != 3)
    {
        Logger().error() << "Usage :" << argv[0] << " <port> <password>";
        return FAILURE;
    }
    if (PORT_MAX < (unsigned)(port = atoi(argv[1])))
    {
        Logger().error() << port << "is out of Port range (0 ~ 65535)";
        return FAILURE;
    }

    FT_IRCD(port, argv[2]).run();
}

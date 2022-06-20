#include "../includes/ft_ircd.hpp"

void
    FT_IRCD::m_disconnect(std::string reason)
{
    const int& fd = _client->get_socket();

    log::print() << "Client disconnect [address :" << _client->get_IP() << ':'
                 << _client->get_addr().sin_port << " FD :" << fd << ']'
                 << log::endl;

    Event::set(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    Event::set(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    close(fd);

    IRCD::m_to_channels(cmd_quit_reply(reason));

    Client::CITER iter = _client->get_channels().begin();
    Client::CITER end  = _client->get_channels().end();

    for (_channel = *iter; iter != end; _channel = *(++iter))
    {
        _channel->part(*_client);
        if (_channel->is_empty())
        {
            _map.channel.erase(_channel->get_name());
            delete _channel;
        }
    }

    if (_client->is_registered())
        _map.client.erase(_client->get_names().nick);
    delete _client;
    _client = nullptr;
}

void
    FT_IRCD::m_send(struct kevent& event)
{
    Client::t_to_client& buffer = _client->get_buffers().to_client;

    if (buffer.queue.empty())
        return;

    int remain_data_len = buffer.queue.front().size() - buffer.offset;

    ssize_t send_data_len
        = send(event.ident, buffer.queue.front().data() + buffer.offset,
               event.data < remain_data_len ? event.data : remain_data_len, 0);

    if (send_data_len >= 0)
    {
        log::print() << "IRC send to " << _client->get_names().nick
                     << log::endl;

        buffer.offset += send_data_len;

        log::print() << "Send " << send_data_len << " bytes from ["
                     << event.ident << "] client" << log::endl;

        if (buffer.queue.front().size() <= buffer.offset)
        {
            buffer.queue.pop();
            buffer.offset = 0;

            log::print() << "Empty buffer from [" << event.ident << "] client"
                         << log::endl;

            if (buffer.queue.empty())
                Event::toggle(*_client, EVFILT_WRITE);
        }
    }
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
                    request.parameter.push_back(
                        buffer.substr(fixed, offset - fixed));
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
        }
    }
    request.type = get_type(request.command);
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
                if (_client && _client->is_registered()
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
        if (_client)
            IRC::_requests->queue.pop();
    }
    IRC::_requests  = nullptr;
    IRC::_to_client = nullptr;
}

void
    FT_IRCD::m_receive(struct kevent& event)
{
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
        m_disconnect("connection closed");
}

void
    FT_IRCD::m_accept()
{
    sockaddr_in addr;
    int         fd = accept(_socket.fd, (sockaddr*)(&addr), &_socket.len);

    if (fd == -1)
    {
        log::print() << "Failed to accept client errno: " << log::endl;
        return;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);
    Client* client = new Client(addr, fd);

    Event::set(fd, EVFILT_READ, EV_ADD, 0, 0, client);
    Event::set(fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, client);

    log::print() << "Accept client [address:" << inet_ntoa(addr.sin_addr) << ":"
                 << addr.sin_port << "fd:" << fd << ']' << log::endl;
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

    log::print() << "[FT_IRCD is running]" << log::endl;
    while (true)
    {
        count = kevent(Event::_kqueue, NULL, 0, _events, EVENTS_MAX, NULL);
        log::print() << count << " new kevent" << log::endl;
        for (index = 0; index < count; ++index)
        {
            IRC::_client = (Client*)_events[index].udata;
            if (_events[index].ident == (unsigned)_socket.fd)
                FT_IRCD::m_accept();
            else if (_events[index].filter == EVFILT_READ)
                FT_IRCD::m_receive(_events[index]);
            else if (_events[index].filter == EVFILT_WRITE)
                FT_IRCD::m_send(_events[index]);
            IRC::_client = nullptr;
        }
    }
}

FT_IRCD::~FT_IRCD()
{
}

FT_IRCD::FT_IRCD(int port, char* password)
{
    Socket::initialize(port);
    Event::initialize(_socket.fd);
    IRC::_password = password;
    IRC::_ft_ircd  = this;
}

int
    main(int argc, char** argv)
{
    int port;

    if (argc != 3)
    {
        log::print() << "Usage :" << argv[0] << " <port> <password>"
                     << log::endl;
        return FAILURE;
    }
    if (PORT_MAX < (unsigned)(port = atoi(argv[1])))
    {
        log::print() << port << "is out of Port range (0 ~ 65535)" << log::endl;
        return FAILURE;
    }

    FT_IRCD(port, argv[2]).run();
}

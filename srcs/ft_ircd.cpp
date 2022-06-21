#include "../includes/ft_ircd.hpp"

void
    FT_IRCD::m_disconnected(std::string reason)
{
    log::print() << "Client disconnect [address :" << _client->get_IP() << ':'
                 << _client->get_addr().sin_port << " FD :" << _fd << ']'
                 << log::endl;
    Event::remove(_fd);

    IRCD::m_to_channels(cmd_quit_reply(reason));
    std::set<Channel*> copy = _client->get_channels();

    Client::t_citer iter = copy.begin();
    Client::t_citer end  = copy.end();

    for (_channel = *iter; iter != end; _channel = *(++iter))
    {
        _channel->part(*_client);
        if (_channel->is_empty())
        {
            _map.channel.erase(_channel->get_name());
            delete _channel;
        }
        _channel = nullptr;
    }

    if (_client->is_registered())
        _map.client.erase(_client->get_names().nick);
    delete _client;
    _client = nullptr;
}

void
    FT_IRCD::m_disconnect(std::string reason)
{
    Socket::disconnect(_client->get_fd());
    m_disconnected(reason);
}

void
    FT_IRCD::m_send()
{
    IRC::_to_client = &_client->get_buffers().to_client;
    if (IRC::_to_client->queue.empty())
        return;
    if (0 <= Socket::send(_events[_index]))
    {
        log::print() << "IRC send to " << _client->get_names().nick
                     << log::endl;
        IRC::_to_client->offset += Socket::_result;
        log::print() << "Send " << Socket::_result << " bytes from ["
                     << _events[_index].ident << "] client" << log::endl;
        if (IRC::_to_client->queue.front().size()
            <= (unsigned)IRC::_to_client->offset)
        {
            IRC::_to_client->queue.pop();
            IRC::_to_client->offset = 0;
            log::print() << "Empty buffer from [" << _events[_index].ident
                         << "] client" << log::endl;
            if (IRC::_to_client->queue.empty())
                Event::toggle(EVFILT_WRITE);
        }
    }
}

void
    FT_IRCD::m_requests_handler()
{
    IRC::_requests  = &_client->get_buffers().requests;
    IRC::_to_client = &_client->get_buffers().to_client;
    while (_requests->queue.size())
    {
        IRCD::parse_request(_requests->queue.front());
        IRCD::_request->type = get_type(_request->command);
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
    FT_IRCD::m_receive()
{
    if (0 < Socket::receive(_events[_index]))
    {
        Client::t_buffers& buffers = _client->get_buffers();
        buffers.buffer.append(Socket::_buffer, Socket::_result);
        while ((buffers.offset = buffers.buffer.find_first_of("\r\n", 0))
               != (int)std::string::npos)
        {
            buffers.requests.queue.push(Client::s_request(
                buffers.buffer.substr(0, buffers.offset), UNKNOWN));
            buffers.buffer.erase(0, buffers.offset + 2);
        }
        if (buffers.requests.queue.size())
            m_requests_handler();
        if (buffers.to_client.queue.size())
            Event::toggle(EVFILT_READ);
    }
    else if (Socket::_result == 0)
        m_disconnected("connection closed");
}

void
    FT_IRCD::m_accept()
{
    if (Socket::accept() == -1)
        return;
    Event::add(new Client(_addr, _fd));
    log::print() << "Accept client [address:" << inet_ntoa(_addr.sin_addr)
                 << ":" << _addr.sin_port << "fd:" << _fd << ']' << log::endl;
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

    log::print() << "[FT_IRCD is running]" << log::endl;
    while (true)
    {
        count = Event::kevent();
        log::print() << count << " new kevent" << log::endl;
        for (Event::_index = 0; Event::_index < count; ++Event::_index)
        {
            IRC::_client = (Client*)_events[_index].udata;
            if (_events[_index].ident == (unsigned)_socket.fd)
                FT_IRCD::m_accept();
            else if (_events[_index].filter == EVFILT_READ)
                FT_IRCD::m_receive();
            else if (_events[_index].filter == EVFILT_WRITE)
                FT_IRCD::m_send();
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

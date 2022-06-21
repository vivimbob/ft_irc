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
    Client::t_to_client& buffer = _client->get_buffers().to_client;

    if (buffer.queue.empty())
        return;
    int     remain_data_len = buffer.queue.front().size() - buffer.offset;
    ssize_t send_data_len   = send(
          _events[_index].ident, buffer.queue.front().data() + buffer.offset,
        _events[_index].data < remain_data_len ? _events[_index].data
                                                 : remain_data_len,
          0);
    if (send_data_len >= 0)
    {
        log::print() << "IRC send to " << _client->get_names().nick
                     << log::endl;
        buffer.offset += send_data_len;
        log::print() << "Send " << send_data_len << " bytes from ["
                     << _events[_index].ident << "] client" << log::endl;
        if (buffer.queue.front().size() <= (unsigned)buffer.offset)
        {
            buffer.queue.pop();
            buffer.offset = 0;
            log::print() << "Empty buffer from [" << _events[_index].ident
                         << "] client" << log::endl;
            if (buffer.queue.empty())
                Event::toggle(EVFILT_WRITE);
        }
    }
}

void
    FT_IRCD::m_request_handler()
{
    int         offset;
    int         fixed;
    std::string buffer;

    if (request.command.size() && (request.command.front() == ':'))

    {
        _request->command.erase(0, _request->command.find_first_of(' '));
        _request->command.erase(0, _request->command.find_first_not_of(' '));
    }
    if (_request->command.size())
    {
        for (offset = 0; (request.command[offset] != ' '
                          && request.command[offset] != '\0');
             ++offset)
            if ((unsigned)request.command[offset] - 'a' < 26)
                request.command[offset] ^= 0b100000;
        buffer = request.command.substr(offset);
        request.command.erase(offset);
    }
    for (offset = 0;
         (fixed = buffer.find_first_not_of(' ')) != (int)std::string::npos;)
    {
        offset = buffer.find_first_of(' ', fixed);
        if ((offset != (int)std::string::npos) && buffer[fixed] != ':')
            request.parameter.push_back(buffer.substr(fixed, offset - fixed));
        else
        {
            if (buffer[fixed] == ':')
                ++fixed;
            request.parameter.push_back(buffer.substr(fixed));
            break;
        }
        buffer.erase(0, offset);
    }
    _request->type = get_type(_request->command);
}

void
    FT_IRCD::m_requests_handler()
{
    IRC::_requests  = &_client->get_buffers().requests;
    IRC::_to_client = &_client->get_buffers().to_client;
    while (_requests->queue.size())
    {
        IRC::_request = &_requests->queue.front();
        m_request_handler();
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
        buffers.buffer.append(Socket::_buffer, Socket::_received);
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
    else if (Socket::_received == 0)
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

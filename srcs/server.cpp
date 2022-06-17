#include "../includes/server.hpp"

/* utility start */

void
    Server::m_prepare_to_send(Client& client, const std::string& str_msg)
{
    client.push_message(str_msg);
    Event::toggle(client, EVFILT_READ);
}

void
    Server::m_send_to_channel(Channel*           channel,
                              const std::string& msg,
                              Client*            exclusion)
{
    const Channel::MemberMap&          user_list = channel->get_user_list();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :" << channel->get_name();
    for (; user != user_list.end(); ++user)
        if (user->first != exclusion)
            m_prepare_to_send(*user->first, msg);
}

void
    Server::m_send_to_channel(Client&            client,
                              const std::string& msg,
                              Client*            exclusion)
{
    std::set<Channel*>::iterator it = client.get_joined_list().begin();
    for (; it != client.get_joined_list().end(); ++it)
        m_send_to_channel(*it, msg, exclusion);
}

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    if (error_check)
        utils::push_message(client, message);
    return error_check;
}

/* utility start */

/* process command start */

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

    temp_map.insert(std::make_pair("JOIN", &Server::m_process_join_command));
    temp_map.insert(std::make_pair("MODE", &Server::m_process_mode_command));
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
    Server::m_process_pass_command(Client& client, Message& msg)
{
    if ((check_error(msg.get_params().empty(), client,
                     msg.err_need_more_params())) ||
        (check_error(client.is_registered(), client,
                     msg.err_already_registred())) ||
        (check_error(msg.get_params()[0] != _password, client,
                     msg.err_passwd_mismatch())))
        return;
    client.set_password_flag();
    if (client.is_registered() && !m_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Server::m_process_nick_command(Client& client, Message& msg)
{

    if (check_error(msg.get_params().empty(), client,
                    msg.err_no_nickname_given()))
        return;

    const std::string& nickname = msg.get_params()[0];

    if (check_error(!utils::is_nickname_valid(nickname), client,
                    msg.err_erroneus_nickname(nickname)))
        return;

    if (m_client_map.count(nickname))
    {
        if (nickname != client.get_names().nick)
            utils::push_message(client, msg.err_nickname_in_use(nickname));
        return;
    }

    if (client.is_registered())
    {
        m_send_to_channel(client, msg.build_nick_reply(nickname), &client);
        utils::push_message(client, msg.build_nick_reply(nickname));
        if (m_client_map.count(client.get_names().nick))
        {
            m_client_map.erase(client.get_names().nick);
            m_client_map[nickname] = &client;
        }
    }

    Logger().debug() << client.get_IP() << " change nick to " << nickname;
    client.set_nickname(nickname);

    if (client.is_registered() && !m_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Server::m_process_user_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().size() < 4, client,
                    msg.err_need_more_params()))
        return;

    if (check_error(client.is_registered(), client,
                    msg.err_already_registred()))
        return;

    client.set_username(msg.get_params()[0]);
    client.set_realname(msg.get_params()[3]);
    if (client.is_registered() && !m_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Server::m_process_quit_command(Client& client, Message& msg)
{
    std::string message = "Quit";
    if (msg.get_params().size())
        message += " :" + msg.get_params()[0];
    m_disconnect_client(client, message);
}

void
    Server::m_process_join_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    ConstStringVector channel_list;

    utils::split_by_comma(channel_list, msg.get_params()[0]);
    for (ConstStringVector::iterator channel_it = channel_list.begin();
         channel_it != channel_list.end(); ++channel_it)
    {
        const std::string& channel_name = *channel_it;

        if (check_error((!utils::is_channel_prefix(channel_name) ||
                         !utils::is_channel_name_valid(channel_name)),
                        client, msg.err_no_such_channel(channel_name)))
            continue;
        if (check_error(!client.is_join_available(), client,
                        msg.err_too_many_channels(channel_name)))
            continue;
        if (!m_channel_map.count(channel_name))
            m_channel_map.insert(
                std::make_pair(channel_name, new Channel(channel_name)));
        Channel* channel = m_channel_map[channel_name];
        if (client.is_already_joined(channel))
            continue;
        else if (check_error(channel->is_full(), client,
                             msg.err_channel_is_full(channel_name)))
            continue;
        channel->add_user(client);
        client.insert_channel(channel);
        if (channel->get_user_list().size() == 1)
            channel->set_operator_flag(true, &client);
        Logger().info() << "Create new channel :" << channel_name << " : @"
                        << client.get_names().nick;
        m_send_to_channel(channel, msg.build_join_reply(channel_name));
        utils::send_topic_reply(channel, client, msg);
        utils::send_name_reply(channel, client, msg);
    }
}

void
    Server::m_process_channel_mode_command(Client&            client,
                                           Message&           msg,
                                           const std::string& channel_name)
{
    if (check_error(!m_channel_map.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = m_channel_map.at(channel_name);
    if (check_error(msg.get_params().size() == 1, client,
                    msg.rpl_channel_mode_is(channel_name)))
        return;
    if (check_error(!channel->is_operator(client), client,
                    msg.err_chanoprivs_needed(channel_name)))
        return;

    utils::push_message(client, msg.err_unknown_mode(msg.get_params()[1]));
}

void
    Server::m_process_user_mode_command(Client&            client,
                                        Message&           msg,
                                        const std::string& nickname)
{
    if (check_error(!m_client_map.count(nickname), client,
                    msg.err_no_such_nick(nickname)))
        return;

    if (check_error(nickname != client.get_names().nick, client,
                    msg.err_users_dont_match(
                        msg.get_params().size() == 1 ? "view" : "change")))
        return;

    if (check_error(msg.get_params().size() == 1, client,
                    msg.rpl_user_mode_is()))
        return;

    utils::push_message(client, msg.err_u_mode_unknown_flag());
}

void
    Server::m_process_mode_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;

    const std::string& target = msg.get_params()[0];

    if (utils::is_channel_prefix(target))
        m_process_channel_mode_command(client, msg, target);
    else
        m_process_user_mode_command(client, msg, target);
}

void
    Server::m_process_invite_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().size() < 2, client,
                    msg.err_need_more_params()))
        return;

    const std::string& nickname     = msg.get_params()[0];
    const std::string& channel_name = msg.get_params()[1];

    if (check_error(!m_client_map.count(nickname), client,
                    msg.err_no_such_nick(nickname)))
        return;
    Client* target_client = m_client_map[nickname];
    if (check_error(!m_channel_map.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = m_channel_map[channel_name];
    if (check_error(!client.is_already_joined(channel), client,
                    msg.err_not_on_channel(channel_name)))
        return;
    if (check_error(target_client->is_already_joined(channel), client,
                    msg.err_user_on_channel(nickname, channel_name)))
        return;
    utils::push_message(client, msg.rpl_inviting(nickname, channel_name));
    m_prepare_to_send(*target_client,
                      msg.build_invite_reply(nickname, channel_name));
}

void
    Server::m_process_kick_command(Client& client, Message& msg)
{
    const std::vector<std::string>& parameter = msg.get_params();
    if (check_error(parameter.size() < 2, client, msg.err_need_more_params()))
        return;

    ConstStringVector channel_list;
    ConstStringVector nick_list;

    utils::split_by_comma(channel_list, parameter[0]);
    utils::split_by_comma(nick_list, parameter[1]);

    if (check_error((!(channel_list.size() == 1 || nick_list.size() == 1) &&
                     channel_list.size() != nick_list.size()),
                    client, msg.err_need_more_params()))
        return;

    ConstStringVector::iterator channel_name = channel_list.begin();
    ConstStringVector::iterator nick_name    = nick_list.begin();

    Channel* channel;
    Client*  target_client;

    for (int i = 0, max_size = std::max(channel_list.size(), nick_list.size());
         i < max_size; ++i)
    {
        if (check_error((!utils::is_channel_prefix(*channel_name) ||
                         !m_channel_map.count(*channel_name)),
                        client, msg.err_no_such_channel(*channel_name)))
            goto next;
        channel = m_channel_map[*channel_name];
        if (check_error(!channel->is_operator(client), client,
                        msg.err_chanoprivs_needed(*channel_name)))
            goto next;
        if (check_error(!m_client_map.count(*nick_name), client,
                        msg.err_no_such_nick(*nick_name)))
            goto next;
        target_client = m_client_map[*nick_name];
        if (check_error(!channel->is_user_on_channel(target_client), client,
                        msg.err_user_not_in_channel(*nick_name, *channel_name)))
            goto next;
        m_send_to_channel(channel,
                          msg.build_kick_reply(*channel_name, *nick_name,
                                               client.get_names().nick));
        channel->delete_user(*target_client);
        target_client->erase_channel(channel);
        if (channel->is_empty())
        {
            m_channel_map.erase(*channel_name);
            delete channel;
        }
    next:
        if (channel_list.size() != 1)
            ++channel_name;
        if (nick_list.size() != 1)
            ++nick_name;
    }
}

void
    Server::m_process_names_command(Client& client, Message& msg)
{
    std::queue<const std::string> nick_queue;
    if (msg.get_params().empty())
    {
        ChannelMap::const_iterator channel_it = m_channel_map.begin();
        for (; channel_it != m_channel_map.end(); ++channel_it)
            utils::send_name_reply(channel_it->second, client, msg);
        ClientMap::const_iterator client_it = m_client_map.begin();
        for (; client_it != m_client_map.end(); ++client_it)
            if (client_it->second->get_joined_list().empty())
                nick_queue.push(client_it->first);
        if (nick_queue.size())
            utils::push_message(client, msg.rpl_namreply("*", nick_queue));
        utils::push_message(client, msg.rpl_endofnames("*"));
        return;
    }
    else
    {
        ConstStringVector channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);
        for (int i = 0, size = channel_list.size(); i < size; ++i)
        {
            if (check_error(!m_channel_map.count(channel_list[i]), client,
                            msg.rpl_endofnames(channel_list[i])))
                continue;
            utils::send_name_reply(m_channel_map[channel_list[i]], client, msg);
        }
    }
}

static void
    send_list_to_client(Channel* channel, Client& client, Message& msg)
{
    utils::push_message(
        client, msg.rpl_list(channel->get_name(),
                             std::to_string(channel->get_user_list().size()),
                             channel->get_topic()));
}

void
    Server::m_process_list_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        ChannelMap::const_iterator channel_it = m_channel_map.begin();
        for (; channel_it != m_channel_map.end(); ++channel_it)
            send_list_to_client(channel_it->second, client, msg);
    }

    else if (msg.get_params().size() == 1)
    {
        ConstStringVector channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        ConstStringVector::iterator channel_it = channel_list.begin();
        for (; channel_it != channel_list.end(); ++channel_it)
            if (m_channel_map.count(*channel_it))
                send_list_to_client(m_channel_map[*channel_it], client, msg);
            else
                utils::push_message(client,
                                    msg.err_no_such_channel(*channel_it));
    }
    utils::push_message(client, msg.rpl_listend());
}

void
    Server::m_process_part_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    ConstStringVector channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);
    ConstStringVector::iterator channel_it = channel_list.begin();
    for (; channel_it != channel_list.end(); ++channel_it)
    {
        if (check_error(!m_channel_map.count(*channel_it), client,
                        msg.err_no_such_channel(*channel_it)))
            continue;
        Channel* channel = m_channel_map[*channel_it];
        if (check_error(!channel->is_user_on_channel(&client), client,
                        msg.err_not_on_channel(*channel_it)))
            continue;
        m_send_to_channel(channel, msg.build_part_reply(*channel_it));
        channel->delete_user(client);
        client.erase_channel(channel);
        if (channel->is_empty())
        {
            m_channel_map.erase(channel->get_name());
            delete channel;
        }
        Logger().debug() << "Remove [" << client.get_names().nick
                         << "] client from [" << channel->get_name()
                         << "] channel";
    }
}

void
    Server::m_process_topic_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    const std::string& channel_name = msg.get_params()[0];
    if (check_error(!m_channel_map.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = m_channel_map[channel_name];
    if (check_error(!channel->is_user_on_channel(&client), client,
                    msg.err_not_on_channel(channel_name)))
        return;
    if (msg.get_params().size() == 1)
    {
        utils::send_topic_reply(channel, client, msg);
        return;
    }
    if (check_error(!channel->is_operator(client), client,
                    msg.err_chanoprivs_needed(channel_name)))
        return;
    channel->set_topic(msg.get_params()[1]);

    Logger().trace() << channel_name << " channel topic change to "
                     << channel->get_topic();
    m_send_to_channel(channel, msg.build_topic_reply());
}

void
    Server::m_process_privmsg_command(Client& client, Message& msg)
{
    const std::vector<std::string>& parameter = msg.get_params();

    if (check_error(parameter.empty(), client, msg.err_no_recipient()))
        return;
    if (check_error(parameter.size() == 1, client, msg.err_no_text_to_send()))
        return;

    ConstStringVector target_list;
    utils::split_by_comma(target_list, parameter[0]);

    ConstStringVector::iterator target_it  = target_list.begin();
    ConstStringVector::iterator target_ite = target_list.end();
    for (; target_it != target_ite; ++target_it)
    {
        if (utils::is_channel_prefix(*target_it))
        {
            if (check_error(!m_channel_map.count(*target_it), client,
                            msg.err_no_such_channel(*target_it)))
                continue;
            m_send_to_channel(m_channel_map[*target_it],
                              msg.build_message_reply(*target_it), &client);
        }
        else if (m_client_map.count(*target_it))
            m_prepare_to_send(*m_client_map[*target_it],
                              msg.build_message_reply(*target_it));
        else if (msg.get_command() != "NOTICE")
            utils::push_message(client, msg.err_no_such_nick(*target_it));
    }
}

void
    Server::m_process_notice_command(Client& client, Message& msg)
{
    m_process_privmsg_command(client, msg);
}

/* process command end */

/* server run start */

void
    Server::m_requests_handler(Client&                  client,
                               std::queue<std::string>& requests)
{
    while (requests.size())
    {
        Message message(requests.front());
        requests.pop();
        message.parse_message();
        const std::string& command = message.get_command();

        if (m_register_command_map.count(command))
            (this->*m_register_command_map[command])(client, message);
        else if (m_channel_command_map.count(command))
        {
            if (client.is_registered())
                (this->*m_channel_command_map[message.get_command()])(client,
                                                                      message);
            else
                utils::push_message(client, message.err_not_registered());
        }
        else if (!command.empty())
            utils::push_message(client, message.err_unknown_command());
    }
}

void
    Server::m_disconnect_client(Client& client, std::string reason)
{
    const unsigned int& clientfd = client.get_socket();

    Logger().info() << "Client disconnect [address :" << client.get_IP() << ':'
                    << client.get_addr().sin_port << " FD :" << clientfd << ']';

    Event::set(clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    Event::set(clientfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

    std::set<Client*>            client_check_list;
    std::set<Channel*>::iterator it = client.get_joined_list().begin();
    for (; it != client.get_joined_list().end(); ++it)
    {
        const Channel::MemberMap&          user_list = (*it)->get_user_list();
        Channel::MemberMap::const_iterator user      = user_list.begin();

        for (; user != user_list.end(); ++user)
            if (!client_check_list.count(user->first) && user->first != &client)
            {
                client_check_list.insert(user->first);
                m_prepare_to_send(*user->first,
                                  Message("QUIT").build_quit_reply(reason));
            }
    }
    std::set<Channel*>::iterator channel_it  = client.get_joined_list().begin();
    std::set<Channel*>::iterator channel_ite = client.get_joined_list().end();

    for (; channel_it != channel_ite; ++channel_it)
    {
        (*channel_it)->delete_user(client);
        if ((*channel_it)->is_empty())
        {
            m_channel_map.erase((*channel_it)->get_name());
            delete (*channel_it);
        }
    }
    m_client_map.erase(client.get_names().nick);
    delete &client;
    close(clientfd);
}

void
    Server::m_register_client(Client& client, Message& msg)
{
    m_client_map[client.get_names().nick] = &client;
    utils::push_message(client, msg.rpl_welcome());
    Logger().info() << client.get_names().nick << " is registered to server";
}

void
    Server::m_send(struct kevent& event)
{
    Client&             client          = (Client&)event.udata;
    Buffer&             buffer          = client.get_buffers().to_client;
    int                 remain_data_len = buffer.size() - buffer.get_offset();
    const unsigned int& clientfd        = client.get_socket();

    ssize_t send_data_len =
        send(clientfd, buffer.data() + buffer.get_offset(),
             event.data < remain_data_len ? event.data : remain_data_len, 0);

    if (send_data_len >= 0)
    {
        Logger().info() << "Server send to " << client.get_names().nick;
        buffer.set_offset(buffer.get_offset() + send_data_len);
        Logger().trace() << "Send " << send_data_len << " bytes from ["
                         << clientfd << "] client";
        if (buffer.size() <= buffer.get_offset())
        {
            if (buffer.size())
                buffer.clear();
            Logger().trace() << "Empty buffer from [" << clientfd << "] client";
            Event::set(clientfd, EVFILT_READ, EV_ENABLE, 0, 0, &client);
            Event::set(clientfd, EVFILT_WRITE, EV_DISABLE, 0, 0, &client);
        }
    }
}

void
    Server::m_receive(struct kevent& event)
{
    Client& client = (Client&)event.udata;
    ssize_t length = recv(event.ident, _buffer, event.data, 0);

    if (length > 0)
    {
        int                     offset;
        std::queue<std::string> requests;

        Client::t_buffers& buffers = client.get_buffers();
        buffers.request.append(_buffer, length);
        while ((offset = buffers.request.find_first_of("\r\n", 0)) !=
               (int)std::string::npos)
        {
            requests.push(buffers.request.substr(0, offset));
            buffers.request.erase(0, offset + 2);
        }
        if (requests.size())
            m_requests_handler(client, requests);
        if (buffers.to_client.size())
            Event::toggle(client, EVFILT_READ);
    }
    else if (length == 0)
        m_disconnect_client(client, "connection closed");
}

void
    Server::m_accept()
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

Server::~Server()
{
}

void
    Server::run()
{
    register int count;
    register int index;

    Logger().info() << "[Server is running]";
    while (true)
    {
        count = kevent(Event::_kqueue, NULL, 0, _events, EVENTS_MAX, NULL);
        Logger().trace() << count << " new kevent";
        for (index = 0; index < count; ++index)
        {
            if (_events[index].ident == (unsigned)_socket.fd)
                Server::m_accept();
            else if (_events[index].filter == EVFILT_READ)
                Server::m_receive(_events[index]);
            else if (_events[index].filter == EVFILT_WRITE)
                Server::m_send(_events[index]);
        }
    }
}

Server::Server(int port, char* password) : _password(password)
{
    Socket::m_initialize(port);
    Event::m_create_kqueue(_socket.fd);
}

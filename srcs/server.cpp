#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include "../lib/logger.hpp"
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

/* utility start */

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
    Server::m_prepare_to_send(Client& client, const std::string& str_msg)
{
    utils::push_message(client, str_msg);
    m_update_event(client.get_socket(), EVFILT_READ, EV_DISABLE, 0, 0, &client);
    m_update_event(client.get_socket(), EVFILT_WRITE, EV_ENABLE, 0, 0, &client);
    Logger().trace() << client.get_nickname() << " disable read event";
    Logger().trace() << client.get_nickname() << " enable write event";
}

void
    Server::m_send_to_channel(Channel*           channel,
                              const std::string& msg,
                              Client*            exclusion)
{
    const Channel::MemberMap&          user_list = channel->get_user_list();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :"
                     << channel->get_channel_name();
    for (; user != user_list.end(); ++user)
        if (user->first != exclusion)
            m_prepare_to_send(*user->first, msg);
}

void
    Server::m_send_to_channel(Client&            client,
                              const std::string& msg,
                              Client*            exclusion)
{
    std::set<Channel*>::iterator it = client.get_channel_list().begin();
    for (; it != client.get_channel_list().end(); ++it)
        m_send_to_channel(*it, msg, exclusion);
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

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    if (error_check)
        utils::push_message(client, message);
    return error_check;
}

void
    Server::m_process_pass_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    if (check_error(client.is_registered(), client,
                    msg.err_already_registred()))
        return;
    if (check_error(msg.get_params()[0] != m_password, client,
                    msg.err_passwd_mismatch()))
        return;
    client.set_password_flag();
    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
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
        if (nickname != client.get_nickname())
            utils::push_message(client, msg.err_nickname_in_use(nickname));
        return;
    }

    if (client.is_registered())
    {
        m_send_to_channel(client, msg.build_nick_reply(nickname), &client);
        utils::push_message(client, msg.build_nick_reply(nickname));
        if (m_client_map.count(client.get_nickname()))
        {
            m_client_map.erase(client.get_nickname());
            m_client_map[nickname] = &client;
        }
    }

    Logger().debug() << client.get_client_IP() << " change nick to "
                     << nickname;
    client.set_nickname(nickname);

    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
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
    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
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

    std::vector<const std::string> channel_list;

    utils::split_by_comma(channel_list, msg.get_params()[0]);

    for (std::vector<const std::string>::iterator channel_it =
             channel_list.begin();
         channel_it != channel_list.end(); ++channel_it)
    {
        Channel*           channel      = NULL;
        const std::string& channel_name = *channel_it;

        if (m_channel_map.count(channel_name))
            channel = m_channel_map[channel_name];

        if (check_error((!utils::is_channel_prefix(channel_name) ||
                         !utils::is_channel_name_valid(channel_name)),
                        client, msg.err_no_such_channel(channel_name)))
            continue;

        else if (!channel)
        {
            if (check_error(!client.is_join_available(), client,
                            msg.err_too_many_channels(channel_name)))
                continue;
            channel = new Channel(channel_name);
            m_channel_map.insert(std::make_pair(channel_name, channel));
            channel->add_user(client);
            channel->set_operator_flag(true, &client);
            client.insert_channel(channel);
        }
        else // join할 채널이 존재하는 경우
        {
            if (client.is_already_joined(channel)) // 이미 join된 경우
                continue;
            if (check_error(channel->is_full(), client,
                            msg.err_channel_is_full(channel_name)))
                continue;
            channel->add_user(client);
            client.insert_channel(channel);
        }

        Logger().info() << "Create new channel :" << channel_name << " : @"
                        << client.get_nickname();
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

    if (check_error(nickname != client.get_nickname(), client,
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
    if (parameter.size() < 2)
    {
        utils::push_message(client, msg.err_need_more_params());
        return;
    }

    std::vector<const std::string> channel_list;
    std::vector<const std::string> nick_list;

    utils::split_by_comma(channel_list, parameter[0]);
    utils::split_by_comma(nick_list, parameter[1]);

    if (check_error((!(channel_list.size() == 1 || nick_list.size() == 1) &&
                     channel_list.size() != nick_list.size()),
                    client, msg.err_need_more_params()))
        return;

    Channel* channel;

    if (channel_list.size() == 1)
    {
        const std::string& channel_name = channel_list[0];

        if (check_error((!utils::is_channel_prefix(channel_name) ||
                         !m_channel_map.count(channel_name)),
                        client, msg.err_no_such_channel(channel_name)))
            return;

        channel = m_channel_map[channel_name];

        if (check_error(!channel->is_operator(client), client,
                        msg.err_chanoprivs_needed(channel_name)))
            return;

        std::vector<const std::string>::iterator nick_it  = nick_list.begin();
        std::vector<const std::string>::iterator nick_ite = nick_list.end();

        for (; nick_it != nick_ite; ++nick_it)
        {
            const std::string& nick = *nick_it;

            if (check_error(!m_client_map.count(nick), client,
                            msg.err_no_such_nick(nick)))
                continue;
            Client* target_client = m_client_map[nick];

            if (check_error(!channel->is_user_on_channel(target_client), client,
                            msg.err_user_not_in_channel(nick, channel_name)))
                continue;

            m_send_to_channel(channel,
                              msg.build_kick_reply(channel_name, nick,
                                                   client.get_nickname()));
            channel->delete_user(*target_client);
            target_client->erase_channel(channel);
        }
    }
    else if (nick_list.size() == 1)
    {
        const std::string& nick = nick_list[0];

        if (check_error(!m_client_map.count(nick), client,
                        msg.err_no_such_nick(nick)))
            return;
        Client* target_client = m_client_map[nick];
        std::vector<const std::string>::iterator channel_it =
            channel_list.begin();
        std::vector<const std::string>::iterator channel_ite =
            channel_list.end();
        for (; channel_it != channel_ite; ++channel_it)
        {
            const std::string& channel_name = *channel_it;

            if (check_error((!utils::is_channel_prefix(channel_name) ||
                             !m_channel_map.count(channel_name)),
                            client, msg.err_no_such_channel(channel_name)))
                continue;

            channel = m_channel_map[channel_name];

            if (check_error(!channel->is_operator(client), client,
                            msg.err_chanoprivs_needed(channel_name)))
                continue;

            if (check_error(!channel->is_user_on_channel(target_client), client,
                            msg.err_user_not_in_channel(nick, channel_name)))
                continue;

            m_send_to_channel(channel,
                              msg.build_kick_reply(channel_name, nick,
                                                   client.get_nickname()));
            channel->delete_user(*target_client);
            target_client->erase_channel(channel);
        }
    }
    else
    {
        std::vector<const std::string>::iterator nick_it  = nick_list.begin();
        std::vector<const std::string>::iterator nick_ite = nick_list.end();
        std::vector<const std::string>::iterator channel_it =
            channel_list.begin();

        for (; nick_it != nick_ite; ++nick_it, ++channel_it)
        {
            const std::string& channel_name = *channel_it;
            const std::string& nick         = *nick_it;

            if (check_error((!utils::is_channel_prefix(channel_name) ||
                             !m_channel_map.count(channel_name)),
                            client, msg.err_no_such_channel(channel_name)))
                continue;

            channel = m_channel_map[channel_name];

            if (check_error(!channel->is_user_on_channel(&client), client,
                            msg.err_not_on_channel(channel_name)))
                continue;

            if (check_error(!channel->is_operator(client), client,
                            msg.err_chanoprivs_needed(channel_name)))
                continue;

            if (check_error(!m_client_map.count(nick), client,
                            msg.err_no_such_nick(nick)))
                continue;

            Client* target_client = m_client_map[nick];

            if (check_error(channel->is_user_on_channel(target_client), client,
                            msg.err_user_not_in_channel(nick, channel_name)))
                continue;
            m_send_to_channel(channel,
                              msg.build_kick_reply(channel_name, nick,
                                                   client.get_nickname()));
            channel->delete_user(*target_client);
            target_client->erase_channel(channel);
        }
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

        // 클라이언트가 어느 채널에도 속하지 않을 때
        ClientMap::const_iterator client_it = m_client_map.begin();
        for (; client_it != m_client_map.end(); ++client_it)
            if (client_it->second->get_channel_list().empty())
                nick_queue.push(client_it->first);

        if (nick_queue.size())
            utils::push_message(client, msg.rpl_namreply("*", nick_queue));
        utils::push_message(client, msg.rpl_endofnames("*"));
        return;
    }
    else
    {
        std::vector<const std::string> channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        for (int i = 0, size = channel_list.size(); i < size; ++i)
        {
            // 잘못된 채널일 때
            if (check_error(!m_channel_map.count(channel_list[i]), client,
                            msg.rpl_endofnames(channel_list[i])))
                continue;

            utils::send_name_reply(m_channel_map[channel_list[i]], client, msg);
        }
    }
}

void
    send_list_to_client(Channel* channel, Client& client, Message& msg)
{
    utils::push_message(
        client, msg.rpl_list(channel->get_channel_name(),
                             std::to_string(channel->get_user_list().size()),
                             channel->get_channel_topic()));
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
        std::vector<const std::string> channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        std::vector<const std::string>::iterator channel_it =
            channel_list.begin();
        for (; channel_it != channel_list.end(); ++channel_it)
        {
            if (m_channel_map.count(*channel_it))
                send_list_to_client(m_channel_map[*channel_it], client, msg);
            else
                utils::push_message(client,
                                    msg.err_no_such_channel(*channel_it));
        }
    }
    utils::push_message(client, msg.rpl_listend());
}

void
    Server::m_process_part_command(Client& client, Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;

    std::vector<const std::string> channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);

    std::vector<const std::string>::iterator channel_it = channel_list.begin();
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
            m_channel_map.erase(channel->get_channel_name());
        delete channel;
        Logger().debug() << "Remove [" << client.get_nickname()
                         << "] client from [" << channel->get_channel_name()
                         << "] channel";
    }
}

void
    Server::m_process_topic_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        utils::push_message(client, msg.err_need_more_params());
        return;
    }

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
    channel->set_channel_topic(msg.get_params()[1]);

    Logger().trace() << channel_name << " channel topic change to "
                     << channel->get_channel_topic();

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

    std::vector<const std::string> target_list;
    utils::split_by_comma(target_list, parameter[0]);

    std::vector<const std::string>::iterator target_it  = target_list.begin();
    std::vector<const std::string>::iterator target_ite = target_list.end();
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
        else
        {
            utils::ClientInfo client_info =
                utils::parse_client_info(*target_it);

            ClientMap::iterator client_it                = m_client_map.begin();
            ClientMap::iterator client_ite               = m_client_map.end();
            size_t              number_of_matched_client = 0;
            Client*             matched_client;
            for (; client_it != client_ite; ++client_it)
            {
                if (client_it->second->is_same_client(client_info))
                {
                    matched_client = client_it->second;
                    ++number_of_matched_client;
                }
            }
            if (msg.get_command() != "NOTICE" && number_of_matched_client == 0)
                utils::push_message(client, msg.err_no_such_nick(*target_it));
            else if (number_of_matched_client == 1)
                m_prepare_to_send(*matched_client,
                                  msg.build_message_reply(*target_it));
            else if (msg.get_command() != "NOTICE")
                utils::push_message(client,
                                    msg.err_too_many_targets(*target_it));
        }
    }
}

void
    Server::m_process_notice_command(Client& client, Message& msg)
{
    m_process_privmsg_command(client, msg);
}

/* process command end */

/* initailize server start */

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
    Server::m_initialize_server()
{
    m_create_socket();
    m_bind_socket();
    m_listen_socket();
    m_create_kqueue();
}

/* initailize server end */

/* server run start */

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
        if (message->get_command().empty())
            continue;
        if (!client.is_registered())
        {
            if (m_register_command_map.count(message->get_command()))
                (this->*m_register_command_map[message->get_command()])(
                    client, *message);
            else
            {
                if (m_channel_command_map.count(message->get_command()))
                    utils::push_message(client, message->err_not_registered());
                else
                    utils::push_message(client, message->err_unknown_command());
            }
        }
        else
        {
            if (m_channel_command_map.count(message->get_command()))
                (this->*m_channel_command_map[message->get_command()])(
                    client, *message);
            else
                utils::push_message(client, message->err_unknown_command());
        }
        delete message;
    }
}

void
    Server::m_disconnect_client(Client& client, std::string reason)
{
    const unsigned int& clientfd = client.get_socket();

    Logger().info() << "Client disconnect [address :" << client.get_client_IP()
                    << ':' << client.get_client_addr().sin_port
                    << " FD :" << clientfd << ']';

    m_update_event(clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    m_update_event(clientfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

    std::set<Client*>            client_check_list;
    std::set<Channel*>::iterator it = client.get_channel_list().begin();
    for (; it != client.get_channel_list().end(); ++it)
    {
        const Channel::MemberMap&          user_list = (*it)->get_user_list();
        Channel::MemberMap::const_iterator user      = user_list.begin();

        for (; user != user_list.end(); ++user)
            if (!client_check_list.count(user->first) && user->first != &client)
            {
                client_check_list.insert(user->first);
                m_prepare_to_send(
                    *user->first,
                    Message(&client, "QUIT").build_quit_reply(reason));
            }
    }
    std::set<Channel*>::iterator channel_it = client.get_channel_list().begin();
    std::set<Channel*>::iterator channel_ite = client.get_channel_list().end();

    for (; channel_it != channel_ite; ++channel_it)
    {
        (*channel_it)->delete_user(client);
        if ((*channel_it)->is_empty())
        {
            m_channel_map.erase((*channel_it)->get_channel_name());
            delete (*channel_it);
        }
    }
    m_client_map.erase(client.get_nickname());
    delete &client;
    close(clientfd);
}

void
    Server::m_register_client(Client& client, Message& msg)
{
    m_client_map[client.get_nickname()] = &client;
    utils::push_message(client, msg.rpl_welcome());
    Logger().info() << client.get_nickname() << " is registered to server";
}

void
    Server::m_receive_client_msg(Client& client, int data_len)
{
    const unsigned int& clientfd = client.get_socket();

    char*   buffer        = m_read_buffer;
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
        }
    }
    else if (recv_data_len == 0)
        m_disconnect_client(client, "connection closed");
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
        }
    }
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
    if (m_port < 0 || m_port > PORT_MAX)
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

/* server run end */

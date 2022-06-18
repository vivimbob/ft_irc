#include "../includes/daemon.hpp"
#include "../includes/ft_ircd.hpp"
#include <cstddef>
#include <string>

TYPE
    Daemon::get_type(std::string command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return UNKNOWN;
}

std::string
    Daemon::get_command(TYPE type)
{
    if (_type_to_command.count(type))
        return _type_to_command[type];
    return "꺼져";
}

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    if (error_check)
        utils::push_message(client, message);
    return error_check;
}

void
    Daemon::m_void(Message& msg)
{
    (void)msg;
}

void
    Daemon::m_pass(Message& msg)
{
    Client& client = msg.get_from();

    if ((check_error(msg.get_params().empty(), client,
                     msg.err_need_more_params())) ||
        (check_error(client.is_registered(), client,
                     msg.err_already_registred())) ||
        (check_error(msg.get_params()[0] != _ft_ircd->_password, client,
                     msg.err_passwd_mismatch())))
        return;
    client.set_password_flag();
    if (client.is_registered() &&
        !_ft_ircd->_ft_ircd->_map.client.count(client.get_names().nick))
        _ft_ircd->m_regist(client, msg);
}

void
    Daemon::m_nick(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().empty(), client,
                    msg.err_no_nickname_given()))
        return;

    const std::string& nickname = msg.get_params()[0];

    if (check_error(!utils::is_nickname_valid(nickname), client,
                    msg.err_erroneus_nickname(nickname)))
        return;

    if (_ft_ircd->_map.client.count(nickname))
    {
        if (nickname != client.get_names().nick)
            utils::push_message(client, msg.err_nickname_in_use(nickname));
        return;
    }

    if (client.is_registered())
    {
        _ft_ircd->m_send_to_channel(client, msg.build_nick_reply(nickname),
                                    &client);
        utils::push_message(client, msg.build_nick_reply(nickname));
        if (_ft_ircd->_map.client.count(client.get_names().nick))
        {
            _ft_ircd->_map.client.erase(client.get_names().nick);
            _ft_ircd->_map.client[nickname] = &client;
        }
    }

    Logger().debug() << client.get_IP() << " change nick to " << nickname;
    client.set_nickname(nickname);

    if (client.is_registered() &&
        !_ft_ircd->_map.client.count(client.get_names().nick))
        _ft_ircd->m_regist(client, msg);
}

void
    Daemon::m_user(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().size() < 4, client,
                    msg.err_need_more_params()))
        return;

    if (check_error(client.is_registered(), client,
                    msg.err_already_registred()))
        return;

    client.set_username(msg.get_params()[0]);
    client.set_realname(msg.get_params()[3]);
    if (client.is_registered() &&
        !_ft_ircd->_map.client.count(client.get_names().nick))
        _ft_ircd->m_regist(client, msg);
}

void
    Daemon::m_quit(Message& msg)
{
    Client& client = msg.get_from();

    std::string message = "Quit";
    if (msg.get_params().size())
        message += " :" + msg.get_params()[0];
    _ft_ircd->m_disconnect(client, message);
}

void
    Daemon::m_join(Message& msg)
{
    Client& client = msg.get_from();

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
        if (!_ft_ircd->_map.channel.count(channel_name))
            _ft_ircd->_map.channel.insert(
                std::make_pair(channel_name, new Channel(channel_name)));
        Channel* channel = _ft_ircd->_map.channel[channel_name];
        if (client.is_already_joined(channel))
            continue;
        else if (check_error(channel->is_full(), client,
                             msg.err_channel_is_full(channel_name)))
            continue;
        channel->join(client);
        client.insert_channel(channel);
        if (channel->get_members().size() == 1)
            channel->set_operator(&client);
        Logger().info() << "Create new channel :" << channel_name << " : @"
                        << client.get_names().nick;
        _ft_ircd->m_send_to_channel(channel,
                                    msg.build_join_reply(channel_name));
        utils::send_topic_reply(channel, client, msg);
        utils::send_name_reply(channel, client, msg);
    }
}

void
    Daemon::m_mode_channel(Message& msg, const std::string& channel_name)
{
    Client& client = msg.get_from();

    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = _ft_ircd->_map.channel.at(channel_name);
    if (check_error(msg.get_params().size() == 1, client,
                    msg.rpl_channel_mode_is(channel_name)))
        return;
    if (check_error(!channel->is_operator(client), client,
                    msg.err_chanoprivs_needed(channel_name)))
        return;

    utils::push_message(client, msg.err_unknown_mode(msg.get_params()[1]));
}

void
    Daemon::m_mode_user(Message& msg, const std::string& nickname)
{
    Client& client = msg.get_from();

    if (check_error(!_ft_ircd->_map.client.count(nickname), client,
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
    Daemon::m_mode(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;

    const std::string& target = msg.get_params()[0];

    if (utils::is_channel_prefix(target))
        m_mode_channel(msg, target);
    else
        m_mode_user(msg, target);
}

void
    Daemon::m_invite(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().size() < 2, client,
                    msg.err_need_more_params()))
        return;

    const std::string& nickname     = msg.get_params()[0];
    const std::string& channel_name = msg.get_params()[1];

    if (check_error(!_ft_ircd->_map.client.count(nickname), client,
                    msg.err_no_such_nick(nickname)))
        return;
    Client* target_client = _ft_ircd->_map.client[nickname];
    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = _ft_ircd->_map.channel[channel_name];
    if (check_error(!client.is_already_joined(channel), client,
                    msg.err_not_on_channel(channel_name)))
        return;
    if (check_error(target_client->is_already_joined(channel), client,
                    msg.err_user_on_channel(nickname, channel_name)))
        return;
    utils::push_message(client, msg.rpl_inviting(nickname, channel_name));
    _ft_ircd->m_prepare_to_send(*target_client,
                                msg.build_invite_reply(nickname, channel_name));
}

void
    Daemon::m_kick(Message& msg)
{
    Client& client = msg.get_from();

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
                         !_ft_ircd->_map.channel.count(*channel_name)),
                        client, msg.err_no_such_channel(*channel_name)))
            goto next;
        channel = _ft_ircd->_map.channel[*channel_name];
        if (check_error(!channel->is_operator(client), client,
                        msg.err_chanoprivs_needed(*channel_name)))
            goto next;
        if (check_error(!_ft_ircd->_map.client.count(*nick_name), client,
                        msg.err_no_such_nick(*nick_name)))
            goto next;
        target_client = _ft_ircd->_map.client[*nick_name];
        if (check_error(!channel->is_joined(target_client), client,
                        msg.err_user_not_in_channel(*nick_name, *channel_name)))
            goto next;
        _ft_ircd->m_send_to_channel(
            channel, msg.build_kick_reply(*channel_name, *nick_name,
                                          client.get_names().nick));
        channel->part(*target_client);
        target_client->erase_channel(channel);
        if (channel->is_empty())
        {
            _ft_ircd->_map.channel.erase(*channel_name);
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
    Daemon::m_names(Message& msg)
{
    Client& client = msg.get_from();

    std::queue<const std::string> nick_queue;
    if (msg.get_params().empty())
    {
        FT_IRCD::ChannelMap::const_iterator channel_it =
            _ft_ircd->_map.channel.begin();
        for (; channel_it != _ft_ircd->_map.channel.end(); ++channel_it)
            utils::send_name_reply(channel_it->second, client, msg);
        FT_IRCD::ClientMap::const_iterator client_it =
            _ft_ircd->_map.client.begin();
        for (; client_it != _ft_ircd->_map.client.end(); ++client_it)
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
            if (check_error(!_ft_ircd->_map.channel.count(channel_list[i]),
                            client, msg.rpl_endofnames(channel_list[i])))
                continue;
            utils::send_name_reply(_ft_ircd->_map.channel[channel_list[i]],
                                   client, msg);
        }
    }
}

static void
    send_list_to_client(Channel* channel, Client& client, Message& msg)
{
    utils::push_message(
        client, msg.rpl_list(channel->get_name(),
                             std::to_string(channel->get_members().size()),
                             channel->get_topic()));
}

void
    Daemon::m_list(Message& msg)
{
    Client& client = msg.get_from();

    if (msg.get_params().empty())
    {
        FT_IRCD::ChannelMap::const_iterator channel_it =
            _ft_ircd->_map.channel.begin();
        for (; channel_it != _ft_ircd->_map.channel.end(); ++channel_it)
            send_list_to_client(channel_it->second, client, msg);
    }

    else if (msg.get_params().size() == 1)
    {
        ConstStringVector channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        ConstStringVector::iterator channel_it = channel_list.begin();
        for (; channel_it != channel_list.end(); ++channel_it)
            if (_ft_ircd->_map.channel.count(*channel_it))
                send_list_to_client(_ft_ircd->_map.channel[*channel_it], client,
                                    msg);
            else
                utils::push_message(client,
                                    msg.err_no_such_channel(*channel_it));
    }
    utils::push_message(client, msg.rpl_listend());
}

void
    Daemon::m_part(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    ConstStringVector channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);
    ConstStringVector::iterator channel_it = channel_list.begin();
    for (; channel_it != channel_list.end(); ++channel_it)
    {
        if (check_error(!_ft_ircd->_map.channel.count(*channel_it), client,
                        msg.err_no_such_channel(*channel_it)))
            continue;
        Channel* channel = _ft_ircd->_map.channel[*channel_it];
        if (check_error(!channel->is_joined(&client), client,
                        msg.err_not_on_channel(*channel_it)))
            continue;
        _ft_ircd->m_send_to_channel(channel, msg.build_part_reply(*channel_it));
        channel->part(client);
        client.erase_channel(channel);
        if (channel->is_empty())
        {
            _ft_ircd->_map.channel.erase(channel->get_name());
            delete channel;
        }
        Logger().debug() << "Remove [" << client.get_names().nick
                         << "] client from [" << channel->get_name()
                         << "] channel";
    }
}

void
    Daemon::m_topic(Message& msg)
{
    Client& client = msg.get_from();

    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;
    const std::string& channel_name = msg.get_params()[0];
    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = _ft_ircd->_map.channel[channel_name];
    if (check_error(!channel->is_joined(&client), client,
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
    _ft_ircd->m_send_to_channel(channel, msg.build_topic_reply());
}

void
    Daemon::m_privmsg(Message& msg)
{
    Client& client = msg.get_from();

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
            if (check_error(!_ft_ircd->_map.channel.count(*target_it), client,
                            msg.err_no_such_channel(*target_it)))
                continue;
            _ft_ircd->m_send_to_channel(_ft_ircd->_map.channel[*target_it],
                                        msg.build_message_reply(*target_it),
                                        &client);
        }
        else if (_ft_ircd->_map.client.count(*target_it))
            _ft_ircd->m_prepare_to_send(*_ft_ircd->_map.client[*target_it],
                                        msg.build_message_reply(*target_it));
        else if (msg.get_command() != "NOTICE")
            utils::push_message(client, msg.err_no_such_nick(*target_it));
    }
}

void
    Daemon::m_notice(Message& msg)
{
    m_privmsg(msg);
}

Daemon::~Daemon()
{
}

Daemon::Daemon()
{
}

#include "../includes/commands.hpp"

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    if (error_check)
        utils::push_message(client, message);
    return error_check;
}

void
    Commands::m_pass(Message& msg)
{
    Client& client = msg.get_from();

    if ((check_error(msg.get_params().empty(), client,
                     msg.err_need_more_params())) ||
        (check_error(client.is_registered(), client,
                     msg.err_already_registred())) ||
        (check_error(msg.get_params()[0] != _password, client,
                     msg.err_passwd_mismatch())))
        return;
    client.set_password_flag();
    if (client.is_registered() && !_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Commands::m_nick(Message& msg)
{

    if (check_error(msg.get_params().empty(), client,
                    msg.err_no_nickname_given()))
        return;

    const std::string& nickname = msg.get_params()[0];

    if (check_error(!utils::is_nickname_valid(nickname), client,
                    msg.err_erroneus_nickname(nickname)))
        return;

    if (_client_map.count(nickname))
    {
        if (nickname != client.get_names().nick)
            utils::push_message(client, msg.err_nickname_in_use(nickname));
        return;
    }

    if (client.is_registered())
    {
        m_send_to_channel(client, msg.build_nick_reply(nickname), &client);
        utils::push_message(client, msg.build_nick_reply(nickname));
        if (_client_map.count(client.get_names().nick))
        {
            _client_map.erase(client.get_names().nick);
            _client_map[nickname] = &client;
        }
    }

    Logger().debug() << client.get_IP() << " change nick to " << nickname;
    client.set_nickname(nickname);

    if (client.is_registered() && !_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Commands::m_user(Message& msg)
{
    if (check_error(msg.get_params().size() < 4, client,
                    msg.err_need_more_params()))
        return;

    if (check_error(client.is_registered(), client,
                    msg.err_already_registred()))
        return;

    client.set_username(msg.get_params()[0]);
    client.set_realname(msg.get_params()[3]);
    if (client.is_registered() && !_client_map.count(client.get_names().nick))
        m_register_client(client, msg);
}

void
    Commands::m_quit(Message& msg)
{
    std::string message = "Quit";
    if (msg.get_params().size())
        message += " :" + msg.get_params()[0];
    m_disconnect_client(client, message);
}

void
    Commands::m_join(Message& msg)
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
        if (!_channel_map.count(channel_name))
            _channel_map.insert(
                std::make_pair(channel_name, new Channel(channel_name)));
        Channel* channel = _channel_map[channel_name];
        if (client.is_already_joined(channel))
            continue;
        else if (check_error(channel->is_full(), client,
                             msg.err_channel_is_full(channel_name)))
            continue;
        channel->join(client);
        client.insert_channel(channel);
        if (channel->get_users().size() == 1)
            channel->set_operator(&client);
        Logger().info() << "Create new channel :" << channel_name << " : @"
                        << client.get_names().nick;
        m_send_to_channel(channel, msg.build_join_reply(channel_name));
        utils::send_topic_reply(channel, client, msg);
        utils::send_name_reply(channel, client, msg);
    }
}

void
    Commands::m_mode_channel(Message& msg, const std::string& channel_name)
{
    if (check_error(!_channel_map.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = _channel_map.at(channel_name);
    if (check_error(msg.get_params().size() == 1, client,
                    msg.rpl_channel_mode_is(channel_name)))
        return;
    if (check_error(!channel->is_operator(client), client,
                    msg.err_chanoprivs_needed(channel_name)))
        return;

    utils::push_message(client, msg.err_unknown_mode(msg.get_params()[1]));
}

void
    Commands::m_mode_user(Message& msg, const std::string& nickname)
{
    if (check_error(!_client_map.count(nickname), client,
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
    Commands::m_mode(Message& msg)
{
    if (check_error(msg.get_params().empty(), client,
                    msg.err_need_more_params()))
        return;

    const std::string& target = msg.get_params()[0];

    if (utils::is_channel_prefix(target))
        m_mode_channel(client, msg, target);
    else
        m_mode_user(client, msg, target);
}

void
    Commands::m_invite(Message& msg)
{
    if (check_error(msg.get_params().size() < 2, client,
                    msg.err_need_more_params()))
        return;

    const std::string& nickname     = msg.get_params()[0];
    const std::string& channel_name = msg.get_params()[1];

    if (check_error(!_client_map.count(nickname), client,
                    msg.err_no_such_nick(nickname)))
        return;
    Client* target_client = _client_map[nickname];
    if (check_error(!_channel_map.count(channel_name), client,
                    msg.err_no_such_channel(channel_name)))
        return;
    Channel* channel = _channel_map[channel_name];
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
    Commands::m_kick(Message& msg)
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
                         !_channel_map.count(*channel_name)),
                        client, msg.err_no_such_channel(*channel_name)))
            goto next;
        channel = _channel_map[*channel_name];
        if (check_error(!channel->is_operator(client), client,
                        msg.err_chanoprivs_needed(*channel_name)))
            goto next;
        if (check_error(!_client_map.count(*nick_name), client,
                        msg.err_no_such_nick(*nick_name)))
            goto next;
        target_client = _client_map[*nick_name];
        if (check_error(!channel->is_joined(target_client), client,
                        msg.err_user_not_in_channel(*nick_name, *channel_name)))
            goto next;
        m_send_to_channel(channel,
                          msg.build_kick_reply(*channel_name, *nick_name,
                                               client.get_names().nick));
        channel->part(*target_client);
        target_client->erase_channel(channel);
        if (channel->is_empty())
        {
            _channel_map.erase(*channel_name);
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
    Commands::m_names(Message& msg)
{
    std::queue<const std::string> nick_queue;
    if (msg.get_params().empty())
    {
        ChannelMap::const_iterator channel_it = _channel_map.begin();
        for (; channel_it != _channel_map.end(); ++channel_it)
            utils::send_name_reply(channel_it->second, client, msg);
        ClientMap::const_iterator client_it = _client_map.begin();
        for (; client_it != _client_map.end(); ++client_it)
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
            if (check_error(!_channel_map.count(channel_list[i]), client,
                            msg.rpl_endofnames(channel_list[i])))
                continue;
            utils::send_name_reply(_channel_map[channel_list[i]], client, msg);
        }
    }
}

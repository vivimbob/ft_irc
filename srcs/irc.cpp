#include "../includes/irc.hpp"
#include "../includes/ft_ircd.hpp"

TYPE
    IRC::get_type(std::string command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return UNKNOWN;
}

bool
    IRC::m_checker()
{
    switch (_request->type)
    {
    case PASS:
    case NICK:
    case USER:
    case QUIT:
    case JOIN:
    case PART:
    case TOPIC:
    case NAMES:
    case LIST:
    case INVITE:
    case KICK:
    case MODE:
    case PRIVMSG:
    case NOTICE:
    case UNKNOWN:
    case UNREGISTERED:
    default:
        break;
    }

    return true;
    // type: PRIVMGS
    /////// if (check_error(parameter.empty(), client, msg.err_no_recipient()))
    ///////     return;
    /////// if (check_error(parameter.size() == 1, client,
    /////// msg.err_no_text_to_send()))
    ///////     return;
    // type CHANNEL
    // if (check_error(!_ft_ircd->_map.channel.count(*target_it), client,
    //            msg.err_no_such_channel(*target_it)))
    // continue;
}

void
    IRC::m_to_client(Client& client, const std::string& str_msg)
{
    client.push_message(str_msg);
    // Event::toggle(client, EVFILT_READ);
}

void
    IRC::m_to_members(Channel*           channel,
                      const std::string& msg,
                      Client*            exclusion)
{
    const Channel::MemberMap&          user_list = channel->get_members();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :" << channel->get_name();
    for (; user != user_list.end(); ++user)
        if (user->first != exclusion)
            m_to_client(*user->first, msg);
}

void
    IRC::m_to_members(Client& client, const std::string& msg, Client* exclusion)
{
    std::set<Channel*>::iterator it = client.get_joined_list().begin();
    for (; it != client.get_joined_list().end(); ++it)
        m_to_members(*it, msg, exclusion);
}

static void
    split_by_comma(ConstStringVector& splited_params, const std::string& params)
{
    std::istringstream iss(params);

    std::string elem;
    while (std::getline(iss, elem, ','))
        splited_params.push_back(elem);
}

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    if (error_check)
        utils::push_message(client, message);
    return error_check;
}

void
    IRC::empty()
{
}

void
    IRC::pass()
{
    //
    //    if ((check_error(request.parameter.empty(), client,
    //                     msg.err_need_more_params())) ||
    //        (check_error(client.is_registered(), client,
    //                     msg.err_already_registred())) ||
    //        (check_error(request.parameter[0] != _ft_ircd->_password, client,
    //                     msg.err_passwd_mismatch())))
    //        return;
    //    client.set_password_flag();
    //    if (client.is_registered() &&
    //        !_ft_ircd->_ft_ircd->_map.client.count(client.get_names().nick))
    //        _ft_ircd->m_regist(client, msg);
}

void
    IRC::nick()
{
    //
    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_no_nickname_given()))
    //        return;
    //
    //    const std::string& nickname = request.parameter[0];
    //
    //    if (check_error(!utils::is_nickname_valid(nickname), client,
    //                    msg.err_erroneus_nickname(nickname)))
    //        return;
    //
    //    if (_ft_ircd->_map.client.count(nickname))
    //    {
    //        if (nickname != client.get_names().nick)
    //            utils::push_message(client,
    //            msg.err_nickname_in_use(nickname));
    //        return;
    //    }
    //
    //    if (client.is_registered())
    //    {
    //        _ft_ircd->m_send_to_channel(client,
    //        msg.build_nick_reply(nickname),
    //                                    &client);
    //        utils::push_message(client, msg.build_nick_reply(nickname));
    //        if (_ft_ircd->_map.client.count(client.get_names().nick))
    //        {
    //            _ft_ircd->_map.client.erase(client.get_names().nick);
    //            _ft_ircd->_map.client[nickname] = &client;
    //        }
    //    }
    //
    //    Logger().debug() << client.get_IP() << " change nick to " << nickname;
    //    client.set_nickname(nickname);
    //
    //    if (client.is_registered() &&
    //        !_ft_ircd->_map.client.count(client.get_names().nick))
    //        _ft_ircd->m_regist(client, msg);
}

void
    IRC::user()
{

    //    if (check_error(request.parameter.size() < 4, client,
    //                    msg.err_need_more_params()))
    //        return;
    //
    //    if (check_error(client.is_registered(), client,
    //                    msg.err_already_registred()))
    //        return;
    //
    //    client.set_username(request.parameter[0]);
    //    client.set_realname(request.parameter[3]);
    //    if (client.is_registered() &&
    //        !_ft_ircd->_map.client.count(client.get_names().nick))
    //        _ft_ircd->m_regist(client, msg);
}

void
    IRC::quit()
{
    //    std::string message = "Quit";
    //    if (request.parameter.size())
    //        message += " :" + request.parameter[0];
    //    _ft_ircd->m_disconnect(client, message);
}

void
    IRC::join()
{
    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //    ConstStringVector channel_list;
    //
    //    utils::split_by_comma(channel_list, request.parameter[0]);
    //    for (ConstStringVector::iterator channel_it = channel_list.begin();
    //         channel_it != channel_list.end(); ++channel_it)
    //    {
    //        const std::string& channel_name = *channel_it;
    //
    //        if (check_error((!utils::is_channel_prefix(channel_name) ||
    //                         !utils::is_channel_name_valid(channel_name)),
    //                        client, msg.err_no_such_channel(channel_name)))
    //            continue;
    //        if (check_error(!client.is_join_available(), client,
    //                        msg.err_too_many_channels(channel_name)))
    //            continue;
    //        if (!_ft_ircd->_map.channel.count(channel_name))
    //            _ft_ircd->_map.channel.insert(
    //                std::make_pair(channel_name, new Channel(channel_name)));
    //        Channel* channel = _ft_ircd->_map.channel[channel_name];
    //        if (client.is_already_joined(channel))
    //            continue;
    //        else if (check_error(channel->is_full(), client,
    //                             msg.err_channel_is_full(channel_name)))
    //            continue;
    //        channel->join(client);
    //        client.insert_channel(channel);
    //        if (channel->get_members().size() == 1)
    //            channel->set_operator(&client);
    //        Logger().info() << "Create new channel :" << channel_name << " :
    //        @"
    //                        << client.get_names().nick;
    //        _ft_ircd->m_send_to_channel(channel,
    //                                    msg.build_join_reply(channel_name));
    //        utils::send_topic_reply(channel, client, msg);
    //        utils::send_name_reply(channel, client, msg);
    //    }
}

void
    IRC::m_mode_channel(Client::t_requests& requests,
                        const std::string&  channel_name)
{
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
    //                    msg.err_no_such_channel(channel_name)))
    //        return;
    //    Channel* channel = _ft_ircd->_map.channel.at(channel_name);
    //    if (check_error(request.parameter.size() == 1, client,
    //                    msg.rpl_channel_mode_is(channel_name)))
    //        return;
    //    if (check_error(!channel->is_operator(client), client,
    //                    msg.err_chanoprivs_needed(channel_name)))
    //        return;
    //
    //    utils::push_message(client,
    //    msg.err_unknown_mode(request.parameter[1]));
}

void
    IRC::m_mode_user(Client::t_requests& requests, const std::string& nickname)
{
    //
    //    if (check_error(!_ft_ircd->_map.client.count(nickname), client,
    //                    msg.err_no_such_nick(nickname)))
    //        return;
    //
    //    if (check_error(nickname != client.get_names().nick, client,
    //                    msg.err_users_dont_match(
    //                        request.parameter.size() == 1 ? "view" :
    //                        "change")))
    //        return;
    //
    //    if (check_error(request.parameter.size() == 1, client,
    //                    msg.rpl_user_mode_is()))
    //        return;
    //
    //    utils::push_message(client, msg.err_u_mode_unknown_flag());
}

void
    IRC::mode()
{
    //
    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //
    //    const std::string& target = request.parameter[0];
    //
    //    if (utils::is_channel_prefix(target))
    //        m_mode_channel(msg, target);
    //    else
    //        m_mode_user(msg, target);
}

void
    IRC::invite()
{

    //    if (check_error(request.parameter.size() < 2, client,
    //                    msg.err_need_more_params()))
    //        return;
    //
    //    const std::string& nickname     = request.parameter[0];
    //    const std::string& channel_name = request.parameter[1];
    //
    //    if (check_error(!_ft_ircd->_map.client.count(nickname), client,
    //                    msg.err_no_such_nick(nickname)))
    //        return;
    //    Client* target_client = _ft_ircd->_map.client[nickname];
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
    //                    msg.err_no_such_channel(channel_name)))
    //        return;
    //    Channel* channel = _ft_ircd->_map.channel[channel_name];
    //    if (check_error(!client.is_already_joined(channel), client,
    //                    msg.err_not_on_channel(channel_name)))
    //        return;
    //    if (check_error(target_client->is_already_joined(channel), client,
    //                    msg.err_user_on_channel(nickname, channel_name)))
    //        return;
    //    utils::push_message(client, msg.rpl_inviting(nickname, channel_name));
    //    _ft_ircd->m_prepare_to_send(*target_client,
    //                                msg.build_invite_reply(nickname,
    //                                channel_name));
}

void
    IRC::kick()
{

    //    const std::vector<std::string>& parameter = request.parameter;
    //    if (check_error(parameter.size() < 2, client,
    //    msg.err_need_more_params()))
    //        return;
    //
    //    ConstStringVector channel_list;
    //    ConstStringVector nick_list;
    //
    //    utils::split_by_comma(channel_list, parameter[0]);
    //    utils::split_by_comma(nick_list, parameter[1]);
    //
    //    if (check_error((!(channel_list.size() == 1 || nick_list.size() == 1)
    //    &&
    //                     channel_list.size() != nick_list.size()),
    //                    client, msg.err_need_more_params()))
    //        return;
    //
    //    ConstStringVector::iterator channel_name = channel_list.begin();
    //    ConstStringVector::iterator nick_name    = nick_list.begin();
    //
    //    Channel* channel;
    //    Client*  target_client;
    //
    //    for (int i = 0, max_size = std::max(channel_list.size(),
    //    nick_list.size());
    //         i < max_size; ++i)
    //    {
    //        if (check_error((!utils::is_channel_prefix(*channel_name) ||
    //                         !_ft_ircd->_map.channel.count(*channel_name)),
    //                        client, msg.err_no_such_channel(*channel_name)))
    //            goto next;
    //        channel = _ft_ircd->_map.channel[*channel_name];
    //        if (check_error(!channel->is_operator(client), client,
    //                        msg.err_chanoprivs_needed(*channel_name)))
    //            goto next;
    //        if (check_error(!_ft_ircd->_map.client.count(*nick_name), client,
    //                        msg.err_no_such_nick(*nick_name)))
    //            goto next;
    //        target_client = _ft_ircd->_map.client[*nick_name];
    //        if (check_error(!channel->is_joined(target_client), client,
    //                        msg.err_user_not_in_channel(*nick_name,
    //                        *channel_name)))
    //            goto next;
    //        _ft_ircd->m_send_to_channel(
    //            channel, msg.build_kick_reply(*channel_name, *nick_name,
    //                                          client.get_names().nick));
    //        channel->part(*target_client);
    //        target_client->erase_channel(channel);
    //        if (channel->is_empty())
    //        {
    //            _ft_ircd->_map.channel.erase(*channel_name);
    //            delete channel;
    //        }
    //    next:
    //        if (channel_list.size() != 1)
    //            ++channel_name;
    //        if (nick_list.size() != 1)
    //            ++nick_name;
    //    }
}

void
    IRC::names()
{
    //    std::queue<const std::string> nick_queue;
    //    if (request.parameter.empty())
    //    {
    //        FT_IRCD::ChannelMap::const_iterator channel_it =
    //            _ft_ircd->_map.channel.begin();
    //        for (; channel_it != _ft_ircd->_map.channel.end(); ++channel_it)
    //            utils::send_name_reply(channel_it->second, client, msg);
    //        FT_IRCD::ClientMap::const_iterator client_it =
    //            _ft_ircd->_map.client.begin();
    //        for (; client_it != _ft_ircd->_map.client.end(); ++client_it)
    //            if (client_it->second->get_joined_list().empty())
    //                nick_queue.push(client_it->first);
    //        if (nick_queue.size())
    //            utils::push_message(client, msg.rpl_namreply("*",
    //            nick_queue));
    //        utils::push_message(client, msg.rpl_endofnames("*"));
    //        return;
    //    }
    //    else
    //    {
    //        ConstStringVector channel_list;
    //        utils::split_by_comma(channel_list, request.parameter[0]);
    //        for (int i = 0, size = channel_list.size(); i < size; ++i)
    //        {
    //            if
    //            (check_error(!_ft_ircd->_map.channel.count(channel_list[i]),
    //                            client, msg.rpl_endofnames(channel_list[i])))
    //                continue;
    //            utils::send_name_reply(_ft_ircd->_map.channel[channel_list[i]],
    //                                   client, msg);
    //        }
    //    }
}

// static void
//     send_list_to_client(Channel* channel, Client& client,
//     Client::t__requests& _requests)
//{
//     utils::push_message(
//         client, msg.rpl_list(channel->get_name(),
//                              std::to_string(channel->get_members().size()),
//                              channel->get_topic()));
// }

void
    IRC::list()
{
    //    if (request.parameter.empty())
    //    {
    //        FT_IRCD::ChannelMap::const_iterator channel_it =
    //            _ft_ircd->_map.channel.begin();
    //        for (; channel_it != _ft_ircd->_map.channel.end(); ++channel_it)
    //            send_list_to_client(channel_it->second, client, msg);
    //    }
    //
    //    else if (request.parameter.size() == 1)
    //    {
    //        ConstStringVector channel_list;
    //        utils::split_by_comma(channel_list, request.parameter[0]);
    //
    //        ConstStringVector::iterator channel_it = channel_list.begin();
    //        for (; channel_it != channel_list.end(); ++channel_it)
    //            if (_ft_ircd->_map.channel.count(*channel_it))
    //                send_list_to_client(_ft_ircd->_map.channel[*channel_it],
    //                client,
    //                                    msg);
    //            else
    //                utils::push_message(client,
    //                                    msg.err_no_such_channel(*channel_it));
    //    }
    //    utils::push_message(client, msg.rpl_listend());
}

void
    IRC::part()
{
    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //    ConstStringVector channel_list;
    //    utils::split_by_comma(channel_list, request.parameter[0]);
    //    ConstStringVector::iterator channel_it = channel_list.begin();
    //    for (; channel_it != channel_list.end(); ++channel_it)
    //    {
    //        if (check_error(!_ft_ircd->_map.channel.count(*channel_it),
    //        client,
    //                        msg.err_no_such_channel(*channel_it)))
    //            continue;
    //        Channel* channel = _ft_ircd->_map.channel[*channel_it];
    //        if (check_error(!channel->is_joined(&client), client,
    //                        msg.err_not_on_channel(*channel_it)))
    //            continue;
    //        _ft_ircd->m_send_to_channel(channel,
    //        msg.build_part_reply(*channel_it)); channel->part(client);
    //        client.erase_channel(channel);
    //        if (channel->is_empty())
    //        {
    //            _ft_ircd->_map.channel.erase(channel->get_name());
    //            delete channel;
    //        }
    //        Logger().debug() << "Remove [" << client.get_names().nick
    //                         << "] client from [" << channel->get_name()
    //                         << "] channel";
    //    }
}

void
    IRC::topic()
{

    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //    const std::string& channel_name = request.parameter[0];
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name), client,
    //                    msg.err_no_such_channel(channel_name)))
    //        return;
    //    Channel* channel = _ft_ircd->_map.channel[channel_name];
    //    if (check_error(!channel->is_joined(&client), client,
    //                    msg.err_not_on_channel(channel_name)))
    //        return;
    //    if (request.parameter.size() == 1)
    //    {
    //        utils::send_topic_reply(channel, client, msg);
    //        return;
    //    }
    //    if (check_error(!channel->is_operator(client), client,
    //                    msg.err_chanoprivs_needed(channel_name)))
    //        return;
    //    channel->set_topic(request.parameter[1]);
    //    Logger().trace() << channel_name << " channel topic change to "
    //                     << channel->get_topic();
    //    _ft_ircd->m_send_to_channel(channel, msg.build_topic_reply());
}

void
    IRC::privmsg()
{
    //    // m_checker(PRIVMSG관련);
    //
    //    ConstStringVector targets;
    //    split_by_comma(targets, request.parameter.front());
    //
    //    ConstStringVector::iterator target_it  = targets.begin();
    //    ConstStringVector::iterator target_ite = targets.end();
    //    for (; target_it != target_ite; ++target_it)
    //    {
    //        if (utils::is_channel_prefix(*target_it))
    //        {
    //            // m_checker(채널있냐?)
    //            m_to_members(_ft_ircd->_map.channel[*target_it],
    //                         msg.build_message_reply(*target_it), &client);
    //        }
    //        else if (_ft_ircd->_map.client.count(*target_it))
    //            _ft_ircd->m_prepare_to_send(*_ft_ircd->_map.client[*target_it],
    //                                        msg.build_message_reply(*target_it));
    //        else if (msg.get_command() != "NOTICE")
    //            utils::push_message(client, msg.err_no_such_nick(*target_it));
    //    }
}

void
    IRC::notice()
{
    //   m_privmsg(msg);
}

void
    IRC::unknown()
{
    //   m_privmsg(msg);
}

void
    IRC::unregistered()
{
    //   m_privmsg(msg);
}

IRC::~IRC()
{
}

IRC::IRC()
{
    _commands.push_back(&IRC::empty);
    _commands.push_back(&IRC::pass);
    _commands.push_back(&IRC::nick);
    _commands.push_back(&IRC::user);
    _commands.push_back(&IRC::quit);
    _commands.push_back(&IRC::join);
    _commands.push_back(&IRC::part);
    _commands.push_back(&IRC::topic);
    _commands.push_back(&IRC::names);
    _commands.push_back(&IRC::list);
    _commands.push_back(&IRC::invite);
    _commands.push_back(&IRC::kick);
    _commands.push_back(&IRC::mode);
    _commands.push_back(&IRC::privmsg);
    _commands.push_back(&IRC::notice);
    _commands.push_back(&IRC::unknown);
}

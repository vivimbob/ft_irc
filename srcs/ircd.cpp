#include "../includes/ft_ircd.hpp"
#include "../includes/irc.hpp"

TYPE
    IRCD::get_type(std::string command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return UNKNOWN;
}

void
    IRCD::registration()
{
    _ft_ircd->_map.client[_client->get_names().nick] = _client;
    m_to_client(rpl_welcome());
    Logger().info() << _client->get_names().nick << " is registered";
}

static bool
    check_error(bool error_check, Client& client, std::string message)
{
    return error_check;
}

static void
    split_by_comma(std::vector<const std::string>& splited_params,
                   const std::string&              params)
{
    std::istringstream iss(params);

    std::string elem;
    while (std::getline(iss, elem, ','))
        splited_params.push_back(elem);
}

void
    IRCD::m_to_client(Client& client, const std::string& str)
{
    client.get_buffers().to_client.queue.push(str);
    _ft_ircd->toggle(client, EVFILT_READ);
}

RESULT
IRCD::m_to_client(std::string str)
{
    _to_client->queue.push(str);
    return ERROR;
}

RESULT
IRCD::m_empty()
{
    return OK;
}

void
    IRCD::empty()
{
    m_empty();
}

RESULT
IRCD::m_pass()
{
    if (_request->parameter.empty())
        return m_to_client(err_need_more_params());
    if (_client->is_registered())
        return m_to_client(err_already_registred());
    if (_request->parameter[0] != _ft_ircd->_password)
        return m_to_client(err_passwd_mismatch());
    return OK;
}

void
    IRCD::pass()
{
    if (m_pass() == ERROR)
        return;
    _client->set_status(PASS);
}

static inline bool
    is_special(char c)
{
    return std::memchr(SPECIALCHAR, c, 8);
}

RESULT
IRCD::m_is_valid(TYPE type)
{
    switch (type)
    {
        case NICK:
        {
            if (NICK_LENGTH_MAX < _target->length())
                return ERROR;
            if (!std::isalpha((*_target)[0]))
                return ERROR;
            for (size_t index = 1; index < _target->length(); ++index)
                if (!std::isalpha((*_target)[index])
                    && !std::isdigit((*_target)[index])
                    && !is_special((*_target)[index]))
                    return ERROR;
            break;
        }
        case CHANNEL_PREFIX:
        {
            if ((*_target)[0] != CHANNEL_PREFIX)
                return ERROR;
            break;
        }
        case CHANNEL_NAME:
        {
            if (CHANNEL_LENGTH_MAX < _target->length())
                return ERROR;
            for (size_t index = 0; index < _target->length(); ++index)
                if (std::memchr(CHSTRING, (*_target)[index], 5))
                    return ERROR;
            break;
        }
        default:
            break;
    }
    return OK;
}

void
    IRCD::m_to_channel(const std::string& str)
{
    Channel::CITER iter = _channel->get_members().begin();
    Channel::CITER end  = _channel->get_members().end();

    Logger().trace() << "to channel :" << _channel->get_name();

    for (; iter != end; ++iter)
        if (iter->first != _client)
            m_to_client(*iter->first, str);
}

void
    IRCD::m_to_channels(const std::string& str)
{
    Client::CITER iter = _client->get_channels().begin();
    Client::CITER end  = _client->get_channels().end();

    for (_channel = *iter; iter != end; _channel = *(++iter))
        m_to_channel(str);
}

RESULT
IRCD::m_nick()
{
    if (_request->parameter.empty())
        return m_to_client(err_no_nickname_given());
    if (m_is_valid(NICK) == ERROR)
        return m_to_client(err_erroneus_nickname(*_target));
    if (_ft_ircd->_map.client.count(*_target))
    {
        if (*_target != _client->get_names().nick)
            return m_to_client(err_nickname_in_use(*_target));
        return ERROR;
    }
    return OK;
}

void
    IRCD::nick()
{
    if (m_nick() == ERROR)
        return;
    _target = &_request->parameter[0];

    if (_client->is_registered())
    {
        if (_client->get_channels().size())
            m_to_channels(cmd_nick_reply(*_target));
        _ft_ircd->_map.client.erase(_client->get_names().nick);
        _ft_ircd->_map.client[*_target] = _client;
    }
    _client->set_nickname(*_target);
    Logger().debug() << _client->get_IP() << " change nick to "
                     << _client->get_names().nick;
}

RESULT
IRCD::m_user()
{
    if (_request->parameter.size() < 4)
        return m_to_client(err_need_more_params());
    if (_client->is_registered())
        return m_to_client(err_already_registred());
    return OK;
}

void
    IRCD::user()
{
    if (m_user() == ERROR)
        return;

    _client->set_username(_request->parameter[0]);
    _client->set_realname(_request->parameter[3]);
}

RESULT
IRCD::m_quit()
{
    return OK;
}

void
    IRCD::quit()
{
    m_quit();
    std::string message = "Quit";
    if (_request->parameter.size())
        message += " :" + _request->parameter[0];
    _ft_ircd->m_disconnect(message);
}

RESULT
IRCD::m_join(PHASE phase, Channel* channel)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if (!m_is_valid(CHANNEL_PREFIX))
            return m_to_client(err_no_such_channel(*_target));
        if (!m_is_valid(CHANNEL_NAME))
            return m_to_client(err_no_such_channel(*_target));
        if (CLIENT_CAHNNEL_LIMIT <= _client->get_channels().size())
            return m_to_client(err_too_many_channels(*_target));
    }
    else if (phase == THREE)
    {
        if (_client->is_joined(channel))
            return ERROR;
        else if (channel->is_full())
            return m_to_client(err_channel_is_full(*_target));
    }
    return OK;
}

std::vector<const std::string>
    split(const std::string& params, char delimiter)
{
    std::vector<const std::string> splited;
    std::istringstream             iss(params);
    std::string                    element;

    while (std::getline(iss, element, delimiter))
        splited.push_back(element);
    return splited;
}

// static void
//     send_name_reply(Channel* channel, Client& client, IRC& msg)
//{
//     std::queue<const std::string> nick_queue;

//    Channel::MemberMap::const_iterator user_it =
//    channel->get_members().begin(); Channel::MemberMap::const_iterator
//    user_ite = channel->get_members().end();

//    for (; user_it != user_ite; ++user_it)
//        nick_queue.push(masked_nick(channel->is_operator(*user_it->first),
//                                    user_it->first->get_names().nick,
//                                    client.is_already_joined(channel)));

//    if (nick_queue.size())
//        client.push_message(
//            msg.rpl_namreply("= " + channel->get_name(), nick_queue));
//    utils::push_message(client, msg.rpl_endofnames(channel->get_name()));
//}

void
    IRCD::join()
{
    if (m_join(ONE) == ERROR)
        return;
    std::vector<const std::string> channels
        = split(_request->parameter[0], DELIMITER);
    IRCD::ITER iter = channels.begin();
    IRCD::ITER end  = channels.end();
    for (; iter != end; ++iter)
    {
        _target = iter.base();
        if (m_join(TWO) == ERROR)
            continue;
        if (!_ft_ircd->_map.channel.count(*_target))
            _ft_ircd->_map.channel.insert(
                std::make_pair(*_target, new Channel(*_target)));
        _channel = _ft_ircd->_map.channel[*_target];
        if (m_join(THREE, _channel) == ERROR)
            continue;
        _channel->join(*_client);
        if (_channel->get_members().size() == 1)
        {
            _channel->set_operator(_client);
            Logger().info() << "Create new channel :" << *_target << " : @"
                            << _client->get_names().nick;
        }
        m_to_channel(cmd_join_reply(*_target));
        if (_channel->get_topic().size())
            m_to_client(rpl_topic(_channel->get_name(), _channel->get_topic()));
        else
            m_to_client(rpl_notopic(_channel->get_name()));

        // utils::send_name_reply(channel, _client, msg);
        m_to_client(rpl_endofnames(_channel->get_name()));
    }
}

void
    IRCD::m_mode_channel(const std::string& channel_name)
{
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name),
    //    client,
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
    IRCD::m_mode_user(const std::string& nickname)
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
    IRCD::mode()
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
    IRCD::invite()
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
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name),
    //    client,
    //                    msg.err_no_such_channel(channel_name)))
    //        return;
    //    Channel* channel = _ft_ircd->_map.channel[channel_name];
    //    if (check_error(!client.is_already_joined(channel), client,
    //                    msg.err_not_on_channel(channel_name)))
    //        return;
    //    if (check_error(target_client->is_already_joined(channel), client,
    //                    msg.err_user_on_channel(nickname, channel_name)))
    //        return;
    //    utils::push_message(client, msg.rpl_inviting(nickname,
    //    channel_name)); _ft_ircd->m_prepare_to_send(*target_client,
    //                                msg.cmd_invite_reply(nickname,
    //                                channel_name));
}

void
    IRCD::kick()
{

    //    const std::vector<std::string>& parameter = request.parameter;
    //    if (check_error(parameter.size() < 2, client,
    //    msg.err_need_more_params()))
    //        return;
    //
    //    std::vector<const std::string> channel_list;
    //    std::vector<const std::string> nick_list;
    //
    //    utils::split_by_comma(channel_list, parameter[0]);
    //    utils::split_by_comma(nick_list, parameter[1]);
    //
    //    if (check_error((!(channel_list.size() == 1 || nick_list.size() ==
    //    1)
    //    &&
    //                     channel_list.size() != nick_list.size()),
    //                    client, msg.err_need_more_params()))
    //        return;
    //
    //    IRCD::ITER channel_name =
    //    channel_list.begin(); IRCD::ITER
    //    nick_name    = nick_list.begin();
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
    //                        client,
    //                        msg.err_no_such_channel(*channel_name)))
    //            goto next;
    //        channel = _ft_ircd->_map.channel[*channel_name];
    //        if (check_error(!channel->is_operator(client), client,
    //                        msg.err_chanoprivs_needed(*channel_name)))
    //            goto next;
    //        if (check_error(!_ft_ircd->_map.client.count(*nick_name),
    //        client,
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

RESULT
IRCD::m_names()
{
    if (_request->parameter.empty())
    {
        IRC::CH_CITER ch_iter = _ft_ircd->_map.channel.begin();
        for (; ch_iter != _ft_ircd->_map.channel.end(); ++ch_iter)
            ; // utils::send_name_reply(ch_iter->second, client, msg);
        IRC::CL_CITER cl_iter = _ft_ircd->_map.client.begin();
        for (; cl_iter != _ft_ircd->_map.client.end(); ++cl_iter)
            if (cl_iter->second->get_channels().empty())
                //        nick_queue.push(cl_iter->first);
                if (nick_queue.size())
                    utils::push_message(client,
                                        msg.rpl_namreply("*", nick_queue));
        utils::push_message(client, msg.rpl_endofnames("*"));
        return;
    }
    else
    {
        std::vector<const std::string> channel_list;
        utils::split_by_comma(channel_list, request.parameter[0]);
        for (int i = 0, size = channel_list.size(); i < size; ++i)
        {
            if (check_error(!_ft_ircd->_map.channel.count(channel_list[i]),
                            client, msg.rpl_endofnames(channel_list[i])))
                continue;
            utils::send_name_reply(_ft_ircd->_map.channel[channel_list[i]],
                                   client, msg);
        }
    }
    return OK;
}

void
    IRCD::names()
{
    //    std::queue<const std::string> nick_queue;
    //    if (request.parameter.empty())
    //    {
    //        IRC::CH_CITER iter =
    //            _ft_ircd->_map.channel.begin();
    //        for (; channel_it != _ft_ircd->_map.channel.end();
    //        ++channel_it)
    //            utils::send_name_reply(channel_it->second, client, msg);
    //        IRC::CL_CITER client_it =
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
    //        std::vector<const std::string> channel_list;
    //        utils::split_by_comma(channel_list, request.parameter[0]);
    //        for (int i = 0, size = channel_list.size(); i < size; ++i)
    //        {
    //            if
    //            (check_error(!_ft_ircd->_map.channel.count(channel_list[i]),
    //                            client,
    //                            msg.rpl_endofnames(channel_list[i])))
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
    IRCD::list()
{
    //    if (request.parameter.empty())
    //    {
    //        IRC::CH_CITER channel_it =
    //            _ft_ircd->_map.channel.begin();
    //        for (; channel_it != _ft_ircd->_map.channel.end();
    //        ++channel_it)
    //            send_list_to_client(channel_it->second, client, msg);
    //    }
    //
    //    else if (request.parameter.size() == 1)
    //    {
    //        std::vector<const std::string> channel_list;
    //        utils::split_by_comma(channel_list, request.parameter[0]);
    //
    //        IRCD::ITER channel_it =
    //        channel_list.begin(); for (; channel_it != channel_list.end();
    //        ++channel_it)
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
    IRCD::part()
{
    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //    std::vector<const std::string> channel_list;
    //    utils::split_by_comma(channel_list, request.parameter[0]);
    //    IRCD::ITER channel_it =
    //    channel_list.begin(); for (; channel_it != channel_list.end();
    //    ++channel_it)
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
    IRCD::topic()
{

    //    if (check_error(request.parameter.empty(), client,
    //                    msg.err_need_more_params()))
    //        return;
    //    const std::string& channel_name = request.parameter[0];
    //    if (check_error(!_ft_ircd->_map.channel.count(channel_name),
    //    client,
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
    IRCD::privmsg()
{
    //    // m_checker(PRIVMSG관련);
    //
    //    std::vector<const std::string> targets;
    //    split_by_comma(targets, request.parameter.front());
    //
    //    IRCD::ITER target_it  = targets.begin();
    //    IRCD::ITER target_ite = targets.end();
    //    for (; target_it != target_ite; ++target_it)
    //    {
    //        if (utils::is_channel_prefix(*target_it))
    //        {
    //            // m_checker(채널있냐?)
    //            m_to_members(_ft_ircd->_map.channel[*target_it],
    //                         msg.build_message_reply(*target_it),
    //                         &client);
    //        }
    //        else if (_ft_ircd->_map.client.count(*target_it))
    //            _ft_ircd->m_prepare_to_send(*_ft_ircd->_map.client[*target_it],
    //                                        msg.build_message_reply(*target_it));
    //        else if (msg.get_command() != "NOTICE")
    //            utils::push_message(client,
    //            msg.err_no_such_nick(*target_it));
    //    }
}

void
    IRCD::notice()
{
    //   m_privmsg(msg);
}

void
    IRCD::unknown()
{
    //   m_privmsg(msg);
}

void
    IRCD::unregistered()
{
    //   m_privmsg(msg);
}

IRCD::~IRCD()
{
}

IRCD::IRCD()
{
    _commands.push_back(&IRCD::empty);
    _commands.push_back(&IRCD::pass);
    _commands.push_back(&IRCD::nick);
    _commands.push_back(&IRCD::user);
    _commands.push_back(&IRCD::quit);
    _commands.push_back(&IRCD::join);
    _commands.push_back(&IRCD::part);
    _commands.push_back(&IRCD::topic);
    _commands.push_back(&IRCD::names);
    _commands.push_back(&IRCD::list);
    _commands.push_back(&IRCD::invite);
    _commands.push_back(&IRCD::kick);
    _commands.push_back(&IRCD::mode);
    _commands.push_back(&IRCD::privmsg);
    _commands.push_back(&IRCD::notice);
    _commands.push_back(&IRCD::unknown);
}

#include "../includes/ft_ircd.hpp"
#include "../includes/irc.hpp"

static CSTR_VECTOR
    split(const std::string& params, char delimiter)
{
    CSTR_VECTOR        splited;
    std::istringstream iss(params);
    std::string        element;

    while (std::getline(iss, element, delimiter))
        splited.push_back(element);
    return splited;
}

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
    split_by_comma(CSTR_VECTOR& splited_params, const std::string& params)
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

void
    IRCD::join()
{
    if (m_join(ONE) == ERROR)
        return;
    CSTR_VECTOR channels = split(_request->parameter[0], DELIMITER);
    IRCD::ITER  iter     = channels.begin();
    IRCD::ITER  end      = channels.end();
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
        m_names();
    }
}

RESULT
IRCD::m_mode(PHASE phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if (!_ft_ircd->_map.channel.count(*_target))
            return m_to_client(err_no_such_channel(*_target));
        _channel = _ft_ircd->_map.channel.at(*_target);
        if ((1 < _request->parameter.size())
            && !_channel->is_operator(*_client))
            return m_to_client(err_chanoprivs_needed(*_target));
    }
    else if (phase == THREE)
    {
        if (!_ft_ircd->_map.client.count(*_target))
            return m_to_client(err_no_such_nick(*_target));
        else if (*_target != _client->get_names().nick)
            return m_to_client(err_users_dont_match(
                _request->parameter.size() == 1 ? "view" : "change"));
        else if (_request->parameter.size() != 1)
            return m_to_client(err_u_mode_unknown_flag());
    }
    return OK;
}

void
    IRCD::mode()
{
    if (m_mode(ONE) == ERROR)
        return;
    _target = &_request->parameter[0];
    if (m_is_valid(CHANNEL_PREFIX))
    {
        if (m_mode(TWO) == ERROR)
            return;
        else if (_request->parameter.size() == 1)
            m_to_client(rpl_channel_mode_is(*_target));
        else
            m_to_client(err_unknown_mode(_request->parameter[1]));
    }
    else
    {
        if (m_mode(THREE) == ERROR)
            return;
        else if (_request->parameter.size() == 1)
            m_to_client(rpl_user_mode_is());
    }
}

RESULT
IRCD::m_invite()
{
    if (_request->parameter.size() < 2)
        return m_to_client(err_need_more_params());
    if (!_ft_ircd->_map.client.count(_request->parameter[0]))
        return m_to_client(err_no_such_nick(_request->parameter[0]));
    Client* target_client = _ft_ircd->_map.client[_request->parameter[0]];
    if (!_ft_ircd->_map.channel.count(_request->parameter[1]))
        return m_to_client(err_no_such_channel(_request->parameter[1]));
    _channel = _ft_ircd->_map.channel[_request->parameter[1]];
    if (!_client->is_joined(_channel))
        return m_to_client(err_not_on_channel(_request->parameter[1]));
    if (target_client->is_joined(_channel))
        return m_to_client(err_user_on_channel(_request->parameter[0],
                                               _request->parameter[1]));
    return OK;
}

void
    IRCD::invite()
{
    if (m_invite() == ERROR)
        return;
    m_to_client(rpl_inviting(_request->parameter[0], _request->parameter[1]));
    m_to_client(
        *_ft_ircd->_map.client[_request->parameter[0]],
        cmd_invite_reply(_request->parameter[0], _request->parameter[1]));
}

RESULT
IRCD::m_kick(PHASE phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.size() < 2)
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if ((!m_is_valid(CHANNEL_PREFIX)
             || !_ft_ircd->_map.channel.count(*_target)))
            return m_to_client(err_no_such_channel(*_target));

        _channel = _ft_ircd->_map.channel[*_target];

        if (!_channel->is_operator(*_client))
            return m_to_client(err_chanoprivs_needed(*_target));
        //

        if (!_ft_ircd->_map.client.count(*nick_name))
            return m_to_client(err_no_such_nick(*nick_name));

        _fixed = _ft_ircd->_map.client[*nick_name];

        if (!channel->is_joined(_fixed))
            return m_to_client(err_user_not_in_channel(*nick_name, *_target));
    }

    return OK;
}

void
    IRCD::kick()
{
    if (m_kick(ONE) == ERROR)
        return;

    CSTR_VECTOR param_0 = split(_request->parameter[0], ',');
    CSTR_VECTOR param_1 = split(_request->parameter[1], ',');

    if ((!(param_0.size() == 1 || param_1.size() == 1)
         && param_0.size() != param_1.size()))
    {
        m_to_client(err_need_more_params());
        return;
    }

    IRCD::ITER names = param_0.begin();
    IRCD::ITER nicks = param_1.begin();

    for (int i = 0, max = std::max(param_0.size(), param_1.size()); i < max;
         ++i)
    {
        _target = names.base();

        if (m_kick(TWO) == ERROR)
            goto next;

        _channel->part(*_fixed);
        if (_channel->is_empty())
        {
            _ft_ircd->_map.channel.erase(*names);
            delete _channel;
            m_to_client(
                cmd_kick_reply(*names, *nicks, _client->get_names().nick));
        }
        else
        {
            m_to_channel(
                cmd_kick_reply(*names, *nicks, _client->get_names().nick));
            m_to_client(*_fixed, cmd_kick_reply(*names, *nicks,
                                                _client->get_names().nick));
        }

    next:
        if (param_0.size() != 1)
            ++names;
        if (param_1.size() != 1)
            ++nicks;
    }
}

RESULT
IRCD::m_names()
{
    _channel            = _ft_ircd->_map.channel[*_target];
    _buffer             = "= " + _channel->get_name() + " :";
    Channel::CITER iter = _channel->get_members().begin();
    Channel::CITER end  = _channel->get_members().end();
    for (; iter != end; ++iter)
        _buffer.append(_channel->get_prefix(iter->first)
                       + iter->first->get_names().nick + " ");
    m_to_client(rpl_namereply(_buffer));
    m_to_client(rpl_endofnames(_channel->get_name()));
    _buffer.clear();
    return OK;
}

void
    IRCD::names()
{
    if (_request->parameter.empty())
    {
        IRC::CH_CITER ch_iter = _ft_ircd->_map.channel.begin();
        for (; ch_iter != _ft_ircd->_map.channel.end(); ++ch_iter)
        {
            _target = &ch_iter->first;
            m_names();
        }
        IRC::CL_CITER cl_iter = _ft_ircd->_map.client.begin();
        for (; cl_iter != _ft_ircd->_map.client.end(); ++cl_iter)
            if (cl_iter->second->get_channels().empty())
                _buffer.append(cl_iter->first + " ");
        if (_buffer.size())
            m_to_client(rpl_namereply("= * :" + _buffer));
        m_to_client(rpl_endofnames("*"));
        _buffer.clear();
        return;
    }
    else
    {
        CSTR_VECTOR channels = split(_request->parameter[0], ',');
        for (int i = 0; i < channels.size(); ++i)
        {
            _target = &channels[i];
            if (_ft_ircd->_map.channel.count(*_target))
                m_names();
            else
                m_to_client(rpl_endofnames(*_target));
        }
    }
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
    //        CSTR_VECTOR channel_list;
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
    //    CSTR_VECTOR channel_list;
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
    //    CSTR_VECTOR targets;
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
    //    privmsg();
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

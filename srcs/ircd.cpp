#include "../includes/ft_ircd.hpp"
#include "../includes/irc.hpp"

static IRCD::CSTR_VECTOR
    split(const std::string& params, char delimiter)
{
    IRCD::CSTR_VECTOR  splited;
    std::istringstream iss(params);
    std::string        element;

    while (std::getline(iss, element, delimiter))
        splited.push_back(element);
    return splited;
}

static inline bool
    is_special(char c)
{
    return std::memchr(SPECIALCHAR, c, 8);
}

TYPE
    IRCD::get_type(std::string command)
{
    if (_command_to_type.count(command))
        return _command_to_type[command];
    return UNKNOWN;
}

RESULT
IRCD::m_is_valid(TYPE type)
{
    if (type == NICK)
    {
        if (NICK_LENGTH_MAX < _target_0->length())
            return ERROR;
        if (!std::isalpha((*_target_0)[0]))
            return ERROR;
        for (size_t index = 1; index < _target_0->length(); ++index)
            if (!std::isalpha((*_target_0)[index])
                && !std::isdigit((*_target_0)[index])
                && !is_special((*_target_0)[index]))
                return ERROR;
    }
    if (type == CHANNEL_PREFIX)
    {
        if ((*_target_0)[0] != CHANNEL_PREFIX)
            return ERROR;
    }
    if (type == CHANNEL_NAME)
    {
        if (CHANNEL_LENGTH_MAX < _target_0->length())
            return ERROR;
        for (size_t index = 0; index < _target_0->length(); ++index)
            if (std::memchr(CHSTRING, (*_target_0)[index], 5))
                return ERROR;
    }
    return OK;
}

RESULT
IRCD::m_to_client(std::string str)
{
    _to_client->queue.push(str);
    return ERROR;
}

void
    IRCD::m_to_client(Client& client, const std::string& str)
{
    client.get_buffers().to_client.queue.push(str);
    _ft_ircd->toggle(client, EVFILT_READ);
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
    Client::CITER     iter = _client->get_channels().begin();
    Client::CITER     end  = _client->get_channels().end();
    std::set<Client*> check;

    for (_channel = *iter; iter != end; _channel = *(++iter))
    {
        Channel::CITER users = _channel->get_members().begin();
        Channel::CITER u_end = _channel->get_members().end();

        for (; users != u_end; ++users)
            if (!check.count(users->first) && users->first != _client)
            {
                check.insert(users->first);
                IRCD::m_to_client(*users->first, str);
            }
    }
}

void
    IRCD::registration()
{
    _ft_ircd->_map.client[_client->get_names().nick] = _client;
    m_to_client(rpl_welcome());
    Logger().info() << _client->get_names().nick << " is registered";
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

RESULT
IRCD::m_nick()
{
    if (_request->parameter.empty())
        return m_to_client(err_no_nickname_given());
    _target_0 = &_request->parameter[0];
    if (m_is_valid(NICK) == ERROR)
        return m_to_client(err_erroneus_nickname(*_target_0));
    if (_ft_ircd->_map.client.count(*_target_0))
    {
        if (*_target_0 != _client->get_names().nick)
            return m_to_client(err_nickname_in_use(*_target_0));
        return ERROR;
    }
    return OK;
}

void
    IRCD::nick()
{
    if (m_nick() == ERROR)
        return;

    if (_client->is_registered())
    {
        if (_client->get_channels().size())
            m_to_channels(cmd_nick_reply(*_target_0));
        _ft_ircd->_map.client.erase(_client->get_names().nick);
        _ft_ircd->_map.client[*_target_0] = _client;
    }
    _client->set_nickname(*_target_0);
    m_to_client(cmd_nick_reply(*_target_0));
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
            return m_to_client(err_no_such_channel(*_target_0));
        if (!m_is_valid(CHANNEL_NAME))
            return m_to_client(err_no_such_channel(*_target_0));
        if (CLIENT_CAHNNEL_LIMIT <= _client->get_channels().size())
            return m_to_client(err_too_many_channels(*_target_0));
    }
    else if (phase == THREE)
    {
        if (_client->is_joined(channel))
            return ERROR;
        else if (channel->is_full())
            return m_to_client(err_channel_is_full(*_target_0));
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
        _target_0 = iter.base();
        if (m_join(TWO) == ERROR)
            continue;
        if (!_ft_ircd->_map.channel.count(*_target_0))
            _ft_ircd->_map.channel.insert(
                std::make_pair(*_target_0, new Channel(*_target_0)));
        _channel = _ft_ircd->_map.channel[*_target_0];
        if (m_join(THREE, _channel) == ERROR)
            continue;
        _channel->join(*_client);
        if (_channel->get_members().size() == 1)
        {
            _channel->set_operator(_client);
            Logger().info() << "Create new channel :" << *_target_0 << " : @"
                            << _client->get_names().nick;
        }
        m_to_channel(cmd_join_reply(*_target_0));
        m_to_client(cmd_join_reply(*_target_0));
        if (_channel->get_topic().size())
            m_to_client(rpl_topic(_channel->get_name(), _channel->get_topic()));
        else
            m_to_client(rpl_notopic(_channel->get_name()));
        m_names();
    }
}

RESULT
IRCD::m_part(PHASE phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if (!_ft_ircd->_map.channel.count(*_target_0))
            return m_to_client(err_no_such_channel(*_target_0));
        _channel = _ft_ircd->_map.channel[*_target_0];
        if (!_channel->is_joined(_client))
            return m_to_client(err_not_on_channel(*_target_0));
    }
    return OK;
}

void
    IRCD::part()
{
    if (m_part(ONE) == ERROR)
        return;
    CSTR_VECTOR channels = split(_request->parameter[0], ',');
    for (int i = 0, size = channels.size(); i < size; ++i)
    {
        _target_0 = &channels[i];
        if (m_part(TWO) == ERROR)
            return;
        _channel->part(*_client);
        m_to_channel(cmd_part_reply(*_target_0));
        m_to_client(cmd_part_reply(*_target_0));
        if (_channel->is_empty())
        {
            _ft_ircd->_map.channel.erase(_channel->get_name());
            delete _channel;
        }
        Logger().debug() << "Remove [" << _client->get_names().nick
                         << "] client from [" << _channel->get_name()
                         << "] channel";
    }
}

RESULT
IRCD::m_topic()
{
    if (_request->parameter.empty())
        return m_to_client(err_need_more_params());
    _target_0 = &_request->parameter[0];
    if (!_ft_ircd->_map.channel.count(*_target_0))
        return m_to_client(err_no_such_channel(*_target_0));
    _channel = _ft_ircd->_map.channel[*_target_0];
    if (!_channel->is_joined(_client))
        return m_to_client(err_not_on_channel(*_target_0));
    if ((_request->parameter.size() > 1) && (!_channel->is_operator(*_client)))
        return m_to_client(err_chanoprivs_needed(*_target_0));
    return OK;
}

void
    IRCD::topic()
{
    if (m_topic() == ERROR)
        return;
    if (_request->parameter.size() == 1)
    {
        if (_channel->get_topic().size())
            m_to_client(rpl_topic(*_target_0, _channel->get_topic()));
        else
            m_to_client(rpl_notopic(*_target_0));
    }
    else
    {
        _channel->set_topic(_request->parameter[1]);
        Logger().trace() << *_target_0 << " channel topic change to "
                         << _channel->get_topic();
        m_to_client(cmd_topic_reply());
        m_to_channel(cmd_topic_reply());
    }
}

RESULT
IRCD::m_names()
{
    _channel            = _ft_ircd->_map.channel[*_target_0];
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
            _target_0 = &ch_iter->first;
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
            _target_0 = &channels[i];
            if (_ft_ircd->_map.channel.count(*_target_0))
                m_names();
            else
                m_to_client(rpl_endofnames(*_target_0));
        }
    }
}

RESULT
IRCD::m_list()
{
    m_to_client(rpl_list(_channel->get_name(),
                         std::to_string(_channel->get_members().size()),
                         _channel->get_topic()));
    return OK;
}

void
    IRCD::list()
{
    if (_request->parameter.empty())
    {
        IRC::CH_CITER iter = _ft_ircd->_map.channel.begin();
        for (_channel = iter->second; iter != _ft_ircd->_map.channel.end();
             _channel = (++iter)->second)
            m_list();
    }
    else if (_request->parameter.size() == 1)
    {
        CSTR_VECTOR channels = split(_request->parameter[0], ',');
        for (int i = 0, size = channels.size(); i < size; ++i)
            if (_ft_ircd->_map.channel.count(channels[i])
                && (_channel = _ft_ircd->_map.channel[channels[i]]))
                m_list();
            else
                m_to_client(err_no_such_channel(channels[i]));
    }
    m_to_client(rpl_listend());
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
             || !_ft_ircd->_map.channel.count(*_target_0)))
            return m_to_client(err_no_such_channel(*_target_0));
        _channel = _ft_ircd->_map.channel[*_target_0];
        if (!_channel->is_operator(*_client))
            return m_to_client(err_chanoprivs_needed(*_target_0));
        if (!_ft_ircd->_map.client.count(*_target_1))
            return m_to_client(err_no_such_nick(*_target_1));
        _fixed = _ft_ircd->_map.client[*_target_1];
        if (!_channel->is_joined(_fixed))
            return m_to_client(err_user_not_in_channel(*_target_1, *_target_0));
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
        _target_0 = names.base();
        _target_1 = nicks.base();
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
            m_to_client(
                cmd_kick_reply(*names, *nicks, _client->get_names().nick));
        }
    next:
        if (param_0.size() != 1)
            ++names;
        if (param_1.size() != 1)
            ++nicks;
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
        if (!_ft_ircd->_map.channel.count(*_target_0))
            return m_to_client(err_no_such_channel(*_target_0));
        _channel = _ft_ircd->_map.channel.at(*_target_0);
        if ((1 < _request->parameter.size())
            && !_channel->is_operator(*_client))
            return m_to_client(err_chanoprivs_needed(*_target_0));
    }
    else if (phase == THREE)
    {
        if (!_ft_ircd->_map.client.count(*_target_0))
            return m_to_client(err_no_such_nick(*_target_0));
        else if (*_target_0 != _client->get_names().nick)
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
    _target_0 = &_request->parameter[0];
    if (m_is_valid(CHANNEL_PREFIX))
    {
        if (m_mode(TWO) == ERROR)
            return;
        else if (_request->parameter.size() == 1)
            m_to_client(rpl_channel_mode_is(*_target_0));
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
IRCD::m_privmsg(PHASE phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_no_recipient());
        if (_request->parameter.size() == 1)
            return m_to_client(err_no_text_to_send());
    }
    return OK;
}

void
    IRCD::privmsg()
{
    if (m_privmsg(ONE) == ERROR)
        return;
    CSTR_VECTOR targets = split(_request->parameter.front(), ',');
    for (int i = 0, size = targets.size(); i < size; ++i)
    {
        _target_0 = &targets[i];
        if (m_is_valid(CHANNEL_PREFIX))
        {
            if (_ft_ircd->_map.channel.count(*_target_0)
                && (_channel = _ft_ircd->_map.channel[*_target_0]))
                m_to_channel(cmd_message_reply(*_target_0));
            else
                m_to_client(err_no_such_channel(*_target_0));
        }
        else if (_ft_ircd->_map.client.count(*_target_0))
            m_to_client(*_ft_ircd->_map.client[*_target_0],
                        cmd_message_reply(*_target_0));
        else if (_request->command != "NOTICE")
            m_to_client(err_no_such_nick(*_target_0));
    }
}

void
    IRCD::notice()
{
    privmsg();
}

void
    IRCD::unknown()
{
    m_to_client(err_unknown_command());
}

void
    IRCD::unregistered()
{
    m_to_client(err_not_registered());
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

#include "../../includes/ircd.hpp"

void
    IRCD::m_mode_sign(const char c)
{
    _channel->reserve_sign(c);
}

void
    IRCD::m_mode_valid(const char c)
{
    _channel->reserve_flags(c);
}

void
    IRCD::m_mode_invalid(const char c)
{
    if (_ascii[(int)c])
        return;
    m_to_client(err_unknown_mode(c));
    _ascii[(int)c] = true;
}

e_result
    IRCD::parse_flag(const std::string& flag)
{
    std::string result;

    for (_offset = flag.find_first_of("+-");
         (_index = flag.find_first_not_of("+-", _offset))
         != (int)std::string::npos;)
    {
        _channel->reserve_clear();
        _channel->reserve_sign(flag[_index - 1]);
        _offset = flag.find_first_of("+-", _index) != std::string::npos
                      ? flag.find_first_of("+-", _index)
                      : flag.size();
        for (int i = _index; i < _offset; ++i)
            if ((unsigned)flag[i] - 105 < 128)
                _channel->reserve_flags(flag[i]);
        if (_channel->is_reserved())
            _channel->set_status(result);
    }
    if (result.size())
        return m_to_client(cmd_mode_reply(_channel->get_name(), result));
    return OK;
}

e_result
    IRCD::m_mode(e_phase phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if (!_map.channel.count(*_target))
            return m_to_client(err_no_such_channel(*_target));
        _channel = _map.channel.at(*_target);
        if ((1 < _request->parameter.size()) && !_channel->is_operator(_client))
            return m_to_client(err_chanoprivs_needed(*_target));
    }
    else if (phase == THREE)
    {
        for (int i = 0, size = _request->parameter[1].size(); i < size; ++i)
            if ((unsigned)_request->parameter[1][i] - 32 < 127)
                (this->*IRCD::_modes[(int)_request->parameter[1][i]])(
                    _request->parameter[1][i]);
        std::memset((void*)_ascii, 0, sizeof(_ascii));
        if (!_channel->is_reserved())
            return ERROR;
    }
    else if (phase == FOUR)
    {
        if (!_map.client.count(*_target))
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
            m_to_client(rpl_channel_mode_is(*_target, _channel->get_status()));
        else if (m_mode(THREE) == ERROR)
            return;
        else if (!_channel->is_signed() || parse_flag(_request->parameter[1]))
            m_to_client(rpl_channel_mode_is(_channel->get_name(),
                                            _channel->get_status()));
    }
    else
    {
        if (m_mode(FOUR) == ERROR)
            return;
        else if (_request->parameter.size() == 1)
            m_to_client(rpl_user_mode_is());
    }
}

void
    IRCD::m_mode_initialize()
{
    for (int i = 0; i < 127; ++i)
        _modes[i] = &IRCD::m_mode_invalid;
    _modes[(int)'+'] = &IRCD::m_mode_sign;
    _modes[(int)'-'] = &IRCD::m_mode_sign;
    _modes[(int)'i'] = &IRCD::m_mode_valid;
    _modes[(int)'n'] = &IRCD::m_mode_valid;
    _modes[(int)'t'] = &IRCD::m_mode_valid;
    std::memset((void*)_ascii, 0, sizeof(_ascii));
}

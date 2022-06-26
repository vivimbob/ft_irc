#include "../../includes/ircd.hpp"

e_result
    IRCD::m_kick(e_phase phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.size() < 2)
            return m_to_client(err_need_more_params());
    }
    else if (phase == TWO)
    {
        if ((!m_is_valid(CHANNEL_PREFIX) || !_map.channel.count(*_target)))
            return m_to_client(err_no_such_channel(*_target));
        _channel = _map.channel[*_target];
        if (!_channel->is_operator(_client))
            return m_to_client(err_chanoprivs_needed(*_target));
        if (!_map.client.count(*_target_sub))
            return m_to_client(err_no_such_nick(*_target_sub));
        _fixed = _map.client[*_target_sub];
        if (!_channel->is_joined(_fixed))
            return m_to_client(err_user_not_in_channel(*_target_sub, *_target));
    }
    return OK;
}

void
    IRCD::kick()
{
    if (m_kick(ONE) == ERROR)
        return;
    t_cstr_vector param_0 = split(_request->parameter[0], ',');
    t_cstr_vector param_1 = split(_request->parameter[1], ',');
    if ((!(param_0.size() == 1 || param_1.size() == 1)
         && param_0.size() != param_1.size()))
    {
        m_to_client(err_need_more_params());
        return;
    }
    IRCD::t_iter names = param_0.begin();
    IRCD::t_iter nicks = param_1.begin();
    for (int i = 0, max = std::max(param_0.size(), param_1.size()); i < max;
         ++i)
    {
        _target     = names.base();
        _target_sub = nicks.base();
        if (m_kick(TWO) == ERROR)
            goto next;
        _channel->part(_fixed);
        if (_channel->is_empty())
        {
            _map.channel.erase(*names);
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

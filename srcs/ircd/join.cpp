#include "../../includes/ircd.hpp"

e_result
    IRCD::m_join(e_phase phase, Channel* channel)
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
        if (CLIENT_CAHNNEL_MAX <= _client->get_channels().size())
            return m_to_client(err_too_many_channels(*_target));
    }
    else if (phase == THREE)
    {
        if (_client->is_joined(channel))
            return ERROR;
        else if (channel->is_full())
            return m_to_client(err_channel_is_full(*_target));
        else if ((_channel->get_status(INVITE))
                 && (!_channel->is_invited(_client)))
            return m_to_client(err_invite_only_channel(_channel->get_name()));
    }
    return OK;
}

void
    IRCD::join()
{
    if (m_join(ONE) == ERROR)
        return;
    t_cstr_vector channels = split(_request->parameter[0], DELIMITER);
    IRCD::t_iter  iter     = channels.begin();
    IRCD::t_iter  end      = channels.end();
    for (; iter != end; ++iter)
    {
        _target = iter.base();
        if (m_join(TWO) == ERROR)
            continue;
        if (!_map.channel.count(*_target))
        {
            _map.channel.insert(
                std::make_pair(*_target, new Channel(*_target, _client)));
            _channel = _map.channel[*_target];
        }
        else
        {
            _channel = _map.channel[*_target];
            if (m_join(THREE, _channel) == ERROR)
                continue;
            _channel->join(_client);
        }
        m_to_channel(cmd_join_reply(*_target));
        m_to_client(cmd_join_reply(*_target));
        if (_channel->get_topic().size())
            m_to_client(rpl_topic(_channel->get_name(), _channel->get_topic()));
        else
            m_to_client(rpl_notopic(_channel->get_name()));
        m_names();
    }
}

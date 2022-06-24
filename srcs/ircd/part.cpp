#include "../../includes/ircd.hpp"

e_result
    IRCD::m_part(e_phase phase)
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
        _channel = _map.channel[*_target];
        if (!_channel->is_joined(_client))
            return m_to_client(err_not_on_channel(*_target));
    }
    return OK;
}

void
    IRCD::part()
{
    if (m_part(ONE) == ERROR)
        return;
    t_cstr_vector channels = split(_request->parameter[0], ',');
    for (int i = 0, size = channels.size(); i < size; ++i)
    {
        _target = &channels[i];
        if (m_part(TWO) == ERROR)
            return;
        _channel->part(_client);
        m_to_channel(cmd_part_reply(*_target));
        m_to_client(cmd_part_reply(*_target));
        if (_channel->is_empty())
        {
            _map.channel.erase(_channel->get_name());
            delete _channel;
        }
        log::print() << "remove " << _client->get_names().nick
                     << " client from " << _channel->get_name() << " channel"
                     << log::endl;
    }
}

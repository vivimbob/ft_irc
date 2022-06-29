#include "../../includes/ircd.hpp"

e_result
    IRCD::m_privmsg(e_phase phase)
{
    if (phase == ONE)
    {
        if (_request->parameter.empty())
            return m_to_client(err_no_recipient());
        if (_request->parameter.size() == 1)
            return m_to_client(err_no_text_to_send());
    }
    if (phase == TWO)
    {
        if (!_map.channel.count(*_target))
            return m_to_client(err_no_such_channel(*_target));
        _channel = _map.channel[*_target];
        if (_channel->get_status(NOMSG) && !_channel->is_joined(_client))
            return m_to_client(
                err_cannot_send_to_channel(_channel->get_name(), 'n'));
    }
    return OK;
}

void
    IRCD::privmsg()
{
    if (m_privmsg(ONE) == ERROR)
        return;
    t_cstr_vector targets = split(_request->parameter.front(), ',');
    for (int i = 0, size = targets.size(); i < size; ++i)
    {
        _target = &targets[i];
        if (m_is_valid(CHANNEL_PREFIX))
        {
            if (m_privmsg(TWO) == OK)
                m_to_channel(cmd_message_reply(*_target));
        }
        else if (_map.client.count(*_target))
            m_to_client(*_map.client[*_target], cmd_message_reply(*_target));
        else if (_request->type == PRIVMSG)
            m_to_client(err_no_such_nick(*_target));
    }
}

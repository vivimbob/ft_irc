#include "../../includes/ircd.hpp"

e_result
    IRCD::m_topic()
{
    if (_request->parameter.empty())
        return m_to_client(err_need_more_params());
    _target = &_request->parameter[0];
    if (!_map.channel.count(*_target))
        return m_to_client(err_no_such_channel(*_target));
    _channel = _map.channel[*_target];
    if (!_channel->is_joined(_client))
        return m_to_client(err_not_on_channel(*_target));
    if ((1 < _request->parameter.size()) && (_channel->get_status(TOPIC))
        && (!_channel->is_operator(_client)))
        return m_to_client(err_chanoprivs_needed(*_target));
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
            m_to_client(rpl_topic(*_target, _channel->get_topic()));
        else
            m_to_client(rpl_notopic(*_target));
    }
    else
    {
        _channel->set_topic(_request->parameter[1]);
        log::print() << *_target << " channel topic: " << _channel->get_topic()
                     << log::endl;
        m_to_client(cmd_topic_reply());
        m_to_channel(cmd_topic_reply());
    }
}

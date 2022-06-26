#include "../../includes/ircd.hpp"

e_result
    IRCD::m_invite()
{
    if (_request->parameter.size() < 2)
        return m_to_client(err_need_more_params());
    if (!_map.client.count(_request->parameter[0]))
        return m_to_client(err_no_such_nick(_request->parameter[0]));
    _fixed = _map.client[_request->parameter[0]];
    if (!_map.channel.count(_request->parameter[1]))
        return m_to_client(err_no_such_channel(_request->parameter[1]));
    _channel = _map.channel[_request->parameter[1]];
    if (!_client->is_joined(_channel))
        return m_to_client(err_not_on_channel(_request->parameter[1]));
    if (_fixed->is_joined(_channel))
        return m_to_client(err_user_on_channel(_request->parameter[0],
                                               _request->parameter[1]));
    if ((_channel->get_status(INVITE)) && !(_channel->is_operator(_client)))
        return m_to_client(err_chanoprivs_needed(_channel->get_name()));
    return OK;
}

void
    IRCD::invite()
{
    if (m_invite() == ERROR)
        return;
    _channel->invitation(_fixed);
    m_to_client(rpl_inviting(_request->parameter[0], _request->parameter[1]));
    m_to_client(*_fixed, cmd_invite_reply(_request->parameter[0],
                                          _request->parameter[1]));
}

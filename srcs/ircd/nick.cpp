#include "../../includes/ircd.hpp"

e_result
    IRCD::m_nick()
{
    if (_request->parameter.empty())
        return m_to_client(err_no_nickname_given());
    _target = &_request->parameter[0];
    if (m_is_valid(NICK) == ERROR)
        return m_to_client(err_erroneus_nickname(*_target));
    if (_map.client.count(*_target))
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

    if (_client->is_registered())
    {
        if (_client->get_channels().size())
            m_to_channels(cmd_nick_reply(*_target));
        _map.client.erase(_client->get_names().nick);
        _map.client[*_target] = _client;
        m_to_client(cmd_nick_reply(*_target));
    }
    _client->set_nickname(*_target);
    log::print() << "fd " << _client->get_fd()
                 << " client nick: " << _client->get_names().nick << log::endl;
}

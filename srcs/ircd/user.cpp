#include "../../includes/ircd.hpp"

e_result
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

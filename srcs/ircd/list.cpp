#include "../../includes/ircd.hpp"

e_result
    IRCD::m_list()
{
	int member_number = _channel->get_members().size()
		+ _channel->get_operator() == nullptr ? 0 : 1;
    m_to_client(rpl_list(_channel->get_name(),
                         std::to_string(member_number),
                         _channel->get_topic()));
    return OK;
}

void
    IRCD::list()
{
    if (_request->parameter.empty())
    {
        IRC::t_iter_ch iter = _map.channel.begin();
        for (_channel = iter->second; iter != _map.channel.end();
             _channel = (++iter)->second)
            m_list();
    }
    else if (_request->parameter.size() == 1)
    {
        t_cstr_vector channels = split(_request->parameter[0], ',');
        for (int i = 0, size = channels.size(); i < size; ++i)
            if (_map.channel.count(channels[i])
                && (_channel = _map.channel[channels[i]]))
                m_list();
            else
                m_to_client(err_no_such_channel(channels[i]));
    }
    m_to_client(rpl_listend());
}

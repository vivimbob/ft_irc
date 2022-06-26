#include "../../includes/ircd.hpp"

e_result
    IRCD::m_names()
{
    _channel = _map.channel[*_target];
    _buffer  = "= " + _channel->get_name() + " :";
    if (_channel->get_operator())
        _buffer.append("@" + _channel->get_operator()->get_names().nick + " ");
    Channel::t_citer_member iter = _channel->get_members().begin();
    Channel::t_citer_member end  = _channel->get_members().end();
    for (; iter != end; ++iter)
        _buffer.append((*iter)->get_names().nick + " ");
    m_to_client(rpl_namereply(_buffer));
    m_to_client(rpl_endofnames(_channel->get_name()));
    _buffer.clear();
    return OK;
}

void
    IRCD::names()
{
    if (_request->parameter.empty())
    {
        IRC::t_iter_ch ch_iter = _map.channel.begin();
        for (; ch_iter != _map.channel.end(); ++ch_iter)
        {
            _target = &ch_iter->first;
            m_names();
        }
        IRC::t_iter_cl cl_iter = _map.client.begin();
        for (; cl_iter != _map.client.end(); ++cl_iter)
            if (cl_iter->second->get_channels().empty())
                _buffer.append(cl_iter->first + " ");
        if (_buffer.size())
            m_to_client(rpl_namereply("= * :" + _buffer));
        m_to_client(rpl_endofnames("*"));
        _buffer.clear();
        return;
    }
    else
    {
        t_cstr_vector channels = split(_request->parameter[0], ',');
        for (int i = 0, size = channels.size(); i < size; ++i)
        {
            _target = &channels[i];
            if (_map.channel.count(*_target))
                m_names();
            else
                m_to_client(rpl_endofnames(*_target));
        }
    }
}

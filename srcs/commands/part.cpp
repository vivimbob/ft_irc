#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::process_part_command(Client &client, IRCMessage &msg)
{
		if (msg.get_params().size() < 1 || msg.get_params().size() > 2)
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }
    
    std::vector<const std::string> splited_channel;
    utils::split_by_comma(splited_channel, msg.get_params()[0]);
    
    std::vector<const std::string>::iterator it = splited_channel.begin();
    for (; it != splited_channel.end(); ++it)
    {
        if (!m_channel_map.count(*it) || !utils::is_channel_prefix(*it) || !utils::is_channel_name_valid(*it))
        {
            client.push_message(msg.err_no_such_channel(*it), Logger::Debug);
            return ;
        }
        if (m_channel_map.count(*it) && !m_channel_map[*it]->m_get_user_lists().count(&client))
        {
            client.push_message(msg.err_not_on_channel(*it), Logger::Debug);
            return ;
        }
    }
    it = splited_channel.begin();
    for (; it != splited_channel.end(); ++it)
    {
        m_channel_map[*it]->m_delete_user(client);
        client.m_chan_key_lists.erase(*it);
        Logger().trace() << "Remove [" << client.m_get_nickname() << "] client from [" << m_channel_map[*it]->m_get_channel_name() << "] channel";
        send_to_channel(m_channel_map[*it], msg.build_message());
    }
}

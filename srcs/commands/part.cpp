#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_part_command(Client &client, IRCMessage &msg)
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
        if (!m_channel_map.count(*it))
        {
            client.push_message(msg.err_no_such_channel(*it), Logger::Debug);
            return ;
        }
        if (!m_channel_map[*it]->is_user_on_channel(&client))
        {
            client.push_message(msg.err_not_on_channel(*it), Logger::Debug);
            return ;
        }
    }
    it = splited_channel.begin();
    for (; it != splited_channel.end(); ++it)
    {
        m_channel_map[*it]->delete_user(client);
		client.erase_channel(*it);
        Logger().trace() << "Remove [" << client.get_nickname() << "] client from [" << m_channel_map[*it]->get_channel_name() << "] channel";
        m_send_to_channel(m_channel_map[*it], msg.build_part_reply());
    }
}

#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_names_command(Client& client, Message& msg)
{
    std::queue<const std::string> nick_queue;
    if (msg.get_params().empty())
    {
        ChannelMap::const_iterator channel_it = m_channel_map.begin();
        for (; channel_it != m_channel_map.end(); ++channel_it)
            utils::send_name_reply(channel_it->second, client, msg);

        // 클라이언트가 어느 채널에도 속하지 않을 때
        ClientMap::const_iterator client_it = m_client_map.begin();
        for (; client_it != m_client_map.end(); ++client_it)
            if (client_it->second->get_channel_list().empty())
                nick_queue.push(client_it->first);

        if (nick_queue.size())
            client.push_message(msg.rpl_namreply("*", nick_queue));
        client.push_message(msg.rpl_endofnames("*"));
        return;
    }
    else
    {
        std::vector<const std::string> channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        for (int i = 0, size = channel_list.size(); i < size; ++i)
        {
            // 잘못된 채널일 때
            if (!m_channel_map.count(channel_list[i]))
            {
                client.push_message(msg.rpl_endofnames(channel_list[i]));
                continue;
            }

            utils::send_name_reply(m_channel_map[channel_list[i]], client, msg);
        }
    }
}

#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void
    send_list_to_client(Channel* channel, Client& client, Message& msg)
{
    client.push_message(
        msg.rpl_list(channel->get_channel_name(),
                     std::to_string(channel->get_user_list().size()),
                     channel->get_channel_topic()));
}

void
    Server::m_process_list_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        ChannelMap::const_iterator channel_it = m_channel_map.begin();
        for (; channel_it != m_channel_map.end(); ++channel_it)
            send_list_to_client(channel_it->second, client, msg);
    }

    else if (msg.get_params().size() == 1)
    {
        std::vector<const std::string> channel_list;
        utils::split_by_comma(channel_list, msg.get_params()[0]);

        std::vector<const std::string>::iterator channel_it =
            channel_list.begin();
        for (; channel_it != channel_list.end(); ++channel_it)
            if (m_channel_map.count(*channel_it))
                send_list_to_client(m_channel_map[*channel_it], client, msg);
    }
    client.push_message(msg.rpl_listend());
}

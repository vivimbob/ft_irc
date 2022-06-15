#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_part_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    std::vector<const std::string> channel_list;
    utils::split_by_comma(channel_list, msg.get_params()[0]);

    std::vector<const std::string>::iterator channel_it = channel_list.begin();
    for (; channel_it != channel_list.end(); ++channel_it)
    {
        if (!m_channel_map.count(*channel_it))
        {
            client.push_message(msg.err_no_such_channel(*channel_it),
                                Logger::Debug);
            return;
        }
        Channel* channel = m_channel_map[*channel_it];
        if (!channel->is_user_on_channel(&client))
        {
            client.push_message(msg.err_not_on_channel(*channel_it),
                                Logger::Debug);
            return;
        }
        m_send_to_channel(channel, msg.build_part_reply(*channel_it));
        channel->delete_user(client);
        client.erase_channel(m_channel_map[*channel_it]);
        if (channel->is_empty())
            m_channel_map.erase(channel->get_channel_name());
        Logger().debug() << "Remove [" << client.get_nickname()
                         << "] client from [" << channel->get_channel_name()
                         << "] channel";
    }
}

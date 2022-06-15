#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"

void
    Server::m_process_topic_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    const std::string& channel_name = msg.get_params()[0];

    if (!m_channel_map.count(channel_name))
    {
        client.push_message(msg.err_no_such_channel(channel_name),
                            Logger::Debug);
        return;
    }

    Channel* channel = m_channel_map[channel_name];

    if (!channel->is_user_on_channel(&client))
    {
        client.push_message(msg.err_not_on_channel(channel_name),
                            Logger::Debug);
        return;
    }

    if (msg.get_params().size() == 1)
    {
        utils::send_topic_reply(channel, client, msg);
        return;
    }

    if (!channel->is_operator(client))
    {
        client.push_message(msg.err_chanoprivs_needed(channel_name),
                            Logger::Debug);
        return;
    }
    channel->set_channel_topic(msg.get_params()[1]);

    Logger().trace() << channel_name << " channel topic change to "
                     << channel->get_channel_topic();

    m_send_to_channel(channel, msg.build_topic_reply());
}

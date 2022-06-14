#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"

void
    Server::m_process_channel_mode_command(Client&            client,
                                           Message&           msg,
                                           const std::string& channel_name)
{
    if (!m_channel_map.count(channel_name))
    {
        client.push_message(msg.err_no_such_channel(channel_name),
                            Logger::Debug);
        return;
    }
    Channel* channel = m_channel_map.at(channel_name);
    if (msg.get_params().size() == 1)
    {
        client.push_message(msg.rpl_channel_mode_is(channel_name),
                            Logger::Debug);
        return;
    }
    if (!channel->is_operator(client))
    {
        client.push_message(msg.err_chanoprivs_needed(channel_name),
                            Logger::Debug);
        return;
    }

    client.push_message(msg.err_unknown_mode(msg.get_params()[1]),
                        Logger::Debug);
}

void
    Server::m_process_user_mode_command(Client&            client,
                                        Message&           msg,
                                        const std::string& nickname)
{
    if (nickname != client.get_nickname())
    {
        client.push_message(msg.err_users_dont_match(), Logger::Debug);
        return;
    }

    if (msg.get_params().size() == 1)
    {
        client.push_message(msg.rpl_user_mode_is(), Logger::Debug);
        return;
    }
    client.push_message(msg.err_u_mode_unknown_flag(), Logger::Debug);
}

void
    Server::m_process_mode_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    const std::string& target = msg.get_params()[0];

    if (utils::is_channel_prefix(target))
        m_process_channel_mode_command(client, msg, target);
    else
        m_process_user_mode_command(client, msg, target);
}

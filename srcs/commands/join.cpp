#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"
#include <utility>

void
    Server::m_process_join_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    std::vector<const std::string> channel_list;

    utils::split_by_comma(channel_list, msg.get_params()[0]);

    for (std::vector<const std::string>::iterator channel_it =
             channel_list.begin();
         channel_it != channel_list.end();
         ++channel_it) // 채널과 키쌍을 순회하면서 확인
    {
        Channel*           channel      = NULL;
        const std::string& channel_name = *channel_it;

        if (m_channel_map.count(channel_name))
            channel = m_channel_map[channel_name];

        if (!utils::is_channel_prefix(channel_name) ||
            !utils::is_channel_name_valid(channel_name))
        // 채널이름 앞에 #, & 있는지, 이름이 유효한지
        {
            client.push_message(msg.err_no_such_channel(channel_name),
                                Logger::Debug);
            continue;
        }

        else if (!channel) // join할 채널이 없는 경우(새로 만듦)
        {
            if (!client.is_join_available()) // join할 클라이언트가 이미 참여할
                                             // 수 있는 채널 갯수에 도달했을때
            {
                client.push_message(msg.err_too_many_channels(channel_name),
                                    Logger::Debug);
                continue;
            }
            channel = new Channel(channel_name);
            m_channel_map.insert(std::make_pair(channel_name, channel));
            channel->add_user(client);
            channel->set_operator_flag(true, &client);
            client.insert_channel(channel);
        }
        else // join할 채널이 존재하는 경우
        {
            if (client.is_already_joined(channel)) // 이미 join된 경우
                continue;
            if (channel->is_full()) // 현재 채널이 포함할 수 있는 최대 유저 수에
                                    // 도달했을 때
            {
                client.push_message(msg.err_channel_is_full(channel_name),
                                    Logger::Debug);
                continue;
            }
            channel->add_user(client);
            client.insert_channel(channel);
        }

        Logger().info() << "Create new channel :" << channel_name << " : @"
                        << client.get_nickname();
        m_send_to_channel(channel, msg.build_join_reply(channel_name));
        utils::send_topic_reply(channel, client, msg);
        utils::send_name_reply(channel, client, msg);
    }
}

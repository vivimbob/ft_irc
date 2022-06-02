#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"
#include <utility>

void
    Server::m_join_channel(Client &client, IRCMessage &msg, ChannelKeyPairMap &chan_key_pair)
{
    ChannelKeyPairMap::iterator channel_key_it = chan_key_pair.begin();
    for (; channel_key_it != chan_key_pair.end(); ++channel_key_it) // 채널과 키쌍을 순회하면서 확인
    {
		Channel *channel = m_channel_map[channel_key_it->first];
		const std::string &channel_name = channel_key_it->first;
		const std::string &key = channel_key_it->second;
        if (!utils::is_channel_prefix(channel_name) || !utils::is_channel_name_valid(channel_name)) // 채널이름 앞에 #, & 있는지, 이름이 유효한지
        {
            client.push_message(msg.err_bad_chan_mask(channel_name), Logger::Debug);
            return ;
        }

        ChannelMap::iterator map_it = m_channel_map.find(channel_name);
        if ((map_it != m_channel_map.end()) && ((channel->is_key_mode()) && (key != map_it->second->get_key()))) // join할 채널 이름은 있는데 키가 안 맞는 경우
        {
            client.push_message(msg.err_bad_channel_key(channel_name), Logger::Debug);
            return ;
        }
        else if (map_it == m_channel_map.end()) // join할 채널이 없는 경우(새로 만듦)
        {
            if (client.m_chan_key_lists.size() >= client.m_channel_limits) //join할 클라이언트가 이미 참여할 수 있는 채널 갯수에 도달했을때
            {
                client.push_message(msg.err_too_many_channels(channel_name), Logger::Debug);
                return ;
            }
            m_channel_map.insert(std::make_pair(channel_name, new Channel(channel_name, key)));
            channel->add_user(client);
            channel->add_operator(client);
            client.m_chan_key_lists.insert(std::make_pair(channel_name, key));
            Logger().info() << "Create new channel :" << channel_name << " with " << key << " key by " << client.get_nickname();
        }
        else // join할 채널이 존재하는 경우
        {
            size_t temp_channel_users = channel->get_user_lists().size();
            if (channel->is_limit_mode() && (temp_channel_users >= channel->get_user_limits())) // 현재 채널이 포함할 수 있는 최대 유저 수에 도달했을 때
            {
                client.push_message(msg.err_channel_is_full(channel_name), Logger::Debug);
                return ;
            }
            if (channel->is_invite_only_mode()) // invite-only인 경우
            {
                client.push_message(msg.err_invite_only_chan(channel_name), Logger::Debug);
                return ;
            }
            if (client.m_chan_key_lists.count(channel_name)) // 이미 join된 경우
            {
                client.push_message(":You have already joined in <" + channel_name + "> channel\r\n", Logger::Debug);
                return ;
            }
            if ((channel->is_key_mode()) && (key != map_it->second->get_key())) // key mode인데 key가 안 맞을 때
            {
                client.push_message(msg.err_bad_channel_key(channel_name), Logger::Debug);
                return ;
            }
            channel->add_user(client);
            client.m_chan_key_lists.insert(std::make_pair(channel_name, key));
            Logger().info() << "Join channel :" << channel_name << " with " << key << " key by " << client.get_nickname();
        }
        const Channel::MemberMap &user_list = channel->get_user_lists();
        Channel::MemberMap::const_iterator user = user_list.begin();
        std::queue<const std::string> temp_nick_queue;
        for (; user != user_list.end(); ++user)
            temp_nick_queue.push(user->first->get_nickname());
        m_prepare_to_send(client, msg.rpl_namreply(channel_name, temp_nick_queue));
        Logger().trace() << client.get_nickname() << " [" << msg.rpl_namreply(channel_name, temp_nick_queue) << ']';

	    std::string reply_msg;
	    const std::string& channel_topic = channel->get_channel_topic();

    	if (channel_topic.empty())
    		reply_msg = msg.rpl_notopic(channel_name);
    	else
    		reply_msg = msg.rpl_topic(channel_name, channel_topic); 
    
        client.push_message(reply_msg, Logger::Debug);
	}
}

void
    Server::m_process_join_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() < 1)
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }

    ChannelKeyPairMap channel_key_pair;
    std::vector<const std::string> splited_channel;

    utils::split_by_comma(splited_channel, msg.get_params()[0]);

    if (msg.get_params().size() == 2) // key가 있을 때
    {
        std::vector<const std::string> splited_key;
        utils::split_by_comma(splited_key, msg.get_params()[1]);
        if ((splited_channel.size() < splited_key.size())) // key개수가 채널 개수보다 많을 때
        {
            client.push_message(msg.err_bad_channel_key(msg.get_command()), Logger::Debug);
            return ;
        }
        std::vector<const std::string>::iterator itc = splited_channel.begin();
        std::vector<const std::string>::iterator itk = splited_key.begin();
        for (; itc != splited_channel.end(); ++itc)
        {
            if (itk == splited_key.end())
            {
                channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, ""));
                continue;
            }
            else
            {
                channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, *itk));
                ++itk;
            }
        }
    }
    else // key가 없을 때
    {
        std::vector<const std::string>::iterator itc = splited_channel.begin();
        for (; itc != splited_channel.end(); ++itc)
          channel_key_pair.insert(std::make_pair<const std::string, const std::string>(*itc, ""));
    }
    m_join_channel(client, msg, channel_key_pair);
}

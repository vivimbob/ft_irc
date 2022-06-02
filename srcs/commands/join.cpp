#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"
#include <utility>

void
    Server::m_join_channel(Client &client, IRCMessage &msg, ChannelKeyPairMap &chan_key_pair)
{
    ChannelKeyPairMap::iterator pair_it = chan_key_pair.begin();
    for (; pair_it != chan_key_pair.end(); ++pair_it) // 채널과 키쌍을 순회하면서 확인
    {
        if (!utils::is_channel_prefix(pair_it->first) || !utils::is_channel_name_valid(pair_it->first)) // 채널이름 앞에 #, & 있는지, 이름이 유효한지
        {
            client.push_message(msg.err_bad_chan_mask(pair_it->first), Logger::Debug);
            return ;
        }

        ChannelMap::iterator map_it = m_channel_map.find(pair_it->first);
        if ((map_it != m_channel_map.end()) && ((m_channel_map[pair_it->first]->is_key_mode()) && (pair_it->second != map_it->second->get_key()))) // join할 채널 이름은 있는데 키가 안 맞는 경우
        {
            client.push_message(msg.err_bad_channel_key(pair_it->first), Logger::Debug);
            return ;
        }
        else if (map_it == m_channel_map.end()) // join할 채널이 없는 경우(새로 만듦)
        {
            if (client.m_chan_key_lists.size() >= client.m_channel_limits) //join할 클라이언트가 이미 참여할 수 있는 채널 갯수에 도달했을때
            {
                client.push_message(msg.err_too_many_channels(pair_it->first), Logger::Debug);
                return ;
            }
            m_channel_map.insert(std::make_pair(pair_it->first, new Channel(pair_it->first, pair_it->second)));
            m_channel_map[pair_it->first]->add_user(client);
            m_channel_map[pair_it->first]->add_operator(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().info() << "Create new channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.get_nickname();
        }
        else // join할 채널이 존재하는 경우
        {
            size_t temp_channel_users = m_channel_map[pair_it->first]->get_user_lists().size();
            if (m_channel_map[pair_it->first]->is_limit_mode() && (temp_channel_users >= m_channel_map[pair_it->first]->get_user_limits())) // 현재 채널이 포함할 수 있는 최대 유저 수에 도달했을 때
            {
                client.push_message(msg.err_channel_is_full(pair_it->first), Logger::Debug);
                return ;
            }
            if (m_channel_map[pair_it->first]->is_invite_only_mode()) // invite-only인 경우
            {
                client.push_message(msg.err_invite_only_chan(pair_it->first), Logger::Debug);
                return ;
            }
            if (client.m_chan_key_lists.count(pair_it->first)) // 이미 join된 경우
            {
                client.push_message(":You have already joined in <" + pair_it->first + "> channel\r\n", Logger::Debug);
                return ;
            }
            if ((m_channel_map[pair_it->first]->is_key_mode()) && (pair_it->second != map_it->second->get_key())) // key mode인데 key가 안 맞을 때
            {
                client.push_message(msg.err_bad_channel_key(pair_it->first), Logger::Debug);
                return ;
            }
            m_channel_map[pair_it->first]->add_user(client);
            client.m_chan_key_lists.insert(std::make_pair(pair_it->first, pair_it->second));
            Logger().info() << "Join channel :" << pair_it->first << " with " << pair_it->second << " key by " << client.get_nickname();
        }
        const Channel::MemberMap &user_list = m_channel_map[pair_it->first]->get_user_lists();
        Channel::MemberMap::const_iterator user = user_list.begin();
        std::queue<const std::string> temp_nick_queue;
        for (; user != user_list.end(); ++user)
            temp_nick_queue.push(user->first->get_nickname());
        m_prepare_to_send(client, msg.rpl_namreply(pair_it->first, temp_nick_queue));
        Logger().trace() << client.get_nickname() << " [" << msg.rpl_namreply(pair_it->first, temp_nick_queue) << ']';

	    std::string reply_msg;
	    const std::string& channel_topic = m_channel_map[pair_it->first]->get_channel_topic();

    	if (channel_topic.empty())
    		reply_msg = msg.rpl_notopic(pair_it->first);
    	else
    		reply_msg = msg.rpl_topic(pair_it->first, channel_topic); 
    
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

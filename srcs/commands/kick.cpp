#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_kick_command(Client &client, Message &msg)
{
	const std::vector<std::string> &parameter = msg.get_params();
	if (parameter.size() < 2)
	{
		client.push_message(msg.err_need_more_params(), Logger::Debug);
		return ;
	}

	std::vector<const std::string> channel_list;
	std::vector<const std::string> nick_list;

	utils::split_by_comma(channel_list, parameter[0]);
	utils::split_by_comma(nick_list, parameter[1]);

	if (channel_list.size() > 1 && channel_list.size() != nick_list.size())
	{
		client.push_message(msg.err_need_more_params(), Logger::Debug);
		return ;
	}

	Channel *channel;

	if (channel_list.size() == 1)
	{
		const std::string &channel_name = channel_list[0];
		
		if (!utils::is_channel_prefix(channel_name))
		{
			client.push_message(msg.err_bad_chan_mask(channel_name), Logger::Debug);
			return;
		}
		if (!m_channel_map.count(channel_name))
		{
			client.push_message(msg.err_no_such_channel(channel_name), Logger::Debug);
			return;
		}

		channel = m_channel_map[channel_name];

		if (!channel->is_user_on_channel(&client))
		{
			client.push_message(msg.err_not_on_channel(channel_name), Logger::Debug);
			return;
		}
		if (!channel->is_operator(client))
		{
			client.push_message(msg.err_chanoprivs_needed(channel_name), Logger::Debug);
			return;
		}

		std::vector<const std::string>::iterator nick_it = nick_list.begin();
		std::vector<const std::string>::iterator nick_ite = nick_list.end();

		for (;nick_it != nick_ite; ++nick_it)
		{
			const std::string &nick = *nick_it;

			if (!m_client_map.count(nick))
			{
				client.push_message(msg.err_user_not_in_channel(nick, channel_name), Logger::Debug);
				continue;
			}
			Client *target_client = m_client_map[nick];

			if (channel->is_user_on_channel(target_client))
			{
				client.push_message(msg.err_user_not_in_channel(nick, channel_name), Logger::Debug);
				continue;
			}
			m_send_to_channel(channel, msg.build_kick_reply(channel_name, nick));
			channel->delete_user(*target_client);
			target_client->erase_channel(channel);
		}
	}
	else
	{
		std::vector<const std::string>::iterator nick_it = nick_list.begin();
		std::vector<const std::string>::iterator nick_ite = nick_list.end();
		std::vector<const std::string>::iterator channel_it = channel_list.begin();

		for (;nick_it != nick_ite; ++nick_it, ++channel_it)
		{
			const std::string &channel_name = *channel_it;
			const std::string &nick = *nick_it;
			
			if (!utils::is_channel_prefix(channel_name))
			{
				client.push_message(msg.err_bad_chan_mask(channel_name), Logger::Debug);
				continue;
			}

			if (!m_channel_map.count(channel_name))
			{
				client.push_message(msg.err_no_such_channel(channel_name), Logger::Debug);
				continue;
			}

			channel = m_channel_map[channel_name];

			if (!channel->is_user_on_channel(&client))
			{
				client.push_message(msg.err_not_on_channel(channel_name), Logger::Debug);
				continue;
			}

			if (!channel->is_operator(client))
			{
				client.push_message(msg.err_chanoprivs_needed(channel_name), Logger::Debug);
				continue;
			}

			if (!m_client_map.count(nick))
			{
				client.push_message(msg.err_user_not_in_channel(nick, channel_name), Logger::Debug);
				continue;
			}

			Client *target_client = m_client_map[nick];

			if (channel->is_user_on_channel(target_client))
			{
				client.push_message(msg.err_user_not_in_channel(nick, channel_name), Logger::Debug);
				continue;
			}
			m_send_to_channel(channel, msg.build_kick_reply(channel_name, nick));
			channel->delete_user(*target_client);
			target_client->erase_channel(channel);
		}
	}
}

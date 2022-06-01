#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"

void
    Server::m_process_topic_command(Client &client, IRCMessage &msg)
{
	if (msg.get_params().empty())
	{
		client.push_message(msg.err_need_more_params(), Logger::Debug);
		return;
	}

	const std::string& channel_name = msg.get_params()[0];
	
	if (!m_channel_map.count(channel_name))
	{
		client.push_message(msg.err_no_such_channel(channel_name), Logger::Debug);
		return ;
	}

	Channel* channel = m_channel_map[channel_name];

	if (msg.get_params().size() == 2)
	{
		if (channel->is_protected_topic() && !channel->is_operator(client))	
		{
			client.push_message(msg.err_chanoprivs_needed(channel_name), Logger::Debug);
			return ;
		}
		channel->set_channel_topic(msg.get_params()[1]);

		Logger().trace() << channel_name << " channel topic change to " << channel->get_channel_topic();
	}

	std::string reply_msg;

	if (channel->get_channel_topic().empty())
		reply_msg = msg.rpl_notopic(channel_name);
	else
		reply_msg = msg.rpl_topic(channel_name, channel->get_channel_topic());

	if (msg.get_params().size() == 1)
	{
		client.push_message(reply_msg, Logger::Debug);
		return ;
	}

	m_send_to_channel(channel, reply_msg);
}

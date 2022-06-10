#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_notice_command(Client &client, Message &msg)
{
	const std::vector<std::string> &parameter = msg.get_params();
    
    if (parameter.size()  < 2)
		return ;
    
    std::vector<const std::string> target_list;
    utils::split_by_comma(target_list, parameter[0]);

    std::vector<const std::string>::iterator target_it = target_list.begin();
    std::vector<const std::string>::iterator target_ite = target_list.end();
    for (;target_it != target_ite; ++target_it)
    {
       if (utils::is_channel_prefix(*target_it))
       {
           if (!m_channel_map.count(*target_it))
                client.push_message(msg.err_no_such_channel(*target_it), Logger::Debug);
            m_send_to_channel(m_channel_map[*target_it], msg.build_message_reply(*target_it));
       }
       else
       {
		   utils::ClientInfo client_info = utils::parse_client_info(*target_it);

		   ClientMap::iterator client_it = m_client_map.begin();
		   ClientMap::iterator client_ite = m_client_map.end();
		   size_t number_of_matched_client = 0;
		   Client *matched_client;
		   for (; client_it != client_ite; ++client_it)
		   {
			   if (client_it->second->is_same_client(client_info))
			   {
				   matched_client = client_it->second;
				   ++number_of_matched_client;
			   }
		   }
		   if (number_of_matched_client == 1)
			   m_prepare_to_send(*matched_client, msg.build_message_reply(*target_it));
       }
    }
}

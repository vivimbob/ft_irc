#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_privmsg_command(Client &client, Message &msg)
{
	const std::vector<std::string> &parameter = msg.get_params();
    
    if (parameter.empty())
        client.push_message(msg.err_no_recipient(), Logger::Debug);
    if (parameter.size() == 1)
        client.push_message(msg.err_no_text_to_send(), Logger::Debug);
    
    std::vector<const std::string> target_list;
    utils::split_by_comma(target_list, parameter[0]);

    std::vector<const std::string>::iterator it = target_list.begin();
    std::vector<const std::string>::iterator ite = target_list.end();
    for (;it != ite; ++it)
    {
       if (utils::is_channel_prefix(*it))
       {
       }
       else
       {
       }
    }
}
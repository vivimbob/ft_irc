#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"

void
    Server::m_process_pass_command(Client &client, Message &msg)
{
    if (!msg.get_params().size())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }
    if (client.is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return ;
    }
    client.set_password(msg.get_params()[0]);
	if (client.is_registered() && !m_client_map.count(client.get_nickname()))
		m_register_client(client, msg);
}


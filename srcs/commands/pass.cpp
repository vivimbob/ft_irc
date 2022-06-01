#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"

void
    Server::process_pass_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }
    if (client.m_is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return ;
    }
    client.m_set_password(msg.get_params()[0]);
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}


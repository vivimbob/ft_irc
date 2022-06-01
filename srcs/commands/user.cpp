#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"

void
  Server::process_user_command(Client &client, IRCMessage &msg)
{
    if (msg.get_params().size() != 4)
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }

    if (client.m_is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return ;
    }

    const std::string &username = msg.get_params()[0];
    const std::string &hostname = msg.get_params()[1];
    client.m_set_username(username);
    client.m_set_hostname(hostname);
    Logger().debug() << client.m_get_client_IP()  << " set username to " << username;
    Logger().debug() << client.m_get_client_IP()  << " set hostname to " << hostname;
	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}

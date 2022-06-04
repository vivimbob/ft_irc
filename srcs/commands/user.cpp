#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"

void
  Server::m_process_user_command(Client &client, Message &msg)
{
    if (msg.get_params().size() != 4)
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return ;
    }

    if (client.is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return ;
    }

    const std::string &username = msg.get_params()[0];
    const std::string &hostname = msg.get_params()[1];
    client.set_username(username);
    client.set_hostname(hostname);
    Logger().debug() << client.get_client_IP()  << " set username to " << username;
    Logger().debug() << client.get_client_IP()  << " set hostname to " << hostname;
	if (client.is_registered() && !m_client_map.count(client.get_nickname()))
		m_register_client(client, msg);
}

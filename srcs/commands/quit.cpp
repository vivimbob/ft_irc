#include "../../includes/server.hpp"

void
    Server::m_process_quit_command(Client& client, Message& msg)
{
	std::string message = "Quit";
	if (msg.get_params().size())
		message += " :" + msg.get_params()[0];
    m_disconnect_client(client, message);
}

#include "../../includes/server.hpp"

void
    Server::m_process_quit_command(Client &client, Message &msg)
{
    (void)msg;
    m_disconnect_client(client);
}

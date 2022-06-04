#include "../../includes/server.hpp"

void
    Server::m_process_quit_command(Client &client, Message &msg)
{
    m_send_to_channel(client, msg.build_quit_reply());
    m_disconnect_client(client);
}

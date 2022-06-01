#include "../../includes/server.hpp"

void
    Server::m_process_quit_command(Client &client, IRCMessage &msg)
{
    m_send_to_channel(client, msg.build_message());
    m_disconnect_client(client);
}

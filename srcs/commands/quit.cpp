#include "../../includes/server.hpp"

void
    Server::process_quit_command(Client &client, IRCMessage &msg)
{
    send_to_channel(client, msg.build_message());
    disconnect_client(client);
}

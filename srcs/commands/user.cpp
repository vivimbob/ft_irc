#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"

void
    Server::m_process_user_command(Client &client, Message &msg)
{
    if (msg.get_params().size() < 4)
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    if (client.is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return;
    }

    client.set_username(msg.get_params()[0]);
    client.set_hostname(msg.get_params()[1]);
    client.set_servername(msg.get_params()[2]);
    client.set_realname(msg.get_params()[3]);
    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
        m_register_client(client, msg);
}

#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"

void
    Server::m_process_pass_command(Client& client, Message& msg)
{
    if (!msg.get_params().size())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }
    if (client.is_registered())
    {
        client.push_message(msg.err_already_registred(), Logger::Debug);
        return;
    }
    if (msg.get_params()[0] != m_password)
    {
        client.push_message(msg.err_passwd_mismatch(), Logger::Debug);
        return;
    }
    client.set_password_flag();
    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
        m_register_client(client, msg);
}

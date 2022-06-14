#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_nick_command(Client& client, Message& msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_no_nickname_given(), Logger::Debug);
        return;
    }

    const std::string& nickname = msg.get_params()[0];

    if (!utils::is_nickname_valid(nickname))
    {
        client.push_message(msg.err_erroneus_nickname(nickname), Logger::Debug);
        return;
    }

    if (m_client_map.count(nickname))
    {
		if (nickname != client.get_nickname())
        	client.push_message(msg.err_nickname_in_use(nickname), Logger::Debug);
        return;
    }

    if (client.is_registered())
    {
        ClientMap::iterator it = m_client_map.begin();
        for (; it != m_client_map.end(); ++it)
            m_prepare_to_send(client, msg.build_nick_reply(nickname));
        if (m_client_map.count(client.get_nickname()))
        {
            m_client_map.erase(client.get_nickname());
            m_client_map[nickname] = &client;
        }
    }

    Logger().debug() << client.get_client_IP() << " change nick to "
                     << nickname;
    client.set_nickname(nickname);

    if (client.is_registered() && !m_client_map.count(client.get_nickname()))
        m_register_client(client, msg);
}

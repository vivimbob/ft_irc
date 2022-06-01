#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void
    Server::process_nick_command(Client &client, IRCMessage &msg)
{
    if (!msg.get_params().size())
    {
        client.push_message(msg.err_no_nickname_given(), Logger::Debug);
        return ;
    }
    
    const std::string &nickname = msg.get_params()[0];

    if (!utils::is_nickname_valid(nickname))
    {
        client.push_message(msg.err_erroneus_nickname(nickname), Logger::Debug);
        return ; 
    }

	if (m_client_map.count(nickname))
	{
		client.push_message(msg.err_nickname_in_use(nickname), Logger::Debug);
        return ;
        // kill command 중복된 닉네임 가진 모든 클라이언트 연결 해제
	}

    if (client.m_is_registered())
    {
		ClientMap::iterator it = m_client_map.begin();
        for (; it != m_client_map.end(); ++it)
        {
            prepare_to_send(*it->second, ":" + client.m_get_nickname() + " NICK " + nickname + "\r\n");
        }
    }
    Logger().debug() << client.m_get_client_IP()  << " change nick to " << nickname;
    client.m_set_nickname(nickname);

	if (client.m_is_registered() && !m_client_map.count(client.m_get_nickname()))
		register_client(client, msg);
}

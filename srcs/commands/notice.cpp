#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void
    Server::m_process_notice_command(Client& client, Message& msg)
{
	m_process_privmsg_command(client, msg);
}

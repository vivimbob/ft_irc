#include "../../includes/ircd.hpp"

void
	IRCD::pong()
{
}

void
	IRCD::ping()
{
	m_to_client(cmd_pong_reply());
}

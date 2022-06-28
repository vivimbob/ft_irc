#include "../../includes/ircd.hpp"

void
	IRCD::ping()
{
	//핑 메시지가 왔음
	//퐁 메시지를 보내줘야함
	m_to_client(cmd_pong_reply());
}

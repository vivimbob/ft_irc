#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void Server::m_process_invite_command(Client &client, Message &msg)
{
  (void)client;
  (void)msg;
  if (msg.get_params().size() < 2)
  {
    client.push_message(msg.err_need_more_params(), Logger::Debug);
    return ;
  }
  else if (msg.get_params().size() == 2)
  {
    
  }
}
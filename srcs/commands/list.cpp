#include "../../includes/server.hpp"
#include "../../includes/logger.hpp"
#include "../../includes/utils.hpp"

void Server::m_process_list_command(Client &client, Message &msg)
{
  (void)client;
  (void)msg;
  if (msg.get_params().empty())
  {

  }
  else if (msg.get_params().size() == 1)
  {
    
  }
}
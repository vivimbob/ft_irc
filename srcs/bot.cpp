#include "../includes/bot.hpp"
#include "../includes/logger.hpp"

Bot::CommandMap Bot::m_bot_command_map = Bot::m_initial_bot_command_map();

Bot::CommandMap
    Bot::m_initial_bot_command_map()
{
    Bot::CommandMap temp_map;

    temp_map.insert(std::make_pair("HELP", &Bot::m_process_help_command));
    temp_map.insert(std::make_pair("TIME", &Bot::m_process_time_command));

    return (temp_map);
}

void Bot::m_process_help_command(Client &client, Message &msg)
{
  client.push_message("Hi! " + client.get_nickname() + " \r\n");
  client.push_message("There are some commands that you can use in command line\r\n");
  client.push_message("command list : [help, time]\r\n");
}

void Bot::m_process_time_command(Client &client, Message &msg)
{
  (void)client;
  (void)msg;
}

Bot::Bot(std::string nickname)
{
  m_nickname = nickname;
}

Bot::~Bot()
{

}

void Bot::store_line_by_line()
{
  SendBuffer &bot_buffer = this->get_send_buffer();
  size_t position = bot_buffer.find_first_of("\r\n", 0);
  while (position != static_cast<size_t>(std::string::npos))
  {
      this->get_commands().push(new Message(this,
      std::string(bot_buffer.begin(), bot_buffer.begin() + position)));
      bot_buffer.erase(0, position + 2);
      position = bot_buffer.find_first_of("\r\n", 0);
  }
}

void Bot::handle_messages(Client &client)
{
  while (this->get_commands().size())
  {
      Message *message = this->get_commands().front();
      Logger().debug() << this->get_nickname() << " send [" << message->get_message() << ']';
      this->get_commands().pop();
      message->parse_message();
      if (m_bot_command_map.count(message->get_command()))
          (this->*m_bot_command_map[message->get_command()])(client, *message);
      else
          client.push_message(message->err_unknown_command(), Logger::Debug);
      delete message;
  }
}
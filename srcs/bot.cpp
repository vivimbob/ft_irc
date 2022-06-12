#include "../includes/bot.hpp"
#include "../includes/logger.hpp"

Bot::CommandMap Bot::m_bot_command_map = Bot::m_initial_bot_command_map();

Bot::CommandMap
    Bot::m_initial_bot_command_map()
{
    Bot::CommandMap temp_map;

    temp_map.insert(std::make_pair("HELP", &Bot::m_process_help_command));
    temp_map.insert(std::make_pair("DATE", &Bot::m_process_date_command));
    temp_map.insert(std::make_pair("TIME", &Bot::m_process_time_command));

    return (temp_map);
}

Bot::Bot(std::string nickname)
{
  m_nickname = nickname;
}

Bot::~Bot()
{
}

void Bot::m_process_help_command(Client &client, Message &msg)
{
  client.push_message("Hi! " + client.get_nickname() + " \r\n");
  client.push_message("This is " + msg.get_params()[1] + " command\r\n");
  client.push_message("There are some commands that you can use in command line\r\n");
  client.push_message("command list : [help, date]\r\n");
}

void Bot::m_process_date_command(Client &client, Message &msg)
{
  time_t m_raw_time;
  char buffer[50];
  time(&m_raw_time);

  if(std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&m_raw_time)))
  {
    client.push_message("Hi! " + client.get_nickname() + " \r\n");
    client.push_message("This is " + msg.get_params()[1] + " command\r\n");
    client.push_message("The current date is " + std::string(buffer) + "\r\n");
  }

}

void Bot::m_process_time_command(Client &client, Message &msg)
{
  time_t m_raw_time;
  char buffer[50];
  time(&m_raw_time);

  if(std::strftime(buffer, sizeof(buffer), "%H:%m:%S", std::localtime(&m_raw_time)))
  {
    client.push_message("Hi! " + client.get_nickname() + " \r\n");
    client.push_message("This is " + msg.get_params()[1] + " command\r\n");
    client.push_message("The current time is " + std::string(buffer) + "\r\n");
  }

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
      std::string bot_command = message->get_params()[1];
      std::string::iterator it = bot_command.begin();
      std::string::iterator ite = bot_command.end();
      for (; it != ite; ++it)
        if ((unsigned)*it - 'a' < 26)
          *it ^= 0b100000;
      if (m_bot_command_map.count(bot_command))
          (this->*m_bot_command_map[bot_command])(client, *message);
      else
          client.push_message(message->err_unknown_command(), Logger::Debug);
      delete message;
  }
}
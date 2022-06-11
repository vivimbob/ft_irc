#include "../includes/bot.hpp"

Bot::CommandMap Bot::m_bot_command_map = Bot::m_initial_bot_command_map();

Bot::CommandMap
    Bot::m_initial_bot_command_map()
{
    Bot::CommandMap temp_map;

    temp_map.insert(std::make_pair("HELP", &Bot::m_process_help_command));
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
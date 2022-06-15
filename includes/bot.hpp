#ifndef BOT_HPP
#define BOT_HPP

#include "client.hpp"
#include <cstdlib>

class Bot : public Client {
public:
  typedef std::map<std::string, void (Bot::*)(Client &)> CommandMap;

private:
  static CommandMap m_bot_command_map;

  static CommandMap m_initial_bot_command_map();

  std::string attach_nickmask_prefix();

  void m_process_help_command(Client &client);
  void m_process_date_command(Client &client);
  void m_process_time_command(Client &client);
  void m_process_coin_command(Client &client);

public:
  Bot(std::string nickname);
  ~Bot(void);
  void store_line_by_line(void);
  void handle_messages(Client &client);
};

#endif /* BOT_HPP */
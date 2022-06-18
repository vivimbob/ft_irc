#ifndef DAEMON_HPP
#define DAEMON_HPP

#include "../includes/command.hpp"
#include "../includes/message.hpp"
#include "../includes/utils.hpp"
#include "resources.hpp"
#include <string>

class Daemon : public Command
{
  public:
    friend class Client;
    // typedef std::map<std::string, void (Daemon::*)(Message&)> Command_Map;
    // typedef std::vector<void (Daemon::*)(Message&)> Command;

  private:
    void m_mode_channel(Message& message, const std::string& channel_name);
    void m_mode_user(Message& message, const std::string& nickname);

  protected:
    TYPE        get_type(std::string command);
    std::string get_command(TYPE type);

    Daemon();
    ~Daemon();
};

#endif /* DAEMON_HPP */

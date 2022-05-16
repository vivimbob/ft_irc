#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <ctime>
#include "server.hpp"

class Channel
{
  private:
    std::string channel_name;
    time_t channel_init_time;
    // std::vector<client> user_lists;
    // std::vector<client> operator_lists;
    std::string channel_topic;
    // std::vector<client> banned_lists;

    Channel();
    Channel(const Channel& cp);
    Channel &operator=(const Channel& cp);

  public:
    Channel(const std::string &name, time_t time);
    ~Channel();
    const std::string &get_channel_name() const;
    const std::string &get_channel_topic() const;
    void set_channel_name(const std::string &name);
    void set_channel_topic(const std::string &topic);
    void display_channel_info();
};

#endif /* CHANNEL_HPP */
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <ctime>
#include "server.hpp"

class Channel
{
  // server에 있는 중첩된 client 클래스를 밖으로 빼고 list vector 관련 구현할 예정.
  private:
    std::string m_channel_name;
    time_t m_channel_init_time;
    // std::vector<client> m_user_lists;
    // std::vector<client> m_operator_lists;
    std::string m_channel_topic;
    // std::vector<client> m_banned_lists;

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
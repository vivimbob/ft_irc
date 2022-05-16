#include "../includes/channel.hpp"
#include "../includes/logger.hpp"

Channel::Channel(const std::string &name, time_t time)
 : m_channel_name(name),
   m_channel_init_time(time)
{
}

Channel::~Channel()
{
}

const
  std::string &Channel::get_channel_name() const
{
  return m_channel_name;
}

const
  std::string &Channel::get_channel_topic() const
{
  return m_channel_topic;
}

void
  Channel::set_channel_name(const std::string &name)
{
  this->m_channel_name = name;
}

void
  Channel::set_channel_topic(const std::string &topic)
{
  this->m_channel_topic = topic;
}

void
  Channel::display_channel_info()
{
  // 일단 확인용으로 로거 출력해놓음.
  Logger().info() << "channel's name : " << this->m_channel_name;
  Logger().info() << "channel's init time : " << this->m_channel_init_time;
  Logger().info() << "channel's topic : " << this->m_channel_topic;
}
#include "../includes/channel.hpp"

Channel::Channel(const std::string &name, time_t time) : channel_name(name), channel_init_time(time)
{
}

Channel::~Channel()
{
}
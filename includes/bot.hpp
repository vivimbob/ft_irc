#ifndef BOT_HPP
#define BOT_HPP

#include "client.hpp"

class Bot : public Client
{
  public:
    Bot(std::string nickname);
    ~Bot(void);
};

#endif /* BOT_HPP */
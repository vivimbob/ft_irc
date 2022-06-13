#include "../includes/membership.hpp"

MemberShip::MemberShip() : client(), channel()
{
}

MemberShip::MemberShip(Client *client, Channel *channel)
    : client(client),
      channel(channel)
{
    mode.operater = false;
    mode.voice    = false;
}

MemberShip::MemberShip(const MemberShip &copy)
    : client(copy.client),
      channel(copy.channel)
{
    this->mode.operater = copy.mode.operater;
    this->mode.voice    = copy.mode.voice;
}

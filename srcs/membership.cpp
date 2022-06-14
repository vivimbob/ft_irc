#include "../includes/membership.hpp"

MemberShip::MemberShip() : client(), channel()
{
}

MemberShip::MemberShip(Client* client, Channel* channel)
    : client(client),
      channel(channel)
{
    mode_operater = false;
}

MemberShip::MemberShip(const MemberShip& copy)
    : client(copy.client),
      channel(copy.channel)
{
    this->mode_operater = copy.mode_operater;
}

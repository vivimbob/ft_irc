#ifndef MEMBERSHIP_HPP
#define MEMBERSHIP_HPP

class Client;
class Channel;

class MemberShip
{
  public:
    MemberShip();
    MemberShip(Client* client, Channel* channel);
    MemberShip(const MemberShip& copy);

    Client* const  client;
    Channel* const channel;
    bool           mode_operater;
};

#endif /* MEMBERSHIP_HPP */

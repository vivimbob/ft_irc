#ifndef MEMBERSHIP_HPP
#define MEMBERSHIP_HPP

class Client;
class Channel;

class MemberShip
{
  public:
    struct s_mode
    {
        bool operater;
        bool voice;
    };

    MemberShip();
    MemberShip(Client* client, Channel* channel);
    MemberShip(const MemberShip& copy);

    Client* const  client;
    Channel* const channel;
    s_mode         mode;
};

#endif /* MEMBERSHIP_HPP */

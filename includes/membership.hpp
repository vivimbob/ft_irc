#ifndef MEMBERSHIP_HPP
#define MEMBERSHIP_HPP

class Client;
class Channel;

class MemberShip
{
public :
	struct s_mode
	{
		bool o:1;
		bool v:1;
	};

	MemberShip(Client *client, Channel *channel)
		: client(client),
		  channel(channel)
	{
		mode.o = false;
		mode.v = false;
	}	
	
	Client *const client;
	Channel *const channel;
	s_mode	mode;

};

#endif /* MEMBERSHIP_HPP */

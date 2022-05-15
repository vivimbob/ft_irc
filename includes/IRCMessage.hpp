#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

class IRCMessage
{
public:
    IRCMessage(void);
    IRCMessage(unsigned int clientfd, const std::string &message);
    ~IRCMessage(void);

    void    parse_message(void);
};

#endif /* IRCMESSAGE_HPP */

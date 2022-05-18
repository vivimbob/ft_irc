#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

#define MESSAGE_MEX_LEN 512

class IRCMessage
{
private:
    std::string m_message;
    std::string m_prefix;
    std::string m_command;
    std::vector<std::string> m_parameters;
    bool m_valid_message; 
    int m_position;

    IRCMessage(void);
    IRCMessage(const IRCMessage& copy);
	IRCMessage	&operator=(const IRCMessage& other);

    int next_position(void);
public:
    IRCMessage(unsigned int clientfd, const std::string &message);
    ~IRCMessage(void);

    void    parse_message(void);
    const std::string   &get_prefix(void) const;
	const std::string   &get_command(void) const;
	const std::vector<std::string>  &get_params(void) const;
    const bool &is_valid_message(void) const;
};

#endif /* IRCMESSAGE_HPP */